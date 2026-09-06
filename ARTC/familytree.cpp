#include "familytree.h"

#include <QHash>
#include <QObject>
#include <QQueue>
#include <QRegularExpression>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

namespace {

/**
 * @brief Matches the names Pedigree::createSide generates, e.g. "FGF21-7".
 *
 * side (F|M), grandparent (GF|GM), generation, sequence, then the host's id.
 */
const QRegularExpression &slotNamePattern()
{
    static const QRegularExpression pattern(QStringLiteral("^[FM]G[FM]\\d+-\\d+$"));
    return pattern;
}

/** Read one row into a node. */
TreeNode *nodeFromQuery(const QSqlQuery &query)
{
    auto *node = new TreeNode;
    node->id = query.value(0).toInt();
    node->firstName = query.value(1).toString();
    node->middleName = query.value(2).toString();
    node->lastName = query.value(3).toString();
    const QString sex = query.value(4).toString();
    node->sex = sex.isEmpty() ? QChar() : sex.at(0);
    node->birthdate = query.value(5).toDate();
    node->slotCode = query.value(8).toString();
    return node;
}

} // namespace

bool TreeNode::isPlaceholder() const
{
    // An empty slot simply has no name. The pattern test is only for rows
    // written before slotCode existed, which kept the code in firstName.
    return firstName.trimmed().isEmpty()
           || firstName == QLatin1String("FATHER") || firstName == QLatin1String("MOTHER")
           || slotNamePattern().match(firstName).hasMatch();
}

QString TreeNode::displayName() const
{
    if (isPlaceholder()) {
        return QString();
    }

    QStringList parts;
    for (const QString &part : {firstName, middleName, lastName}) {
        if (!part.trimmed().isEmpty()) {
            parts << part.trimmed();
        }
    }
    return parts.join(QLatin1Char(' '));
}

FamilyTree::~FamilyTree()
{
    qDeleteAll(allNodes);
}

bool FamilyTree::load(int hostId, QString *error)
{
    qDeleteAll(allNodes);
    allNodes.clear();
    rootNode = nullptr;

    QSqlQuery query;
    query.prepare(QStringLiteral(
        "SELECT ID, firstName, middleName, lastName, gender, birthdate, fatherId, "
        "motherId, slotCode FROM tblPerson WHERE ID = :id"));

    // Breadth-first from the host, so generation numbers fall out of the walk
    // and a cycle in the parent links cannot loop forever.
    QHash<int, TreeNode *> byId;
    QQueue<QPair<int, TreeNode *>> pending; // parent id, node whose slot it fills
    QQueue<TreeNode *> frontier;

    query.bindValue(QStringLiteral(":id"), hostId);
    if (!query.exec()) {
        if (error) {
            *error = QObject::tr("Could not read the pedigree.\n\n%1")
                         .arg(query.lastError().text());
        }
        return false;
    }
    if (!query.next()) {
        if (error) {
            *error = QObject::tr("No person with id %1.").arg(hostId);
        }
        return false;
    }

    rootNode = nodeFromQuery(query);
    rootNode->generation = 0;
    byId.insert(rootNode->id, rootNode);
    allNodes.append(rootNode);

    struct Link { TreeNode *child; int fatherId; int motherId; };
    QQueue<Link> links;
    links.enqueue({rootNode, query.value(6).toInt(), query.value(7).toInt()});

    while (!links.isEmpty()) {
        const Link link = links.dequeue();

        for (int which = 0; which < 2; ++which) {
            const int parentId = which == 0 ? link.fatherId : link.motherId;
            if (parentId <= 0 || byId.contains(parentId)) {
                continue;
            }

            query.bindValue(QStringLiteral(":id"), parentId);
            if (!query.exec() || !query.next()) {
                continue; // a dangling parent id simply ends that branch
            }

            TreeNode *parent = nodeFromQuery(query);
            parent->generation = link.child->generation + 1;
            byId.insert(parent->id, parent);
            allNodes.append(parent);

            if (which == 0) {
                link.child->father = parent;
            } else {
                link.child->mother = parent;
            }

            links.enqueue({parent, query.value(6).toInt(), query.value(7).toInt()});
        }
    }

    return true;
}

