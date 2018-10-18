package com.SDA.admin;

import java.awt.BorderLayout;
import java.awt.Canvas;
import java.awt.Color;

import javax.swing.JFrame;
import javax.swing.JPanel;

import com.sun.jna.Native;
import com.sun.jna.NativeLibrary;

import uk.co.caprica.vlcj.binding.LibVlc;
import uk.co.caprica.vlcj.player.MediaPlayerFactory;
import uk.co.caprica.vlcj.player.embedded.EmbeddedMediaPlayer;
import uk.co.caprica.vlcj.player.embedded.videosurface.CanvasVideoSurface;
import uk.co.caprica.vlcj.player.embedded.windows.Win32FullScreenStrategy;
import uk.co.caprica.vlcj.runtime.RuntimeUtil;

public class App {

  private static final String SERVER_IP = "localhost??";
  private static final Short SERVER_PORT = 7700;
  private static final String MRL = "unknow YET";

  public static void main(String[] args) {

    JFrame frame = new JFrame();
    frame.setLocation(100, 100);
    frame.setSize(1000, 600);
    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    frame.setVisible(true);

    Canvas canvas = new Canvas();
    canvas.setBackground(Color.black);
    JPanel panel = new JPanel();
    panel.setLayout(new BorderLayout());
    panel.add(canvas);
    frame.add(panel);

    /**
     * Reading a video file using vlcj and the native library of VLC
     */
    //Load the native library of VLC from the directory where VLC is installed
    NativeLibrary.addSearchPath(RuntimeUtil.getLibVlcName(), "D:\\Programming\\vlc-3.0.4");
    Native.loadLibrary(RuntimeUtil.getLibVlcName(), LibVlc.class);

    //Initialize media player object
    MediaPlayerFactory mpf = new MediaPlayerFactory();
    EmbeddedMediaPlayer emp = mpf.newEmbeddedMediaPlayer(new Win32FullScreenStrategy(frame));
    emp.setVideoSurface(mpf.newVideoSurface(canvas));
    //Hide mouse
    emp.setEnableMouseInputHandling(false);
    //Disable keyboard
    emp.setEnableKeyInputHandling(false);
    String file = "D:\\Workspace\\workspace-dev\\streaming-desktop-app\\video_samples\\video_720p_24fps.mp4";
    emp.prepareMedia(file);
    emp.play();

  }

  /**
   * To be tested with a real RTP connection
   */
  public void ReadRTPstream() {
    String mediatorIP = "230.0.0.1";
    short mediatorPort = 3300;
    String publicIP, publicServer, localIP, localServer, clientIP;
    short publicPort, localPort;

    MediaPlayerFactory mediaPlayerFactory = new MediaPlayerFactory(MRL);
    EmbeddedMediaPlayer mediaPlayer = mediaPlayerFactory.newEmbeddedMediaPlayer();

    Canvas canvas = new Canvas();
    canvas.setBackground(Color.black);
    CanvasVideoSurface videoSurface = mediaPlayerFactory.newVideoSurface(canvas);
    mediaPlayer.setVideoSurface(videoSurface);

    JFrame f = new JFrame();
    //f.setIconImage(new ImageIcon(Client.class.getResource("icons/vlcj-logo.png")).getImage());
    f.add(canvas);
    f.setSize(800, 600);
    f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    f.setVisible(true);
    publicIP = SERVER_IP; // Recv public server's ip from mediator
    publicPort = SERVER_PORT; // Recv public server's port from mediator
    publicServer = formatRtpStream(publicIP, publicPort);
    System.out.println("Capturing from '" + publicServer + "'");
    f.setTitle("Capturing from Public Server 'rtp://" + publicIP + ":" + publicPort + "'");
    mediaPlayer.playMedia(publicServer);

  }

  private static String formatRtpStream(String serverAddress, int serverPort) {
    StringBuilder sb = new StringBuilder(60);
    sb.append(":sout=#duplicate{dst=display,dst=rtp{dst=");
    sb.append(serverAddress);
    sb.append(",port=");
    sb.append(serverPort);
    sb.append(",mux=ts}}");
    return sb.toString();
  }

  /* USED ONLY FOR JAVAFX, but AWT canvas seems to be more efficient following this link https://github.com/caprica/vlcj-javafx
  public void start(Stage primaryStage) {
    try {
      //Initialise the UI using FXML file
      Parent root = FXMLLoader.load(getClass().getResource("sample.fxml"));
      Scene scene = new Scene(root);
      primaryStage.setTitle("Streaming desktop app");
      primaryStage.setScene(scene);
      primaryStage.show();
    } catch (Exception e) {
      e.printStackTrace();
    }
  }*/

}
