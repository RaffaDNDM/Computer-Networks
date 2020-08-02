import sys
import socket

IP_ADDRESS = '192.168.1.81'
PORT = 80

def connection(IP_ADDRESS, PORT):
    if(len(sys.argv)==3):
        IP_ADDRESS = sys.argv[1]
        PORT = int(sys.argv[2])

    sd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sd.connect((IP_ADDRESS, PORT))

    return sd


def request(sd):
    cmd = 'GET /prova.html HTTP/1.1\r\nHost: www.google.com\r\n\r\n'
    sd.send(cmd.encode())    


def response(sd):
    response=''
    while(True):
        data = sd.recv(512)
        if len(data)<1:
            break
        response+=data.decode('utf-8', 'ignore')
        
    parts_message = response.split('\r\n\r\n')
    headers = parts_message[0].split('\r\n')
    print(headers)
    request_line = headers[0].split(' ')
    print(request_line)
    d = dict(x.split(":", 1) for x in headers[1:])
    print(d)
    print(parts_message[1])


def main():
    sd = connection(IP_ADDRESS, PORT)
    request(sd)
    response(sd)
    sd.close()

if __name__ == "__main__":
    print(sys.argv)
    main()
