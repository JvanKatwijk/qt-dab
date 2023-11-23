/********************************************************************************
** Form generated from reading UI file 'rtlsdr-widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.11
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
    QSpinBox *ppm_selector;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *gain_selector;
    QLabel *label_5;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *autogain_selector;
    QCheckBox *filter_selector;
    QHBoxLayout *horizontalLayout_4;
    QCheckBox *biasT_selector;
    QSpinBox *filterDepth_selector;
    QPushButton *xml_dump_selector;
    QPushButton *iq_dump_selector;
    QLabel *label;
    QLabel *product_display;
    QLabel *deviceVersion;

    void setupUi(QWidget *dabstickWidget)
    {
        if (dabstickWidget->objectName().isEmpty())
            dabstickWidget->setObjectName(QString::fromUtf8("dabstickWidget"));
        dabstickWidget->resize(242, 370);
        verticalLayout = new QVBoxLayout(dabstickWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_4 = new QLabel(dabstickWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout->addWidget(label_4);

        ppm_selector = new QSpinBox(dabstickWidget);
        ppm_selector->setObjectName(QString::fromUtf8("ppm_selector"));
        ppm_selector->setMinimum(-500);
        ppm_selector->setMaximum(500);

        horizontalLayout->addWidget(ppm_selector);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        gain_selector = new QComboBox(dabstickWidget);
        gain_selector->setObjectName(QString::fromUtf8("gain_selector"));
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(gain_selector->sizePolicy().hasHeightForWidth());
        gain_selector->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(gain_selector);


        verticalLayout->addLayout(horizontalLayout_2);

        label_5 = new QLabel(dabstickWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        verticalLayout->addWidget(label_5);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        autogain_selector = new QCheckBox(dabstickWidget);
        autogain_selector->setObjectName(QString::fromUtf8("autogain_selector"));

        horizontalLayout_3->addWidget(autogain_selector);

        filter_selector = new QCheckBox(dabstickWidget);
        filter_selector->setObjectName(QString::fromUtf8("filter_selector"));

        horizontalLayout_3->addWidget(filter_selector);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        biasT_selector = new QCheckBox(dabstickWidget);
        biasT_selector->setObjectName(QString::fromUtf8("biasT_selector"));

        horizontalLayout_4->addWidget(biasT_selector);

        filterDepth_selector = new QSpinBox(dabstickWidget);
        filterDepth_selector->setObjectName(QString::fromUtf8("filterDepth_selector"));
        filterDepth_selector->setMinimum(5);
        filterDepth_selector->setMaximum(25);

        horizontalLayout_4->addWidget(filterDepth_selector);


        verticalLayout->addLayout(horizontalLayout_4);

        xml_dump_selector = new QPushButton(dabstickWidget);
        xml_dump_selector->setObjectName(QString::fromUtf8("xml_dump_selector"));

        verticalLayout->addWidget(xml_dump_selector);

        iq_dump_selector = new QPushButton(dabstickWidget);
        iq_dump_selector->setObjectName(QString::fromUtf8("iq_dump_selector"));

        verticalLayout->addWidget(iq_dump_selector);

        label = new QLabel(dabstickWidget);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        product_display = new QLabel(dabstickWidget);
        product_display->setObjectName(QString::fromUtf8("product_display"));
        product_display->setFrameShape(QFrame::Box);

        verticalLayout->addWidget(product_display);

        deviceVersion = new QLabel(dabstickWidget);
        deviceVersion->setObjectName(QString::fromUtf8("deviceVersion"));
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
        gain_selector->setToolTip(QCoreApplication::translate("dabstickWidget", "<html><head/><body><p>Set gain in dB, only possible values are displayed.</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        label_5->setText(QCoreApplication::translate("dabstickWidget", "gain", nullptr));
        autogain_selector->setText(QCoreApplication::translate("dabstickWidget", "autogain", nullptr));
#if QT_CONFIG(tooltip)
        filter_selector->setToolTip(QCoreApplication::translate("dabstickWidget", "<html><head/><body><p>DABsticks do not have much filtering. A software filter - 1560KHz wide - can be selected</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        filter_selector->setText(QCoreApplication::translate("dabstickWidget", "filter", nullptr));
        biasT_selector->setText(QCoreApplication::translate("dabstickWidget", "biasT", nullptr));
#if QT_CONFIG(tooltip)
        filterDepth_selector->setToolTip(QCoreApplication::translate("dabstickWidget", "<html><head/><body><p>select the depth of the software 1560 KHz filter. Be aware that for a filter depth N N* 2048000 extra additions and complex multiplications are done</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        xml_dump_selector->setToolTip(QCoreApplication::translate("dabstickWidget", "<html><head/><body><p>Dumps the raw input from the rtlsdr dongle into a self describing file (with header in xml format containing recorded time, container format, frequency, device name, Qt-DAB version)</p><p><br/></p><p>Opens a 'Save as ...' dialog. Press again to stop recording.</p><p><br/></p><p>Very experimental! The resulting file can be read-in by using the xml file handler (if configured)</p><p><br/></p><p>Warning: Produces large files!</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        xml_dump_selector->setText(QCoreApplication::translate("dabstickWidget", "dump to xml file", nullptr));
        iq_dump_selector->setText(QCoreApplication::translate("dabstickWidget", "dump to raw file", nullptr));
        label->setText(QCoreApplication::translate("dabstickWidget", "dabstick", nullptr));
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
