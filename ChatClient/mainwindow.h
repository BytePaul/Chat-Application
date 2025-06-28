#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QListWidgetItem>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &username, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QString username;
    QSettings *settings;

    void formatAndDisplayMessage(const QString &sender, const QString &message, const QString &time);
    void loadUserPreferences();
    void saveUserPreferences();
    void updateStatusLabel();
    void processCommand(const QString &command);
    void applyDarkMode(bool enabled);
    void addUserToList(const QString &user);
    void removeUserFromList(const QString &user);
    void populateEmojiSelector();

private slots:
    void sendMessage();
    void receiveMessage();
    void insertEmoji();
    void toggleDarkMode(bool checked);
    void clearChat();
    void saveChat();
    void handleSocketError(QAbstractSocket::SocketError socketError);
    void onConnected();
    void onDisconnected();
};

#endif // MAINWINDOW_H
