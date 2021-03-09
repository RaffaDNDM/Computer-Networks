import socket
import threading
from termcolor import colored

class Client:

    def __init__(self, IP_address, port):
        self.sd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sd.connect((IP_address, port))
        self.mutex = threading.Lock()

    def __enter__(self):
        self.NICKNAME = input(colored('Insert your nickname: ', 'red'))
        msg = self.NICKNAME.encode()+b'\r\nLOGGED\r\n'
        self.sd.send(msg)
        
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.sd.close()

    def send_data(self):
        try:
            while True:
                msg = input(f'{self.NICKNAME}>: ')
                
                with self.mutex:
                    print(colored(f'{self.NICKNAME}', 'green', attrs=['bold',]))
                    print(msg, end='\n\n')
                
                msg = msg.encode()
                final_msg = self.NICKNAME.encode() + b'\r\n' +\
                            str(len(msg)).encode() + b'\r\n' +\
                            msg

                self.sd.send(final_msg)

        except KeyboardInterrupt:
            msg = f'{self.NICKNAME}\r\nQUIT\r\n'
            self.sd.send(msg.encode())

    def receive_data(self):
        while True:
            nickname = ''

            while True:  
                nickname += self.sd.recv(1).decode()
                
                if nickname[-2:] == '\r\n':
                    nickname = nickname[:-2]
                    break

            size = ''
            while True:
                size += self.sd.recv(1).decode()
                
                if size[-2:] == '\r\n':
                    size = size[:-2]
                    break

            msg = self.sd.recv(int(size)).decode()

            with self.mutex:
                print(colored(f'\r{self.NICKNAME}', 'blue', attrs=['bold',]))
                print(msg, end='\n\n')
                print(f'{self.NICKNAME}>: ')

    def run_chat(self):
        #Receive data on secondary thread
        receiver = threading.Thread(target=self.receive_data)
        receiver.start()
        #Send data on main thread 
        self.send_data()

def main():
    IP_ADDRESS = '127.0.0.1'
    PORT = 8080

    with Client(IP_ADDRESS, PORT) as c:
        c.run_chat()


if __name__=='__main__':
    main()