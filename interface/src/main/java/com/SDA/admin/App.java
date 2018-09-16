package com.SDA.admin;

import javafx.application.Application;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.layout.GridPane;
import javafx.stage.Stage;

public class App extends Application {

  public static void main(String[] args) {
    launch(args);
  }

  @Override
  public void start(Stage primaryStage) {
    Button btn_start = new Button();
    btn_start.setText("Start");
    btn_start.setOnAction(new EventHandler<ActionEvent>() {
      public void handle(ActionEvent event) {
        streamingStart();
      }
    });

    Button btn_stop = new Button();
    btn_stop.setText("Stop");
    btn_stop.setOnAction(new EventHandler<ActionEvent>() {
      public void handle(ActionEvent event) {
        streamingStop();
      }
    });

    GridPane grid = new GridPane();
    grid.setAlignment(Pos.CENTER);
    grid.setHgap(10);
    grid.setVgap(10);
    grid.setPadding(new Insets(25, 25, 25, 25));
    grid.add(btn_start, 1, 1);
    grid.add(btn_stop, 2, 1);

    Scene scene = new Scene(grid, 300, 250);
    primaryStage.setTitle("Streaming Desktop App");
    primaryStage.setScene(scene);
    primaryStage.show();
  }

  public void streamingStart() {
    System.out.println("Starting streaming...");

  }

  public void streamingStop() {
    System.out.println("Stopping streaming...");

  }

}
