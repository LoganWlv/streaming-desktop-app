package com.SDA.admin;

import java.io.IOException;

public class StreamingSession {

  private ProcessBuilder pb;
  private Process currentProcess;

  private StreamingSession() {
  }

  private static StreamingSession SessionInstance = null;

  public static synchronized StreamingSession getInstance() {
    if (SessionInstance == null) {
      SessionInstance = new StreamingSession();
    }
    return SessionInstance;
  }

  public void setProcess(String gameName) {
    //TODO Based on gameName find in configuration/database? how to launch the process
    pb = new ProcessBuilder("D:\\Programming\\Notepad++\\notepad++.exe");
  }

  public void startSession() {
    if (pb != null) {
      try {
        currentProcess = pb.start();
      } catch (IOException e) {
        System.out.println("Error while starting the process");
        e.printStackTrace();
      }
    } else {
      System.out.println("Please set your process");
    }
  }

  public void stopSession() {
    currentProcess.destroy();
  }

}
