#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hostdlg.h"
#include "accountrepository.h"
#include "databasehelper.h"
#include "homewidget.h"
#include "familytree.h"
#include "pedigree.h"
#include "pedigreeview.h"
#include "persondialog.h"
#include "registerwidget.h"
#include "signinwidget.h"
#include "uitheme.h"

#include <QFrame>
#include <QLabel>
#include <QApplication>
#include <QHBoxLayout>
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
    pedigreeScreen = new PedigreeView(this);

    screens = new QStackedWidget(this);
    screens->addWidget(homeScreen);
    screens->addWidget(signInScreen);
    screens->addWidget(registerScreen);
    screens->addWidget(workspaceScreen);
    screens->addWidget(pedigreeScreen);

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
    constexpr int CardWidth = 560;
    constexpr int CardMargin = 40;
    constexpr int ContentWidth = CardWidth - 2 * CardMargin;

    auto *page = new QWidget(this);
    page->setObjectName(QStringLiteral("screen"));
    page->setAttribute(Qt::WA_StyledBackground, true);

    auto *card = new QFrame(page);
    card->setObjectName(QStringLiteral("card"));
    card->setFixedWidth(CardWidth);
    UiTheme::applyCardShadow(card);

    auto *title = new QLabel(tr("Workspace"), card);
    title->setObjectName(QStringLiteral("screenTitle"));

    workspaceGreeting = new WrappingLabel(QString(), ContentWidth, card);
    workspaceGreeting->setObjectName(QStringLiteral("bodyText"));

    auto *hint = new WrappingLabel(tr("Choose File → New Host to enter the root person "
                                      "and generate their pedigree chart."),
                                   ContentWidth, card);
    hint->setObjectName(QStringLiteral("bodyText"));

    auto *signOutButton = new QPushButton(tr("Sign Out"), card);
    signOutButton->setObjectName(QStringLiteral("secondaryButton"));
    signOutButton->setCursor(Qt::PointingHandCursor);

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(CardMargin, 36, CardMargin, 32);
    cardLayout->setSpacing(0);
    cardLayout->addWidget(title);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(workspaceGreeting);
    cardLayout->addSpacing(20);
    cardLayout->addWidget(UiTheme::createDivider(card));
    cardLayout->addSpacing(20);
    cardLayout->addWidget(hint);
    cardLayout->addSpacing(26);

    auto *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(24, 24, 24, 24);
    pageLayout->addStretch(1);
    pageLayout->addWidget(card, 0, Qt::AlignHCenter);
    pageLayout->addStretch(1);

    openTreeButton = new QPushButton(tr("Open Family Tree"), card);
    openTreeButton->setObjectName(QStringLiteral("primaryButton"));
    openTreeButton->setCursor(Qt::PointingHandCursor);
    openTreeButton->hide();

    auto *actions = new QHBoxLayout;
    actions->setSpacing(12);
    actions->addWidget(openTreeButton);
    actions->addWidget(signOutButton);
    actions->addStretch(1);
    cardLayout->addLayout(actions);

    connect(signOutButton, &QPushButton::clicked, this, &MainWindow::signOut);
    connect(openTreeButton, &QPushButton::clicked, this, &MainWindow::showFamilyTree);

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

    connect(pedigreeScreen, &PedigreeView::slotActivated, this, &MainWindow::editSlot);

    connect(pedigreeScreen, &PedigreeView::backRequested, this, [this] {
        setWorkspaceChromeVisible(true);
        screens->setCurrentWidget(workspaceScreen);
    });
}

/**
 * @brief MainWindow::showFamilyTree
 */
void MainWindow::showFamilyTree()
{
    QString error;
    if (!ensureConnected(&error)) {
        QMessageBox::critical(this, tr("Family tree"), error);
        return;
    }

    const int hostId = FamilyTree::currentHostId();
    if (hostId <= 0) {
        QMessageBox::information(this, tr("No host yet"),
                                 tr("Create a host first: File \u2192 New Host."));
        return;
    }

    if (!pedigreeScreen->showHost(hostId, &error)) {
        QMessageBox::critical(this, tr("Family tree"), error);
        return;
    }

    setWorkspaceChromeVisible(true);
    screens->setCurrentWidget(pedigreeScreen);
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
    // The connection belongs to the application, not the person signed in, so
    // it is left open; only the session is cleared.
    signedInAccount = Account();
    showHome();
}

/**
 * @brief MainWindow::ensureConnected
 * @param error
 * @return
 */
bool MainWindow::ensureConnected(QString *error)
{
    if (connected) {
        return true;
    }

    if (!databaseHelper.createConnection()) {
        if (error) {
            *error = databaseHelper.lastError();
        }
        return false;
    }

    connected = true;
    return true;
}

