#include "registerwidget.h"
#include "uitheme.h"

#include <QDateEdit>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QVBoxLayout>

namespace {

constexpr int CardWidth = 580;
constexpr int CardMargin = 40;
constexpr int ContentWidth = CardWidth - 2 * CardMargin;

/** Shortest password the form will accept. */
constexpr int MinimumPasswordLength = 8;

/** Earliest birth year offered, which also bounds the date editor. */
constexpr int EarliestBirthYear = 1900;

/** Shown in the field caption too, so the expected form is never a guess. */
const QString DateFormat = QStringLiteral("yyyy-MM-dd");

/**
 * @brief Deliberately permissive email check.
 *
 * Anything stricter rejects addresses that are in fact deliverable; the real
 * confirmation is a verification mail, once there is a backend to send one.
 */
bool looksLikeEmail(const QString &value)
{
    static const QRegularExpression pattern(QStringLiteral(R"(^[^@\s]+@[^@\s]+\.[^@\s]+$)"));
    return pattern.match(value).hasMatch();
}

} // namespace

RegisterWidget::RegisterWidget(QWidget *parent) :
    QWidget(parent)
{
    setObjectName(QStringLiteral("screen"));
    setAttribute(Qt::WA_StyledBackground, true);

    auto *card = new QFrame(this);
    card->setObjectName(QStringLiteral("card"));
    card->setFixedWidth(CardWidth);
    UiTheme::applyCardShadow(card);

    auto *title = new QLabel(tr("Create your account"), card);
    title->setObjectName(QStringLiteral("screenTitle"));

    auto *blurb = new WrappingLabel(tr("Enter the name as it appears on your birth "
                                       "certificate — it becomes the root of your "
                                       "pedigree chart."), ContentWidth, card);
    blurb->setObjectName(QStringLiteral("bodyText"));


    firstNameEdit = new QLineEdit(card);
    firstNameEdit->setPlaceholderText(tr("Given name"));

    middleNameEdit = new QLineEdit(card);
    middleNameEdit->setPlaceholderText(tr("Optional"));

    lastNameEdit = new QLineEdit(card);
    lastNameEdit->setPlaceholderText(tr("Family name"));

    birthdateEdit = new QDateEdit(card);
    birthdateEdit->setDisplayFormat(DateFormat);
    birthdateEdit->setCalendarPopup(true);
    birthdateEdit->setDateRange(QDate(EarliestBirthYear, 1, 1), QDate::currentDate());
    birthdateEdit->setDate(QDate(1990, 1, 1));

    emailEdit = new QLineEdit(card);
    emailEdit->setPlaceholderText(tr("you@example.com"));

    passwordEdit = new QLineEdit(card);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText(tr("At least %1 characters").arg(MinimumPasswordLength));

    confirmEdit = new QLineEdit(card);
    confirmEdit->setEchoMode(QLineEdit::Password);
    confirmEdit->setPlaceholderText(tr("Repeat password"));

    auto *form = new QGridLayout;
    form->setHorizontalSpacing(16);
    form->setVerticalSpacing(16);
    form->addWidget(UiTheme::createField(tr("First name"), firstNameEdit, card), 0, 0);
    form->addWidget(UiTheme::createField(tr("Middle name"), middleNameEdit, card), 0, 1);
    form->addWidget(UiTheme::createField(tr("Last name"), lastNameEdit, card), 1, 0);
    form->addWidget(UiTheme::createField(tr("Date of birth (%1)").arg(DateFormat.toLower()),
                                        birthdateEdit, card), 1, 1);
    form->addWidget(UiTheme::createField(tr("Email"), emailEdit, card), 2, 0, 1, 2);
    form->addWidget(UiTheme::createField(tr("Password"), passwordEdit, card), 3, 0);
    form->addWidget(UiTheme::createField(tr("Confirm password"), confirmEdit, card), 3, 1);
    form->setColumnStretch(0, 1);
    form->setColumnStretch(1, 1);

    errorLabel = new WrappingLabel(QString(), ContentWidth, card);
    errorLabel->setObjectName(QStringLiteral("errorLabel"));
    errorLabel->hide();

    auto *createButton = new QPushButton(tr("Create Account"), card);
    createButton->setObjectName(QStringLiteral("primaryButton"));
    createButton->setCursor(Qt::PointingHandCursor);
    createButton->setDefault(true);

    auto *backButton = new QPushButton(tr("Back"), card);
    backButton->setObjectName(QStringLiteral("secondaryButton"));
    backButton->setCursor(Qt::PointingHandCursor);

    auto *signInLink = new QPushButton(tr("Already have an account? Sign in"), card);
    signInLink->setObjectName(QStringLiteral("linkButton"));
    signInLink->setCursor(Qt::PointingHandCursor);
    signInLink->setFlat(true);

    auto *actions = new QHBoxLayout;
    actions->setSpacing(12);
    actions->addWidget(backButton);
    actions->addStretch(1);
    actions->addWidget(createButton);

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(CardMargin, 36, CardMargin, 32);
    cardLayout->setSpacing(0);
    cardLayout->addWidget(title);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(blurb);
    cardLayout->addSpacing(24);
    cardLayout->addLayout(form);
    cardLayout->addSpacing(16);
    cardLayout->addWidget(errorLabel);
    cardLayout->addSpacing(12);
    cardLayout->addLayout(actions);
    cardLayout->addSpacing(18);
    cardLayout->addWidget(UiTheme::createDivider(card));
    cardLayout->addSpacing(14);
    cardLayout->addWidget(signInLink, 0, Qt::AlignHCenter);

    auto *screenLayout = new QVBoxLayout(this);
    screenLayout->setContentsMargins(24, 24, 24, 24);
    screenLayout->addStretch(1);
    screenLayout->addWidget(card, 0, Qt::AlignHCenter);
    screenLayout->addStretch(1);

    connect(createButton, &QPushButton::clicked, this, &RegisterWidget::submit);
    connect(confirmEdit, &QLineEdit::returnPressed, this, &RegisterWidget::submit);
    connect(backButton, &QPushButton::clicked, this, &RegisterWidget::backRequested);
    connect(signInLink, &QPushButton::clicked, this, &RegisterWidget::signInRequested);

    setTabOrder(firstNameEdit, middleNameEdit);
    setTabOrder(middleNameEdit, lastNameEdit);
    setTabOrder(lastNameEdit, birthdateEdit);
    setTabOrder(birthdateEdit, emailEdit);
    setTabOrder(emailEdit, passwordEdit);
    setTabOrder(passwordEdit, confirmEdit);
}

