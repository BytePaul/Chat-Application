#include "mainwindow.h"
#include <QApplication>
#include <QInputDialog>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString username = QInputDialog::getText(nullptr, "Enter Name", "Username:");
//    if (username.isEmpty()) {
//        username = "Anonymous";
//    }

    MainWindow w(username);
    w.show();
    return a.exec();
}
