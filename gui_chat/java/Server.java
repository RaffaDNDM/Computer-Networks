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

    private boolean isUserAlreadyConnected(String username)
    {
        for(ClientThread ct : clientsList)
        {
            if(ct.username.compareTo(username)==0)
                return true;
        }

        return false;
    }

    public synchronized void registerUser(ClientThread ct)
    {   
        while (true)
        {
            try
            {
                String username = ct.fromClient.readLine();

                if (username.length()<5 || isUserAlreadyConnected(username))
                {
                    ct.toClient.println("NO");
                }
                else
                {
                    ct.toClient.println("OK");
                    ct.username = username;
                    clientsList.add(ct);
                    return;
                }
            }
            catch(IOException e)
            {
                return;
            }
        }
    }

    public synchronized void removeUser(ClientThread ct)
    {
        int index = clientsList.indexOf(ct);
        clientsList.remove(index);
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
                registerUser(this);

                for(ClientThread ct : clientsList)
                    System.out.print(ct+" ");

                System.out.print("\n");

                String online_users = "USERS\n";
                            
                for(ClientThread ct : clientsList)
                {
                    online_users += (ct.username + ":");
                }

                broadcast(online_users+"\nEND", false);
            }
            catch(IOException e)
            {
                return;
            }
        }

        public void run()
        {
            while(true)
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
                        else
                            final_msg += "\n";                        
                    }
    
                    System.out.println(final_msg);
    
                    if (final_msg.compareTo("__EXIT__\nEND") == 0)
                    {
                        removeUser(this);
                        
                        for(ClientThread ct : clientsList)
                            System.out.print(ct+" ");


                        String online_users = "USERS\n";
                            
                        for(ClientThread ct : clientsList)
                        {
                            online_users += (ct.username + ":");
                        }

                        broadcast(online_users+"\nEND", false);
                    }
                    else
                    {
                        broadcast(final_msg, true);
                    }
                }
                catch (IOException e)
                {
                    break;
                }   
            }   
        }

        public void broadcast(String msg, boolean withUsername)
        {
            for (ClientThread ct : clientsList)
            {
                if(withUsername)
                {
                    if (ct!=this)
                        ct.toClient.println(this.username+":"+msg);
                }
                else
                    ct.toClient.println(msg);
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