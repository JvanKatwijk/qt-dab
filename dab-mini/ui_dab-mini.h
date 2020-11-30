/********************************************************************************
** Form generated from reading UI file 'dab-mini.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DAB_2D_MINI_H
#define UI_DAB_2D_MINI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "presetcombobox.h"
#include "smallcombobox.h"
#include "smallpushbutton.h"
#include "smallqlistview.h"
#include "smallspinbox.h"

QT_BEGIN_NAMESPACE

class Ui_dab_mini
{
public:
    smallComboBox *streamoutSelector;
    QLabel *dynamicLabel;
    presetComboBox *presetSelector;
    smallSpinBox *lnaSelector;
    QCheckBox *agcControl;
    QLabel *timeDisplay;
    smallSpinBox *gainSelector;
    smallComboBox *channelSelector;
    smallQListView *ensembleDisplay;
    QLabel *serviceLabel;
    QLabel *ensembleId;
    smallPushButton *prevchannelButton;
    smallPushButton *prev_serviceButton;
    smallPushButton *next_serviceButton;
    smallPushButton *nextchannelButton;
    QLabel *copyrightLabel;
    QLabel *ficLabel;
    QPushButton *muteButton;
    QLabel *stereoLabel;

    void setupUi(QWidget *dab_mini)
    {
        if (dab_mini->objectName().isEmpty())
            dab_mini->setObjectName(QString::fromUtf8("dab_mini"));
        dab_mini->resize(314, 357);
        streamoutSelector = new smallComboBox(dab_mini);
        streamoutSelector->setObjectName(QString::fromUtf8("streamoutSelector"));
        streamoutSelector->setGeometry(QRect(180, 210, 126, 37));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(streamoutSelector->sizePolicy().hasHeightForWidth());
        streamoutSelector->setSizePolicy(sizePolicy);
        dynamicLabel = new QLabel(dab_mini);
        dynamicLabel->setObjectName(QString::fromUtf8("dynamicLabel"));
        dynamicLabel->setGeometry(QRect(10, 260, 301, 41));
        presetSelector = new presetComboBox(dab_mini);
        presetSelector->addItem(QString());
        presetSelector->setObjectName(QString::fromUtf8("presetSelector"));
        presetSelector->setGeometry(QRect(10, 310, 151, 37));
        sizePolicy.setHeightForWidth(presetSelector->sizePolicy().hasHeightForWidth());
        presetSelector->setSizePolicy(sizePolicy);
        lnaSelector = new smallSpinBox(dab_mini);
        lnaSelector->setObjectName(QString::fromUtf8("lnaSelector"));
        lnaSelector->setGeometry(QRect(180, 80, 121, 31));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lnaSelector->sizePolicy().hasHeightForWidth());
        lnaSelector->setSizePolicy(sizePolicy1);
        agcControl = new QCheckBox(dab_mini);
        agcControl->setObjectName(QString::fromUtf8("agcControl"));
        agcControl->setGeometry(QRect(210, 10, 52, 25));
        timeDisplay = new QLabel(dab_mini);
        timeDisplay->setObjectName(QString::fromUtf8("timeDisplay"));
        timeDisplay->setGeometry(QRect(180, 240, 131, 20));
        gainSelector = new smallSpinBox(dab_mini);
        gainSelector->setObjectName(QString::fromUtf8("gainSelector"));
        gainSelector->setGeometry(QRect(180, 40, 121, 31));
        sizePolicy1.setHeightForWidth(gainSelector->sizePolicy().hasHeightForWidth());
        gainSelector->setSizePolicy(sizePolicy1);
        channelSelector = new smallComboBox(dab_mini);
        channelSelector->setObjectName(QString::fromUtf8("channelSelector"));
        channelSelector->setGeometry(QRect(180, 120, 121, 31));
        sizePolicy.setHeightForWidth(channelSelector->sizePolicy().hasHeightForWidth());
        channelSelector->setSizePolicy(sizePolicy);
        ensembleDisplay = new smallQListView(dab_mini);
        ensembleDisplay->setObjectName(QString::fromUtf8("ensembleDisplay"));
        ensembleDisplay->setGeometry(QRect(10, 73, 159, 171));
        sizePolicy1.setHeightForWidth(ensembleDisplay->sizePolicy().hasHeightForWidth());
        ensembleDisplay->setSizePolicy(sizePolicy1);
        serviceLabel = new QLabel(dab_mini);
        serviceLabel->setObjectName(QString::fromUtf8("serviceLabel"));
        serviceLabel->setGeometry(QRect(10, 40, 159, 31));
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        font.setItalic(true);
        font.setWeight(75);
        serviceLabel->setFont(font);
        ensembleId = new QLabel(dab_mini);
        ensembleId->setObjectName(QString::fromUtf8("ensembleId"));
        ensembleId->setGeometry(QRect(20, 10, 151, 19));
        QFont font1;
        font1.setPointSize(12);
        font1.setBold(true);
        font1.setItalic(true);
        font1.setWeight(75);
        ensembleId->setFont(font1);
        prevchannelButton = new smallPushButton(dab_mini);
        prevchannelButton->setObjectName(QString::fromUtf8("prevchannelButton"));
        prevchannelButton->setGeometry(QRect(180, 150, 51, 27));
        prev_serviceButton = new smallPushButton(dab_mini);
        prev_serviceButton->setObjectName(QString::fromUtf8("prev_serviceButton"));
        prev_serviceButton->setGeometry(QRect(180, 180, 54, 26));
        next_serviceButton = new smallPushButton(dab_mini);
        next_serviceButton->setObjectName(QString::fromUtf8("next_serviceButton"));
        next_serviceButton->setGeometry(QRect(250, 180, 51, 26));
        nextchannelButton = new smallPushButton(dab_mini);
        nextchannelButton->setObjectName(QString::fromUtf8("nextchannelButton"));
        nextchannelButton->setGeometry(QRect(250, 150, 51, 27));
        copyrightLabel = new QLabel(dab_mini);
        copyrightLabel->setObjectName(QString::fromUtf8("copyrightLabel"));
        copyrightLabel->setGeometry(QRect(280, 10, 31, 20));
        ficLabel = new QLabel(dab_mini);
        ficLabel->setObjectName(QString::fromUtf8("ficLabel"));
        ficLabel->setGeometry(QRect(186, 10, 21, 20));
        muteButton = new QPushButton(dab_mini);
        muteButton->setObjectName(QString::fromUtf8("muteButton"));
        muteButton->setGeometry(QRect(230, 310, 71, 36));
        stereoLabel = new QLabel(dab_mini);
        stereoLabel->setObjectName(QString::fromUtf8("stereoLabel"));
        stereoLabel->setGeometry(QRect(160, 320, 67, 21));

        retranslateUi(dab_mini);

        QMetaObject::connectSlotsByName(dab_mini);
    } // setupUi

    void retranslateUi(QWidget *dab_mini)
    {
        dab_mini->setWindowTitle(QCoreApplication::translate("dab_mini", "dabMini", nullptr));
        dynamicLabel->setText(QString());
        presetSelector->setItemText(0, QCoreApplication::translate("dab_mini", "Presets", nullptr));

#if QT_CONFIG(tooltip)
        lnaSelector->setToolTip(QCoreApplication::translate("dab_mini", "LNA state for the sdrplay, the range of values depends on the model of the RSP", nullptr));
#endif // QT_CONFIG(tooltip)
        agcControl->setText(QCoreApplication::translate("dab_mini", "agc", nullptr));
        timeDisplay->setText(QString());
#if QT_CONFIG(tooltip)
        gainSelector->setToolTip(QCoreApplication::translate("dab_mini", "gain(reduction).\n"
"For the SDRplay: it is gain reduction in the range 20 .. 59\n"
"For the RTLSDR is is gain, range depending on the device", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        channelSelector->setToolTip(QCoreApplication::translate("dab_mini", "Channel selector. ", nullptr));
#endif // QT_CONFIG(tooltip)
        serviceLabel->setText(QString());
        ensembleId->setText(QString());
#if QT_CONFIG(tooltip)
        prevchannelButton->setToolTip(QCoreApplication::translate("dab_mini", "Select the previous channel.", nullptr));
#endif // QT_CONFIG(tooltip)
        prevchannelButton->setText(QCoreApplication::translate("dab_mini", "<", nullptr));
#if QT_CONFIG(tooltip)
        prev_serviceButton->setToolTip(QCoreApplication::translate("dab_mini", "Select the previous service in the displayed list of services.", nullptr));
#endif // QT_CONFIG(tooltip)
        prev_serviceButton->setText(QCoreApplication::translate("dab_mini", "<", nullptr));
#if QT_CONFIG(tooltip)
        next_serviceButton->setToolTip(QCoreApplication::translate("dab_mini", "Select the next service in the list of displayed services.", nullptr));
#endif // QT_CONFIG(tooltip)
        next_serviceButton->setText(QCoreApplication::translate("dab_mini", ">", nullptr));
#if QT_CONFIG(tooltip)
        nextchannelButton->setToolTip(QCoreApplication::translate("dab_mini", "Select the next channel.", nullptr));
#endif // QT_CONFIG(tooltip)
        nextchannelButton->setText(QCoreApplication::translate("dab_mini", ">", nullptr));
        copyrightLabel->setText(QCoreApplication::translate("dab_mini", "\302\251", nullptr));
        ficLabel->setText(QCoreApplication::translate("dab_mini", "FIC", nullptr));
        muteButton->setText(QCoreApplication::translate("dab_mini", "mute", nullptr));
        stereoLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class dab_mini: public Ui_dab_mini {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DAB_2D_MINI_H
