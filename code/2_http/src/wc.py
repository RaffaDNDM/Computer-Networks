import socket
import sys

#Remote machine
address="www.google.com"
port = 80

if(len(sys.argv)==2):
    address, port = sys.argv

#Definition of the socket
sd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

#Connection to remote server
sd.connect((address, port))

#Writing the HTTP/1.1 request
request = 'GET / HTTP/1.0\r\nHost: '+address+'\r\n\r\n'
sd.send(request.encode()) #to convert in byte notation

#Reading the response (concatenation of received messages)
response = sd.recv(1024)

while len(response)>0:
    print(response)
    response = sd.recv(1024)

#Closing the socket
sd.close()
