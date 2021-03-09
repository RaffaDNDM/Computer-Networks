import sys
import socket

IP_ADDRESS = '216.58.206.36'
PORT = 80

def connection(IP_ADDRESS, PORT):
    if(len(sys.argv)==3):
        IP_ADDRESS = sys.argv[1]
        PORT = int(sys.argv[2])

    sd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sd.connect((IP_ADDRESS, PORT))

    return sd


def request(sd):
    cmd = 'GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n'
    sd.send(cmd.encode())    


def response(sd):
    response=''
    while(True):
        data = sd.recv(1)
        if len(data)<1:
            break
        response+=data.decode('utf-8', 'ignore')

        if response[-1]=="\n" and response[-4:]=="\r\n\r\n":
            break

    headers = response[:-4].split('\r\n')
    request_line = headers[0].split(' ')
    print(request_line)
    d = dict(x.split(":", 1) for x in headers[1:])
    
    for k,v in d.items():
        print(k+":"+v)

    body_length = 0
    if 'Content-Length' in d:
        body_length = int(d['Content-Length'])
    elif ('Transfer-Encoding' in d) and (d['Transfer-Encoding']==' chunked'):
        body_length = -1
    
    if body_length > 0:
        data = sd.recv(body_length)
        response=data.decode('utf-8', 'ignore')
    elif body_length == 0:
        data = sd.recv(10000)
        response=data.decode('utf-8', 'ignore')
    else:
        response = ''
        size = '1'
        while int(size,16)>0:
            data = ''
            size = ''
            while True:
                data = sd.recv(1).decode('utf-8', 'ignore')
                print(repr(data))

                #CRLF
                if data=="\r":
                    data = sd.recv(1).decode('utf-8', 'ignore')
                    if data == "\n":
                        break        
                else:
                    size += data
                
            #Remove chunk-extension
            size = size.split(";",1)[0]
            chunk_size = int(size, 16)
            print("Chunk size: "+size+">>>>>>"+str(chunk_size))
            
            while(chunk_size>0):
                data = sd.recv(chunk_size).decode('utf-8', 'ignore')
                chunk_size-=len(data)
                response += data
                #print(repr(response[-(int(size, 16)):]))

            #CRLF
            data = sd.recv(1).decode('utf-8', 'ignore')
            print(repr(data))
            data = sd.recv(1).decode('utf-8', 'ignore')
            print(repr(data))
        
        print(response)


def main():
    sd = connection(IP_ADDRESS, PORT)
    request(sd)
    response(sd)
    sd.close()

if __name__ == "__main__":
    print(sys.argv)
    main()
