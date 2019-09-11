/********************************************************************************
** Form generated from reading UI file 'hostdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HOSTDLG_H
#define UI_HOSTDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_Hostdlg
{
public:
    QDialogButtonBox *buttonBox;
    QRadioButton *maleBtn;
    QRadioButton *femaleBtn;
    QTextEdit *firstNameTxt;
    QTextEdit *middleNameTxt;
    QTextEdit *lastNameTxt;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QDateEdit *dateEdit;
    QLabel *label_4;

    void setupUi(QDialog *Hostdlg)
    {
        if (Hostdlg->objectName().isEmpty())
            Hostdlg->setObjectName(QStringLiteral("Hostdlg"));
        Hostdlg->resize(696, 386);
        buttonBox = new QDialogButtonBox(Hostdlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(110, 300, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        maleBtn = new QRadioButton(Hostdlg);
        maleBtn->setObjectName(QStringLiteral("maleBtn"));
        maleBtn->setGeometry(QRect(500, 50, 100, 21));
        femaleBtn = new QRadioButton(Hostdlg);
        femaleBtn->setObjectName(QStringLiteral("femaleBtn"));
        femaleBtn->setGeometry(QRect(500, 90, 100, 21));
        firstNameTxt = new QTextEdit(Hostdlg);
        firstNameTxt->setObjectName(QStringLiteral("firstNameTxt"));
        firstNameTxt->setGeometry(QRect(150, 40, 241, 31));
        middleNameTxt = new QTextEdit(Hostdlg);
        middleNameTxt->setObjectName(QStringLiteral("middleNameTxt"));
        middleNameTxt->setGeometry(QRect(150, 100, 241, 31));
        lastNameTxt = new QTextEdit(Hostdlg);
        lastNameTxt->setObjectName(QStringLiteral("lastNameTxt"));
        lastNameTxt->setGeometry(QRect(150, 160, 241, 31));
        label = new QLabel(Hostdlg);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 50, 81, 16));
        label_2 = new QLabel(Hostdlg);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 110, 91, 16));
        label_3 = new QLabel(Hostdlg);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 170, 81, 16));
        dateEdit = new QDateEdit(Hostdlg);
        dateEdit->setObjectName(QStringLiteral("dateEdit"));
        dateEdit->setGeometry(QRect(150, 240, 110, 24));
        dateEdit->setDateTime(QDateTime(QDate(1956, 1, 1), QTime(0, 0, 0)));
        label_4 = new QLabel(Hostdlg);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 250, 59, 15));

        retranslateUi(Hostdlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), Hostdlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Hostdlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(Hostdlg);
    } // setupUi

    void retranslateUi(QDialog *Hostdlg)
    {
        Hostdlg->setWindowTitle(QApplication::translate("Hostdlg", "Dialog", Q_NULLPTR));
        maleBtn->setText(QApplication::translate("Hostdlg", "&Male", Q_NULLPTR));
        femaleBtn->setText(QApplication::translate("Hostdlg", "Female", Q_NULLPTR));
        label->setText(QApplication::translate("Hostdlg", "First name", Q_NULLPTR));
        label_2->setText(QApplication::translate("Hostdlg", "Middle name", Q_NULLPTR));
        label_3->setText(QApplication::translate("Hostdlg", "Last name", Q_NULLPTR));
        dateEdit->setDisplayFormat(QApplication::translate("Hostdlg", "M/d/yyyy", Q_NULLPTR));
        label_4->setText(QApplication::translate("Hostdlg", "Birth date", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Hostdlg: public Ui_Hostdlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HOSTDLG_H
