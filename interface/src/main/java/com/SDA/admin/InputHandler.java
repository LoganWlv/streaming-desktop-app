package com.SDA.admin;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import com.google.gson.Gson;

public class InputHandler implements KeyListener, MouseListener, MouseMotionListener {

  private Socket socket;
  private BufferedInputStream sock_in;
  private BufferedOutputStream sock_out;

  public InputHandler() {
    try {
      InetAddress adressResponse = InetAddress.getByName("127.0.0.1");
      socket = new Socket("127.0.0.1", 26000, adressResponse, 0);
      //socket = new Socket("192.168.1.10", 26000);
      System.out.println("Connection succeed to the server.");
      System.out.println("Port de communication côté serveur : " + socket.getPort());
      System.out.println("Port de communication côté client : " + socket.getLocalPort());
      System.out.println("Nom de l'hôte distant : " + socket.getInetAddress().getHostName());
      System.out.println("Adresse de l'hôte distant : " + socket.getInetAddress().getHostAddress());
      System.out.println("Adresse socket de l'hôte distant : " + socket.getRemoteSocketAddress());
      sock_in = new BufferedInputStream(socket.getInputStream());
      sock_out = new BufferedOutputStream(socket.getOutputStream());
      sock_out.write("Hey I am the client !\n\0".getBytes());
      sock_out.flush();

    } catch (UnknownHostException e) {
      System.out.println("Unknow host " + e.getMessage());
      e.printStackTrace();
    } catch (IOException e) {
      System.out.println(e.getMessage());
      e.printStackTrace();
    } /*finally {
      if (socket != null) {
        try {
          socket.close();
        } catch (IOException e) {
          e.printStackTrace();
          socket = null;
        }
      }
      }*/
  }

  @Override
  public void mouseDragged(MouseEvent e) {
    Gson gson = new Gson();
    //System.out.println("Mouse dragged, event object: ");
    //System.out.println(gson.toJson(e));

  }

  @Override
  public void mouseMoved(MouseEvent e) {
    Gson gson = new Gson();
    //System.out.println("Mouse moved, event object: ");
    //System.out.println(gson.toJson(e));
  }

  @Override
  public void mouseClicked(MouseEvent e) {
    Gson gson = new Gson();
    //System.out.println("Mouse click, event object: ");
    //System.out.println(gson.toJson(e));
  }

  @Override
  public void mousePressed(MouseEvent e) {
    Gson gson = new Gson();
    //System.out.println("Mouse pressed, event object: ");
    //System.out.println(gson.toJson(e));
  }

  @Override
  public void mouseReleased(MouseEvent e) {
    Gson gson = new Gson();
    //System.out.println("Mouse released, event object: ");
    //System.out.println(gson.toJson(e));
  }

  @Override
  public void mouseEntered(MouseEvent e) {
    Gson gson = new Gson();
    //System.out.println("Mouse entered, event object: ");
    //System.out.println(gson.toJson(e));
  }

  @Override
  public void mouseExited(MouseEvent e) {
    Gson gson = new Gson();
    //System.out.println("Mouse exited, event object: ");
    //System.out.println(gson.toJson(e));
  }

  @Override
  public void keyTyped(KeyEvent e) {
    Gson gson = new Gson();
    System.out.println("Key typed, event object: ");
    //System.out.println(gson.toJson(e));
    try {
      sock_out.write((e.getKeyChar() + "\0").getBytes());
      sock_out.flush();
    } catch (IOException e1) {
      e1.printStackTrace();
    }
  }

  @Override
  public void keyPressed(KeyEvent e) {
    Gson gson = new Gson();
    //System.out.println("Key pressed, event object: ");
    //System.out.println(gson.toJson(e));

  }

  @Override
  public void keyReleased(KeyEvent e) {
    Gson gson = new Gson();
    //System.out.println("Key released, event object: ");
    //System.out.println(gson.toJson(e));
  }

}
