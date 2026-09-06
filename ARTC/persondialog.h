#ifndef PERSONDIALOG_H
#define PERSONDIALOG_H

#include <QDate>
#include <QDialog>
#include <QString>

class QDateEdit;
class QLabel;
class QLineEdit;
struct TreeNode;

/**
 * @brief The details a user typed for one slot.
 */
struct PersonDetails
{
    QString firstName;
    QString middleName;
    QString lastName;
    QDate birthdate;
};

/**
 * @brief Fill or empty one slot in the pedigree.
 *
 * A slot is a fixed position in the chart — "Mother of Byron Coughlin" — that
 * exists whether or not anyone is known to occupy it. This dialog never
 * creates or destroys the slot itself, only the person in it, so the branch
 * of ancestors hanging above it is never disturbed.
 */
class PersonDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @param node the slot being edited
     * @param relationship how the slot relates to the host, e.g. "Mother"
     * @param hostName the host's name, for the heading
     */
    PersonDialog(const TreeNode *node, const QString &relationship,
                 const QString &hostName, QWidget *parent = nullptr);

    /** @brief The details entered, valid when the dialog was accepted. */
    PersonDetails details() const { return entered; }

    /** @brief True when the user chose to empty the slot rather than fill it. */
    bool removalRequested() const { return removing; }

private slots:
    void save();
    void remove();

private:
    QLineEdit *firstNameEdit;
    QLineEdit *middleNameEdit;
    QLineEdit *lastNameEdit;
    QDateEdit *birthdateEdit;
    QLabel *errorLabel;
    PersonDetails entered;
    bool removing = false;
};

#endif // PERSONDIALOG_H
