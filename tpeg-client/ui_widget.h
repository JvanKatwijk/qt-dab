/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_widget
{
public:
    QPushButton *connectButton;
    QLineEdit *hostLineEdit;
    QLabel *state;
    QPushButton *terminateButton;

    void setupUi(QDialog *widget)
    {
        if (widget->objectName().isEmpty())
            widget->setObjectName(QString::fromUtf8("widget"));
        widget->resize(230, 195);
        connectButton = new QPushButton(widget);
        connectButton->setObjectName(QString::fromUtf8("connectButton"));
        connectButton->setGeometry(QRect(10, 10, 131, 21));
        hostLineEdit = new QLineEdit(widget);
        hostLineEdit->setObjectName(QString::fromUtf8("hostLineEdit"));
        hostLineEdit->setGeometry(QRect(10, 30, 131, 21));
        state = new QLabel(widget);
        state->setObjectName(QString::fromUtf8("state"));
        state->setGeometry(QRect(10, 130, 131, 31));
        terminateButton = new QPushButton(widget);
        terminateButton->setObjectName(QString::fromUtf8("terminateButton"));
        terminateButton->setGeometry(QRect(150, 10, 71, 31));

        retranslateUi(widget);

        QMetaObject::connectSlotsByName(widget);
    } // setupUi

    void retranslateUi(QDialog *widget)
    {
        widget->setWindowTitle(QCoreApplication::translate("widget", "Dialog", nullptr));
        connectButton->setText(QCoreApplication::translate("widget", "connect", nullptr));
        state->setText(QString());
        terminateButton->setText(QCoreApplication::translate("widget", "quit", nullptr));
    } // retranslateUi

};

namespace Ui {
    class widget: public Ui_widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
