# GUI multi-users chat
Chat with central server and graphical applications, one for each client. The number of clients could be greater than 2.

```bash
pip3 install termcolor
```
or<br>
```bash
pip3 install -r requirements.txt
```
To run the server in a Docker container, you need to type the following commands on terminal:
```bash
docker build -t username/server .
docker run -p 8080:10000 -it username/server
```
with:<br>
**-t tagname** to specify a tagname to identify the container<br>
**-it** only if you want to see debugging info while running the container<br>
**-p 8080:10000** to map port **8080** of the host to the port **10000** of the container<br><br>
or<br><br>
```bash
docker build -t username/server .
docker run -p 8080:10000 -d username/server
```
**-d** to start the contained and running the server as a deamon (in background).