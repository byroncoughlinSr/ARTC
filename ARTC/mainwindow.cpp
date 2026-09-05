#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hostdlg.h"
#include "databasehelper.h"
#include "homewidget.h"
#include "pedigree.h"
#include "registerwidget.h"
#include "signinwidget.h"
#include "uitheme.h"

#include <QFrame>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    homeScreen = new HomeWidget(this);
    signInScreen = new SignInWidget(this);
    registerScreen = new RegisterWidget(this);
    workspaceScreen = createWorkspacePage();

    screens = new QStackedWidget(this);
    screens->addWidget(homeScreen);
    screens->addWidget(signInScreen);
    screens->addWidget(registerScreen);
    screens->addWidget(workspaceScreen);

    auto *centralLayout = new QVBoxLayout(ui->centralWidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->addWidget(screens);

    connectScreens();
    setWorkspaceChromeVisible(false);
    showHome();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::createWorkspacePage
 * @return the page shown once the user is signed in
 */
QWidget *MainWindow::createWorkspacePage()
{
    auto *page = new QWidget(this);
    page->setObjectName(QStringLiteral("screen"));
    page->setAttribute(Qt::WA_StyledBackground, true);

    auto *card = new QFrame(page);
    card->setObjectName(QStringLiteral("card"));
    card->setFixedWidth(560);
    UiTheme::applyCardShadow(card);

    auto *title = new QLabel(tr("Workspace"), card);
    title->setObjectName(QStringLiteral("screenTitle"));

    workspaceGreeting = new QLabel(card);
    workspaceGreeting->setObjectName(QStringLiteral("bodyText"));
    workspaceGreeting->setWordWrap(true);

    auto *hint = new QLabel(tr("Choose File → New Host to enter the root person and "
                               "generate their pedigree chart."), card);
    hint->setObjectName(QStringLiteral("bodyText"));
    hint->setWordWrap(true);

    auto *signOutButton = new QPushButton(tr("Sign Out"), card);
    signOutButton->setObjectName(QStringLiteral("secondaryButton"));
    signOutButton->setCursor(Qt::PointingHandCursor);

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(40, 36, 40, 32);
    cardLayout->setSpacing(0);
    cardLayout->addWidget(title);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(workspaceGreeting);
    cardLayout->addSpacing(20);
    cardLayout->addWidget(UiTheme::createDivider(card));
    cardLayout->addSpacing(20);
    cardLayout->addWidget(hint);
    cardLayout->addSpacing(26);
    cardLayout->addWidget(signOutButton, 0, Qt::AlignLeft);

    auto *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(24, 24, 24, 24);
    pageLayout->addStretch(1);
    pageLayout->addWidget(card, 0, Qt::AlignHCenter);
    pageLayout->addStretch(1);

    connect(signOutButton, &QPushButton::clicked, this, &MainWindow::signOut);

    return page;
}

/**
 * @brief MainWindow::connectScreens
 */
void MainWindow::connectScreens()
{
    connect(homeScreen, &HomeWidget::signInRequested, this, &MainWindow::showSignIn);
    connect(homeScreen, &HomeWidget::registerRequested, this, &MainWindow::showRegister);

    connect(signInScreen, &SignInWidget::signInRequested, this, &MainWindow::attemptSignIn);
    connect(signInScreen, &SignInWidget::registerRequested, this, &MainWindow::showRegister);
    connect(signInScreen, &SignInWidget::backRequested, this, &MainWindow::showHome);

    connect(registerScreen, &RegisterWidget::registrationSubmitted,
            this, &MainWindow::handleRegistration);
    connect(registerScreen, &RegisterWidget::signInRequested, this, &MainWindow::showSignIn);
    connect(registerScreen, &RegisterWidget::backRequested, this, &MainWindow::showHome);
}

/**
 * @brief MainWindow::setWorkspaceChromeVisible
 * @param visible
 */
void MainWindow::setWorkspaceChromeVisible(bool visible)
{
    ui->menuBar->setVisible(visible);
    ui->mainToolBar->setVisible(visible);
    ui->statusBar->setVisible(visible);
}

void MainWindow::showHome()
{
    setWorkspaceChromeVisible(false);
    screens->setCurrentWidget(homeScreen);
}

void MainWindow::showSignIn()
{
    setWorkspaceChromeVisible(false);
    signInScreen->reset();
    screens->setCurrentWidget(signInScreen);
}

void MainWindow::showRegister()
{
    setWorkspaceChromeVisible(false);
    registerScreen->reset();
    screens->setCurrentWidget(registerScreen);
}

void MainWindow::signOut()
{
    databaseHelper = DatabaseHelper(QString(), QString());
    showHome();
}

/**
 * @brief MainWindow::attemptSignIn
 * @param username
 * @param password
 */
void MainWindow::attemptSignIn(const QString &username, const QString &password)
{
    databaseHelper = DatabaseHelper(username, password);

    if (!databaseHelper.createConnection()) {
        signInScreen->showError(databaseHelper.lastError());
        return;
    }

    workspaceGreeting->setText(tr("Signed in as %1.").arg(username));
    setWorkspaceChromeVisible(true);
    screens->setCurrentWidget(workspaceScreen);
    statusBar()->showMessage(tr("Connected as %1").arg(username));
}

/**
 * @brief MainWindow::handleRegistration
 *
 * Registration is validated but not stored: there is no account table yet.
 * Once one exists, insert @p details here and sign the new user in.
 *
 * @param details the validated account details
 */
void MainWindow::handleRegistration(const RegistrationDetails &details)
{
    QMessageBox::information(
        this, tr("Account not saved"),
        tr("%1's details are valid, but account storage has not been built yet.\n\n"
           "Sign in with an existing database account to continue.")
            .arg(details.firstName));

    showSignIn();
}

void MainWindow::on_action_New_Host_triggered()
{
    int fatherId;
    int motherId;
    Hostdlg hostdlg;
    Pedigree pedigree;
    Person::Individual father;
    Person::Individual mother;

    hostdlg.setModal(true);
    hostdlg.exec();
    person = hostdlg.getHost();

    //Add root person to database
    databaseHelper.addPerson(person);
    person.id = databaseHelper.getPersonId(person);

    //Add mother and father to database

    father.firstName = "FATHER";
    father.lastName = QString::number(person.id);
    father.sex = 'M';
    father.birthdate.setDate(0001, 1, 1);
    databaseHelper.addPerson(father);

    //Add mother to database
    mother.firstName = "MOTHER";
    motherId = person.id;
    mother.lastName = QString::number(person.id);
    mother.sex = 'F';
    mother.birthdate.setDate(0001, 1, 1);
    databaseHelper.addPerson(mother);

    fatherId = databaseHelper.getFatherId("FATHER", QString::number( person.id));
    motherId = databaseHelper.getMotherId("MOTHER", QString::number( person.id));
    databaseHelper.addParents(person.id, fatherId, motherId);
    person.fatherId = fatherId;
    person.motherId = motherId;
    pedigree.createPedigree(person, databaseHelper);
}
