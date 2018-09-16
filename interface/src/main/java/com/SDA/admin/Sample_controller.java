package com.SDA.admin;

import java.awt.Rectangle;
import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.image.BufferedImage;
import java.net.URL;
import java.util.ResourceBundle;

import javafx.embed.swing.SwingFXUtils;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

public class Sample_controller implements Initializable {

  @FXML
  private ImageView screen_display;

  @FXML
  private Button start_b;

  @FXML
  private Button stop_b;

  @FXML
  public void streamingStart(ActionEvent event) {
    System.out.println("Starting streaming...");
    for (int i = 0; i < 100; i++) {
      try {
        System.out.println("Thread start..");
        Robot robot = new Robot();
        Rectangle box = new Rectangle(Toolkit.getDefaultToolkit().getScreenSize());
        BufferedImage pic = robot.createScreenCapture(box);
        Image myPic = SwingFXUtils.toFXImage(pic, null);
        screen_display.setImage(myPic);
        System.out.println("Thread sleeping..");
        Thread.currentThread().sleep(5 * 1000);
      } catch (Exception e) {
        System.out.println("Error: " + e);
      }

    }

  }

  @FXML
  public void streamingStop() {
    System.out.println("Stopping streaming...");

  }

  public void initialize(URL location, ResourceBundle resources) {
    // TODO Auto-generated method stub

  }

}
