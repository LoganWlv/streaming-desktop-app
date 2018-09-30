package com.SDA.admin;

import java.awt.AWTException;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.net.URL;
import java.util.ResourceBundle;

import javax.imageio.ImageIO;

import javafx.concurrent.Task;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.image.ImageView;
import javafx.scene.text.Text;
import javafx.scene.text.TextFlow;

public class Sample_controller implements Initializable {

  @FXML
  private ImageView screen_display;

  @FXML
  private Button start_b;

  @FXML
  private Button stop_b;

  @FXML
  private TextFlow text_input_details;

  @FXML
  private TextFlow text_output_details;

  private boolean stoppped = false;

  @FXML
  public void streamingStart(ActionEvent event) {
    System.out.println("Starting streaming...");
    Task t = new Task() {
      @Override
      protected Object call() {
        try {
          System.out.println("Thread start..");
          // Robot ro = new Robot();
          // BufferedImage im = ro
          // .createScreenCapture(new
          // java.awt.Rectangle(Toolkit.getDefaultToolkit().getScreenSize()));
          fromFrameToVideo();
          // ffmpegDecode(ffmpegEncodeAndSendTo(im));
          // ffmpegEncodeAndSendTo(im);
          /*
           * Platform.runLater(new Runnable() { public void run() { // This updates the
           * imageview to newly created Image System.out.println("Print toto");
           * 
           * // screen_display.setImage(myPic); } });
           */
        } catch (Exception ex) {
          // print stack trace or do other stuffs
          System.out.println(ex);
        }
        return null;
      }
    };
    new Thread(t).start();
  }

  // private int lenght;

  private byte[] convertBufferedImageToBytes(BufferedImage image, String format) throws IOException {
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    ImageIO.write(image, format, baos);
    baos.flush();
    byte[] imageInByte = baos.toByteArray();
    baos.close();

    // baos.;

    // byte[] imageBytes = ((DataBufferByte)
    // image.getData().getDataBuffer()).getData();
    // ComponentColorModel tete;
    // lenght = imageInByte.length;
    /*
     * System.out.println("convertBufferedImageToBytes");
     * 
     * int[] bits = { 8, 8, 8 };
     * 
     * ComponentColorModel cm = new
     * ComponentColorModel(ColorSpace.getInstance(ColorSpace.CS_sRGB), bits, false,
     * false, DirectColorModel.TRANSLUCENT, DataBuffer.TYPE_BYTE);
     * 
     * SampleModel sm = cm.createCompatibleSampleModel(image.getWidth(),
     * image.getHeight()); // DataBufferByte db = new
     * DataBufferByte(image.getWidth()*image.getHeight()*1); // //3 channels buffer
     * DataBufferByte db = new DataBufferByte(image.getWidth() * image.getHeight() *
     * 3); // 3 channels buffer WritableRaster r =
     * WritableRaster.createWritableRaster(sm, db, new java.awt.Point(0, 0));
     * BufferedImage cvImg = new BufferedImage(cm, r, false, null); byte[]
     * imageBytes = ((DataBufferByte) cvImg.getData().getDataBuffer()).getData();
     * System.out.println("tete");
     */
    return imageInByte;
  }

  private void ffmpegDecode(byte[] imageInByte) throws IOException {
    ProcessBuilder pb = new ProcessBuilder("ffmpeg", "-i", "pipe:0", "testa.png");
    Process p = pb.start();
    p.getOutputStream().write(imageInByte);
    System.out.println("ffmpegDecode");

  }

  private byte[] ffmpegEncodeAndSendTo(BufferedImage image) throws IOException {
    ProcessBuilder pb = new ProcessBuilder("ffmpeg", "-i", "pipe:0", "pipe:1");

    // mpegts
    // "-c:v", "libx264", "-preset", "ultrafast", "-crf", "0",
    Process p = pb.start();

    p.getOutputStream().write(convertBufferedImageToBytes(image, "png"));

    System.out.println("ffmpegEncodeAndSendTo 2");
    // int lenghterr = p.getErrorStream().available();
    byte[] a = new byte[10000];
    p.getErrorStream().read(a);

    System.out.println("ffmpegEncodeAndSendTo 3");
    String err = new String(a, 0, a.length, "UTF-8");
    createlogfile(err);

    // System.out.println(p.getInputStream().available());
    // int b = p.getInputStream().read();

    // byte[] b = new byte[10000];
    // p.getInputStream().read(b);

    // System.out.println(b);

    System.out.println("ffmpegEncodeAndSendTo 1");
    // int lenght = p.getInputStream().available();

    // byte[] imageInByte = new byte[lenght];
    // p.getInputStream().read(imageInByte);

    /*
     * ByteArrayOutputStream baos = new ByteArrayOutputStream(); int b = 0; while (b
     * != -1) { System.out.println(b); b = p.getInputStream().read();
     * 
     * baos.write(b); } baos.flush(); byte[] imageInByte = baos.toByteArray();
     * baos.close();
     * 
     * System.out.println(imageInByte.length);
     */

    // return imageInByte;
    return null;
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
        System.out.println("createlogfile");
      } finally {
        // quoiqu'il arrive, on ferme le fichier
        writer.close();
      }
    } catch (Exception e) {
      System.out.println("Impossible de creer le fichier");
    }
  }

  private void fromFrameToVideo() throws AWTException, IOException, InterruptedException {
    ProcessBuilder pb = new ProcessBuilder("ffmpeg", "-f", "gdigrab", "-framerate", "60", "-i", "desktop",
        "output.mkv");
    Process p = pb.start();

    byte[] a = new byte[10000];
    p.getErrorStream().read(a);

    String err = new String(a, 0, a.length, "UTF-8");
    createlogfile(err);

    // ffmpeg -f dshow -i video="screen-capture-recorder" output.mkv
    // "-f", "mpegts", "pipe:1"
    // int i = 0;
    // Robot ro = new Robot();

    // while (i <= 240) {

    // BufferedImage im = ro.createScreenCapture(new
    // Rectangle(Toolkit.getDefaultToolkit().getScreenSize()));
    // File outputfile = new File(i + ".png");
    // ImageIO.write(im, "png", outputfile);
    // p.getOutputStream().write(convertBufferedImageToBytes(im, "png"));
    // i++;
    // Thread.sleep(1000 / 30);
    // System.out.println(i);
    // }

  }

  @FXML
  public void streamingStop() {
    System.out.println("Stopping streaming...");

  }

  public void initialize(URL location, ResourceBundle resources) {
    Text input_text_area = new Text("Flux audio/video information\n");
    this.text_input_details.getChildren().add(input_text_area);
    Text output_text_area = new Text("Flux keyboard information\n");
    this.text_output_details.getChildren().add(output_text_area);
  }
}
