Web server that replies to an HTTP request, made by client.
The server looks for the file, required by the client, in its File System.
The server needs also to manage a possible request for the files in the folder
"/cgi-bin/", for which it needs to execute the command. The command to be executed
is the name of the "file" contained in the folder "/cgi-bin/". The result of this
command needs to be stored that needs to be send back to the client, instead of
sending an normal HTML file.
