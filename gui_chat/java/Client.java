import java.net.Socket;
import java.net.UnknownHostException;
import java.io.InputStreamReader;
import java.awt.Color;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.BufferedWriter;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.Scanner;

import java.awt.GridBagLayout;
import java.awt.GridBagConstraints;
import java.awt.BorderLayout;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextPane;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

public class Client 
{
    private BufferedReader fromServer;
    private PrintWriter toServer;
    private boolean running = true;
    private boolean firstClick = true;
    private JTextArea chatArea;
    private JTextArea usersArea;
    private JTextArea msgArea;
    private String username;
    private final String INSTRUCTION_MSG = "Write the msg!";

    public Client (String address, int port, String username)
    {
        this.username = username;
        createWindow();
        
        try
        {
            Socket sd = new Socket(address, port);
            fromServer = new BufferedReader(new InputStreamReader(sd.getInputStream()));
            toServer = new PrintWriter(new BufferedWriter(new OutputStreamWriter(sd.getOutputStream())), true);
            toServer.println(username);

            FromServerThread updateChat = new FromServerThread(fromServer);
            updateChat.start();
        }
        catch(UnknownHostException e1)
        {
            System.out.println("Unknown host");
        }
        catch(IOException e)
        {}
    }

    public void createWindow()
    {
        JFrame f = new JFrame();
        Container c = f.getContentPane();
        f.setMinimumSize(new Dimension(600,700));

        // Two column grid that fills the width of the page, each column taking up one half the space
        GridBagLayout layout = new GridBagLayout();
        GridBagConstraints constraints = new GridBagConstraints();
        c.setLayout(layout);

        constraints.fill = GridBagConstraints.BOTH;
        constraints.weightx = 0.7;
        constraints.weighty = 0.8;
        constraints.gridx = 0;
        constraints.gridy = 0;
        chatArea = new JTextArea("");

        //chatArea.append("x\n");
        
        chatArea.setEditable(false);
        JScrollPane chat = new JScrollPane(chatArea, 
                                            JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                                            JScrollPane.HORIZONTAL_SCROLLBAR_NEVER );
        layout.setConstraints(chat, constraints);
        c.add(chat, constraints);
        chat.setBorder(BorderFactory.createEmptyBorder(10,10,10,10));

        constraints.fill = GridBagConstraints.BOTH;
        constraints.weightx = 0.3;
        constraints.weighty = 0.8;
        constraints.gridx = 1;
        constraints.gridy = 0;
        usersArea = new JTextArea(username);
        
        usersArea.setEditable(false);
        JScrollPane users = new JScrollPane(usersArea, 
                                            JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                                            JScrollPane.HORIZONTAL_SCROLLBAR_NEVER );

        c.add(users, constraints);
        users.setBorder(BorderFactory.createEmptyBorder(10,10,10,10));
        
        constraints.fill = GridBagConstraints.BOTH;
        constraints.weightx = 0.7;
        constraints.weighty = 0.2;
        constraints.gridx = 0;
        constraints.gridy = 1;
        msgArea = new JTextArea(INSTRUCTION_MSG);
        JScrollPane message = new JScrollPane(msgArea, 
                                                JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                                                JScrollPane.HORIZONTAL_SCROLLBAR_NEVER );

        c.add(message, constraints);
        message.setBorder(BorderFactory.createEmptyBorder(10,10,10,10));

        msgArea.addMouseListener(new MouseAdapter()
        {
            public void mouseClicked(MouseEvent me)
            {
                if (firstClick) 
                {
                    firstClick = false;
                    msgArea.selectAll();
                    msgArea.replaceSelection("");
                }
            }
        });

        constraints.fill = GridBagConstraints.BOTH;
        constraints.weightx = 0.3; //Default
        constraints.weighty = 0.2; //Default
        constraints.gridx = 1;
        constraints.gridy = 1;
        JPanel panel = new JPanel();
        JButton sendButton = new JButton("Send");

        c.add(panel, constraints);
        panel.setLayout(new BorderLayout());
        panel.add(sendButton, BorderLayout.CENTER);

        sendButton.addActionListener(new ActionListener() {

            @Override
            public void actionPerformed(ActionEvent e) {
                //your actions
                String msg = msgArea.getText();
                System.out.print(msg);
                toServer.println(msg+"\nEND");
                firstClick = true;
                msgArea.selectAll();
                msgArea.replaceSelection(INSTRUCTION_MSG);
                chatArea.append(username+": "+msg+"\n");
            }
        });

        f.setVisible(true);
        f.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent evt) {
              onExit();
            }
        });
    }

    public void onExit() {
        running = false;
        toServer.println("__EXIT__\nEND");

        System.err.println("Exit");
        System.exit(0);
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
            try
            {
                sleep(2);
            }
            catch(InterruptedException e)
            {}
            
            while (running)
            {
                try
                {
                    String final_msg = "";
                    String msg = "";
                    boolean USERS = false;

                    while (true)
                    {
                        msg = fromServer.readLine();
                        
                        if (msg.compareTo("END")==0)
                            break;
                        else
                            final_msg += (msg+"\n");
                    }

                    System.out.println(final_msg);

                    if (final_msg.startsWith("USERS"))
                    {
                        final_msg = final_msg.substring(6);
                        String online_users[] = (final_msg.split("\n")[0]).split(":");

                        usersArea.selectAll();
                        usersArea.replaceSelection("");        

                        for(String x : online_users)
                            usersArea.append(x+"\n");
                    }                    
                    else
                    {
                        System.out.println(final_msg);
                        chatArea.append(final_msg);
                    }
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