package com.SDA.admin;

import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.image.BufferedImage;
import java.net.URL;
import java.util.ResourceBundle;

import javafx.application.Platform;
import javafx.concurrent.Task;
import javafx.embed.swing.SwingFXUtils;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.image.WritableImage;

public class Sample_controller implements Initializable {

  @FXML
  private ImageView screen_display;

  @FXML
  private Button start_b;

  @FXML
  private Button stop_b;

  private boolean stoppped = false;

  @FXML
  public void streamingStart(ActionEvent event) {
    System.out.println("Starting streaming...");
    /*
     * for (int i = 0; i < 100; i++) { try { System.out.println("Thread start..");
     * Robot robot = new Robot(); Rectangle box = new
     * Rectangle(Toolkit.getDefaultToolkit().getScreenSize()); BufferedImage pic =
     * robot.createScreenCapture(box); Image myPic = SwingFXUtils.toFXImage(pic,
     * null); screen_display.setImage(myPic);
     * System.out.println("Thread sleeping.."); Thread.currentThread().sleep(5 *
     * 1000); } catch (Exception e) { System.out.println("Error: " + e); }
     * 
     * }
     */

    Task t = new Task() {
      @Override
      protected Object call() {
        while (!stoppped) {
          try {
            System.out.println("Thread start..");
            Robot ro = new Robot();
            BufferedImage im = ro
                .createScreenCapture(new java.awt.Rectangle(Toolkit.getDefaultToolkit().getScreenSize()));
            final WritableImage image = new WritableImage(500, 500);
            final Image myPic = SwingFXUtils.toFXImage(im, null);

            // Calling the JavaFX Thread to update javafx control
            Platform.runLater(new Runnable() {
              public void run() {
                // This updates the imageview to newly created Image
                System.out.println("Print toto");

                screen_display.setImage(myPic);
              }
            });

            // Sleep for 100 millisecond
            Thread.sleep(1000 / 30);

          } catch (Exception ex) {
            // print stack trace or do other stuffs
          }
        }
        return null;
      }

    };

    new Thread(t).start();

  }

  @FXML
  public void streamingStop() {
    System.out.println("Stopping streaming...");

  }

  public void initialize(URL location, ResourceBundle resources) {
    // TODO Auto-generated method stub

  }

}
