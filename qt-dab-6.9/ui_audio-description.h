/********************************************************************************
** Form generated from reading UI file 'audio-description.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUDIO_2D_DESCRIPTION_H
#define UI_AUDIO_2D_DESCRIPTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_audioDescription
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_5;
    QHBoxLayout *horizontalLayout;
    QLabel *serviceName;
    QLabel *dabType;
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
    QLabel *label_7;
    QLabel *programType;
    QLabel *label_8;
    QLabel *Language;
    QLabel *fmLabel;
    QLabel *fmFrequency;

    void setupUi(QWidget *audioDescription)
    {
        if (audioDescription->objectName().isEmpty())
            audioDescription->setObjectName("audioDescription");
        audioDescription->resize(462, 440);
        verticalLayout = new QVBoxLayout(audioDescription);
        verticalLayout->setObjectName("verticalLayout");
        label_5 = new QLabel(audioDescription);
        label_5->setObjectName("label_5");
        QFont font;
        font.setFamilies({QString::fromUtf8("DejaVu Sans Mono")});
        font.setPointSize(11);
        font.setBold(true);
        font.setItalic(true);
        label_5->setFont(font);

        verticalLayout->addWidget(label_5);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        serviceName = new QLabel(audioDescription);
        serviceName->setObjectName("serviceName");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("DejaVu Sans Mono")});
        font1.setPointSize(10);
        font1.setBold(true);
        font1.setItalic(true);
        serviceName->setFont(font1);

        horizontalLayout->addWidget(serviceName);

        dabType = new QLabel(audioDescription);
        dabType->setObjectName("dabType");
        QFont font2;
        font2.setFamilies({QString::fromUtf8("DejaVu Sans Mono")});
        font2.setPointSize(10);
        font2.setBold(true);
        dabType->setFont(font2);

        horizontalLayout->addWidget(dabType);


        verticalLayout->addLayout(horizontalLayout);

        detailGroup = new QWidget(audioDescription);
        detailGroup->setObjectName("detailGroup");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(8);
        sizePolicy.setHeightForWidth(detailGroup->sizePolicy().hasHeightForWidth());
        detailGroup->setSizePolicy(sizePolicy);
        formLayout = new QFormLayout(detailGroup);
        formLayout->setObjectName("formLayout");
        label_9 = new QLabel(detailGroup);
        label_9->setObjectName("label_9");

        formLayout->setWidget(0, QFormLayout::LabelRole, label_9);

        serviceLabel = new QLabel(detailGroup);
        serviceLabel->setObjectName("serviceLabel");

        formLayout->setWidget(0, QFormLayout::FieldRole, serviceLabel);

        label_2 = new QLabel(detailGroup);
        label_2->setObjectName("label_2");

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        subChannelId = new QLabel(detailGroup);
        subChannelId->setObjectName("subChannelId");

        formLayout->setWidget(1, QFormLayout::FieldRole, subChannelId);

        label_3 = new QLabel(detailGroup);
        label_3->setObjectName("label_3");

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        startAddress = new QLabel(detailGroup);
        startAddress->setObjectName("startAddress");

        formLayout->setWidget(2, QFormLayout::FieldRole, startAddress);

        label_4 = new QLabel(detailGroup);
        label_4->setObjectName("label_4");

        formLayout->setWidget(3, QFormLayout::LabelRole, label_4);

        Length = new QLabel(detailGroup);
        Length->setObjectName("Length");

        formLayout->setWidget(3, QFormLayout::FieldRole, Length);

        label = new QLabel(detailGroup);
        label->setObjectName("label");

        formLayout->setWidget(4, QFormLayout::LabelRole, label);

        bitrate = new QLabel(detailGroup);
        bitrate->setObjectName("bitrate");

        formLayout->setWidget(4, QFormLayout::FieldRole, bitrate);

        label_6 = new QLabel(detailGroup);
        label_6->setObjectName("label_6");

        formLayout->setWidget(5, QFormLayout::LabelRole, label_6);

        protectionLevel = new QLabel(detailGroup);
        protectionLevel->setObjectName("protectionLevel");

        formLayout->setWidget(5, QFormLayout::FieldRole, protectionLevel);

        label_7 = new QLabel(detailGroup);
        label_7->setObjectName("label_7");

        formLayout->setWidget(6, QFormLayout::LabelRole, label_7);

        programType = new QLabel(detailGroup);
        programType->setObjectName("programType");

        formLayout->setWidget(6, QFormLayout::FieldRole, programType);

        label_8 = new QLabel(detailGroup);
        label_8->setObjectName("label_8");

        formLayout->setWidget(7, QFormLayout::LabelRole, label_8);

        Language = new QLabel(detailGroup);
        Language->setObjectName("Language");

        formLayout->setWidget(7, QFormLayout::FieldRole, Language);

        fmLabel = new QLabel(detailGroup);
        fmLabel->setObjectName("fmLabel");

        formLayout->setWidget(8, QFormLayout::LabelRole, fmLabel);

        fmFrequency = new QLabel(detailGroup);
        fmFrequency->setObjectName("fmFrequency");

        formLayout->setWidget(8, QFormLayout::FieldRole, fmFrequency);


        verticalLayout->addWidget(detailGroup);


        retranslateUi(audioDescription);

        QMetaObject::connectSlotsByName(audioDescription);
    } // setupUi

    void retranslateUi(QWidget *audioDescription)
    {
        audioDescription->setWindowTitle(QCoreApplication::translate("audioDescription", "audio description", nullptr));
        label_5->setText(QCoreApplication::translate("audioDescription", "audio service", nullptr));
        serviceName->setText(QCoreApplication::translate("audioDescription", "service name", nullptr));
        dabType->setText(QCoreApplication::translate("audioDescription", "DAB type", nullptr));
        label_9->setText(QCoreApplication::translate("audioDescription", "serviceId", nullptr));
        serviceLabel->setText(QCoreApplication::translate("audioDescription", "TextLabel", nullptr));
        label_2->setText(QCoreApplication::translate("audioDescription", "subchannel Id", nullptr));
        subChannelId->setText(QCoreApplication::translate("audioDescription", "TextLabel", nullptr));
        label_3->setText(QCoreApplication::translate("audioDescription", "Start Address", nullptr));
        startAddress->setText(QCoreApplication::translate("audioDescription", "TextLabel", nullptr));
        label_4->setText(QCoreApplication::translate("audioDescription", "Length", nullptr));
        Length->setText(QCoreApplication::translate("audioDescription", "TextLabel", nullptr));
        label->setText(QCoreApplication::translate("audioDescription", "bitRate", nullptr));
        bitrate->setText(QCoreApplication::translate("audioDescription", "TextLabel", nullptr));
        label_6->setText(QCoreApplication::translate("audioDescription", "Protection level", nullptr));
        protectionLevel->setText(QCoreApplication::translate("audioDescription", "TextLabel", nullptr));
        label_7->setText(QCoreApplication::translate("audioDescription", "ProgramType", nullptr));
        programType->setText(QCoreApplication::translate("audioDescription", "TextLabel", nullptr));
        label_8->setText(QCoreApplication::translate("audioDescription", "Language", nullptr));
        Language->setText(QCoreApplication::translate("audioDescription", "TextLabel", nullptr));
        fmLabel->setText(QCoreApplication::translate("audioDescription", "alternative FM", nullptr));
        fmFrequency->setText(QCoreApplication::translate("audioDescription", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class audioDescription: public Ui_audioDescription {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUDIO_2D_DESCRIPTION_H
