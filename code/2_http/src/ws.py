import sys
import socket
import threading
import os

PORT = 8080

def create_server(PORT):
    if(len(sys.argv)==2):
        PORT = int(sys.argv[1])

    sd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        sd.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sd.bind(('', PORT))
        sd.listen(5)
    except KeyboardInterrupt:
        print("Shutdown\n")
    except Exception as e:
        print(e)

    return sd


def handle_client(sd, addr):
    print("Request from "+str(addr))
    request = sd.recv(2000).decode('utf-8', 'ignore')

    #Parser of request)
    try:
        headers = request.split('\r\n')
        print(headers)
        request_line = headers[0].split(' ')
        print(request_line)
        d = dict(x.split(":") for x in headers[1:-2])
        print(d)

        if os.path.exists(request_line[1][1:]):
            status_line = "HTTP/1.1 200 OK\r\n"
            sd.send(status_line.encode())

            with open(request_line[1][1:], "r+") as f:
                x=f.read().encode()
            
            y="Content-Length: "+str(len(x))+"\r\n\r\n"
            sd.send(y.encode())
            sd.send(x)

        else:
            status_line = "HTTP/1.1 404 Not Found\r\n\r\n"
            sd.send(status_line.encode())

        sd.close()

    except KeyboardInterrupt:
        print("Shutdown\n")
    except Exception as e:
        print(e)


def start_server(sd):
    while(1):
        client_sd, addr = sd.accept()
        print("ciao")
        cl = threading.Thread(target=handle_client, args=(client_sd, addr))
        cl.start()


def main():
    sd = create_server(PORT)
    start_server(sd)


if __name__ == "__main__":
    main()