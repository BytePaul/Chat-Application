#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QInputDialog>
#include <QTime>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QSettings>
#include <QScrollBar>
#include <QDebug>
#include <QHostAddress>
#include <QCoreApplication>

MainWindow::MainWindow(const QString &initialUser, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket(new QTcpSocket(this))
    , settings(new QSettings("PaulChat", "ChatClient", this)) // Organization, Application Name
{
    ui->setupUi(this);
    ui->chatBox->setReadOnly(true);

    loadUserPreferences();

    if (!initialUser.isEmpty() && username != initialUser) {
        username = initialUser;
        saveUserPreferences();
    }

    if (username.isEmpty()) {
        bool ok;
        QString newUserName = QInputDialog::getText(this, tr("Choose Username"),
                                                    tr("Please enter your desired username:"), QLineEdit::Normal,
                                                    "", &ok);
        if (ok && !newUserName.isEmpty()) {
            username = newUserName.trimmed();
        } else {
            username = "Guest_" + QString::number(QDateTime::currentSecsSinceEpoch() % 1000);
            QMessageBox::warning(this, tr("Username Required"), tr("No username provided. Using a guest name."));
            saveUserPreferences();
        }
    }


    ui->statusLabel->setText("Connecting...");

    // Connect to host
    socket->connectToHost(QHostAddress::LocalHost, 1234);

    // Update status and apply dark mode based on loaded settings
    updateStatusLabel();
    applyDarkMode(ui->darkModeCheckBox->isChecked());

    // Connect signals and slots
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(ui->emojiButton, &QPushButton::clicked, this, &MainWindow::insertEmoji);
    connect(ui->darkModeCheckBox, &QCheckBox::toggled, this, &MainWindow::toggleDarkMode);
    connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::clearChat);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::saveChat);

    // Socket connections
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::receiveMessage);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &MainWindow::handleSocketError);
    connect(socket, &QTcpSocket::connected, this, &MainWindow::onConnected); // New: Handle connection established
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::onDisconnected); // New: Handle disconnection
}

// In mainwindow.cpp

MainWindow::~MainWindow() {
    qDebug() << "MainWindow destructor called.";

    if (socket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Disconnecting QTcpSocket...";
        socket->disconnectFromHost();
    }

    delete ui;
    qDebug() << "UI deleted.";
    qDebug() << "MainWindow destructor finished.";
}

void MainWindow::onConnected() {
    ui->statusLabel->setText("Connected as: " + username);
    socket->write((username + "\n").toUtf8());
    qDebug() << "Connected to server. Sent username:" << username;
}

void MainWindow::onDisconnected() {
    ui->statusLabel->setText("Disconnected.");
    QMessageBox::information(this, "Disconnected", "Lost connection to the server.");
    ui->userlistWidget->clear();
}


/*
 * ==============================
 * Supported Chat Commands
 * ==============================
 *
 * /nick <new_name>
 *    → Change your username.
 *    → Example: /nick CoolPaul
 *    → Server will broadcast "*** OldName is now NewName ***"
 *
 * /users
 *    → Request a list of all online users.
 *    → Server will reply with a list and client will update user list.
 *
 * /clear
 *    → Clears the chat window.
 *    → Local command, does not affect other users.
 *
 * /save
 *    → Save the current chat log to a text file (.txt).
 *    → Opens a file dialog.
 *
 * Unknown commands:
 *    → Any command not listed above will result in:
 *      "Unknown command: /example"
 *
 */

void MainWindow::sendMessage() {
    QString message = ui->inputBox->text().trimmed();
    if (message.isEmpty()) return;

    if (socket->state() != QAbstractSocket::ConnectedState) {
        QMessageBox::warning(this, "Connection Error", "Not connected to server. Cannot send message.");
        ui->inputBox->clear();
        return;
    }

    if (message.startsWith("/nick ")) {
        QString newName = message.section(' ', 1).trimmed();
        if (newName.isEmpty()) {
            formatAndDisplayMessage("System", "Usage: /nick <new_username>", QTime::currentTime().toString("HH:mm:ss"));
            ui->inputBox->clear();
            return;
        }
        socket->write((message + "\n").toUtf8());
        username = newName;
        saveUserPreferences();
        updateStatusLabel();
        formatAndDisplayMessage("System", QString("Requested nickname change to '%1'. Waiting for server confirmation...").arg(newName), QTime::currentTime().toString("HH:mm:ss"));
    } else {
        socket->write((message + "\n").toUtf8());
    }

    ui->inputBox->clear();
}

