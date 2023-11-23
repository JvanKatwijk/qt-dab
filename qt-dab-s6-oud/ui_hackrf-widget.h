/********************************************************************************
** Form generated from reading UI file 'hackrf-widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HACKRF_2D_WIDGET_H
#define UI_HACKRF_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_hackrfWidget
{
public:
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QCheckBox *biasT_button;
    QCheckBox *AmpEnableButton;
    QLabel *label;
    QSpinBox *ppm_correction;
    QLabel *usb_board_id_display;
    QLabel *serialNumber;
    QLabel *label_4;
    QLabel *serial_number_display;
    QPushButton *dumpButton;
    QVBoxLayout *verticalLayout_2;
    QLCDNumber *vgagainDisplay;
    QSlider *vgaGainSlider;
    QLabel *label_2;
    QVBoxLayout *verticalLayout_3;
    QLCDNumber *lnagainDisplay;
    QSlider *lnaGainSlider;
    QLabel *label_3;

    void setupUi(QWidget *hackrfWidget)
    {
        if (hackrfWidget->objectName().isEmpty())
            hackrfWidget->setObjectName(QString::fromUtf8("hackrfWidget"));
        hackrfWidget->resize(424, 285);
        horizontalLayout = new QHBoxLayout(hackrfWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        biasT_button = new QCheckBox(hackrfWidget);
        biasT_button->setObjectName(QString::fromUtf8("biasT_button"));
        biasT_button->setChecked(false);

        verticalLayout->addWidget(biasT_button);

        AmpEnableButton = new QCheckBox(hackrfWidget);
        AmpEnableButton->setObjectName(QString::fromUtf8("AmpEnableButton"));

        verticalLayout->addWidget(AmpEnableButton);

        label = new QLabel(hackrfWidget);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        ppm_correction = new QSpinBox(hackrfWidget);
        ppm_correction->setObjectName(QString::fromUtf8("ppm_correction"));
        ppm_correction->setMinimum(-100);
        ppm_correction->setMaximum(100);

        verticalLayout->addWidget(ppm_correction);

        usb_board_id_display = new QLabel(hackrfWidget);
        usb_board_id_display->setObjectName(QString::fromUtf8("usb_board_id_display"));

        verticalLayout->addWidget(usb_board_id_display);

        serialNumber = new QLabel(hackrfWidget);
        serialNumber->setObjectName(QString::fromUtf8("serialNumber"));

        verticalLayout->addWidget(serialNumber);

        label_4 = new QLabel(hackrfWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout->addWidget(label_4);

        serial_number_display = new QLabel(hackrfWidget);
        serial_number_display->setObjectName(QString::fromUtf8("serial_number_display"));
        QFont font;
        font.setPointSize(11);
        serial_number_display->setFont(font);

        verticalLayout->addWidget(serial_number_display);

        dumpButton = new QPushButton(hackrfWidget);
        dumpButton->setObjectName(QString::fromUtf8("dumpButton"));

        verticalLayout->addWidget(dumpButton);


        horizontalLayout->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        vgagainDisplay = new QLCDNumber(hackrfWidget);
        vgagainDisplay->setObjectName(QString::fromUtf8("vgagainDisplay"));
        vgagainDisplay->setSegmentStyle(QLCDNumber::Flat);

        verticalLayout_2->addWidget(vgagainDisplay);

        vgaGainSlider = new QSlider(hackrfWidget);
        vgaGainSlider->setObjectName(QString::fromUtf8("vgaGainSlider"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(vgaGainSlider->sizePolicy().hasHeightForWidth());
        vgaGainSlider->setSizePolicy(sizePolicy);
        vgaGainSlider->setMaximum(62);
        vgaGainSlider->setOrientation(Qt::Vertical);

        verticalLayout_2->addWidget(vgaGainSlider);

        label_2 = new QLabel(hackrfWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label_2);


        horizontalLayout->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        lnagainDisplay = new QLCDNumber(hackrfWidget);
        lnagainDisplay->setObjectName(QString::fromUtf8("lnagainDisplay"));
        lnagainDisplay->setDigitCount(3);
        lnagainDisplay->setSegmentStyle(QLCDNumber::Flat);

        verticalLayout_3->addWidget(lnagainDisplay);

        lnaGainSlider = new QSlider(hackrfWidget);
        lnaGainSlider->setObjectName(QString::fromUtf8("lnaGainSlider"));
        sizePolicy.setHeightForWidth(lnaGainSlider->sizePolicy().hasHeightForWidth());
        lnaGainSlider->setSizePolicy(sizePolicy);
        lnaGainSlider->setMaximum(40);
        lnaGainSlider->setOrientation(Qt::Vertical);

        verticalLayout_3->addWidget(lnaGainSlider);

        label_3 = new QLabel(hackrfWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label_3);


        horizontalLayout->addLayout(verticalLayout_3);


        retranslateUi(hackrfWidget);

        QMetaObject::connectSlotsByName(hackrfWidget);
    } // setupUi

    void retranslateUi(QWidget *hackrfWidget)
    {
        hackrfWidget->setWindowTitle(QCoreApplication::translate("hackrfWidget", "HACKRF control", nullptr));
        biasT_button->setText(QCoreApplication::translate("hackrfWidget", "biasT", nullptr));
        AmpEnableButton->setText(QCoreApplication::translate("hackrfWidget", "Amp Enable", nullptr));
        label->setText(QCoreApplication::translate("hackrfWidget", "ppm Correction", nullptr));
        usb_board_id_display->setText(QString());
        serialNumber->setText(QString());
        label_4->setText(QCoreApplication::translate("hackrfWidget", "Serial Number", nullptr));
        serial_number_display->setText(QString());
#if QT_CONFIG(tooltip)
        dumpButton->setToolTip(QCoreApplication::translate("hackrfWidget", "<html><head/><body><p>Dumps the raw input from the HackRF into a self describing file (with header in xml format containing recorded time, container format, frequency, device name, Qt-DAB version)</p><p><br/></p><p>Opens a 'Save as ...' dialog. Press again to stop recording.</p><p><br/></p><p>Very experimental! The resulting file can be read-in by using the xml file handler (if configured)</p><p><br/></p><p>Warning: Produces large files!</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        dumpButton->setText(QCoreApplication::translate("hackrfWidget", "Dump", nullptr));
        label_2->setText(QCoreApplication::translate("hackrfWidget", "vga", nullptr));
        label_3->setText(QCoreApplication::translate("hackrfWidget", "lna", nullptr));
    } // retranslateUi

};

namespace Ui {
    class hackrfWidget: public Ui_hackrfWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HACKRF_2D_WIDGET_H
