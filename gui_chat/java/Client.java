import java.net.Socket;
import java.net.UnknownHostException;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.BufferedWriter;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.Scanner;

public class Client 
{
    private BufferedReader fromServer;
    private PrintWriter toServer;
    private boolean running = true;

    public Client (String address, int port, String username)
    {
        try
        {
            Socket sd = new Socket(address, port);
            fromServer = new BufferedReader(new InputStreamReader(sd.getInputStream()));
            toServer = new PrintWriter(new BufferedWriter(new OutputStreamWriter(sd.getOutputStream())), true);
            toServer.println(username);

            FromServerThread updateChat = new FromServerThread(fromServer);
            updateChat.start();

            while (true)
            {
                Scanner in = new Scanner(System.in);
                String msg = in.nextLine();
                toServer.println(msg+"\nEND");
            }
        }
        catch(UnknownHostException e1)
        {
            System.out.println("Unknown host");
        }
        catch(IOException e)
        {}
    }

    public void printMsg(String username, String message, boolean me)
    {

    }

    private class FromServerThread extends Thread
    {   
        private BufferedReader buffer;

        public FromServerThread(BufferedReader buffer)
        {
            this.buffer = buffer;
        }

        public void run()
        {
            while (running)
            {
                try
                {
                    String line = buffer.readLine();
                    System.out.println(line);
                }
                catch(IOException e)
                {

                }
            }
        }
    }

    public static void main(String[] args) 
    {
        String serverAddress = "127.0.0.1";
        int port = 8080;

        Client c = new Client(serverAddress, port, args[0]);
    }
}