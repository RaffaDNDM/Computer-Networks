import socket
import threading
from termcolor import colored
import tkinter
from tkinter import simpledialog
import tkinter.scrolledtext
import time

class Client:

    def __init__(self, IP_address, port):
        self.mutex = threading.Lock()
        response = ''

        while True:
            self.sd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sd.connect((IP_address, port))

            login_win = tkinter.Tk()
            login_win.withdraw()
            #Wait until the nickname is not already inserted
            self.NICKNAME = simpledialog.askstring('Nickname', 'Insert nickname', parent=login_win)
            #self.NICKNAME = input(colored('Insert your nickname: ', 'red'))
            msg = f'{self.NICKNAME}\r\nLOGGED\r\n'
            self.sd.send(msg.encode())
            response = self.sd.recv(2).decode()
            
            if response != 'OK':
                self.sd.close()
            else:
                break
        
        gui = threading.Thread(target=self.gui_manage)
        gui.start()

    def gui_manage(self):
        self.win = tkinter.Tk()
        self.win.configure(bg='black')
        
        self.chat_label = tkinter.Label(self.win, text='Chat')
        self.chat_label.config(fg='white', bg='black')
        self.chat_label.pack(padx=10, pady=10)
        
        self.chat_area = tkinter.scrolledtext.ScrolledText(self.win)
        self.chat_area.pack(padx=10, pady=10)
        self.chat_area.config(state='disabled')

        self.msg_label = tkinter.Label(self.win, text='Insert a message')
        self.msg_label.config(fg='white', bg='black')
        self.msg_label.pack(padx=10, pady=10)

        self.msg_area = tkinter.Text(self.win, height=4)
        self.msg_area.pack(padx=10, pady=10)

        self.send_button = tkinter.Button(self.win, text='Send', command=self.send_data)
        self.send_button.pack(padx=10, pady=10)

        self.win.protocol("WM_DELETE_WINDOW", self.on_close)
        self.win.mainloop()

    def on_close(self):
        msg = f'{self.NICKNAME}\r\nQUIT\r\n'
        self.sd.send(msg.encode())
        self.sd.close()
        self.win.destroy()

    def send_data(self):
        try:
            msg = self.msg_area.get('1.0', 'end')
            self.msg_area.delete('1.0', 'end')
            #msg = input(f'{self.NICKNAME}>: ')
            
            with self.mutex:
                #print(colored(f'{self.NICKNAME}', 'green', attrs=['bold',]))
                #print(msg, end='\n\n')
                self.chat_area.config(state='normal')
                self.chat_area.insert('end', f'{self.NICKNAME}\n', 'nickname2')
                self.chat_area.insert('end', msg, 'text')
                self.chat_area.insert('end', '\n')
                self.chat_area.tag_config('nickname2', foreground='green')
                self.chat_area.tag_config('text', background='gray')
                self.chat_area.config(state='disabled')

            msg = msg
            final_msg = f'{self.NICKNAME}\r\n{len(msg.encode())}\r\n'

            self.sd.send(final_msg.encode()+msg.encode())

        except KeyboardInterrupt:
            msg = f'{self.NICKNAME}\r\nQUIT\r\n'
            self.sd.send(msg.encode())

    def receive_data(self):
        time.sleep(2)
        
        while True:
            nickname = ''

            while True:  
                nickname += (self.sd.recv(1).decode())
                
                if nickname[-2:] == '\r\n':
                    nickname = nickname[:-2]
                    break

            print(nickname)

            size = ''
            while True:
                size += (self.sd.recv(1).decode())
                
                if size[-2:] == '\r\n':
                    size = size[:-2]
                    break
            
            print(size)

            msg = self.sd.recv(int(size)).decode()
            print(msg)
            
            with self.mutex:
                #print(colored(f'\r{self.NICKNAME}', 'blue', attrs=['bold',]))
                #print(msg, end='\n\n')
                #print(f'{self.NICKNAME}>: ')
                print(msg)
                self.chat_area.config(state='normal')
                self.chat_area.insert('end', f'{nickname}\n', 'nickname')
                self.chat_area.insert('end', msg, 'text')
                self.chat_area.insert('end', '\n')
                self.chat_area.tag_config('nickname', foreground='blue')
                self.chat_area.tag_config('text', background='gray')
                self.chat_area.config(state='disabled')

    def run_chat(self):
        #Receive data on secondary thread
        #receiver = threading.Thread(target=self.receive_data)
        #receiver.start()

        self.receive_data()
        #Send data on main thread 
        #self.send_data()

def main():
    IP_ADDRESS = '127.0.0.1'
    PORT = 8080

    c = Client(IP_ADDRESS, PORT)
    c.run_chat()


if __name__=='__main__':
    main()