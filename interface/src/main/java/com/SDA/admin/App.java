package com.SDA.admin;

import java.awt.Rectangle;
import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.net.URL;
import java.util.ResourceBundle;

import javafx.application.Application;
import javafx.embed.swing.SwingFXUtils;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.stage.Stage;

public class App extends Application {

  @FXML
  private ImageView screen_display;

  public static void main(String[] args) {
    launch(args);
  }

  public void start(Stage primaryStage) {
    Parent root;
    try {
      root = FXMLLoader.load(getClass().getResource("sample.fxml"));
      Scene scene = new Scene(root);
      primaryStage.setScene(scene);
      primaryStage.show();
    } catch (IOException e) {
      e.printStackTrace();
    }
  }

  @FXML
  public void streamingStart() {
    System.out.println("Starting streaming...");
    try {
      Robot robot = new Robot();
      Rectangle box = new Rectangle(Toolkit.getDefaultToolkit().getScreenSize());
      BufferedImage pic = robot.createScreenCapture(box);
      Image myPic = SwingFXUtils.toFXImage(pic, null);
    } catch (Exception e) {
      System.out.println("Error: " + e);
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
