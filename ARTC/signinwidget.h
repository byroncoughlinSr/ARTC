#ifndef SIGNINWIDGET_H
#define SIGNINWIDGET_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;

/**
 * @brief Sign-in screen.
 *
 * Collects credentials and hands them to a caller, which decides whether they
 * are accepted. The screen itself opens no database connection, so a failed
 * attempt can be reported back inline via showError().
 */
class SignInWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SignInWidget(QWidget *parent = nullptr);

    /** @brief Clear the password, the error, and focus the email field. */
    void reset();

    /** @brief Pre-fill the email field, e.g. straight after registering. */
    void setEmail(const QString &email);

    /** @brief Show @p message beneath the form and re-enable the button. */
    void showError(const QString &message);

    /** @brief Disable input while a sign-in attempt is in flight. */
    void setBusy(bool busy);

signals:
    /** @brief The user submitted credentials. */
    void signInRequested(const QString &email, const QString &password);

    /** @brief The user wants the registration screen instead. */
    void registerRequested();

    /** @brief The user backed out to the home screen. */
    void backRequested();

private slots:
    void submit();

private:
    QLineEdit *emailEdit;
    QLineEdit *passwordEdit;
    QPushButton *signInButton;
    QLabel *errorLabel;
};

#endif // SIGNINWIDGET_H
