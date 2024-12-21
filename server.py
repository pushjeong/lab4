import socket
import threading
import os

clients = {}  # {nickname: connection}

def broadcast(message, sender_conn):
    for client_conn in clients.values():
        if client_conn != sender_conn:
            client_conn.send(message)

def handle_client(conn, addr):
    try:
        nickname = conn.recv(1024).decode()  # 닉네임 바로 수신
        clients[nickname] = conn
        broadcast(f"{nickname} joined the chat!".encode(), conn)
        
        while True:
            msg = conn.recv(1024)
            if msg.startswith(b"/file"):  # 파일 전송 요청
                _, filename, filesize = msg.decode().split()
                filesize = int(filesize)
                
                # 파일 수신
                with open(f"received_{filename}", "wb") as f:
                    received = 0
                    while received < filesize:
                        data = conn.recv(1024)
                        received += len(data)
                        f.write(data)

                # 파일 전송 메시지 브로드캐스트
                broadcast(f"{nickname} sent a file: {filename}".encode(), conn)

            else:  # 일반 메시지
                broadcast(f"{nickname}: {msg.decode()}".encode(), conn)
    except:
        conn.close()
        if nickname in clients:
            del clients[nickname]
            broadcast(f"{nickname} left the chat.".encode(), conn)

def main():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(("0.0.0.0", 12345))
    server.listen(5)
    print("Server is running...")
    while True:
        conn, addr = server.accept()
        threading.Thread(target=handle_client, args=(conn, addr)).start()

if __name__ == "__main__":
    main()
