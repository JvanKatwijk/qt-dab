/********************************************************************************
** Form generated from reading UI file 'pluto-widget.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLUTO_2D_WIDGET_H
#define UI_PLUTO_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_plutoWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *biasT_selector;
    QCheckBox *agcControl;
    QSpinBox *gainControl;
    QLabel *state;
    QLabel *label;
    QLabel *contextName;
    QLabel *name_of_device;
    QHBoxLayout *horizontalLayout;
    QPushButton *filterButton;
    QPushButton *debugButton;
    QPushButton *dumpButton;
    QLabel *serial_number_display;

    void setupUi(QWidget *plutoWidget)
    {
        if (plutoWidget->objectName().isEmpty())
            plutoWidget->setObjectName("plutoWidget");
        plutoWidget->resize(324, 224);
        verticalLayout = new QVBoxLayout(plutoWidget);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        biasT_selector = new QCheckBox(plutoWidget);
        biasT_selector->setObjectName("biasT_selector");

        horizontalLayout_2->addWidget(biasT_selector);

        agcControl = new QCheckBox(plutoWidget);
        agcControl->setObjectName("agcControl");

        horizontalLayout_2->addWidget(agcControl);

        gainControl = new QSpinBox(plutoWidget);
        gainControl->setObjectName("gainControl");
        gainControl->setMaximum(73);
        gainControl->setValue(33);

        horizontalLayout_2->addWidget(gainControl);


        verticalLayout->addLayout(horizontalLayout_2);

        state = new QLabel(plutoWidget);
        state->setObjectName("state");

        verticalLayout->addWidget(state);

        label = new QLabel(plutoWidget);
        label->setObjectName("label");

        verticalLayout->addWidget(label);

        contextName = new QLabel(plutoWidget);
        contextName->setObjectName("contextName");

        verticalLayout->addWidget(contextName);

        name_of_device = new QLabel(plutoWidget);
        name_of_device->setObjectName("name_of_device");

        verticalLayout->addWidget(name_of_device);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        filterButton = new QPushButton(plutoWidget);
        filterButton->setObjectName("filterButton");

        horizontalLayout->addWidget(filterButton);

        debugButton = new QPushButton(plutoWidget);
        debugButton->setObjectName("debugButton");

        horizontalLayout->addWidget(debugButton);

        dumpButton = new QPushButton(plutoWidget);
        dumpButton->setObjectName("dumpButton");

        horizontalLayout->addWidget(dumpButton);


        verticalLayout->addLayout(horizontalLayout);

        serial_number_display = new QLabel(plutoWidget);
        serial_number_display->setObjectName("serial_number_display");
        QFont font;
        font.setPointSize(11);
        serial_number_display->setFont(font);

        verticalLayout->addWidget(serial_number_display);


        retranslateUi(plutoWidget);

        QMetaObject::connectSlotsByName(plutoWidget);
    } // setupUi

    void retranslateUi(QWidget *plutoWidget)
    {
        plutoWidget->setWindowTitle(QCoreApplication::translate("plutoWidget", "Pluto control", nullptr));
#if QT_CONFIG(tooltip)
        plutoWidget->setToolTip(QCoreApplication::translate("plutoWidget", "Pluto support is experimental. I do not have one so testing was limited.", nullptr));
#endif // QT_CONFIG(tooltip)
        biasT_selector->setText(QCoreApplication::translate("plutoWidget", "biasT", nullptr));
        agcControl->setText(QCoreApplication::translate("plutoWidget", "agc", nullptr));
        state->setText(QCoreApplication::translate("plutoWidget", "TextLabel", nullptr));
#if QT_CONFIG(tooltip)
        label->setToolTip(QCoreApplication::translate("plutoWidget", "Experimental interface for Adalm Pluto. No garantees!!", nullptr));
#endif // QT_CONFIG(tooltip)
        label->setText(QCoreApplication::translate("plutoWidget", "Pluto interface", nullptr));
        contextName->setText(QCoreApplication::translate("plutoWidget", "context name", nullptr));
        name_of_device->setText(QCoreApplication::translate("plutoWidget", "device name", nullptr));
#if QT_CONFIG(tooltip)
        filterButton->setToolTip(QCoreApplication::translate("plutoWidget", "The filter button. The pluto has a possibility of loading a filter configuration and enabling (or\n"
"disabling it). By default a filter configuration is loaded and enabled. The text on the button states what the effect of touching it is. If the button text reads \"filter on\", it therefore means that the\n"
"filter is disabled, but will be enabled by touching the button.", nullptr));
#endif // QT_CONFIG(tooltip)
        filterButton->setText(QCoreApplication::translate("plutoWidget", "filter", nullptr));
        debugButton->setText(QCoreApplication::translate("plutoWidget", "debug off", nullptr));
        dumpButton->setText(QCoreApplication::translate("plutoWidget", "dump", nullptr));
        serial_number_display->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class plutoWidget: public Ui_plutoWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLUTO_2D_WIDGET_H
