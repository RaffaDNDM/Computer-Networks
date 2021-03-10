import socket
import threading
from termcolor import cprint, colored

class Server:    
    """
    Chat server implementation.

    Args:
        IP_address (str): IP address of the server

        port (int): Port number of the server

    Attributes:
        sd (socket.socket): Socket descriptor of the server

        mutex (threading.Lock): Multithreading lock 

        ONLINE_CLIENTS (dict): dictionary of online clients, 
                               each one defined by a couple (k, v), where:
                               k: nickname of a client
                               v: socket descriptor of a client
    """

    ONLINE_CLIENTS = {}

    def __init__(self, IP_address, port):
        #Create socket descriptor
        self.sd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        #Bind the socket descriptor to the address specified
        self.sd.bind((IP_address, port))
        #Create lock for management of multithreading communication
        self.mutex = threading.Lock()

    def __enter__(self):
        '''
        Define listening queue size
        '''
        
        self.sd.listen(10)
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        #Close server socket descriptor
        self.sd.close()

    def run(self):
        """
        Start server.
        """

        while True:
            #Accept the connection of a specified client
            client_sd, client_address = self.sd.accept()

            #Create thread to manage a client
            t = threading.Thread(target=self.receive_msg, args=(client_sd, ))
            t.start()

    def receive_msg(self, client_sd):
        """
        Management of a single client.

        Args:
            client_sd (socket.socket): socket descriptor of the client
        """

        try:
            #Read messages from a client
            while True:
                #Read the nickname of the client message
                nickname = ''

                while True:  
                    nickname += (client_sd.recv(1).decode())
                    
                    if nickname[-2:]=='\r\n':
                        nickname = nickname[:-2]
                        break

                #Read the size of the client message
                size = ''

                while True:
                    size += (client_sd.recv(1).decode())
                    
                    if size.endswith('\r\n'):
                        size = size[:-2]
                        break

                if size == 'LOGGED':
                    if self.ONLINE_CLIENTS and nickname in self.ONLINE_CLIENTS:
                        #There is already a user with the specified nickname
                        client_sd.send(b'NO')
                        break
                    else:
                        #First login of the user with the specified nickname
                        client_sd.send(b'OK')

                    with self.mutex:
                        #First login, store the couple (nickname, socket descriptor)
                        self.ONLINE_CLIENTS[nickname] = client_sd

                    cprint(f'[{nickname} logged in]', 'red')
                    cprint('Online users: ','green', end=' ')
                    print(list(self.ONLINE_CLIENTS.keys()))

                elif size == 'QUIT':
                    cprint(f'[{nickname} logged out]', 'red')

                    with self.mutex:
                        #Remove the nickname specified because
                        #the user logged out 
                        self.ONLINE_CLIENTS.pop(nickname)

                    cprint('Online users: ','green', end=' ')
                    print(list(self.ONLINE_CLIENTS.keys()))

                    break
                
                else:
                    #If the size was a number, it is the size 
                    #of the message so read it
                    msg = client_sd.recv(int(size)).decode()
                    #Broadcast the msg to other online clients
                    self.broadcast(msg, nickname)

        except:
            pass
        
        #Close the connection with a client (if he logged out)
        client_sd.close()


    def broadcast(self, msg, nickname):
        '''
        Broadcast a message to other online clients with
        nicknames different from the specified nickname
        
        Args:
            msg (str): message to be broadcasted to other users
                       different from the one that sends the msg

            nickname (str): nickname of the user that sends the
                            message and wants to broadcast to others
        '''

        #Creation of message
        msg = msg.encode()
        final_msg = nickname.encode()+b'\r\n'+\
                    str(len(msg)).encode()+ b'\r\n'+\
                    msg

        #Broadcast message to users different from the one
        #that initially creates the message
        for k in self.ONLINE_CLIENTS:
            if k!=nickname:
                self.ONLINE_CLIENTS[k].send(final_msg)

def main():
    #Address of the server
    IP_ADDRESS = '127.0.0.1'
    PORT = 8080

    #Create the server and run it
    with Server(IP_ADDRESS, PORT) as s:
        s.run()


if __name__=='__main__':
    main()