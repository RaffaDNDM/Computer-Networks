from vidstream import StreamingServer
import threading
from termcolor import colored

receiver = StreamingServer('127.0.0.1', 8080)

t = threading.Thread(target=receiver.start_server)
t.start()

while input(colored('\nPress q to stop client execution', 'blue')+'\n')!='q':
    continue

receiver.stop_server()