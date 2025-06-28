#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QHostAddress> // For QHostAddress::Any

class ChatServer : public QTcpServer {
    Q_OBJECT // Required for signals/slots

public:
    ChatServer(QObject *parent = nullptr) : QTcpServer(parent) {
        if (listen(QHostAddress::Any, 1234)) {
            qDebug() << "✅ Server started on port 1234.";
        } else {
            qCritical() << "❌ Server failed to start:" << errorString(); // Include error string
        }
    }

protected:
    void incomingConnection(qintptr socketDescriptor) override {
        QTcpSocket *client = new QTcpSocket(this);
        client->setSocketDescriptor(socketDescriptor);
        qDebug() << "New incoming connection from:" << client->peerAddress().toString();

        // Connect signals for this new client
        connect(client, &QTcpSocket::readyRead, this, [this, client]() {
            // Using a loop with readLine() for robust message parsing
            while (client->bytesAvailable()) {
                QByteArray lineData = client->readLine();
                if (lineData.isEmpty()) continue; // Skip if no complete line yet

                QString message = QString::fromUtf8(lineData).trimmed();

                if (!usernames.contains(client)) {
                    // First message from a new client is expected to be the username
                    QString username = message; // Use the trimmed message directly as username

                    if (username.isEmpty() || usernames.values().contains(username)) {
                        // Inconsistent with client, remove emoji from server response
                        client->write("Username already taken or invalid. Disconnecting.\n");
                        client->disconnectFromHost();
                        qDebug() << "Rejected connection: Invalid or duplicate username -" << username;
                        return; // Stop processing further in this readyRead slot for this client
                    }

                    usernames[client] = username;
                    clients.append(client);

                    QString joinMsg = QString("*** %1 joined the chat ***").arg(username);
                    broadcast(joinMsg, nullptr); // Broadcast to ALL including new client for their confirmation
                    qDebug() << username << "joined the chat.";
                    return; // Stop processing, this was the username.
                }

                // Existing client sending a message
                QString user = usernames.value(client);

                // --- Handle Commands ---
                if (message == "/users") {
                    QStringList userList = usernames.values();
                    QString reply = "Online users:\n" + userList.join("\n") + "\n";
                    client->write(reply.toUtf8());
                    qDebug() << user << "requested user list.";
                    // Do not broadcast /users command to other clients
                    continue; // Process next line if available
                } else if (message.startsWith("/nick ")) {
                    QString oldName = usernames.value(client);
                    QString newName = message.section(' ', 1).trimmed();

                    if (newName.isEmpty() || usernames.values().contains(newName)) {
                        client->write(QString("System: Nickname '%1' is invalid or already taken.\n").arg(newName).toUtf8());
                        qDebug() << oldName << "failed to change nick to" << newName << "(invalid/taken).";
                    } else {
                        usernames[client] = newName; // Update username in hash
                        QString nickChangeMsg = QString("*** %1 is now %2 ***").arg(oldName, newName);
                        broadcast(nickChangeMsg, nullptr); // Broadcast to all
                        qDebug() << oldName << "changed nick to" << newName;
                    }
                    continue; // Process next line if available
                }
                // --- End Handle Commands ---

                // Regular message
                if (!message.isEmpty()) {
                    QString fullMessage = QString("%1: %2").arg(user, message); // Removed timestamp here, client will add it
                    broadcast(fullMessage, nullptr); // Broadcast to ALL
                    qDebug() << fullMessage;
                }
            } // End while (client->bytesAvailable())
        });

        connect(client, &QTcpSocket::disconnected, this, [this, client]() {
            QString user = usernames.value(client, "Unknown");
            usernames.remove(client);
            clients.removeAll(client); // Remove from list of active clients

            QString leaveMsg = QString("*** %1 left the chat ***").arg(user);
            broadcast(leaveMsg, nullptr); // Broadcast to ALL (no exclude for leave messages)

            client->deleteLater(); // Schedule client for deletion
            qDebug() << user << "disconnected.";
        });

        connect(client, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
                this, [this, client](QAbstractSocket::SocketError socketError) {
                    Q_UNUSED(socketError); // Suppress unused parameter warning
                    qWarning() << "Client socket error:" << client->errorString();
                    // Disconnect in case of error, if not already disconnected
                    if (client->state() != QAbstractSocket::UnconnectedState) {
                        client->disconnectFromHost();
                    }
                });
    } // End incomingConnection

private:
    QList<QTcpSocket*> clients;
    QHash<QTcpSocket*, QString> usernames;

    void broadcast(const QString &message, QTcpSocket *exclude = nullptr) {
        QByteArray data = message.toUtf8() + "\n";
        for (QTcpSocket* c : clients) {
            // Only send if the client is connected and is not the excluded client (if any)
            if (c->state() == QAbstractSocket::ConnectedState && c != exclude) {
                c->write(data);
            }
        }
    }
};

#include "main.moc"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    ChatServer server;
    return a.exec();
}
