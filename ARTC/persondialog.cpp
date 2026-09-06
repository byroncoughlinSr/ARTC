#include "persondialog.h"
#include "familytree.h"
#include "uitheme.h"

#include <QCheckBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

namespace {

constexpr int ContentWidth = 470;
constexpr int EarliestYear = 1500;

/** Where a date editor starts when the user first enables it. */
const QDate DefaultDate(1900, 1, 1);
const QString DateFormat = QStringLiteral("yyyy-MM-dd");

/** Stack an "Unknown" checkbox under a date editor. */
QWidget *dateField(QDateEdit *edit, QCheckBox *unknown, QWidget *parent)
{
    auto *group = new QWidget(parent);
    auto *layout = new QVBoxLayout(group);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);
    layout->addWidget(edit);
    layout->addWidget(unknown);
    return group;
}

} // namespace

PersonDialog::PersonDialog(const TreeNode *node, const QString &relationship,
                           const QString &hostName, QWidget *parent) :
    QDialog(parent)
{
    const bool occupied = node && !node->isPlaceholder();

    setWindowTitle(occupied ? tr("Edit person") : tr("Add person"));
    setObjectName(QStringLiteral("screen"));

    auto *title = new QLabel(occupied ? tr("Edit %1").arg(relationship.toLower())
                                      : tr("Add %1").arg(relationship.toLower()), this);
    title->setObjectName(QStringLiteral("screenTitle"));

    const QString subject = hostName.isEmpty() ? tr("the host") : hostName;
    auto *blurb = new WrappingLabel(
        occupied ? tr("This slot is the %1 of %2.").arg(relationship.toLower(), subject)
                 : tr("This slot is the %1 of %2, and is still empty. "
                      "Fill it in if you already know who belongs here; a 23andMe "
                      "match can be assigned to it later.")
                       .arg(relationship.toLower(), subject),
        ContentWidth, this);
    blurb->setObjectName(QStringLiteral("bodyText"));

    firstNameEdit = new QLineEdit(this);
    firstNameEdit->setPlaceholderText(tr("Given name"));
    middleNameEdit = new QLineEdit(this);
    middleNameEdit->setPlaceholderText(tr("Optional"));
    lastNameEdit = new QLineEdit(this);
    lastNameEdit->setPlaceholderText(tr("Family name"));

    // A QDateEdit whose special value text is showing hides its sections, so
    // there is nothing to click into and type — which made an unknown date
    // impossible to fill in. An explicit checkbox keeps the editor usable.
    birthdateEdit = new QDateEdit(this);
    birthdateEdit->setDisplayFormat(DateFormat);
    birthdateEdit->setCalendarPopup(true);
    birthdateEdit->setDateRange(QDate(EarliestYear, 1, 1), QDate::currentDate());
    birthdateEdit->setDate(DefaultDate);
    birthUnknown = new QCheckBox(tr("Unknown"), this);
    birthUnknown->setToolTip(tr("Leave ticked if the birth date is not known. "
                                "Typing a date clears it."));

    deathdateEdit = new QDateEdit(this);
    deathdateEdit->setDisplayFormat(DateFormat);
    deathdateEdit->setCalendarPopup(true);
    deathdateEdit->setDateRange(QDate(EarliestYear, 1, 1), QDate::currentDate());
    deathdateEdit->setDate(DefaultDate);
    deathUnknown = new QCheckBox(tr("Unknown or still living"), this);
    deathUnknown->setToolTip(tr("Leave ticked if there is no death date. "
                                "Typing a date clears it."));

    if (occupied) {
        firstNameEdit->setText(node->firstName);
        middleNameEdit->setText(node->middleName);
        lastNameEdit->setText(node->lastName);
    }

    const bool haveBirth = occupied && node->birthdate.isValid()
                           && node->birthdate.year() >= EarliestYear;
    const bool haveDeath = occupied && node->deathdate.isValid()
                           && node->deathdate.year() >= EarliestYear;
    if (haveBirth) {
        birthdateEdit->setDate(node->birthdate);
    }
    if (haveDeath) {
        deathdateEdit->setDate(node->deathdate);
    }
    birthUnknown->setChecked(!haveBirth);
    deathUnknown->setChecked(!haveDeath);

    // The editors stay enabled whether or not the date is known. Disabling them
    // while "Unknown" was ticked — the default for an empty slot — meant
    // clicking the date field did nothing at all until the checkbox was found
    // and cleared, which read as the field being broken. Typing a date is the
    // primary action, so it works immediately and clears "Unknown" itself.
    connect(birthdateEdit, &QDateEdit::dateChanged, this, [this] {
        birthUnknown->setChecked(false);
    });
    connect(deathdateEdit, &QDateEdit::dateChanged, this, [this] {
        deathUnknown->setChecked(false);
    });

    auto *form = new QGridLayout;
    form->setHorizontalSpacing(16);
    form->setVerticalSpacing(16);
    form->addWidget(UiTheme::createField(tr("First name"), firstNameEdit, this), 0, 0);
    form->addWidget(UiTheme::createField(tr("Middle name"), middleNameEdit, this), 0, 1);
    form->addWidget(UiTheme::createField(tr("Last name"), lastNameEdit, this), 1, 0);
    form->addWidget(UiTheme::createField(tr("Birth date (%1)").arg(DateFormat.toLower()),
                                         dateField(birthdateEdit, birthUnknown, this)), 1, 1);
    form->addWidget(UiTheme::createField(tr("Death date (%1)").arg(DateFormat.toLower()),
                                         dateField(deathdateEdit, deathUnknown, this)), 2, 1);
    form->setColumnStretch(0, 1);
    form->setColumnStretch(1, 1);

    errorLabel = new WrappingLabel(QString(), ContentWidth, this);
    errorLabel->setObjectName(QStringLiteral("errorLabel"));
    errorLabel->hide();

    auto *buttons = new QDialogButtonBox(this);
    QPushButton *saveButton = buttons->addButton(occupied ? tr("Save") : tr("Add Person"),
                                                 QDialogButtonBox::AcceptRole);
    saveButton->setObjectName(QStringLiteral("primaryButton"));
    saveButton->setCursor(Qt::PointingHandCursor);
    UiTheme::refreshStyle(saveButton);

    QPushButton *cancelButton = buttons->addButton(QDialogButtonBox::Cancel);
    cancelButton->setObjectName(QStringLiteral("secondaryButton"));
    cancelButton->setCursor(Qt::PointingHandCursor);
    UiTheme::refreshStyle(cancelButton);

    if (occupied) {
        QPushButton *removeButton = buttons->addButton(tr("Remove Person"),
                                                       QDialogButtonBox::DestructiveRole);
        removeButton->setObjectName(QStringLiteral("secondaryButton"));
        removeButton->setCursor(Qt::PointingHandCursor);
        UiTheme::refreshStyle(removeButton);
        connect(removeButton, &QPushButton::clicked, this, &PersonDialog::remove);
    }

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 36, 40, 32);
    layout->setSpacing(0);
    layout->addWidget(title);
    layout->addSpacing(8);
    layout->addWidget(blurb);
    layout->addSpacing(24);
    layout->addLayout(form);
    layout->addSpacing(16);
    layout->addWidget(errorLabel);
    layout->addSpacing(12);
    layout->addWidget(buttons);

    connect(saveButton, &QPushButton::clicked, this, &PersonDialog::save);
    connect(buttons, &QDialogButtonBox::rejected, this, &PersonDialog::reject);

    setTabOrder(firstNameEdit, middleNameEdit);
    setTabOrder(middleNameEdit, lastNameEdit);
    setTabOrder(lastNameEdit, birthdateEdit);
    firstNameEdit->setFocus();
}

