/********************************************************************************
** Form generated from reading UI file 'AudioDriverDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUDIODRIVERDIALOG_H
#define UI_AUDIODRIVERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_DriverDialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QComboBox *comboBox;
    QLabel *label;

    void setupUi(QDialog *DriverDialog)
    {
        if (DriverDialog->objectName().isEmpty())
            DriverDialog->setObjectName(QStringLiteral("DriverDialog"));
        DriverDialog->resize(492, 127);
        buttonBox = new QDialogButtonBox(DriverDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(140, 90, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBox = new QGroupBox(DriverDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 20, 471, 61));
        comboBox = new QComboBox(groupBox);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(110, 20, 341, 21));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 20, 111, 21));

        retranslateUi(DriverDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), DriverDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DriverDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(DriverDialog);
    } // setupUi

    void retranslateUi(QDialog *DriverDialog)
    {
        DriverDialog->setWindowTitle(QApplication::translate("DriverDialog", "Dialog", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("DriverDialog", "Driver", Q_NULLPTR));
        label->setText(QApplication::translate("DriverDialog", "Audio input driver:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class DriverDialog: public Ui_DriverDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUDIODRIVERDIALOG_H
