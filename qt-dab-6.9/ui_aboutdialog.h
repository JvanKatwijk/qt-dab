/********************************************************************************
** Form generated from reading UI file 'aboutdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTDIALOG_H
#define UI_ABOUTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_AboutDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *appName;
    QLabel *appIcon;
    QLabel *version;
    QLabel *author;
    QLabel *buildInfo;
    QLabel *sourceLocation;
    QLabel *qtVersion;
    QVBoxLayout *verticalLayout_2;
    QLabel *libraries;
    QLabel *configuration;
    QTextEdit *disclaimer;

    void setupUi(QDialog *AboutDialog)
    {
        if (AboutDialog->objectName().isEmpty())
            AboutDialog->setObjectName("AboutDialog");
        AboutDialog->resize(270, 460);
        verticalLayout = new QVBoxLayout(AboutDialog);
        verticalLayout->setObjectName("verticalLayout");
        appName = new QLabel(AboutDialog);
        appName->setObjectName("appName");
        QFont font;
        font.setPointSize(16);
        appName->setFont(font);

        verticalLayout->addWidget(appName, 0, Qt::AlignHCenter);

        appIcon = new QLabel(AboutDialog);
        appIcon->setObjectName("appIcon");

        verticalLayout->addWidget(appIcon, 0, Qt::AlignHCenter);

        version = new QLabel(AboutDialog);
        version->setObjectName("version");

        verticalLayout->addWidget(version);

        author = new QLabel(AboutDialog);
        author->setObjectName("author");

        verticalLayout->addWidget(author);

        buildInfo = new QLabel(AboutDialog);
        buildInfo->setObjectName("buildInfo");

        verticalLayout->addWidget(buildInfo);

        sourceLocation = new QLabel(AboutDialog);
        sourceLocation->setObjectName("sourceLocation");

        verticalLayout->addWidget(sourceLocation);

        qtVersion = new QLabel(AboutDialog);
        qtVersion->setObjectName("qtVersion");

        verticalLayout->addWidget(qtVersion);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        libraries = new QLabel(AboutDialog);
        libraries->setObjectName("libraries");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(libraries->sizePolicy().hasHeightForWidth());
        libraries->setSizePolicy(sizePolicy);
        libraries->setWordWrap(true);

        verticalLayout_2->addWidget(libraries);

        configuration = new QLabel(AboutDialog);
        configuration->setObjectName("configuration");
        sizePolicy.setHeightForWidth(configuration->sizePolicy().hasHeightForWidth());
        configuration->setSizePolicy(sizePolicy);
        configuration->setWordWrap(true);

        verticalLayout_2->addWidget(configuration);

        disclaimer = new QTextEdit(AboutDialog);
        disclaimer->setObjectName("disclaimer");

        verticalLayout_2->addWidget(disclaimer);


        verticalLayout->addLayout(verticalLayout_2);


        retranslateUi(AboutDialog);

        QMetaObject::connectSlotsByName(AboutDialog);
    } // setupUi

    void retranslateUi(QDialog *AboutDialog)
    {
        AboutDialog->setWindowTitle(QCoreApplication::translate("AboutDialog", "About Qt-DAB", nullptr));
        appName->setText(QCoreApplication::translate("AboutDialog", "Qt-DAB radio", nullptr));
        appIcon->setText(QCoreApplication::translate("AboutDialog", "TextLabel", nullptr));
        version->setText(QCoreApplication::translate("AboutDialog", "Version info", nullptr));
        author->setText(QCoreApplication::translate("AboutDialog", "Developed by Jan van Katwijk ", nullptr));
        buildInfo->setText(QCoreApplication::translate("AboutDialog", "build info", nullptr));
        sourceLocation->setText(QCoreApplication::translate("AboutDialog", "source location", nullptr));
        qtVersion->setText(QCoreApplication::translate("AboutDialog", "Based on Qt 5.2.2", nullptr));
        libraries->setText(QCoreApplication::translate("AboutDialog", "Qt-DAB uses following libraries:", nullptr));
        configuration->setText(QCoreApplication::translate("AboutDialog", "Qt-DAB is configured with :", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AboutDialog: public Ui_AboutDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTDIALOG_H
