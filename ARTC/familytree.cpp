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
    return node;
}

} // namespace

bool TreeNode::isPlaceholder() const
{
    return firstName == QLatin1String("FATHER") || firstName == QLatin1String("MOTHER")
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
        "SELECT ID, firstName, middleName, lastName, gender, birthdate, fatherId, motherId "
        "FROM tblPerson WHERE ID = :id"));

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
            "WHERE firstName NOT IN ('FATHER','MOTHER') "
            "AND firstName NOT REGEXP '^[FM]G[FM][0-9]+-[0-9]+$' "
            "ORDER BY ID DESC LIMIT 1"))
        && query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}
