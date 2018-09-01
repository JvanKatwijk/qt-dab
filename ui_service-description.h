/********************************************************************************
** Form generated from reading UI file 'service-description.ui'
**
** Created by: Qt User Interface Compiler version 5.9.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVICE_2D_DESCRIPTION_H
#define UI_SERVICE_2D_DESCRIPTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_serviceDescription
{
public:
    QLabel *serviceName;
    QLabel *subChannelId;
    QLabel *startAddress;
    QLabel *Length;
    QLabel *label;
    QLabel *protectionLevel;
    QLabel *programType;
    QLabel *Language;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *bitrate;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *dabType;

    void setupUi(QWidget *serviceDescription)
    {
        if (serviceDescription->objectName().isEmpty())
            serviceDescription->setObjectName(QStringLiteral("serviceDescription"));
        serviceDescription->resize(331, 276);
        serviceName = new QLabel(serviceDescription);
        serviceName->setObjectName(QStringLiteral("serviceName"));
        serviceName->setGeometry(QRect(30, 20, 151, 20));
        QFont font;
        font.setFamily(QStringLiteral("DejaVu Sans Mono"));
        font.setPointSize(10);
        font.setBold(true);
        font.setItalic(true);
        font.setWeight(75);
        serviceName->setFont(font);
        subChannelId = new QLabel(serviceDescription);
        subChannelId->setObjectName(QStringLiteral("subChannelId"));
        subChannelId->setGeometry(QRect(140, 50, 111, 20));
        startAddress = new QLabel(serviceDescription);
        startAddress->setObjectName(QStringLiteral("startAddress"));
        startAddress->setGeometry(QRect(140, 80, 171, 20));
        Length = new QLabel(serviceDescription);
        Length->setObjectName(QStringLiteral("Length"));
        Length->setGeometry(QRect(140, 110, 171, 20));
        label = new QLabel(serviceDescription);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 140, 101, 20));
        protectionLevel = new QLabel(serviceDescription);
        protectionLevel->setObjectName(QStringLiteral("protectionLevel"));
        protectionLevel->setGeometry(QRect(140, 170, 161, 20));
        programType = new QLabel(serviceDescription);
        programType->setObjectName(QStringLiteral("programType"));
        programType->setGeometry(QRect(140, 200, 161, 20));
        Language = new QLabel(serviceDescription);
        Language->setObjectName(QStringLiteral("Language"));
        Language->setGeometry(QRect(140, 230, 181, 20));
        label_2 = new QLabel(serviceDescription);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 50, 111, 21));
        label_3 = new QLabel(serviceDescription);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 80, 91, 21));
        label_4 = new QLabel(serviceDescription);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(20, 110, 81, 21));
        bitrate = new QLabel(serviceDescription);
        bitrate->setObjectName(QStringLiteral("bitrate"));
        bitrate->setGeometry(QRect(140, 140, 59, 15));
        label_6 = new QLabel(serviceDescription);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(20, 170, 101, 21));
        label_7 = new QLabel(serviceDescription);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(20, 200, 111, 21));
        label_8 = new QLabel(serviceDescription);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(20, 230, 91, 21));
        dabType = new QLabel(serviceDescription);
        dabType->setObjectName(QStringLiteral("dabType"));
        dabType->setGeometry(QRect(220, 20, 81, 21));
        QFont font1;
        font1.setFamily(QStringLiteral("DejaVu Sans Mono"));
        font1.setPointSize(10);
        font1.setBold(true);
        font1.setWeight(75);
        dabType->setFont(font1);

        retranslateUi(serviceDescription);

        QMetaObject::connectSlotsByName(serviceDescription);
    } // setupUi

    void retranslateUi(QWidget *serviceDescription)
    {
        serviceDescription->setWindowTitle(QApplication::translate("serviceDescription", "service description", Q_NULLPTR));
        serviceName->setText(QApplication::translate("serviceDescription", "TextLabel", Q_NULLPTR));
        subChannelId->setText(QApplication::translate("serviceDescription", "TextLabel", Q_NULLPTR));
        startAddress->setText(QApplication::translate("serviceDescription", "TextLabel", Q_NULLPTR));
        Length->setText(QApplication::translate("serviceDescription", "TextLabel", Q_NULLPTR));
        label->setText(QApplication::translate("serviceDescription", "bitRate", Q_NULLPTR));
        protectionLevel->setText(QApplication::translate("serviceDescription", "TextLabel", Q_NULLPTR));
        programType->setText(QApplication::translate("serviceDescription", "TextLabel", Q_NULLPTR));
        Language->setText(QApplication::translate("serviceDescription", "TextLabel", Q_NULLPTR));
        label_2->setText(QApplication::translate("serviceDescription", "subchannel Id", Q_NULLPTR));
        label_3->setText(QApplication::translate("serviceDescription", "Start Address", Q_NULLPTR));
        label_4->setText(QApplication::translate("serviceDescription", "Length", Q_NULLPTR));
        bitrate->setText(QApplication::translate("serviceDescription", "TextLabel", Q_NULLPTR));
        label_6->setText(QApplication::translate("serviceDescription", "Protection level", Q_NULLPTR));
        label_7->setText(QApplication::translate("serviceDescription", "ProgramType", Q_NULLPTR));
        label_8->setText(QApplication::translate("serviceDescription", "Language", Q_NULLPTR));
        dabType->setText(QApplication::translate("serviceDescription", "TextLabel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class serviceDescription: public Ui_serviceDescription {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVICE_2D_DESCRIPTION_H
