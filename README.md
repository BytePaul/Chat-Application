# 💬 ChatApplication

A simple Qt-based TCP chat application that includes:

- `ChatServer`: Handles multiple client connections, manages usernames, and routes messages.
- `ChatClient`: GUI chat interface with username support, dark mode, emoji picker, and online user list.

This project is built using **Qt (Widgets + Network module)** and demonstrates real-time messaging using `QTcpSocket` and `QTcpServer`.

---

## 📁 Repository Structure

```
ChatApplication/
├── ChatClient/    # Qt GUI client
└── ChatServer/    # Qt server console app
```

---

## 🚀 Features

### ChatClient
- Username support (saved using `QSettings`)
- Dark mode toggle
- Emoji picker using `QInputDialog`
- Chat saving to `.txt` files
- Clear chat history
- Online user list
- Handles `/nick`, `/users`, `/save`, and `/clear` commands

### ChatServer
- Accepts multiple TCP clients
- Tracks usernames using `QHash`
- Validates uniqueness of usernames
- Handles:
  - `/nick <new_name>` — Change nickname
  - `/users` — View online users
- Broadcasts join/leave/nickname changes to all clients

---

## 💡 Supported Commands

Inside the client, type:

```txt
/nick <new_username>     # Change your nickname
/users                   # List online users
/clear                   # Clear the chat box
/save                    # Save chat history to a file
```

---

## 🛠️ Requirements

- Qt 5 or Qt 6 (QtCore, QtWidgets, QtNetwork modules)
- C++17 or later
- Tested with Qt Creator and `qmake`

---

## 🧑‍💻 Build Instructions

### 1. Clone the repository

```bash
git clone https://github.com/your-username/ChatApplication.git
cd ChatApplication
```

### 2. Build the Server

```bash
cd ChatServer
qmake
make   # or mingw32-make on Windows
./ChatServer
```

### 3. Build the Client

```bash
cd ../ChatClient
qmake
make
./ChatClient
```

---

## 🧪 How It Works

- Server listens on `127.0.0.1:1234`
- Clients connect, send a username first
- Server maps sockets to usernames and routes incoming messages
- Server sends system messages like:
  - `*** Paul joined the chat ***`
  - `*** Paul is now John ***`
  - `*** John left the chat ***`

---

## 📝 Notes

- If username is not provided, client uses one of the following:
  - Saved name from last session
  - Prompted via `QInputDialog`
  - Fallback: `Guest_<timestamp>` if all else fails
- Data is stored using `QSettings`, so preferences persist after restart.

---

## 📄 License

MIT License © 2025 Paul Gaikwad

---

## 🙌 Acknowledgements

This project was developed to demonstrate a real-time networked Qt application with modern C++ and Qt Widgets.
