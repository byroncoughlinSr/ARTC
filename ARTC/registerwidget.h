#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QDate>
#include <QString>
#include <QWidget>

class QDateEdit;
class QLabel;
class QLineEdit;

/**
 * @brief A validated set of account details ready to be persisted.
 *
 * The name and birthdate fields deliberately mirror Person::Individual so a
 * new account can also seed the pedigree host row once storage exists.
 */
struct RegistrationDetails
{
    QString firstName;
    QString middleName;
    QString lastName;
    QDate birthdate;
    QString email;
    QString password;
};

/**
 * @brief Account creation screen.
 *
 * Collects and validates the details for a new account. It performs no
 * storage of its own — a caller connects to registrationSubmitted() and
 * decides what to do with the result.
 */
class RegisterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWidget(QWidget *parent = nullptr);

    /** @brief Clear every field and any validation error. */
    void reset();

    /** @brief Show @p message above the form as a validation failure. */
    void showError(const QString &message);

signals:
    /** @brief The form validated cleanly and was submitted. */
    void registrationSubmitted(const RegistrationDetails &details);

    /** @brief The user wants the sign-in screen instead. */
    void signInRequested();

    /** @brief The user backed out to the home screen. */
    void backRequested();

private slots:
    void submit();

private:
    /**
     * @brief Validate the form and fill @p details.
     * @param[out] details populated only when validation succeeds
     * @return true when every field is acceptable
     */
    bool validate(RegistrationDetails &details);

    /** @brief Clear the error banner and every field's invalid marker. */
    void clearErrors();

    /** @brief Flag @p editor as invalid, report @p message, and focus it. */
    void failField(QWidget *editor, const QString &message);

    QLineEdit *firstNameEdit;
    QLineEdit *middleNameEdit;
    QLineEdit *lastNameEdit;
    QDateEdit *birthdateEdit;
    QLineEdit *emailEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmEdit;
    QLabel *errorLabel;
};

#endif // REGISTERWIDGET_H
