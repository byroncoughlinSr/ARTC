#include "signinwidget.h"
#include "uitheme.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

SignInWidget::SignInWidget(QWidget *parent) :
    QWidget(parent)
{
    setObjectName(QStringLiteral("screen"));
    setAttribute(Qt::WA_StyledBackground, true);

    auto *card = new QFrame(this);
    card->setObjectName(QStringLiteral("card"));
    card->setFixedWidth(440);
    UiTheme::applyCardShadow(card);

    auto *title = new QLabel(tr("Sign in"), card);
    title->setObjectName(QStringLiteral("screenTitle"));

    auto *blurb = new QLabel(tr("Use your ARTC database credentials."), card);
    blurb->setObjectName(QStringLiteral("bodyText"));
    blurb->setWordWrap(true);

    usernameEdit = new QLineEdit(card);
    usernameEdit->setPlaceholderText(tr("Username"));

    passwordEdit = new QLineEdit(card);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText(tr("Password"));

    errorLabel = new QLabel(card);
    errorLabel->setObjectName(QStringLiteral("errorLabel"));
    errorLabel->setWordWrap(true);
    errorLabel->hide();

    signInButton = new QPushButton(tr("Sign In"), card);
    signInButton->setObjectName(QStringLiteral("primaryButton"));
    signInButton->setCursor(Qt::PointingHandCursor);
    signInButton->setDefault(true);

    auto *backButton = new QPushButton(tr("Back"), card);
    backButton->setObjectName(QStringLiteral("secondaryButton"));
    backButton->setCursor(Qt::PointingHandCursor);

    auto *registerLink = new QPushButton(tr("Need an account? Create one"), card);
    registerLink->setObjectName(QStringLiteral("linkButton"));
    registerLink->setCursor(Qt::PointingHandCursor);
    registerLink->setFlat(true);

    auto *actions = new QHBoxLayout;
    actions->setSpacing(12);
    actions->addWidget(backButton);
    actions->addStretch(1);
    actions->addWidget(signInButton);

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(40, 36, 40, 32);
    cardLayout->setSpacing(0);
    cardLayout->addWidget(title);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(blurb);
    cardLayout->addSpacing(26);
    cardLayout->addWidget(UiTheme::createField(tr("Username"), usernameEdit, card));
    cardLayout->addSpacing(16);
    cardLayout->addWidget(UiTheme::createField(tr("Password"), passwordEdit, card));
    cardLayout->addSpacing(16);
    cardLayout->addWidget(errorLabel);
    cardLayout->addSpacing(12);
    cardLayout->addLayout(actions);
    cardLayout->addSpacing(18);
    cardLayout->addWidget(UiTheme::createDivider(card));
    cardLayout->addSpacing(14);
    cardLayout->addWidget(registerLink, 0, Qt::AlignHCenter);

    auto *screenLayout = new QVBoxLayout(this);
    screenLayout->setContentsMargins(24, 24, 24, 24);
    screenLayout->addStretch(1);
    screenLayout->addWidget(card, 0, Qt::AlignHCenter);
    screenLayout->addStretch(1);

    connect(signInButton, &QPushButton::clicked, this, &SignInWidget::submit);
    connect(usernameEdit, &QLineEdit::returnPressed, this, &SignInWidget::submit);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &SignInWidget::submit);
    connect(backButton, &QPushButton::clicked, this, &SignInWidget::backRequested);
    connect(registerLink, &QPushButton::clicked, this, &SignInWidget::registerRequested);

    setTabOrder(usernameEdit, passwordEdit);
}

void SignInWidget::reset()
{
    passwordEdit->clear();
    errorLabel->clear();
    errorLabel->hide();
    setBusy(false);
    usernameEdit->setFocus();
}

void SignInWidget::showError(const QString &message)
{
    errorLabel->setText(message);
    errorLabel->show();
    setBusy(false);
}

void SignInWidget::setBusy(bool busy)
{
    usernameEdit->setEnabled(!busy);
    passwordEdit->setEnabled(!busy);
    signInButton->setEnabled(!busy);
    signInButton->setText(busy ? tr("Signing in…") : tr("Sign In"));
}

void SignInWidget::submit()
{
    const QString username = usernameEdit->text().trimmed();
    if (username.isEmpty()) {
        showError(tr("Enter a username."));
        usernameEdit->setFocus();
        return;
    }

    errorLabel->hide();
    setBusy(true);
    emit signInRequested(username, passwordEdit->text());
}
