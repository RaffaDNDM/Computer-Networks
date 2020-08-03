import sys
import socket
import threading
import os

PORT = 8080
DAT_FOLDER = "../dat"
RESULTS = "results.txt"

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

    #Parser of request
    try:
        request_parts = request.split('\r\n\r\n')
        headers = request_parts[0].split('\r\n')
        print("\n"+str(headers)+"\n")
        request_line = headers[0].split(' ')
        print("\n"+str(request_line)+"\n")
        d = dict(x.split(':', 1) for x in headers[1:])
        print("\n"+str(d)+"\n")

        file_path=''

        if request_line[0]=="GET":
            if request_line[1][:9]=="/cgi-bin/":
                function = request_line[1][9:].split("?")
                args = dict(x.split('=', 1) for x in function[1].split("&"))
                cmd = "cd "+DAT_FOLDER+"; "+function[0]
                
                for value in args.values():
                    cmd += (" "+value)

                file_path = DAT_FOLDER+"/"+RESULTS
                cmd += (" > "+file_path)
        
                print("\n"+cmd+"\n")
                
                if os.system(cmd)==0:
                    status_line = "HTTP/1.1 200 OK\r\n"
                else:
                    status_line = "HTTP/1.1 400 Bad Request\r\n\r\n"
                    file_path=''

                sd.send(status_line.encode())

            elif os.path.exists(DAT_FOLDER+request_line[1]):
                status_line = "HTTP/1.1 200 OK\r\n"
                sd.send(status_line.encode())
                file_path = DAT_FOLDER+request_line[1]
                
            else:
                status_line = "HTTP/1.1 404 Not Found\r\n\r\n"
                sd.send(status_line.encode())
        else:
            status_line = "HTTP/1.1 501 Not Implemented\r\n\r\n"
            sd.send(status_line.encode()) 
                
        if not file_path=='':
            with open(file_path, "r") as f:
                x=f.read().encode()
                            
            y="Content-Length: "+str(len(x))+"\r\n\r\n"
            sd.send(y.encode())
            sd.send(x)

        sd.close()

    except KeyboardInterrupt:
        print("Shutdown\n")
    except Exception as e:
        print(e)


def start_server(sd):
    while(1):
        try:
            client_sd, addr = sd.accept()
            cl = threading.Thread(target=handle_client, args=(client_sd, addr))
            cl.start()
        except KeyboardInterrupt:
            print("Shutdown\n")
            return
        except Exception as e:
            print(e)


def main():
    sd = create_server(PORT)
    start_server(sd)


if __name__ == "__main__":
    main()