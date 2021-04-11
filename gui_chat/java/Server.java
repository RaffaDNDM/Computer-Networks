import java.util.ArrayList;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

public class Server
{
    private ArrayList<ClientThread> clientsList;

    public Server()
    {
        clientsList = new ArrayList<>();
    }

    public synchronized void registerUser(ClientThread ct)
    {
        clientsList.add(ct);
    }

    public synchronized void removeUser(ClientThread ct)
    {
        clientsList.add(ct);
    }

    private class ClientThread extends Thread
    {
        public String username;
        public BufferedReader fromClient;
        public PrintWriter toClient;

        public ClientThread(Socket sd)
        {
            try
            {
                fromClient = new BufferedReader(new InputStreamReader(sd.getInputStream()));
                toClient = new PrintWriter(new BufferedWriter(new OutputStreamWriter(sd.getOutputStream())), true);

                username = fromClient.readLine();
                System.out.println(username);
                registerUser(this);
            }
            catch(IOException e)
            {}
        }

        public void run()
        {
            while (true)
            {
                receiveMsg();
            }
        }

        public void receiveMsg()
        {
            try
            {
                String final_msg = "";
                String msg = "";
            
                while (true)
                {
                    msg = fromClient.readLine();
                    final_msg += msg;

                    if (msg.compareTo("END")==0)
                        break;
                        
                }

                System.out.println(final_msg);

                if (final_msg.compareTo("__EXIT__") == 0)
                {
                    removeUser(this);
                }
                else
                {
                    broadcast(final_msg);
                }
            }
            catch (IOException e)
            {

            }
            
        }

        public void broadcast(String msg)
        {
            for (ClientThread ct : clientsList)
            {
                if (ct!= this)
                    ct.toClient.println(this.username+":"+msg);
            }
        }
    }

    public static void main(String[] args) 
    {
        int port = 8080;
        Server listener = new Server();

        try(ServerSocket s = new ServerSocket(port))
        {
            while(true)
            {
                System.out.println("Started");
                Socket client = s.accept();
                ClientThread ct = listener.new ClientThread(client);
                ct.start();
            }
        }
        catch (IOException e)
        {
            System.out.println("[Exception] "+e.getStackTrace());
        }
    }    
}