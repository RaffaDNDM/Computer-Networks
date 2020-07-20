import socket
import threading

port = 8080

if(len(sys.argv)==1)
    port = sys.argv

server = socket.gethostbyname(socket.gethostname())

sd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((server, port))

#Management of a single client request
def manage_client(conn, addr):
    msg = conn.recv(1024)

def start_server():
    sd.listen()

    while True:
        conn, addr = sd.accept()
        thread = threading.Thread(target=manage_client, args=(conn, addr))
        thread.start()
        print(f"[CONNECTED CLIENTS] {threading.activeCount()-1}")

print
