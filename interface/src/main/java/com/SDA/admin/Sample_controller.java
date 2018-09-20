package com.SDA.admin;

import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.util.ResourceBundle;

import javax.imageio.ImageIO;

import javafx.concurrent.Task;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.image.ImageView;

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

    /*for (int i = 0; i < 100; i++) {
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
    
    }*/

    Task t = new Task() {
      @Override
      protected Object call() {
        while (!stoppped) {
          try {
            System.out.println("Thread start..");
            Robot ro = new Robot();
            BufferedImage im = ro
                .createScreenCapture(new java.awt.Rectangle(Toolkit.getDefaultToolkit().getScreenSize()));
            byte[] imBytes = toByteArrayAutoClosable(im, "jpg");
            /*
            // Calling the JavaFX Thread to update javafx control
            Platform.runLater(new Runnable() {
              public void run() {
                // This updates the imageview to newly created Image
                System.out.println("Print toto");
            
                screen_display.setImage(myPic);
              }
            });*/

            // Sleep for 100 millisecond
            ProcessBuilder pb = new ProcessBuilder("ffmpeg", "-i", "pipe:0", "-c:v", "libx264", "-preset", "ultrafast",
                "-crf", "0", "pipe:1");
            Process p = pb.start();
            OutputStream stdin = p.getOutputStream();
            InputStream stdout = p.getInputStream();

            InputStream bytesToSend = new ByteArrayInputStream(imBytes);

            int buf;
            int i = 0;

            if (bytesToSend != null) {
              while ((buf = bytesToSend.read()) != -1) {
                if (i == 8172) {
                  stdin.flush();
                }

                stdin.write(buf);
              }

              System.out.println("plouf");
              stdin.close();
            }

            Thread.sleep(1000 / 30);

          } catch (Exception ex) {
            // print stack trace or do other stuffs
          }

        }
        // }
        return null;
      }

    };

    new Thread(t).start();

  }

  /*
   * public static String imgToBase64String(final RenderedImage img, final String
   * formatName) { final ByteArrayOutputStream os = new ByteArrayOutputStream();
   * try { ImageIO.write(img, formatName, Base64.getEncoder().wrap(os)); return
   * os.toString(StandardCharsets.ISO_8859_1.name()); } catch (final IOException
   * ioe) { throw new UncheckedIOException(ioe); } }
   * 
   * public static BufferedImage base64StringToImg(final String base64String) {
   * try { return ImageIO.read(new
   * ByteArrayInputStream(Base64.getDecoder().decode(base64String))); } catch
   * (final IOException ioe) { throw new UncheckedIOException(ioe); } }
   */

  @FXML
  public void streamingStop() {
    System.out.println("Stopping streaming...");

  }

  public void initialize(URL location, ResourceBundle resources) {
  }

  private static byte[] toByteArrayAutoClosable(BufferedImage image, String type) throws IOException {
    try (ByteArrayOutputStream out = new ByteArrayOutputStream()) {
      System.out.println("ImageIO write stream .. \n" + System.currentTimeMillis());
      ImageIO.write(image, type, out);
      System.out.println("ImageIO after write stream .. \n" + System.currentTimeMillis());
      return out.toByteArray();
    }
  }

}
