from vidstream import CameraClient
from vidstream import StreamingServer
import threading
import time
from termcolor import colored

server_IP = '192.168.0.2'
client_IP = '192.168.0.3'
port = 8080

#Initialize camera client
camera = CameraClient(client_IP, port)
#Initialize server side
server = StreamingServer(server_IP, port)

#Start server
server_t = threading.Thread(target=server.start_server)
server_t.start()

#Give time to the client to start the program
time.sleep(2)

#Start client
client_t = threading.Thread(target=camera.start_stream)
client_t.start()

#Stop the camera chat when q is detected on main thread
while input(colored('\nPress q to stop client execution', 'blue')+'\n')!='q':
    continue

server.stop_server()
camera.stop_stream()