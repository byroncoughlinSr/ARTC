#include "hostdlg.h"
#include "uitheme.h"

#include <QDateEdit>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

namespace {

/** Width the wrapped text is laid out at, inside the dialog margins. */
constexpr int ContentWidth = 500;

/** Bounds the date editor, matching the registration screen. */
constexpr int EarliestBirthYear = 1900;

} // namespace

Hostdlg::Hostdlg(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("New Host"));
    setObjectName(QStringLiteral("screen"));

    auto *title = new QLabel(tr("New host"), this);
    title->setObjectName(QStringLiteral("screenTitle"));

    auto *blurb = new WrappingLabel(tr("The host is the root of the pedigree — the person "
                                       "every DNA match is related to. Use the name as it "
                                       "appears on their birth certificate."),
                                    ContentWidth, this);
    blurb->setObjectName(QStringLiteral("bodyText"));

    firstNameEdit = new QLineEdit(this);
    firstNameEdit->setPlaceholderText(tr("Given name"));

    middleNameEdit = new QLineEdit(this);
    middleNameEdit->setPlaceholderText(tr("Optional"));

    lastNameEdit = new QLineEdit(this);
    lastNameEdit->setPlaceholderText(tr("Family name"));

    birthdateEdit = new QDateEdit(this);
    birthdateEdit->setDisplayFormat(QStringLiteral("yyyy-MM-dd"));
    birthdateEdit->setCalendarPopup(true);
    birthdateEdit->setDateRange(QDate(EarliestBirthYear, 1, 1), QDate::currentDate());
    birthdateEdit->setDate(QDate(1956, 1, 1));

    maleButton = new QRadioButton(tr("&Male"), this);
    femaleButton = new QRadioButton(tr("&Female"), this);
    maleButton->setChecked(true);

    auto *sexGroup = new QWidget(this);
    auto *sexLayout = new QHBoxLayout(sexGroup);
    sexLayout->setContentsMargins(0, 0, 0, 0);
    sexLayout->setSpacing(18);
    sexLayout->addWidget(maleButton);
    sexLayout->addWidget(femaleButton);
    sexLayout->addStretch(1);

    auto *form = new QGridLayout;
    form->setHorizontalSpacing(16);
    form->setVerticalSpacing(16);
    form->addWidget(UiTheme::createField(tr("First name"), firstNameEdit, this), 0, 0);
    form->addWidget(UiTheme::createField(tr("Middle name"), middleNameEdit, this), 0, 1);
    form->addWidget(UiTheme::createField(tr("Last name"), lastNameEdit, this), 1, 0);
    form->addWidget(UiTheme::createField(tr("Birth date"), birthdateEdit, this), 1, 1);
    form->addWidget(UiTheme::createField(tr("Sex"), sexGroup, this), 2, 0, 1, 2);
    form->setColumnStretch(0, 1);
    form->setColumnStretch(1, 1);

    errorLabel = new WrappingLabel(QString(), ContentWidth, this);
    errorLabel->setObjectName(QStringLiteral("errorLabel"));
    errorLabel->hide();

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                         this);
    // The object name selects the stylesheet rule, and the widget has already
    // been polished by the time QDialogButtonBox hands it over, so the style
    // has to be recomputed for the new name to take effect.
    QPushButton *okButton = buttons->button(QDialogButtonBox::Ok);
    okButton->setObjectName(QStringLiteral("primaryButton"));
    okButton->setText(tr("Create Host"));
    okButton->setCursor(Qt::PointingHandCursor);
    UiTheme::refreshStyle(okButton);

    QPushButton *cancelButton = buttons->button(QDialogButtonBox::Cancel);
    cancelButton->setObjectName(QStringLiteral("secondaryButton"));
    cancelButton->setCursor(Qt::PointingHandCursor);
    UiTheme::refreshStyle(cancelButton);

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

    connect(buttons, &QDialogButtonBox::accepted, this, &Hostdlg::submit);
    connect(buttons, &QDialogButtonBox::rejected, this, &Hostdlg::reject);

    setTabOrder(firstNameEdit, middleNameEdit);
    setTabOrder(middleNameEdit, lastNameEdit);
    setTabOrder(lastNameEdit, birthdateEdit);
}

/**
 * @brief Hostdlg::submit
 */
void Hostdlg::submit()
{
    const QString firstName = firstNameEdit->text().trimmed();
    const QString lastName = lastNameEdit->text().trimmed();

    // Creating a host writes the person and then generates hundreds of
    // placeholder ancestors, so an empty name must not get that far.
    if (firstName.isEmpty() || lastName.isEmpty()) {
        errorLabel->setText(tr("Enter both a first and a last name."));
        errorLabel->show();
        (firstName.isEmpty() ? firstNameEdit : lastNameEdit)->setFocus();
        return;
    }

    host.firstName = firstName;
    host.middleName = middleNameEdit->text().trimmed();
    host.lastName = lastName;
    host.birthdate = birthdateEdit->date();
    host.sex = maleButton->isChecked() ? 'M' : 'F';

    accept();
}

/**
 * @brief Hostdlg::getHost
 * @return
 */
Person::Individual Hostdlg::getHost() const
{
    return host;
}