/**
 * @brief MainWindow::attemptSignIn
 * @param email
 * @param password
 */
void MainWindow::attemptSignIn(const QString &email, const QString &password)
{
    QString error;
    if (!ensureConnected(&error)) {
        signInScreen->showError(error);
        return;
    }

    if (!AccountRepository::authenticate(email, password, &signedInAccount, &error)) {
        signInScreen->showError(error);
        return;
    }

    workspaceGreeting->setText(tr("Signed in as %1.").arg(signedInAccount.firstName));
    openTreeButton->setVisible(FamilyTree::currentHostId() > 0);
    setWorkspaceChromeVisible(true);
    screens->setCurrentWidget(workspaceScreen);
    statusBar()->showMessage(tr("Signed in as %1").arg(signedInAccount.email));
}

/**
 * @brief MainWindow::handleRegistration
 *
 * Creates the account, then hands the new address to the sign-in screen. The
 * user is deliberately not signed in automatically: proving they can type the
 * password they just chose catches a typo now rather than at the next launch.
 *
 * @param details the validated account details
 */
void MainWindow::handleRegistration(const RegistrationDetails &details)
{
    QString error;
    if (!ensureConnected(&error)) {
        registerScreen->showError(error);
        return;
    }

    if (AccountRepository::emailTaken(details.email, &error)) {
        registerScreen->showError(
            error.isEmpty()
                ? tr("An account already exists for %1.").arg(details.email)
                : error);
        return;
    }

    if (!AccountRepository::create(details, &error)) {
        registerScreen->showError(error);
        return;
    }

    const QString email = details.email;
    showSignIn();
    signInScreen->setEmail(email);
    statusBar()->showMessage(tr("Account created for %1").arg(email));
}

/**
 * @brief MainWindow::editSlot
 * @param slotId
 */
void MainWindow::editSlot(int slotId)
{
    FamilyTree tree;
    QString error;
    if (!tree.load(FamilyTree::currentHostId(), &error)) {
        QMessageBox::critical(this, tr("Family tree"), error);
        return;
    }

    const TreeNode *slot = nullptr;
    for (TreeNode *node : tree.nodes()) {
        if (node->id == slotId) {
            slot = node;
            break;
        }
    }
    if (!slot) {
        return;
    }

    // The host is the root of the chart, not a slot to be emptied.
    if (slot->generation == 0) {
        QMessageBox::information(this, tr("Host"),
                                 tr("%1 is the host — the person the whole chart is "
                                    "built around, so this slot cannot be emptied.")
                                     .arg(slot->displayName()));
        return;
    }

    const QString hostName = tree.root() ? tree.root()->displayName() : QString();
    PersonDialog dialog(slot, slot->relationship(), hostName, this);
    dialog.setModal(true);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    bool ok = false;
    if (dialog.removalRequested()) {
        ok = FamilyTree::clearPerson(slotId, &error);
    } else {
        const PersonDetails d = dialog.details();
        ok = FamilyTree::savePerson(slotId, d.firstName, d.middleName, d.lastName,
                                    d.birthdate, d.deathdate, &error);
    }

    if (!ok) {
        QMessageBox::critical(this, tr("Family tree"), error);
        return;
    }

    pedigreeScreen->reload();
}

void MainWindow::on_action_New_Host_triggered()
{
    QString error;
    if (!ensureConnected(&error)) {
        QMessageBox::critical(this, tr("New host"), error);
        return;
    }

    Hostdlg hostdlg;
    hostdlg.setModal(true);
    if (hostdlg.exec() != QDialog::Accepted) {
        // Cancelling used to fall through and write an empty person, plus the
        // hundreds of placeholder ancestors generated from it.
        return;
    }
    person = hostdlg.getHost();

    // Add the root person. If this fails, stop: getPersonId() would return an
    // uninitialised id and the generator would build a whole tree under it.
    if (!databaseHelper.addPerson(person)) {
        return;
    }

    person.id = databaseHelper.getPersonId(person);
    if (person.id <= 0) {
        QMessageBox::critical(this, tr("New host"),
                              tr("The host was saved but could not be read back, so no "
                                 "pedigree was generated."));
        return;
    }

    databaseHelper.setHost(person.id);

    // The root's own two parents, as empty slots.
    const int fatherId = databaseHelper.addSlot(
        QStringLiteral("FATHER-%1").arg(person.id), 'M');
    const int motherId = databaseHelper.addSlot(
        QStringLiteral("MOTHER-%1").arg(person.id), 'F');
    databaseHelper.addParents(person.id, fatherId, motherId);
    person.fatherId = fatherId;
    person.motherId = motherId;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    Pedigree pedigree;
    pedigree.createPedigree(person, databaseHelper);
    QApplication::restoreOverrideCursor();

    openTreeButton->show();
    showFamilyTree();
}