void PersonDialog::save()
{
    const QString firstName = firstNameEdit->text().trimmed();
    const QString lastName = lastNameEdit->text().trimmed();

    if (firstName.isEmpty() || lastName.isEmpty()) {
        errorLabel->setText(tr("Enter both a first and a last name."));
        errorLabel->show();
        (firstName.isEmpty() ? firstNameEdit : lastNameEdit)->setFocus();
        return;
    }

    const QDate born = birthUnknown->isChecked() ? QDate() : birthdateEdit->date();
    const QDate died = deathUnknown->isChecked() ? QDate() : deathdateEdit->date();

    if (born.isValid() && died.isValid() && died < born) {
        errorLabel->setText(tr("The death date is before the birth date."));
        errorLabel->show();
        deathdateEdit->setFocus();
        return;
    }

    entered.firstName = firstName;
    entered.middleName = middleNameEdit->text().trimmed();
    entered.lastName = lastName;
    entered.birthdate = born;
    entered.deathdate = died;
    removing = false;
    accept();
}

void PersonDialog::remove()
{
    const QString name = QStringLiteral("%1 %2").arg(firstNameEdit->text().trimmed(),
                                                     lastNameEdit->text().trimmed()).trimmed();
    const int answer = QMessageBox::question(
        this, tr("Remove person"),
        tr("Remove %1 from this slot?\n\nThe slot itself stays, along with every "
           "ancestor above it — only the person's details are cleared.")
            .arg(name.isEmpty() ? tr("this person") : name),
        QMessageBox::Cancel | QMessageBox::Yes, QMessageBox::Cancel);

    if (answer != QMessageBox::Yes) {
        return;
    }

    removing = true;
    accept();
}