void RegisterWidget::reset()
{
    const QList<QLineEdit *> editors = {firstNameEdit, middleNameEdit, lastNameEdit,
                                        emailEdit, passwordEdit, confirmEdit};
    for (QLineEdit *editor : editors) {
        editor->clear();
    }
    birthdateEdit->setDate(QDate(1990, 1, 1));
    clearErrors();
    firstNameEdit->setFocus();
}

void RegisterWidget::showError(const QString &message)
{
    errorLabel->setText(message);
    errorLabel->show();
}

void RegisterWidget::submit()
{
    RegistrationDetails details;
    if (!validate(details)) {
        return;
    }

    clearErrors();
    emit registrationSubmitted(details);
}

bool RegisterWidget::validate(RegistrationDetails &details)
{
    clearErrors();

    const QString firstName = firstNameEdit->text().trimmed();
    const QString lastName = lastNameEdit->text().trimmed();
    const QString email = emailEdit->text().trimmed();
    const QString password = passwordEdit->text();

    if (firstName.isEmpty()) {
        failField(firstNameEdit, tr("Enter a first name."));
        return false;
    }

    if (lastName.isEmpty()) {
        failField(lastNameEdit, tr("Enter a last name."));
        return false;
    }

    if (!looksLikeEmail(email)) {
        failField(emailEdit, tr("Enter a valid email address."));
        return false;
    }

    if (password.length() < MinimumPasswordLength) {
        failField(passwordEdit, tr("Use a password of at least %1 characters.")
                                    .arg(MinimumPasswordLength));
        return false;
    }

    if (password != confirmEdit->text()) {
        failField(confirmEdit, tr("The passwords do not match."));
        return false;
    }

    details.firstName = firstName;
    details.middleName = middleNameEdit->text().trimmed();
    details.lastName = lastName;
    details.birthdate = birthdateEdit->date();
    details.email = email;
    details.password = password;

    return true;
}

void RegisterWidget::clearErrors()
{
    errorLabel->clear();
    errorLabel->hide();

    const QList<QWidget *> editors = {firstNameEdit, middleNameEdit, lastNameEdit,
                                      birthdateEdit, emailEdit, passwordEdit, confirmEdit};
    for (QWidget *editor : editors) {
        if (editor->property("invalid").toBool()) {
            editor->setProperty("invalid", false);
            UiTheme::refreshStyle(editor);
        }
    }
}

void RegisterWidget::failField(QWidget *editor, const QString &message)
{
    editor->setProperty("invalid", true);
    UiTheme::refreshStyle(editor);
    editor->setFocus();
    showError(message);
}
