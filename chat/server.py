import socket
import threading

class Server:
    ONLINE_CLIENTS = {}

    def __init__(self, IP_address, port):
        self.sd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sd.bind((IP_address, port))
        self.mutex = threading.Lock()

    def __enter__(self):
        self.sd.listen(10)
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.sd.close()

    def run(self):

        while True:
            client_sd, client_address = self.sd.accept()

            t = threading.Thread(target=self.receive_msg, args=(client_sd, ))
            t.start()

    def receive_msg(self, client_sd):
        count=0

        while True:
            nickname = ''

            while True:  
                nickname += (client_sd.recv(1).decode())
                
                if nickname[-2:]=='\r\n':
                    nickname = nickname[:-2]
                    break

            #print(nickname, end='[') 

            size = ''

            while True:
                size += (client_sd.recv(1).decode())
                
                if size.endswith('\r\n'):
                    size = size[:-2]
                    break

            #print(size, end='] ---> ')
            
            if size == 'LOGGED':
                print(nickname, client_sd, end='\n\n')
                if self.ONLINE_CLIENTS and nickname in self.ONLINE_CLIENTS:
                    client_sd.send(b'NO')
                    break
                else:
                    client_sd.send(b'OK')

                with self.mutex:
                    self.ONLINE_CLIENTS[nickname] = client_sd

            elif size == 'QUIT':
                self.ONLINE_CLIENTS.pop(nickname)
                break
            
            else:
                msg = client_sd.recv(int(size)).decode()
                self.broadcast(msg, nickname)
                print(msg)
        
        client_sd.close()

    def broadcast(self, msg, nickname):
        msg = msg.encode()
        final_msg = nickname.encode()+b'\r\n'+\
                    str(len(msg)).encode()+ b'\r\n'+\
                    msg

        for k in self.ONLINE_CLIENTS:
            if k!=nickname:
                #print(k, self.ONLINE_CLIENTS[k])
                self.ONLINE_CLIENTS[k].send(final_msg)

def main():
    IP_ADDRESS = '127.0.0.1'
    PORT = 8080

    with Server(IP_ADDRESS, PORT) as s:
        s.run()


if __name__=='__main__':
    main()