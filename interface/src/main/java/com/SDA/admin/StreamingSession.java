package com.SDA.admin;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * To be modified depending on the future architecture.
 * 
 * @author LR-PC
 *
 */
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

  public InputStream getProcessOutput() {
    return currentProcess.getInputStream();
  }

  public OutputStream getProcessInput() {
    return currentProcess.getOutputStream();
  }

  public void startSession() {
    if (currentProcess == null || !currentProcess.isAlive()) {
      try {
        currentProcess = pb.start();
      } catch (IOException e) {
        System.out.println("Error while starting the process");
        e.printStackTrace();
      }
    } else {
      System.out.println("No process found in streaming session or already one alive.");
    }
  }

  public void stopSession() {
    currentProcess.destroy();
  }

}
