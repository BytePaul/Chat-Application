/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTextEdit *chatBox;
    QLineEdit *inputBox;
    QPushButton *sendButton;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QListWidget *userlistWidget;
    QPushButton *emojiButton;
    QCheckBox *darkModeCheckBox;
    QLabel *statusLabel;
    QPushButton *clearButton;
    QPushButton *saveButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(870, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        chatBox = new QTextEdit(centralwidget);
        chatBox->setObjectName("chatBox");
        chatBox->setGeometry(QRect(50, 20, 641, 421));
        inputBox = new QLineEdit(centralwidget);
        inputBox->setObjectName("inputBox");
        inputBox->setGeometry(QRect(50, 450, 641, 31));
        inputBox->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);
        inputBox->setCursorMoveStyle(Qt::CursorMoveStyle::LogicalMoveStyle);
        inputBox->setClearButtonEnabled(true);
        sendButton = new QPushButton(centralwidget);
        sendButton->setObjectName("sendButton");
        sendButton->setGeometry(QRect(250, 490, 111, 51));
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(260, 60, 2, 2));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        userlistWidget = new QListWidget(centralwidget);
        userlistWidget->setObjectName("userlistWidget");
        userlistWidget->setGeometry(QRect(700, 20, 161, 421));
        emojiButton = new QPushButton(centralwidget);
        emojiButton->setObjectName("emojiButton");
        emojiButton->setGeometry(QRect(700, 450, 89, 31));
        darkModeCheckBox = new QCheckBox(centralwidget);
        darkModeCheckBox->setObjectName("darkModeCheckBox");
        darkModeCheckBox->setGeometry(QRect(700, 500, 92, 31));
        statusLabel = new QLabel(centralwidget);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setGeometry(QRect(50, 500, 171, 31));
        clearButton = new QPushButton(centralwidget);
        clearButton->setObjectName("clearButton");
        clearButton->setGeometry(QRect(400, 490, 111, 51));
        saveButton = new QPushButton(centralwidget);
        saveButton->setObjectName("saveButton");
        saveButton->setGeometry(QRect(550, 490, 111, 51));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 870, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        chatBox->setMarkdown(QString());
        chatBox->setHtml(QCoreApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", nullptr));
        chatBox->setPlaceholderText(QCoreApplication::translate("MainWindow", "Chats will appear here ", nullptr));
        inputBox->setText(QString());
        inputBox->setPlaceholderText(QCoreApplication::translate("MainWindow", "Send your Message ", nullptr));
        sendButton->setText(QCoreApplication::translate("MainWindow", "Send", nullptr));
        emojiButton->setText(QCoreApplication::translate("MainWindow", "Select emoji", nullptr));
        darkModeCheckBox->setText(QCoreApplication::translate("MainWindow", "Dark Mode", nullptr));
        statusLabel->setText(QString());
        clearButton->setText(QCoreApplication::translate("MainWindow", "Clear", nullptr));
        saveButton->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
