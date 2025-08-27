/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package GUI;

/**
 *
 * @author husak
 */
import javafx.application.Application;
import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;

public class AppGUI extends Application{
    
    @Override
    public void start(Stage stage) throws Exception {
        //System.out.println(getClass().getResource("P000_Main.fxml"));
        Parent root = FXMLLoader.load(getClass().getResource("P000_Main.fxml"));
        stage.setTitle("Majordomus Control");
        stage.setScene(new Scene(root, 1024, 768));
        stage.show();
        
        stage.setOnCloseRequest((WindowEvent event) -> {
            Platform.exit();
            System.exit(0);
        });
    }
       
}
