/********************************************************************************
** Form generated from reading UI file 'rtlsdr-widget.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RTLSDR_2D_WIDGET_H
#define UI_RTLSDR_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_dabstickWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label_4;
    QSpinBox *ppm_correction;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *gainControl;
    QLabel *label_5;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *agcControl;
    QCheckBox *filterSelector;
    QHBoxLayout *horizontalLayout_4;
    QCheckBox *biasControl;
    QSpinBox *filterDepth;
    QPushButton *xml_dumpButton;
    QPushButton *iq_dumpButton;
    QLabel *overflowLabel;
    QLabel *product_display;
    QLabel *deviceVersion;

    void setupUi(QWidget *dabstickWidget)
    {
        if (dabstickWidget->objectName().isEmpty())
            dabstickWidget->setObjectName("dabstickWidget");
        dabstickWidget->resize(242, 345);
        verticalLayout = new QVBoxLayout(dabstickWidget);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label_4 = new QLabel(dabstickWidget);
        label_4->setObjectName("label_4");

        horizontalLayout->addWidget(label_4);

        ppm_correction = new QSpinBox(dabstickWidget);
        ppm_correction->setObjectName("ppm_correction");
        ppm_correction->setMinimum(-500);
        ppm_correction->setMaximum(500);

        horizontalLayout->addWidget(ppm_correction);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        gainControl = new QComboBox(dabstickWidget);
        gainControl->setObjectName("gainControl");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(gainControl->sizePolicy().hasHeightForWidth());
        gainControl->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(gainControl);


        verticalLayout->addLayout(horizontalLayout_2);

        label_5 = new QLabel(dabstickWidget);
        label_5->setObjectName("label_5");

        verticalLayout->addWidget(label_5);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        agcControl = new QCheckBox(dabstickWidget);
        agcControl->setObjectName("agcControl");

        horizontalLayout_3->addWidget(agcControl);

        filterSelector = new QCheckBox(dabstickWidget);
        filterSelector->setObjectName("filterSelector");

        horizontalLayout_3->addWidget(filterSelector);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        biasControl = new QCheckBox(dabstickWidget);
        biasControl->setObjectName("biasControl");

        horizontalLayout_4->addWidget(biasControl);

        filterDepth = new QSpinBox(dabstickWidget);
        filterDepth->setObjectName("filterDepth");
        filterDepth->setMinimum(5);
        filterDepth->setMaximum(25);

        horizontalLayout_4->addWidget(filterDepth);


        verticalLayout->addLayout(horizontalLayout_4);

        xml_dumpButton = new QPushButton(dabstickWidget);
        xml_dumpButton->setObjectName("xml_dumpButton");

        verticalLayout->addWidget(xml_dumpButton);

        iq_dumpButton = new QPushButton(dabstickWidget);
        iq_dumpButton->setObjectName("iq_dumpButton");

        verticalLayout->addWidget(iq_dumpButton);

        overflowLabel = new QLabel(dabstickWidget);
        overflowLabel->setObjectName("overflowLabel");

        verticalLayout->addWidget(overflowLabel);

        product_display = new QLabel(dabstickWidget);
        product_display->setObjectName("product_display");
        product_display->setFrameShape(QFrame::Box);

        verticalLayout->addWidget(product_display);

        deviceVersion = new QLabel(dabstickWidget);
        deviceVersion->setObjectName("deviceVersion");
        deviceVersion->setFrameShape(QFrame::Box);

        verticalLayout->addWidget(deviceVersion);


        retranslateUi(dabstickWidget);

        QMetaObject::connectSlotsByName(dabstickWidget);
    } // setupUi

    void retranslateUi(QWidget *dabstickWidget)
    {
        dabstickWidget->setWindowTitle(QCoreApplication::translate("dabstickWidget", "RT2832 dabstick", nullptr));
        label_4->setText(QCoreApplication::translate("dabstickWidget", "ppm", nullptr));
#if QT_CONFIG(tooltip)
        gainControl->setToolTip(QCoreApplication::translate("dabstickWidget", "<html><head/><body><p>Set gain in dB, only possible values are displayed.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_5->setText(QCoreApplication::translate("dabstickWidget", "gain", nullptr));
        agcControl->setText(QCoreApplication::translate("dabstickWidget", "autogain", nullptr));
#if QT_CONFIG(tooltip)
        filterSelector->setToolTip(QCoreApplication::translate("dabstickWidget", "<html><head/><body><p>DABsticks do not have much filtering. A software filter - 1560KHz wide - can be selected</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        filterSelector->setText(QCoreApplication::translate("dabstickWidget", "filter", nullptr));
        biasControl->setText(QCoreApplication::translate("dabstickWidget", "biasT", nullptr));
#if QT_CONFIG(tooltip)
        filterDepth->setToolTip(QCoreApplication::translate("dabstickWidget", "<html><head/><body><p>select the depth of the software 1560 KHz filter. Be aware that for a filter depth N N* 2048000 extra additions and complex multiplications are done</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        xml_dumpButton->setToolTip(QCoreApplication::translate("dabstickWidget", "<html><head/><body><p>Dumps the raw input from the rtlsdr dongle into a self describing file (with header in xml format containing recorded time, container format, frequency, device name, Qt-DAB version)</p><p><br/></p><p>Opens a 'Save as ...' dialog. Press again to stop recording.</p><p><br/></p><p>Very experimental! The resulting file can be read-in by using the xml file handler (if configured)</p><p><br/></p><p>Warning: Produces large files!</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        xml_dumpButton->setText(QCoreApplication::translate("dabstickWidget", "dump to xml file", nullptr));
        iq_dumpButton->setText(QCoreApplication::translate("dabstickWidget", "dump to raw file", nullptr));
        overflowLabel->setText(QString());
        product_display->setText(QString());
#if QT_CONFIG(tooltip)
        deviceVersion->setToolTip(QCoreApplication::translate("dabstickWidget", "<html><head/><body><p>Device name</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        deviceVersion->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class dabstickWidget: public Ui_dabstickWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RTLSDR_2D_WIDGET_H
