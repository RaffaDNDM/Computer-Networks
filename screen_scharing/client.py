from vidstream import ScreenShareClient
import threading
from termcolor import colored

#Instance of the client that shares the screen
client = ScreenShareClient('127.0.0.1', 8080)

#Thread for the screen sharing 
t = threading.Thread(target=client.start_stream)
t.start()

#Run until the user clicks q key
while input(colored('\nPress q to stop client execution', 'blue')+'\n')!='q':
    continue

#Stop the screen sharing
client.stop_stream()