from vidstream import StreamingServer
import threading
from termcolor import colored

#Instance of the client that receives the screen sharing video
receiver = StreamingServer('127.0.0.1', 8080)

#Thread for reception of the screen sharing
t = threading.Thread(target=receiver.start_server)
t.start()

#Run until the user clicks q key
while input(colored('\nPress q to stop client execution', 'blue')+'\n')!='q':
    continue

#Stop the screen sharing
receiver.stop_server()