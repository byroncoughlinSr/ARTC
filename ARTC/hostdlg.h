#ifndef HOSTDLG_H
#define HOSTDLG_H

#include <QDialog>
#include "person.h"

class QDateEdit;
class QLabel;
class QLineEdit;
class QRadioButton;

/**
 * @brief Collects the root person of a pedigree — the host.
 *
 * Laid out in code rather than from a .ui file. The original form positioned
 * every widget at fixed pixel coordinates, so the application stylesheet's
 * padding pushed the date editor's content outside its 24px box and clipped
 * the labels. Real layouts size each widget from its own content instead.
 */
class Hostdlg : public QDialog
{
    Q_OBJECT

public:
    explicit Hostdlg(QWidget *parent = nullptr);

    /** @brief The details entered, valid once the dialog was accepted. */
    Person::Individual getHost() const;

private slots:
    /** @brief Validate the form and, if it passes, accept the dialog. */
    void submit();

private:
    QLineEdit *firstNameEdit;
    QLineEdit *middleNameEdit;
    QLineEdit *lastNameEdit;
    QDateEdit *birthdateEdit;
    QRadioButton *maleButton;
    QRadioButton *femaleButton;
    QLabel *errorLabel;
    Person::Individual host;
};

#endif // HOSTDLG_H
