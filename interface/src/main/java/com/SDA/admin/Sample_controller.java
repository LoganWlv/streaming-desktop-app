package com.SDA.admin;

import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.net.URL;
import java.util.ResourceBundle;

import javax.imageio.ImageIO;

import javafx.application.Platform;
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
        // while (!stoppped) {
        try {
          System.out.println("Thread start..");
          Robot ro = new Robot();
          BufferedImage im = ro
              .createScreenCapture(new java.awt.Rectangle(Toolkit.getDefaultToolkit().getScreenSize()));

          ffmpegEncodeAndSendTo(im);

          // final WritableImage image = new WritableImage(500, 500);
          // final Image myPic = SwingFXUtils.toFXImage(im, null);

          // WritableRaster raster = im.getRaster();
          // DataBufferByte buffer = (DataBufferByte) raster.getDataBuffer();
          // byte[] bytes = buffer.getData();

          // ProcessBuilder pb = new ProcessBuilder("ffmpeg", "-i", "pipe:0", "-c:v",
          // "libx264", "-preset", "ultrafast",
          // "-crf", "0", "test5.png");

          // "CaptureTest.png"
          // "-c:v", "libx264", "-preset", "ultrafast", "-crf", "0",

          // pipe:1
          // Process p = pb.start();

          // p.getErrorStream()

          // System.out.println("Print eaea");
          // p.getOutputStream().write(convertBufferedImageToBytes(im, "png"));

          // byte[] b = new byte[1000];

          // p.getErrorStream().read(b);

          // String err = new String(b, 0, b.length, "UTF-8");

          // System.out.println(b.length);

          // createlogfile(err);

          // byte[] b = new byte[imageInByte.length];
          // p.getInputStream().read(b);
          // System.out.println(b);

          // InputStream out = new ByteArrayInputStream(b);
          // BufferedImage bImageFromConvert = ImageIO.read(out);

          // final WritableImage image = new WritableImage(500, 500);
          // final Image myPic = SwingFXUtils.toFXImage(bImageFromConvert, null);

          // byte[] imageInByte= new BigInteger(hex, 16).toByteArray();
          // InputStream in = new ByteArrayInputStream(imageInByte);
          // BufferedImage bImageFromConvert = ImageIO.read(in);
          // p.

          // Calling the JavaFX Thread to update javafx control

          Platform.runLater(new Runnable() {
            public void run() { // This updates the imageview to newly created Image
              System.out.println("Print toto");

              // screen_display.setImage(myPic);
            }
          });

          // Sleep for 100 millisecond
          Thread.sleep(1000 / 30);

        } catch (Exception ex) {
          // print stack trace or do other stuffs
        }
        // }
        return null;
      }

    };

    new Thread(t).start();

  }

  private byte[] convertBufferedImageToBytes(BufferedImage image, String format) throws IOException {
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    ImageIO.write(image, format, baos);
    baos.flush();
    byte[] imageInByte = baos.toByteArray();
    baos.close();
    return imageInByte;
  }

  private void ffmpegEncodeAndSendTo(BufferedImage image) throws IOException {
    ProcessBuilder pb = new ProcessBuilder("ffmpeg", "-i", "pipe:0", "-c:v", "libx264", "-preset", "ultrafast", "-crf",
        "0", "test5.png");
    Process p = pb.start();
    p.getOutputStream().write(convertBufferedImageToBytes(image, "png"));
    byte[] b = new byte[10000];
    p.getErrorStream().read(b);
    String err = new String(b, 0, b.length, "UTF-8");
    createlogfile(err);
  }

  private void createlogfile(String string) {
    final String chemin = "logffmpeg.txt";
    final File fichier = new File(chemin);
    try {
      // Creation du fichier
      fichier.createNewFile();
      // creation d'un writer (un écrivain)
      final FileWriter writer = new FileWriter(fichier);
      try {
        writer.write(string);
      } finally {
        // quoiqu'il arrive, on ferme le fichier
        writer.close();
      }
    } catch (Exception e) {
      System.out.println("Impossible de creer le fichier");
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
