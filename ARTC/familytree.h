#ifndef FAMILYTREE_H
#define FAMILYTREE_H

#include <QChar>
#include <QDate>
#include <QList>
#include <QString>

/**
 * @brief One position in a pedigree chart.
 *
 * Every slot exists as a row in tblPerson from the moment a host is created.
 * Most are placeholders: the generator writes a name that encodes the slot's
 * position rather than a person, and they stay that way until DNA matches are
 * imported and resolved onto them.
 */
struct TreeNode
{
    int id = 0;
    QString slotCode;
    QString firstName;
    QString middleName;
    QString lastName;
    QChar sex;
    QDate birthdate;
    QDate deathdate;
    int generation = 0;

    TreeNode *father = nullptr;
    TreeNode *mother = nullptr;

    /** @brief True while this slot holds a generated placeholder, not a person. */
    bool isPlaceholder() const;

    /** @brief "Given Middle Family", or an empty string for a placeholder. */
    QString displayName() const;

    /** @brief "b. 1930 - d. 1998", or an empty string when neither is known. */
    QString lifespan() const;

    /** @brief Name this slot by its position, e.g. "Great-grandmother". */
    QString relationship() const;
};

/**
 * @brief Loads a host's pedigree out of tblPerson.
 *
 * Owns every node it creates; destroying the tree frees them.
 */
class FamilyTree
{
public:
    FamilyTree() = default;
    ~FamilyTree();

    FamilyTree(const FamilyTree &) = delete;
    FamilyTree &operator=(const FamilyTree &) = delete;

    /**
     * @brief Read the host and every ancestor reachable from it.
     * @param hostId tblPerson.ID of the root person
     * @param[out] error message suitable for showing to the user
     * @return true when the host was found
     */
    bool load(int hostId, QString *error = nullptr);

    /** @brief The host, or null before a successful load(). */
    TreeNode *root() const { return rootNode; }

    /** @brief Every node, host first, in breadth-first order. */
    QList<TreeNode *> nodes() const { return allNodes; }

    /** @brief Deepest generation present; the host is generation 0. */
    int depth() const;

    /** @brief How many slots are still placeholders. */
    int placeholderCount() const;

    /**
     * @brief The id of the most recently created host, or 0 if there is none.
     *
     * Reads tblHost, falling back to the newest non-placeholder person so a
     * pedigree built before setHost() was wired up can still be opened.
     */
    static int currentHostId();

    /**
     * @brief Write a person into the slot with id @p slotId.
     * @param[out] error message suitable for showing to the user
     */
    static bool savePerson(int slotId, const QString &firstName, const QString &middleName,
                           const QString &lastName, const QDate &birthdate,
                           const QDate &deathdate, QString *error = nullptr);

    /**
     * @brief Empty the slot with id @p slotId, keeping the slot and its branch.
     * @param[out] error message suitable for showing to the user
     */
    static bool clearPerson(int slotId, QString *error = nullptr);

private:
    TreeNode *rootNode = nullptr;
    QList<TreeNode *> allNodes;
};

#endif // FAMILYTREE_H
