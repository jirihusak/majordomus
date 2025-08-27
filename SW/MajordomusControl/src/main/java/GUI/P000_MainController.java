/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package GUI;

import java.io.IOException;
import java.net.URL;
import java.util.ResourceBundle;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Node;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.layout.StackPane;

/**
 * FXML Controller class
 *
 * @author husak
 */
public class P000_MainController implements Initializable {

    private Button settingsbtn;
    private Label titleLabel;
    @FXML
    private StackPane pagesStack;
    @FXML
    private Button devicesBtn;
    @FXML
    private Button settingsBtn;
    

    final int PAGE_DEVICES      = 0;
    final int PAGE_UPDATE       = 1;
//    final int PAGE_HELP         = 5;
//    final int PAGE_HOME         = 0;
//    final int PAGE_SCRIPTS      = 1;
//    final int PAGE_STATISTICS   = 2;
    @FXML
    private Button updateBtn;
    @FXML
    private Button helpBtn;
    
    
    
   
    /**
     * Initializes the controller class.
     */
    @Override
    public void initialize(URL url, ResourceBundle rb) {
        
        // buttons action
        //overviewBtn.setOnAction(this::overviewBtnClicked);
        //roomsBtn.setOnAction(this::roomsBtnClicked);
        //statisticsBtn.setOnAction(this::statisticsBtnClicked);
        //serviceBtn.setOnAction(this::serviceBtnClicked);
        devicesBtn.setOnAction(this::devicesBtnClicked);
        updateBtn.setOnAction(this::updateBtnClicked);
        
        try {
            pagesStack.getChildren().add(PAGE_DEVICES, FXMLLoader.load(getClass().getResource("P110_Devices.fxml")));
            pagesStack.getChildren().add(PAGE_UPDATE, FXMLLoader.load(getClass().getResource("P120_Update.fxml")));
 
        } catch (IOException ex) {
            ex.printStackTrace();
        }
        
        showPage(PAGE_DEVICES);
    }  
    
    
    private void devicesBtnClicked(ActionEvent event)
    {
        showPage(PAGE_DEVICES);
    }
    
    private void updateBtnClicked(ActionEvent event)
    {
        showPage(PAGE_UPDATE);
    }
    
    private void showPage(int index)
    {
        for(Node node : pagesStack.getChildren()){
            node.setVisible(false);
        }
        pagesStack.getChildren().get(index).setVisible(true);
    }

}
