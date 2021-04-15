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
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.text.BadLocationException;
import javax.swing.text.DefaultHighlighter;
import javax.swing.text.Highlighter;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.AttributeSet;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyledDocument;

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
    private boolean firstClickUser = true;
    private JTextPane chatArea;
    private JTextPane usersArea;
    private JTextArea msgArea;
    private String username = "";
    private final String INSTRUCTION_MSG = "Write the msg!";
    //private SimpleAttributeSet chatStyle = new SimpleAttributeSet();
    //private SimpleAttributeSet usersStyle = new SimpleAttributeSet();
    private SimpleAttributeSet meChatName = new SimpleAttributeSet();
    private SimpleAttributeSet otherChatName = new SimpleAttributeSet();
    private SimpleAttributeSet meChatMsg = new SimpleAttributeSet();
    private SimpleAttributeSet otherChatMsg = new SimpleAttributeSet();
    private SimpleAttributeSet meUsers = new SimpleAttributeSet();
    private SimpleAttributeSet otherUsers = new SimpleAttributeSet();
    private SimpleAttributeSet defaultAttribute = new SimpleAttributeSet();

    //Background color of chat
    //private final Color BACKGROUND_CHAT = new Color(96,96,96);

    //Background color for on-line users pane
    //private final Color BACKGROUND_USERS = new Color(32,32,32);

    //Color of chat messages
    private final Color BACKGROUND_OTHER_NAME = new Color(255,255,255);
    private final Color BACKGROUND_OTHER_MSG = new Color(0,102,51);
    private final Color FOREGROUND_OTHER_NAME = new Color(0,51,0);
    private final Color FOREGROUND_OTHER_MSG = new Color(255,255,255);
    private final Color BACKGROUND_ME_NAME = new Color(255,255,255);
    private final Color BACKGROUND_ME_MSG = new Color(0,76,153);
    private final Color FOREGROUND_ME_NAME = new Color(0,51,102);
    private final Color FOREGROUND_ME_MSG = new Color(255,255,255);  
    
    //Colors of on-line users
    private final Color FOREGROUND_OTHER_USERS = new Color(139,0,139);
    private final Color FOREGROUND_ME_USERS = new Color(244,70,17);

    public Client (String address, int port)
    {   
        try
        {
            Socket sd = new Socket(address, port);
            fromServer = new BufferedReader(new InputStreamReader(sd.getInputStream()));
            toServer = new PrintWriter(new BufferedWriter(new OutputStreamWriter(sd.getOutputStream())), true);
            usernameWindow(this);

            while(this.username.compareTo("")==0)
            {
                System.out.print("");
            }
            
            System.out.println("HERE");
            chatWindow();
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

    public void usernameWindow(Client ct)
    {
        String usernameMsg = "";
        JFrame window = new JFrame("Login");
        window.setMinimumSize(new Dimension(300,130));

        Container c = window.getContentPane();
        GridBagLayout layout = new GridBagLayout();
        GridBagConstraints constraints = new GridBagConstraints();
        c.setLayout(layout);

        JPanel userPanel = new JPanel();
        JLabel userLabel = new JLabel("Username: ");
        JTextField userField = new JTextField(20);
        constraints.gridx = 0;
        constraints.gridy = 0;
        userPanel.add(userLabel);
        userPanel.add(userField);
        c.add(userPanel, constraints);
       
        userField.addMouseListener(new MouseAdapter()
        {
            public void mouseClicked(MouseEvent me)
            {
                if (firstClickUser)
                {
                    firstClickUser = false;
                    userField.selectAll();
                    userField.replaceSelection("");
                }
            }
        });

        JButton submit = new JButton("Username");
        constraints.gridx = 0;
        constraints.gridy = 1;
        c.add(submit, constraints);

        submit.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                //your actions
                String usernameMsg = userField.getText();
            
                while(usernameMsg.endsWith("\n"))
                    usernameMsg = usernameMsg.substring(0, usernameMsg.length()-1);
        
                toServer.println(usernameMsg);
                firstClickUser = true;

                userField.selectAll();
                userField.replaceSelection("Username");
                System.out.println(usernameMsg);
                
                try
                {
                    String response = fromServer.readLine();
                    
                    if (response.compareTo("OK")==0)
                    {
                        System.out.println("OK");
                        setUsername(usernameMsg);
                        window.dispose();
                    }
                    else
                    {
                        System.out.println("NO");
                        userField.selectAll();
                        userField.replaceSelection("Try another username");
                    }
                }
                catch(IOException exception)
                {}
            }
        });

        window.setVisible(true);
    }

    public void setUsername(String usernameMsg)
    {
        this.username = usernameMsg;
    }

    public void chatWindow()
    {
        StyleConstants.setBackground(meChatName, BACKGROUND_ME_NAME);
        StyleConstants.setForeground(meChatName, FOREGROUND_ME_NAME);
        StyleConstants.setBold(meChatName, true);
        StyleConstants.setBackground(meChatMsg, BACKGROUND_ME_MSG);
        StyleConstants.setForeground(meChatMsg, FOREGROUND_ME_MSG);

        StyleConstants.setBackground(otherChatName, BACKGROUND_OTHER_NAME);
        StyleConstants.setForeground(otherChatName, FOREGROUND_OTHER_NAME);
        StyleConstants.setBold(otherChatName, true);
        StyleConstants.setBackground(otherChatMsg, BACKGROUND_OTHER_MSG);
        StyleConstants.setForeground(otherChatMsg, FOREGROUND_OTHER_MSG);

        StyleConstants.setForeground(meUsers, FOREGROUND_ME_USERS);
        StyleConstants.setBold(meUsers, true);
        StyleConstants.setForeground(otherUsers, FOREGROUND_OTHER_USERS);
        StyleConstants.setBold(otherUsers, true);

        JFrame f = new JFrame("Multi-client chat");
        Container c = f.getContentPane();
        f.setMinimumSize(new Dimension(600,700));

        // Two column grid that fills the width of the page, each column taking up one half the space
        GridBagLayout layout = new GridBagLayout();
        GridBagConstraints constraints = new GridBagConstraints();
        c.setLayout(layout);

        constraints.fill = GridBagConstraints.BOTH;
        constraints.weightx = 0.7;
        constraints.weighty = 0.02;
        constraints.gridx = 0;
        constraints.gridy = 0;
        JLabel chatLabel = new JLabel("CHAT");
        c.add(chatLabel, constraints);
        chatLabel.setBorder(BorderFactory.createEmptyBorder(0,10,0,10));

        constraints.fill = GridBagConstraints.BOTH;
        constraints.weightx = 0.3;
        constraints.weighty = 0.02;
        constraints.gridx = 1;
        constraints.gridy = 0;
        JLabel onlineLabel = new JLabel("ON-LINE");
        c.add(onlineLabel, constraints);
        onlineLabel.setBorder(BorderFactory.createEmptyBorder(0,10,0,10));

        constraints.fill = GridBagConstraints.BOTH;
        constraints.weightx = 0.7;
        constraints.weighty = 0.78;
        constraints.gridx = 0;
        constraints.gridy = 1;
        chatArea = new JTextPane();
        //chatArea.setCharacterAttributes(chatStyle, true);
        chatArea.setEditable(false);

        JScrollPane chat = new JScrollPane(chatArea, 
                                            JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                                            JScrollPane.HORIZONTAL_SCROLLBAR_NEVER );
        c.add(chat, constraints);
        chat.setBorder(BorderFactory.createEmptyBorder(10,10,10,10));

        constraints.fill = GridBagConstraints.BOTH;
        constraints.weightx = 0.3;
        constraints.weighty = 0.78;
        constraints.gridx = 1;
        constraints.gridy = 1;
        usersArea = new JTextPane();
        usersArea.setText(username);
        //usersArea.setCharacterAttributes(usersStyle, true);
        usersArea.setEditable(false);
        
        JScrollPane users = new JScrollPane(usersArea,
                                            JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                                            JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);

        c.add(users, constraints);
        users.setBorder(BorderFactory.createEmptyBorder(10,10,10,10));
        
        constraints.fill = GridBagConstraints.BOTH;
        constraints.weightx = 0.7;
        constraints.weighty = 0.2;
        constraints.gridx = 0;
        constraints.gridy = 2;
        msgArea = new JTextArea();
        msgArea.setText(INSTRUCTION_MSG);

        JScrollPane message = new JScrollPane(msgArea, 
                                                JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                                                JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);

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
        constraints.gridy = 2;
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
                
                try
                {
                    while(msg.endsWith("\n"))
                        msg = msg.substring(0, msg.length()-1);
                            
                    StyledDocument doc = chatArea.getStyledDocument();
                    doc.insertString(doc.getLength(), " "+username+" ", meChatName);
                    doc.insertString(doc.getLength(), "\n", defaultAttribute);
                    doc.insertString(doc.getLength(), " "+msg+" ", meChatMsg);
                    doc.insertString(doc.getLength(), "\n", defaultAttribute);
                }
                catch(BadLocationException exception)
                {}
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

    //public void printMsg(String username, String message, boolean me){}

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

                    //System.out.println(final_msg);

                    if (final_msg.startsWith("USERS"))
                    {
                        final_msg = final_msg.substring(6);
                        String online_users[] = (final_msg.split("\n")[0]).split(":");

                        try
                        {
                            usersArea.setText("");
                            StyledDocument doc = usersArea.getStyledDocument();

                            for(String x : online_users)
                            {
                                doc = usersArea.getStyledDocument();

                                if (x.compareTo(username) == 0)
                                {
                                    doc.insertString(doc.getLength(), x+"\n", meUsers);
                                }
                                else
                                {
                                    doc.insertString(doc.getLength(), x+"\n", otherUsers);
                                }
                            }
                        }
                        catch(BadLocationException exception)
                        {}    
                    }
                    else
                    {
                        try
                        {
                            StyledDocument doc = chatArea.getStyledDocument();
                            
                            while(final_msg.endsWith("\n"))
                                final_msg = final_msg.substring(0, final_msg.length()-1);
                            
                            String msg_parts[] = final_msg.split(":");
                            doc.insertString(doc.getLength(), " "+msg_parts[0]+" ", otherChatName);
                            doc.insertString(doc.getLength(), "\n", defaultAttribute);
                            doc.insertString(doc.getLength(), " "+msg_parts[1]+" ", otherChatMsg);
                            doc.insertString(doc.getLength(), "\n", defaultAttribute);
                        }
                        catch(BadLocationException exception)
                        {}

                        System.out.println(final_msg);
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

        Client c = new Client(serverAddress, port);
    }
}