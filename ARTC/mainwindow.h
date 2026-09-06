#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "databasehelper.h"
#include "person.h"
#include "accountrepository.h"
#include "registerwidget.h"

class HomeWidget;
class QLabel;
class QStackedWidget;
class RegisterWidget;
class SignInWidget;

namespace Ui {
class MainWindow;
}

/**
 * @brief Application shell and screen router.
 *
 * Holds every screen in a QStackedWidget and moves between them. The menu bar
 * and tool bar belong to the workspace, so they stay hidden until a sign-in
 * has succeeded.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    DatabaseHelper databaseHelper = DatabaseHelper(QString(), QString());
    Person::Individual person;

private slots:
    void on_action_New_Host_triggered();

    /** @brief Check @p email / @p password against tblAccount. */
    void attemptSignIn(const QString &email, const QString &password);

    /** @brief Create an account from @p details. */
    void handleRegistration(const RegistrationDetails &details);

    void showHome();
    void showSignIn();
    void showRegister();
    void signOut();

private:
    /** @brief Build the post-sign-in workspace page. */
    QWidget *createWorkspacePage();

    /** @brief Wire every screen's navigation signals to this window. */
    void connectScreens();

    /** @brief Show or hide the workspace chrome. */
    void setWorkspaceChromeVisible(bool visible);

    /**
     * @brief Open the database with the application's own account if needed.
     * @param[out] error why the connection failed, suitable for a screen
     * @return true when a usable connection is open
     */
    bool ensureConnected(QString *error);

    bool connected = false;
    Account signedInAccount;

    Ui::MainWindow *ui;
    QStackedWidget *screens;
    HomeWidget *homeScreen;
    SignInWidget *signInScreen;
    RegisterWidget *registerScreen;
    QWidget *workspaceScreen;
    QLabel *workspaceGreeting;
    Person::Individual father;
    Person::Individual mother;
};

#endif // MAINWINDOW_H
