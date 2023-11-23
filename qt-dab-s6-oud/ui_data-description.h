/********************************************************************************
** Form generated from reading UI file 'data-description.ui'
**
** Created by: Qt User Interface Compiler version 5.15.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DATA_2D_DESCRIPTION_H
#define UI_DATA_2D_DESCRIPTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_dataDescription
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_5;
    QHBoxLayout *horizontalLayout;
    QLabel *serviceName;
    QLabel *appType;
    QWidget *detailGroup;
    QFormLayout *formLayout;
    QLabel *label_9;
    QLabel *serviceLabel;
    QLabel *label_2;
    QLabel *subChannelId;
    QLabel *label_3;
    QLabel *startAddress;
    QLabel *label_4;
    QLabel *Length;
    QLabel *label;
    QLabel *bitrate;
    QLabel *label_6;
    QLabel *protectionLevel;
    QLabel *label_8;
    QLabel *packetAddress;
    QLabel *label_7;
    QLabel *FECscheme;

    void setupUi(QWidget *dataDescription)
    {
        if (dataDescription->objectName().isEmpty())
            dataDescription->setObjectName(QString::fromUtf8("dataDescription"));
        dataDescription->resize(297, 443);
        verticalLayout = new QVBoxLayout(dataDescription);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_5 = new QLabel(dataDescription);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        QFont font;
        font.setFamily(QString::fromUtf8("DejaVu Sans Mono"));
        font.setPointSize(11);
        font.setBold(true);
        font.setItalic(true);
        font.setWeight(75);
        label_5->setFont(font);

        verticalLayout->addWidget(label_5);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        serviceName = new QLabel(dataDescription);
        serviceName->setObjectName(QString::fromUtf8("serviceName"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("DejaVu Sans Mono"));
        font1.setPointSize(10);
        font1.setBold(true);
        font1.setItalic(true);
        font1.setWeight(75);
        serviceName->setFont(font1);

        horizontalLayout->addWidget(serviceName);

        appType = new QLabel(dataDescription);
        appType->setObjectName(QString::fromUtf8("appType"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("DejaVu Sans Mono"));
        font2.setPointSize(10);
        font2.setBold(true);
        font2.setWeight(75);
        appType->setFont(font2);

        horizontalLayout->addWidget(appType);


        verticalLayout->addLayout(horizontalLayout);

        detailGroup = new QWidget(dataDescription);
        detailGroup->setObjectName(QString::fromUtf8("detailGroup"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(8);
        sizePolicy.setHeightForWidth(detailGroup->sizePolicy().hasHeightForWidth());
        detailGroup->setSizePolicy(sizePolicy);
        formLayout = new QFormLayout(detailGroup);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_9 = new QLabel(detailGroup);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_9);

        serviceLabel = new QLabel(detailGroup);
        serviceLabel->setObjectName(QString::fromUtf8("serviceLabel"));

        formLayout->setWidget(0, QFormLayout::FieldRole, serviceLabel);

        label_2 = new QLabel(detailGroup);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        subChannelId = new QLabel(detailGroup);
        subChannelId->setObjectName(QString::fromUtf8("subChannelId"));

        formLayout->setWidget(1, QFormLayout::FieldRole, subChannelId);

        label_3 = new QLabel(detailGroup);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        startAddress = new QLabel(detailGroup);
        startAddress->setObjectName(QString::fromUtf8("startAddress"));

        formLayout->setWidget(2, QFormLayout::FieldRole, startAddress);

        label_4 = new QLabel(detailGroup);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_4);

        Length = new QLabel(detailGroup);
        Length->setObjectName(QString::fromUtf8("Length"));

        formLayout->setWidget(3, QFormLayout::FieldRole, Length);

        label = new QLabel(detailGroup);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label);

        bitrate = new QLabel(detailGroup);
        bitrate->setObjectName(QString::fromUtf8("bitrate"));

        formLayout->setWidget(4, QFormLayout::FieldRole, bitrate);

        label_6 = new QLabel(detailGroup);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_6);

        protectionLevel = new QLabel(detailGroup);
        protectionLevel->setObjectName(QString::fromUtf8("protectionLevel"));

        formLayout->setWidget(5, QFormLayout::FieldRole, protectionLevel);

        label_8 = new QLabel(detailGroup);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_8);

        packetAddress = new QLabel(detailGroup);
        packetAddress->setObjectName(QString::fromUtf8("packetAddress"));

        formLayout->setWidget(6, QFormLayout::FieldRole, packetAddress);

        label_7 = new QLabel(detailGroup);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout->setWidget(7, QFormLayout::LabelRole, label_7);

        FECscheme = new QLabel(detailGroup);
        FECscheme->setObjectName(QString::fromUtf8("FECscheme"));

        formLayout->setWidget(7, QFormLayout::FieldRole, FECscheme);


        verticalLayout->addWidget(detailGroup);


        retranslateUi(dataDescription);

        QMetaObject::connectSlotsByName(dataDescription);
    } // setupUi

    void retranslateUi(QWidget *dataDescription)
    {
        dataDescription->setWindowTitle(QCoreApplication::translate("dataDescription", "data description", nullptr));
        label_5->setText(QCoreApplication::translate("dataDescription", "data service", nullptr));
        serviceName->setText(QCoreApplication::translate("dataDescription", "TextLabel", nullptr));
        appType->setText(QCoreApplication::translate("dataDescription", "TextLabel", nullptr));
        label_9->setText(QCoreApplication::translate("dataDescription", "serviceId", nullptr));
        serviceLabel->setText(QCoreApplication::translate("dataDescription", "TextLabel", nullptr));
        label_2->setText(QCoreApplication::translate("dataDescription", "subchannel Id", nullptr));
        subChannelId->setText(QCoreApplication::translate("dataDescription", "TextLabel", nullptr));
        label_3->setText(QCoreApplication::translate("dataDescription", "Start Address", nullptr));
        startAddress->setText(QCoreApplication::translate("dataDescription", "TextLabel", nullptr));
        label_4->setText(QCoreApplication::translate("dataDescription", "Length", nullptr));
        Length->setText(QCoreApplication::translate("dataDescription", "TextLabel", nullptr));
        label->setText(QCoreApplication::translate("dataDescription", "bitRate", nullptr));
        bitrate->setText(QCoreApplication::translate("dataDescription", "TextLabel", nullptr));
        label_6->setText(QCoreApplication::translate("dataDescription", "Protection level", nullptr));
        protectionLevel->setText(QCoreApplication::translate("dataDescription", "TextLabel", nullptr));
        label_8->setText(QCoreApplication::translate("dataDescription", "packet address", nullptr));
        packetAddress->setText(QCoreApplication::translate("dataDescription", "TextLabel", nullptr));
        label_7->setText(QCoreApplication::translate("dataDescription", "FEC scheme", nullptr));
        FECscheme->setText(QCoreApplication::translate("dataDescription", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class dataDescription: public Ui_dataDescription {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DATA_2D_DESCRIPTION_H
