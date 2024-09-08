/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/javafx/FXMLController.java to edit this template
 */
package GUI;

import Configuration.ConfXmlObject;
import Configuration.ConfXmlObject.Device;
import Configuration.Configuration;
import SerialCom.SerialCommunication;
import java.io.File;
import java.net.URL;
import java.util.ResourceBundle;
import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressBar;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.layout.HBox;
import javafx.stage.FileChooser;

/**
 * FXML Controller class
 *
 * @author husak
 */
public class P120_UpdateController implements Initializable {

    @FXML
    private HBox update;
    
    private Button startBtn;
    @FXML
    private TextField updateFilePath;
    @FXML
    private Button updateOpenFile;
    @FXML
    private ComboBox<String> updateSelectConnection;
    @FXML
    private Button updateStart;
    @FXML
    private Button updateStop;
    @FXML
    private ProgressBar updateProgress;
    @FXML
    private Label updateProgressText;
    @FXML
    private Button updateClearLog;
    @FXML
    private TextArea updateLog;
    @FXML
    private ComboBox<String> updateSelectDevice;

    final FileChooser fileChooser = new FileChooser();
    
    private static P120_UpdateController singletonObj = null;
            
    public static P120_UpdateController getInstance() {

        return singletonObj;
    }
    @FXML
    private Button changeIdBtn;
    @FXML
    private TextField newIdText;
    
    public P120_UpdateController()
    {
        singletonObj = this;
    }

    /**
     * Initializes the controller class.
     */
    @Override
    public void initialize(URL url, ResourceBundle rb) {
        updateStart.setOnAction(this::startUpdate);
        updateStop.setOnAction(this::stopUpdate);
        changeIdBtn.setOnAction(this::changeId);
        updateOpenFile.setOnAction(this::updateChooseFile);
        
        // add all devices
        for (ConfXmlObject.Connection c: Configuration.getXMLObj().getConnectionsList()){
            updateSelectConnection.getItems().add(c.name);
        }
           
        updateSelectConnection.setOnAction(this::updateComboConnChanged);
        updateSelectConnection.getSelectionModel().selectFirst();
        updateComboConnChanged(null);
    }    


    private void startUpdate(ActionEvent event) {
        //System.out.println("GUI.P120_UpdateController.startBtn()!!!!!!!!!!!!!!!!!!!!!!!!!!!!" );
        SerialCom.SerialCommunication.getInstance().getSerialUpdater().startUpdate(updateFilePath.getText(), updateSelectConnection.getSelectionModel().getSelectedItem(),
                updateSelectDevice.getSelectionModel().getSelectedItem());
    }
    
    private void stopUpdate(ActionEvent event) {
        SerialCom.SerialCommunication.getInstance().getSerialUpdater().stopUpdate();
    }
    
    void updateChooseFile(ActionEvent event)
    {
        fileChooser.setTitle("Open HEX file");
        fileChooser.setInitialDirectory(
            new File(System.getProperty("user.home"))
        ); 
        File file = fileChooser.showOpenDialog(update.getScene().getWindow());
        if (file != null) {
            updateFilePath.setText(file.getAbsolutePath());
        }
    }
    
    private void updateComboConnChanged(ActionEvent event) {
        
        String conn = updateSelectConnection.getSelectionModel().getSelectedItem();
        
        updateSelectDevice.getItems().clear();
        
        // add all devices
        for (ConfXmlObject.Device c: Configuration.getXMLObj().getDevicesList()){
            if(c.connection.equals(conn)){
                updateSelectDevice.getItems().add(c.name);
            }
        }
        updateSelectDevice.getSelectionModel().selectFirst();

    }
    
    public void setUpdateProgessBar(double percentage)
    {
        Platform.runLater(() -> updateProgress.setProgress(percentage));
        Platform.runLater(() -> updateProgressText.setText(String.format("%.1f %%", percentage*100)));

    }
    
    public void changeId(ActionEvent event)
    {
        String msg = "id:" + updateSelectDevice.getSelectionModel().getSelectedItem() + ",msg:config,newId:" + newIdText.getText();
        char crc = SerialCom.SerialCommunication.getInstance().crc8(0, msg.toCharArray(), msg.length());
        msg += String.format(",crc:%02x\r\n", (int)crc);
        System.out.println(msg);
        SerialCommunication.getInstance().sendCommand(updateSelectConnection.getSelectionModel().getSelectedItem(), msg);
    }
    
}
