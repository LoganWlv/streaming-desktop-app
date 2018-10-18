package com.SDA.admin;

import java.awt.Dimension;
import java.awt.Toolkit;
import java.net.URL;
import java.util.ResourceBundle;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.image.ImageView;
import javafx.scene.layout.AnchorPane;
import javafx.scene.text.Text;
import javafx.scene.text.TextFlow;
import uk.co.caprica.vlcj.component.DirectMediaPlayerComponent;

public class Sample_controller implements Initializable {

  @FXML
  private AnchorPane main_window;

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
  private final Dimension SCREEN_SIZE = Toolkit.getDefaultToolkit().getScreenSize();
  private DirectMediaPlayerComponent mp;

  @FXML
  public void streamingStart(ActionEvent event) {
    System.out.println("Streaming start");

  }

  @FXML
  public void streamingStop() {
    System.out.println("Streaming stop");

  }

  public void initialize(URL location, ResourceBundle resources) {
    Text input_text_area = new Text("Flux audio/video information\n");
    this.text_input_details.getChildren().add(input_text_area);
    Text output_text_area = new Text("Flux keyboard information\n");
    this.text_output_details.getChildren().add(output_text_area);
  }

}
