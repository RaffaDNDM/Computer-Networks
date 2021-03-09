from vidstream import ScreenShareClient
import threading
from termcolor import colored

client = ScreenShareClient('127.0.0.1', 8080)

t = threading.Thread(target=client.start_stream)
t.start()

while input(colored('\nPress q to stop client execution', 'blue')+'\n')!='q':
    continue

client.stop_stream()