void MainWindow::receiveMessage() {
    while (socket->bytesAvailable()) {
        QByteArray lineData = socket->readLine();
        if (lineData.isEmpty()) continue;

        QString msg = QString::fromUtf8(lineData).trimmed();
        QString time = QTime::currentTime().toString("HH:mm:ss");

        if (msg.startsWith("*** ")) {
            ui->chatBox->append("[" + time + "] " + msg);

            if (msg.contains(" joined the chat ***")) {
                QString joinedUser = msg.section(' ', 1, 1);
                if (ui->userlistWidget->findItems(joinedUser, Qt::MatchExactly).isEmpty()) {
                    addUserToList(joinedUser);
                }
            } else if (msg.contains(" left the chat ***")) {
                QString leftUser = msg.section(' ', 1, 1);
                removeUserFromList(leftUser);
            } else if (msg.contains(" is now ") && msg.endsWith(" ***")) {
                // NEW: Handle Nickname Change message
                // Format: "*** OldName is now NewName ***"
                QStringList parts = msg.split(" is now ", Qt::SkipEmptyParts);
                if (parts.size() == 2) {
                    QString oldName = parts.at(0).trimmed();
                    oldName = oldName.mid(4); // Remove "*** " prefix
                    QString newName = parts.at(1).trimmed();
                    newName = newName.left(newName.length() - 4); // Remove " ***" suffix

                    if (!oldName.isEmpty() && !newName.isEmpty()) {
                        removeUserFromList(oldName);
                        addUserToList(newName);
                    }
                }
            }
        }
        else if (msg.startsWith("Online users:")) {
            QStringList usersList = msg.split('\n', Qt::SkipEmptyParts);
            ui->userlistWidget->clear();
            for (int i = 1; i < usersList.size(); ++i) {
                addUserToList(usersList.at(i));
            }
            ui->chatBox->append("[" + time + "] " + msg);
        }
        else if (msg.startsWith("Username already taken.") || msg.startsWith("Unknown command: ") || msg.startsWith("System:")) {
            ui->chatBox->append("[" + time + "] " + msg);
        }

        else {
            int sepIndex = msg.indexOf(": ");
            if (sepIndex > 0) {
                QString sender = msg.left(sepIndex);
                QString messageContent = msg.mid(sepIndex + 2);
                formatAndDisplayMessage(sender, messageContent, time);

                if (ui->userlistWidget->findItems(sender, Qt::MatchExactly).isEmpty()) {
                    addUserToList(sender);
                }
            } else {
                ui->chatBox->append("[" + time + "] " + msg); // Fallback for unexpected format
            }
        }
    }
    ui->chatBox->verticalScrollBar()->setValue(ui->chatBox->verticalScrollBar()->maximum());
}
void MainWindow::formatAndDisplayMessage(const QString &sender, const QString &message, const QString &time) {
    QString formattedMessage;
    if (sender == "System") {
        formattedMessage = QString("<b>[%1] <span style='color:#ff8c00;'>%2</span></b>: %3")
        .arg(time, sender, message.toHtmlEscaped());
    } else if (sender == username) { // Style for own messages
        formattedMessage = QString("<b>[%1] <span style='color:#00AA00;'>%2</span></b>: %3")
                               .arg(time, sender, message.toHtmlEscaped());
    }
    else { // Style for others' messages
        formattedMessage = QString("<b>[%1] <span style='color:#007acc;'>%2</span></b>: %3")
                               .arg(time, sender, message.toHtmlEscaped());
    }
    ui->chatBox->append(formattedMessage);
}

void MainWindow::loadUserPreferences() {
    username = settings->value("username", "").toString();
    bool darkMode = settings->value("darkMode", false).toBool();
    ui->darkModeCheckBox->setChecked(darkMode);
}

void MainWindow::saveUserPreferences() {
    settings->setValue("username", username);
    settings->setValue("darkMode", ui->darkModeCheckBox->isChecked());
}

void MainWindow::updateStatusLabel() {
    ui->statusLabel->setText("Connected as: " + username);
}

void MainWindow::applyDarkMode(bool enabled) {
    if (enabled) {
        qApp->setStyleSheet("QWidget { background-color: #2e2e2e; color: white; } QLineEdit { background-color: #444; border: 1px solid #555; } QTextEdit { background-color: #444; border: 1px solid #555; } QListWidget { background-color: #444; border: 1px solid #555; } QPushButton { background-color: #555; border: 1px solid #666; } QCheckBox { color: white; } QLabel { color: white; }");
    } else {
        qApp->setStyleSheet("");
    }
    saveUserPreferences();
}

void MainWindow::insertEmoji() {
    QStringList emojis = { "😊", "😂", "👍", "❤️", "🔥", "🎉", "🙏", "😎", "💬" };
    bool ok;
    QString emoji = QInputDialog::getItem(this, "Select Emoji", "Emojis:", emojis, 0, false, &ok);
    if (ok && !emoji.isEmpty()) {
        ui->inputBox->insert(emoji);
    }
}

void MainWindow::toggleDarkMode(bool checked) {
    applyDarkMode(checked);
}

void MainWindow::clearChat() {
    ui->chatBox->clear();
}

void MainWindow::saveChat() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Chat", "", "Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << ui->chatBox->toPlainText();
            file.close();
            QMessageBox::information(this, "Saved", "Chat saved to " + QFileInfo(fileName).fileName());
        } else {
            QMessageBox::warning(this, "Error", "Cannot write to file: " + file.errorString());
        }
    }
}

void MainWindow::handleSocketError(QAbstractSocket::SocketError socketError) {
    Q_UNUSED(socketError)
    QMessageBox::critical(this, "Connection Error", socket->errorString());
    close();
}

void MainWindow::processCommand(const QString &command) {
    if (command.startsWith("/nick ")) {
        formatAndDisplayMessage("System", "Nickname change command sent. Waiting for server confirmation.", QTime::currentTime().toString("HH:mm:ss"));
    } else if (command == "/clear") {
        clearChat();
    } else if (command == "/save") {
        saveChat();
    } else if (command == "/users") {
        socket->write(QString("/users\n").toUtf8());
        formatAndDisplayMessage("System", "Requesting online users...", QTime::currentTime().toString("HH:mm:ss"));
    }
    else {
        formatAndDisplayMessage("System", "Unknown command: " + command, QTime::currentTime().toString("HH:mm:ss"));
    }
}

void MainWindow::addUserToList(const QString &user) {
    if (ui->userlistWidget->findItems(user, Qt::MatchExactly).isEmpty()) {
        ui->userlistWidget->addItem(user);
        qDebug() << "Added user to list:" << user;
    }
}

void MainWindow::removeUserFromList(const QString &user) {
    QList<QListWidgetItem *> items = ui->userlistWidget->findItems(user, Qt::MatchExactly);
    for (QListWidgetItem *item : items) {
        delete item;
        qDebug() << "Removed user from list:" << user;
    }
}