int FamilyTree::depth() const
{
    int deepest = 0;
    for (const TreeNode *node : allNodes) {
        deepest = qMax(deepest, node->generation);
    }
    return deepest;
}

int FamilyTree::placeholderCount() const
{
    int count = 0;
    for (const TreeNode *node : allNodes) {
        if (node->isPlaceholder()) {
            ++count;
        }
    }
    return count;
}

QString TreeNode::relationship() const
{
    const bool male = sex != QLatin1Char('F');
    switch (generation) {
    case 0: return QObject::tr("Host");
    case 1: return male ? QObject::tr("Father") : QObject::tr("Mother");
    case 2: return male ? QObject::tr("Grandfather") : QObject::tr("Grandmother");
    case 3: return male ? QObject::tr("Great-grandfather")
                        : QObject::tr("Great-grandmother");
    default: {
        const int greats = generation - 2;
        const QString suffix = (greats % 100 >= 11 && greats % 100 <= 13) ? QStringLiteral("th")
                             : greats % 10 == 1 ? QStringLiteral("st")
                             : greats % 10 == 2 ? QStringLiteral("nd")
                             : greats % 10 == 3 ? QStringLiteral("rd")
                                                : QStringLiteral("th");
        const QString ord = QStringLiteral("%1%2").arg(greats).arg(suffix);
        return male ? QObject::tr("%1 great-grandfather").arg(ord)
                    : QObject::tr("%1 great-grandmother").arg(ord);
    }
    }
}

bool FamilyTree::savePerson(int slotId, const QString &firstName, const QString &middleName,
                            const QString &lastName, const QDate &birthdate, QString *error)
{
    QSqlQuery query;
    query.prepare(QStringLiteral(
        "UPDATE tblPerson SET firstName = :first, middleName = :middle, "
        "lastName = :last, birthdate = :born WHERE ID = :id"));
    query.bindValue(QStringLiteral(":first"), firstName);
    query.bindValue(QStringLiteral(":middle"), middleName.isEmpty() ? QVariant() : middleName);
    query.bindValue(QStringLiteral(":last"), lastName);
    query.bindValue(QStringLiteral(":born"), birthdate.isValid() ? QVariant(birthdate)
                                                                 : QVariant());
    query.bindValue(QStringLiteral(":id"), slotId);

    if (!query.exec()) {
        if (error) {
            *error = QObject::tr("Could not save the person.\n\n%1")
                         .arg(query.lastError().text());
        }
        return false;
    }
    return true;
}

bool FamilyTree::clearPerson(int slotId, QString *error)
{
    // The row stays: it is the slot, and every ancestor above it is reached
    // through its fatherId and motherId. Only the person is cleared.
    QSqlQuery query;
    query.prepare(QStringLiteral(
        "UPDATE tblPerson SET firstName = NULL, middleName = NULL, lastName = NULL, "
        "birthdate = NULL WHERE ID = :id"));
    query.bindValue(QStringLiteral(":id"), slotId);

    if (!query.exec()) {
        if (error) {
            *error = QObject::tr("Could not empty the slot.\n\n%1")
                         .arg(query.lastError().text());
        }
        return false;
    }
    return true;
}

int FamilyTree::currentHostId()
{
    QSqlQuery query;
    if (query.exec(QStringLiteral("SELECT personId FROM tblHost ORDER BY ID DESC LIMIT 1"))
        && query.next()) {
        return query.value(0).toInt();
    }

    // tblHost went unwritten for a long time, so fall back to the newest person
    // whose name is not one of the generator's placeholders.
    if (query.exec(QStringLiteral(
            "SELECT ID FROM tblPerson "
            "WHERE firstName IS NOT NULL AND firstName <> '' "
            "ORDER BY ID DESC LIMIT 1"))
        && query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}
