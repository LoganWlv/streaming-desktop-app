package com.SDA.admin;

import java.awt.AWTException;
import java.awt.Rectangle;
import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;

public class TestApp {

  public static void main(String[] args) throws IOException {
    Robot robot;
    try {
      robot = new Robot();
      String format = "jpg";
      String fileName = "FullScreenshot." + format;

      Rectangle screenRect = new Rectangle(Toolkit.getDefaultToolkit().getScreenSize());
      BufferedImage screenFullImage = robot.createScreenCapture(screenRect);
      ImageIO.write(screenFullImage, format, new File(fileName));

      System.out.println("A full screenshot saved!");
    } catch (AWTException e) {

      e.printStackTrace();
    }

  }

}