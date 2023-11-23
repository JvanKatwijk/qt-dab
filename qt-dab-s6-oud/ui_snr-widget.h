/********************************************************************************
** Form generated from reading UI file 'snr-widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SNR_2D_WIDGET_H
#define UI_SNR_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qwt_plot.h"

QT_BEGIN_NAMESPACE

class Ui_snrWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSlider *snrSlider;
    QwtPlot *snrPlot;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *snrDumpButton;
    QSpinBox *snrCompressionSelector;
    QSpinBox *snrLengthSelector;

    void setupUi(QWidget *snrWidget)
    {
        if (snrWidget->objectName().isEmpty())
            snrWidget->setObjectName(QString::fromUtf8("snrWidget"));
        snrWidget->resize(398, 174);
        verticalLayout = new QVBoxLayout(snrWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        snrSlider = new QSlider(snrWidget);
        snrSlider->setObjectName(QString::fromUtf8("snrSlider"));
        snrSlider->setMaximumSize(QSize(16777215, 141));
        snrSlider->setOrientation(Qt::Vertical);

        horizontalLayout->addWidget(snrSlider);

        snrPlot = new QwtPlot(snrWidget);
        snrPlot->setObjectName(QString::fromUtf8("snrPlot"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(snrPlot->sizePolicy().hasHeightForWidth());
        snrPlot->setSizePolicy(sizePolicy);
        snrPlot->setMaximumSize(QSize(16777215, 141));

        horizontalLayout->addWidget(snrPlot);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        snrDumpButton = new QPushButton(snrWidget);
        snrDumpButton->setObjectName(QString::fromUtf8("snrDumpButton"));

        horizontalLayout_2->addWidget(snrDumpButton);

        snrCompressionSelector = new QSpinBox(snrWidget);
        snrCompressionSelector->setObjectName(QString::fromUtf8("snrCompressionSelector"));
        snrCompressionSelector->setMinimum(1);

        horizontalLayout_2->addWidget(snrCompressionSelector);

        snrLengthSelector = new QSpinBox(snrWidget);
        snrLengthSelector->setObjectName(QString::fromUtf8("snrLengthSelector"));
        snrLengthSelector->setMinimum(100);
        snrLengthSelector->setMaximum(500);

        horizontalLayout_2->addWidget(snrLengthSelector);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(snrWidget);

        QMetaObject::connectSlotsByName(snrWidget);
    } // setupUi

    void retranslateUi(QWidget *snrWidget)
    {
        snrWidget->setWindowTitle(QCoreApplication::translate("snrWidget", "snr widget", nullptr));
#if QT_CONFIG(tooltip)
        snrPlot->setToolTip(QCoreApplication::translate("snrWidget", "<html><head/><body><p>snr display. As well known a DAB frame starts with a so-called null period, a period where the amplitude of the transmitted signal is (should be) zero. So, it is an excellent noise floor, and the SNR shown is computed as 20 * log10 (average amplitude from data blocks / average amplitude null period). One thing though. in many SFN's the transmitters add an encoding og the transmitter id (tii) to each second DABframe. So for uneven compressions you see the difference on SNR between the first and second frames</p><p><br/></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        snrDumpButton->setText(QCoreApplication::translate("snrWidget", "dump", nullptr));
    } // retranslateUi

};

namespace Ui {
    class snrWidget: public Ui_snrWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SNR_2D_WIDGET_H
