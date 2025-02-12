/********************************************************************************
** Form generated from reading UI file 'airspy-widget.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AIRSPY_2D_WIDGET_H
#define UI_AIRSPY_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_airspyWidget
{
public:
    QTabWidget *tabWidget;
    QWidget *tab_1;
    QSlider *sensitivitySlider;
    QLCDNumber *sensitivity_lnaDisplay;
    QLCDNumber *sensitivity_mixerDisplay;
    QLCDNumber *sensitivity_vgaDisplay;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLCDNumber *sensitivityDisplay;
    QWidget *tab_2;
    QSlider *linearitySlider;
    QLCDNumber *linearityDisplay;
    QLCDNumber *linearity_lnaDisplay;
    QLCDNumber *linearity_mixerDisplay;
    QLCDNumber *linearity_vgaDisplay;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QWidget *tab_3;
    QSlider *lnaSlider;
    QLabel *label;
    QLCDNumber *lnaDisplay;
    QSlider *mixerSlider;
    QLCDNumber *mixerDisplay;
    QLabel *label_2;
    QSlider *vgaSlider;
    QLabel *label_3;
    QLCDNumber *vgaDisplay;
    QCheckBox *lnaButton;
    QCheckBox *mixerButton;
    QCheckBox *biasButton;
    QLabel *displaySerial;
    QLabel *label_4;
    QPushButton *dumpButton;
    QSpinBox *convQuality_setter;

    void setupUi(QWidget *airspyWidget)
    {
        if (airspyWidget->objectName().isEmpty())
            airspyWidget->setObjectName("airspyWidget");
        airspyWidget->resize(416, 330);
        tabWidget = new QTabWidget(airspyWidget);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setGeometry(QRect(10, 90, 381, 201));
        tab_1 = new QWidget();
        tab_1->setObjectName("tab_1");
        sensitivitySlider = new QSlider(tab_1);
        sensitivitySlider->setObjectName("sensitivitySlider");
        sensitivitySlider->setGeometry(QRect(10, 20, 251, 20));
        sensitivitySlider->setMaximum(21);
        sensitivitySlider->setValue(10);
        sensitivitySlider->setOrientation(Qt::Horizontal);
        sensitivity_lnaDisplay = new QLCDNumber(tab_1);
        sensitivity_lnaDisplay->setObjectName("sensitivity_lnaDisplay");
        sensitivity_lnaDisplay->setGeometry(QRect(220, 60, 64, 23));
        sensitivity_lnaDisplay->setFrameShape(QFrame::NoFrame);
        sensitivity_lnaDisplay->setDigitCount(2);
        sensitivity_lnaDisplay->setSegmentStyle(QLCDNumber::Flat);
        sensitivity_mixerDisplay = new QLCDNumber(tab_1);
        sensitivity_mixerDisplay->setObjectName("sensitivity_mixerDisplay");
        sensitivity_mixerDisplay->setGeometry(QRect(220, 80, 64, 23));
        sensitivity_mixerDisplay->setFrameShape(QFrame::NoFrame);
        sensitivity_mixerDisplay->setDigitCount(2);
        sensitivity_mixerDisplay->setSegmentStyle(QLCDNumber::Flat);
        sensitivity_vgaDisplay = new QLCDNumber(tab_1);
        sensitivity_vgaDisplay->setObjectName("sensitivity_vgaDisplay");
        sensitivity_vgaDisplay->setGeometry(QRect(220, 100, 64, 23));
        sensitivity_vgaDisplay->setFrameShape(QFrame::NoFrame);
        sensitivity_vgaDisplay->setDigitCount(2);
        sensitivity_vgaDisplay->setSegmentStyle(QLCDNumber::Flat);
        label_5 = new QLabel(tab_1);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(100, 60, 101, 20));
        label_6 = new QLabel(tab_1);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(100, 80, 91, 20));
        label_7 = new QLabel(tab_1);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(100, 100, 81, 20));
        sensitivityDisplay = new QLCDNumber(tab_1);
        sensitivityDisplay->setObjectName("sensitivityDisplay");
        sensitivityDisplay->setGeometry(QRect(280, 20, 41, 23));
        sensitivityDisplay->setDigitCount(2);
        sensitivityDisplay->setSegmentStyle(QLCDNumber::Flat);
        tabWidget->addTab(tab_1, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        linearitySlider = new QSlider(tab_2);
        linearitySlider->setObjectName("linearitySlider");
        linearitySlider->setGeometry(QRect(10, 20, 251, 20));
        linearitySlider->setMaximum(21);
        linearitySlider->setValue(10);
        linearitySlider->setOrientation(Qt::Horizontal);
        linearityDisplay = new QLCDNumber(tab_2);
        linearityDisplay->setObjectName("linearityDisplay");
        linearityDisplay->setGeometry(QRect(270, 20, 51, 23));
        linearityDisplay->setDigitCount(2);
        linearityDisplay->setSegmentStyle(QLCDNumber::Flat);
        linearity_lnaDisplay = new QLCDNumber(tab_2);
        linearity_lnaDisplay->setObjectName("linearity_lnaDisplay");
        linearity_lnaDisplay->setGeometry(QRect(220, 60, 64, 23));
        linearity_lnaDisplay->setFrameShape(QFrame::NoFrame);
        linearity_lnaDisplay->setDigitCount(2);
        linearity_lnaDisplay->setSegmentStyle(QLCDNumber::Flat);
        linearity_mixerDisplay = new QLCDNumber(tab_2);
        linearity_mixerDisplay->setObjectName("linearity_mixerDisplay");
        linearity_mixerDisplay->setGeometry(QRect(220, 80, 64, 23));
        linearity_mixerDisplay->setFrameShape(QFrame::NoFrame);
        linearity_mixerDisplay->setDigitCount(2);
        linearity_mixerDisplay->setSegmentStyle(QLCDNumber::Flat);
        linearity_vgaDisplay = new QLCDNumber(tab_2);
        linearity_vgaDisplay->setObjectName("linearity_vgaDisplay");
        linearity_vgaDisplay->setGeometry(QRect(220, 100, 64, 23));
        linearity_vgaDisplay->setFrameShape(QFrame::NoFrame);
        linearity_vgaDisplay->setDigitCount(2);
        linearity_vgaDisplay->setSegmentStyle(QLCDNumber::Flat);
        label_8 = new QLabel(tab_2);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(100, 60, 91, 20));
        label_9 = new QLabel(tab_2);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(100, 80, 101, 20));
        label_10 = new QLabel(tab_2);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(100, 100, 71, 20));
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName("tab_3");
        lnaSlider = new QSlider(tab_3);
        lnaSlider->setObjectName("lnaSlider");
        lnaSlider->setGeometry(QRect(80, 10, 201, 20));
        lnaSlider->setMaximum(15);
        lnaSlider->setValue(10);
        lnaSlider->setOrientation(Qt::Horizontal);
        label = new QLabel(tab_3);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 10, 66, 20));
        lnaDisplay = new QLCDNumber(tab_3);
        lnaDisplay->setObjectName("lnaDisplay");
        lnaDisplay->setGeometry(QRect(290, 10, 51, 23));
        lnaDisplay->setDigitCount(2);
        lnaDisplay->setSegmentStyle(QLCDNumber::Flat);
        mixerSlider = new QSlider(tab_3);
        mixerSlider->setObjectName("mixerSlider");
        mixerSlider->setGeometry(QRect(80, 40, 201, 20));
        mixerSlider->setMaximum(15);
        mixerSlider->setValue(10);
        mixerSlider->setOrientation(Qt::Horizontal);
        mixerDisplay = new QLCDNumber(tab_3);
        mixerDisplay->setObjectName("mixerDisplay");
        mixerDisplay->setGeometry(QRect(290, 40, 51, 23));
        mixerDisplay->setDigitCount(2);
        mixerDisplay->setSegmentStyle(QLCDNumber::Flat);
        label_2 = new QLabel(tab_3);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 40, 66, 20));
        vgaSlider = new QSlider(tab_3);
        vgaSlider->setObjectName("vgaSlider");
        vgaSlider->setGeometry(QRect(80, 70, 201, 20));
        vgaSlider->setMaximum(15);
        vgaSlider->setValue(10);
        vgaSlider->setOrientation(Qt::Horizontal);
        label_3 = new QLabel(tab_3);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(10, 70, 66, 20));
        vgaDisplay = new QLCDNumber(tab_3);
        vgaDisplay->setObjectName("vgaDisplay");
        vgaDisplay->setGeometry(QRect(290, 70, 51, 23));
        vgaDisplay->setDigitCount(2);
        vgaDisplay->setSegmentStyle(QLCDNumber::Flat);
        lnaButton = new QCheckBox(tab_3);
        lnaButton->setObjectName("lnaButton");
        lnaButton->setGeometry(QRect(20, 121, 81, 21));
        mixerButton = new QCheckBox(tab_3);
        mixerButton->setObjectName("mixerButton");
        mixerButton->setGeometry(QRect(110, 121, 101, 21));
        biasButton = new QCheckBox(tab_3);
        biasButton->setObjectName("biasButton");
        biasButton->setGeometry(QRect(200, 121, 101, 21));
        tabWidget->addTab(tab_3, QString());
        displaySerial = new QLabel(airspyWidget);
        displaySerial->setObjectName("displaySerial");
        displaySerial->setGeometry(QRect(20, 60, 231, 21));
        label_4 = new QLabel(airspyWidget);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(10, 20, 141, 20));
        dumpButton = new QPushButton(airspyWidget);
        dumpButton->setObjectName("dumpButton");
        dumpButton->setGeometry(QRect(150, 20, 104, 31));
        convQuality_setter = new QSpinBox(airspyWidget);
        convQuality_setter->setObjectName("convQuality_setter");
        convQuality_setter->setGeometry(QRect(300, 20, 91, 31));
        convQuality_setter->setMaximum(4);
        convQuality_setter->setValue(4);

        retranslateUi(airspyWidget);

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(airspyWidget);
    } // setupUi

    void retranslateUi(QWidget *airspyWidget)
    {
        airspyWidget->setWindowTitle(QCoreApplication::translate("airspyWidget", "airspy", nullptr));
        label_5->setText(QCoreApplication::translate("airspyWidget", "lna gain", nullptr));
        label_6->setText(QCoreApplication::translate("airspyWidget", "mixer gain", nullptr));
        label_7->setText(QCoreApplication::translate("airspyWidget", "vga gain", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_1), QCoreApplication::translate("airspyWidget", "sensitivity", nullptr));
        label_8->setText(QCoreApplication::translate("airspyWidget", "lna gain", nullptr));
        label_9->setText(QCoreApplication::translate("airspyWidget", "mixer gain", nullptr));
        label_10->setText(QCoreApplication::translate("airspyWidget", "vga gain", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("airspyWidget", "linearity", nullptr));
        label->setText(QCoreApplication::translate("airspyWidget", "lna", nullptr));
        label_2->setText(QCoreApplication::translate("airspyWidget", "mixer", nullptr));
        label_3->setText(QCoreApplication::translate("airspyWidget", "vga", nullptr));
        lnaButton->setText(QCoreApplication::translate("airspyWidget", "lna", nullptr));
        mixerButton->setText(QCoreApplication::translate("airspyWidget", "mixer ", nullptr));
        biasButton->setText(QCoreApplication::translate("airspyWidget", "bias", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QCoreApplication::translate("airspyWidget", "classic view", nullptr));
        displaySerial->setText(QString());
        label_4->setText(QCoreApplication::translate("airspyWidget", "A I R S P Y  handler", nullptr));
#if QT_CONFIG(tooltip)
        dumpButton->setToolTip(QCoreApplication::translate("airspyWidget", "<html><head/><body><p>Dumps the raw input from the airspy into a self describing file (with header in xml format containing recorded time, container format, frequency, device name, Qt-DAB version)</p><p><br/></p><p>Opens a 'Save as ...' dialog. Press again to stop recording.</p><p><br/></p><p>Very experimental! The resulting file can be read-in by using the xml file handler (if configured)</p><p><br/></p><p>Warning: Produces large files!</p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        dumpButton->setText(QCoreApplication::translate("airspyWidget", "dump", nullptr));
#if QT_CONFIG(tooltip)
        convQuality_setter->setToolTip(QCoreApplication::translate("airspyWidget", "<html><head/><body><p>Quality level for samplerate conversion. Note that AN airSPY DEVICE emits either 2.5 Ms/s or 3 Ms/s and we need 2048000, so samplerate conversion is required.</p><p>For the Down sampling we use the &quot;libsamplerate&quot; library (all rights reserved) and that library provides a choice between 5 quality levels.</p><p>Level 0  - the default here - is merely a linear interpolation, level 4is based on sinc interpolation and pretty good. Of course, higher quality requires more computation. Levels 2 and 3 work fine, level 4  might require too much cpu cycles, resulting in data loss,</p><p><br/></p></body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class airspyWidget: public Ui_airspyWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AIRSPY_2D_WIDGET_H
