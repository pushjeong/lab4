import os
import socket
import threading
from PyQt5.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QTextEdit, QLineEdit, QPushButton, QFileDialog, QMessageBox, QInputDialog
)
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QGuiApplication

# 소프트웨어 렌더링 활성화
QGuiApplication.setAttribute(Qt.AA_UseSoftwareOpenGL)


class ChatClient(QWidget):
    def __init__(self, host, port):
        super().__init__()
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((host, port))

        self.init_ui()
        self.nickname = self.get_nickname()
        self.sock.send(self.nickname.encode())

        # Start a thread to receive messages
        self.thread = threading.Thread(target=self.receive_messages, daemon=True)
        self.thread.start()

    def init_ui(self):
        self.setWindowTitle("Chat Client")
        self.setGeometry(200, 200, 400, 500)

        self.layout = QVBoxLayout()

        self.chat_display = QTextEdit()
        self.chat_display.setReadOnly(True)
        self.layout.addWidget(self.chat_display)

        self.message_input = QLineEdit()
        self.message_input.setPlaceholderText("Enter your message here...")
        self.layout.addWidget(self.message_input)

        self.send_button = QPushButton("Send")
        self.send_button.clicked.connect(self.send_message)
        self.layout.addWidget(self.send_button)

        self.file_button = QPushButton("Send File")
        self.file_button.clicked.connect(self.send_file)
        self.layout.addWidget(self.file_button)

        self.setLayout(self.layout)

    def get_nickname(self):
        nickname, ok = QInputDialog.getText(self, "Nickname", "Enter your nickname:")
        if not ok or not nickname.strip():
            QMessageBox.critical(self, "Error", "Nickname is required to join the chat.")
            self.close()
            exit(1)
        return nickname.strip()

    def send_message(self):
        message = self.message_input.text()
        if message:
            self.sock.send(message.encode())
            self.message_input.clear()

    def send_file(self):
        filepath, _ = QFileDialog.getOpenFileName(self, "Select File")
        if filepath:
            filename = os.path.basename(filepath)
            filesize = os.path.getsize(filepath)

            # Send file metadata
            self.sock.send(f"/file {filename} {filesize}".encode())

            # Send file data
            with open(filepath, "rb") as f:
                while chunk := f.read(1024):
                    self.sock.send(chunk)

            self.chat_display.append(f"File {filename} sent!")

    def receive_messages(self):
        while True:
            try:
                message = self.sock.recv(1024).decode()
                self.chat_display.append(message)
            except Exception as e:
                self.chat_display.append(f"Error receiving message: {e}")
                break


if __name__ == "__main__":
    import sys

    app = QApplication(sys.argv)
    client = ChatClient("127.0.0.1", 12345)
    client.show()
    sys.exit(app.exec_())
