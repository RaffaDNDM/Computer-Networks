import socket
import threading
from termcolor import colored
import tkinter
from tkinter import simpledialog
import tkinter.scrolledtext
from tkinter import messagebox
import time
import sys

class Client:
    """
    Chat client implementation.

    Args:
        IP_address (str): IP address of the server

        port (int): Port number of the server

    Attributes:
        sd (socket.socket): Socket descriptor of the client

        mutex (threading.Lock): Multithreading lock 

        NICKNAME (str): Nickname of the client

        COLOR_OTHER_USERS_NICKNAME (str): Foreground color of the
                                          nickname of a remote client

        COLOR_OTHER_USERS_TEXT (str): Background color of the message
                                      sent by a remote client
    
        COLOR_USER_NICKNAME (str): Foreground color of the nickname
                                   of the client

        COLOR_USER_TEXT (str): Background color of the message sent
                               by the client

        __MIN_LENGTH_NICKNAME (int): Minimum number of characters
                                     that compose a nickname    

        win (tkinter.Tk): Window of the chat

        chat_label (tkinter.Label): Label of the chat

        chat_area (tkinter.scrolledtext.ScrolledText): Text box for the
                                                       chat messages

        msg_label (tkinter.Label): Label of the message box

        msg_area.pack (tkinter.Text): Text box for sending a message from 
                                      the client

        send_button (tkinter.Button): Button for sending the specified 
                                      message in msg_area

    """

    #Color of remote clients messages in the chat
    COLOR_OTHER_USERS_NICKNAME = 'blue4'
    COLOR_OTHER_USERS_TEXT = 'RoyalBlue1'
    
    #Color of the client messages in the chat
    COLOR_USER_NICKNAME = 'dark green'
    COLOR_USER_TEXT = 'yellow green'

    #Minimum number of characters that compose a nickname
    __MIN_LENGTH_NICKNAME = 5

    def __init__(self, IP_address, port):
        self.mutex = threading.Lock()
        response = ''

        #Wait until the client inserts a valid nickname 
        while response != 'OK':
            self.sd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sd.connect((IP_address, port))

            #Nickname insertion
            login_win = tkinter.Tk()
            login_win.withdraw()
            self.NICKNAME = simpledialog.askstring('Nickname', 'Insert nickname', parent=login_win)
            
            #The nickname of the client must be composed of at least
            #self.__MIN_LENGTH_NICKNAME characters
            if len(self.NICKNAME) < self.__MIN_LENGTH_NICKNAME:
                messagebox.showwarning('Too short nickname', 'Use a nickname of at least 5 characters')
            else:
                #Communicate the nickname to the server
                msg = f'{self.NICKNAME}\r\nLOGGED\r\n'
                self.sd.send(msg.encode())
                response = self.sd.recv(2).decode()

                if response == 'NO':
                    #User is already logged, change nickname
                    messagebox.showerror('User already logged in', 'Please, use another nickname')
                else:
                    #Nickname inserted is ok
                    messagebox.showinfo('Logged in', f'Hello {self.NICKNAME}!')

        #Creation of the chat window                
        gui = threading.Thread(target=self.gui_manage)
        gui.start()

        #Client is running
        self.RUNNING = True

    def on_click(self, event=None):
        '''
        Actions to be performed when the message box is clicked
        by the client.
        '''

        #Remove the help message in the message box
        self.msg_area.delete('1.0', 'end')

    def gui_manage(self):
        '''
        Creation of the GUI window.
        '''

        #Create the window
        self.win = tkinter.Tk()
        self.win.configure(bg='black')
        
        #Label of the chat
        self.chat_label = tkinter.Label(self.win, text='Chat')
        self.chat_label.config(fg='white', bg='black')
        self.chat_label.pack(padx=10, pady=10)

        #Box for chat messages
        self.chat_area = tkinter.scrolledtext.ScrolledText(self.win)
        self.chat_area.pack(padx=10, pady=10)
        #No possibility to modify the messages inside it
        self.chat_area.config(state='disabled')

        #Label for the msg box of the user
        self.msg_label = tkinter.Label(self.win, text=f'{self.NICKNAME}')
        self.msg_label.config(fg='white', bg='black')
        self.msg_label.pack(padx=10, pady=10)

        #Box to insert a message and sent to other users
        self.msg_area = tkinter.Text(self.win, height=4)
        self.msg_area.config(font=("Monospace", "8"))
        self.msg_area.pack(padx=10, pady=10)
        #Help for the user
        self.msg_area.insert('end', 'Insert the message here...')
        #Event if the user clicks to insert the message
        self.msg_area.bind('<Button-1>', self.on_click)

        #Button to send the message
        self.send_button = tkinter.Button(self.win, text='Send', command=self.send_data)
        self.send_button.pack(padx=10, pady=10)

        #Even for the management of the click of X button on the window
        self.win.protocol("WM_DELETE_WINDOW", self.on_close)

        #Show the window
        self.win.mainloop()

    def on_close(self):
        '''
        Actions to be performed when close (X) button of
        the window is pressed.
        '''

        #Notify to server that the user was logged out
        msg = f'{self.NICKNAME}\r\nQUIT\r\n'
        self.sd.send(msg.encode())
        self.sd.close()

        #Destroy the window
        self.win.destroy()

        #Stop the main thread        
        with self.mutex:
            self.RUNNING = False

    def send_data(self):
        '''
        Send the message inserted by the client in the message area.
        '''
        
        try:
            #Read the message inserted by the client
            msg = self.msg_area.get('1.0', 'end')
            #Write the help message for the user
            self.msg_area.insert('end', 'Insert the message here...')
            
            with self.mutex:
                #Update chat messages, inserting the message that the client sent
                self.chat_area.config(state='normal')
                self.chat_area.insert('end', f'{self.NICKNAME}\n', 'my_nickname')
                self.chat_area.insert('end', msg, 'my_text')
                self.chat_area.insert('end', '\n')
                self.chat_area.tag_config('my_nickname', foreground=self.COLOR_USER_NICKNAME, font=("Monospace", "10", "bold"))
                self.chat_area.tag_config('my_text', background=self.COLOR_USER_TEXT, font=("Monospace", "8"))
                self.chat_area.config(state='disabled')
            
            #Send the message to other clients
            final_msg = f'{self.NICKNAME}\r\n{len(msg.encode())}\r\n'
            self.sd.send(final_msg.encode()+msg.encode())

        except KeyboardInterrupt:
            #Notify to server that the user was logged out
            msg = f'{self.NICKNAME}\r\nQUIT\r\n'
            self.sd.send(msg.encode())

    def receive_data(self):
        '''
        Wait messages from other remote clients.
        '''

        #Wait until GUI is created
        time.sleep(2)
        
        try:
            while True:
                #Read the nickname of the remote client that sent the message
                nickname = ''

                while True:  
                    nickname += (self.sd.recv(1).decode())
                    
                    if nickname[-2:] == '\r\n':
                        nickname = nickname[:-2]
                        break

                #Read the size of the message that the remote client sent
                size = ''
                while True:
                    size += (self.sd.recv(1).decode())
                    
                    if size[-2:] == '\r\n':
                        size = size[:-2]
                        break
                
                #Read the message
                msg = self.sd.recv(int(size)).decode()
                
                with self.mutex:
                    #Update chat messages, inserting the message that the client sent
                    self.chat_area.config(state='normal')
                    self.chat_area.insert('end', f'{nickname}\n', 'nickname')
                    self.chat_area.insert('end', msg, 'text')
                    self.chat_area.insert('end', '\n')
                    self.chat_area.tag_config('nickname', foreground=self.COLOR_OTHER_USERS_NICKNAME, font=("Monospace", "10", "bold"))
                    self.chat_area.tag_config('text', background=self.COLOR_OTHER_USERS_TEXT, font=("Monospace", "8"))
                    self.chat_area.config(state='disabled')
        except:
            return

    def run_chat(self):
        '''
        Run the GUI chat.
        '''

        #Receive data from the server on secondary thread
        receiver = threading.Thread(target=self.receive_data)
        receiver.start()

        #Closed window, terminate program
        while self.RUNNING:
            pass

        if not self.RUNNING:
            self.sd.close()

def main():
    #Address of the server
    IP_ADDRESS = '127.0.0.1'
    PORT = 8080

    #Create the client instance
    c = Client(IP_ADDRESS, PORT)
    #Create the chat and run it
    c.run_chat()

if __name__=='__main__':
    main()