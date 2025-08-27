/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/javafx/FXMLController.java to edit this template
 */
package GUI;

import Configuration.ConfXmlObject;
import Configuration.Configuration;
import Devices.DeviceGeneric;
import Devices.DeviceInterface;
import java.net.URL;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.ResourceBundle;
import javafx.beans.property.SimpleStringProperty;
import javafx.collections.FXCollections;
import javafx.collections.MapChangeListener;
import javafx.collections.ObservableList;
import javafx.collections.ObservableMap;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.TreeItem;
import javafx.scene.control.TreeView;
import javafx.scene.control.cell.MapValueFactory;
import javafx.scene.control.cell.TextFieldTableCell;

/**
 * FXML Controller class
 *
 * @author husak
 */
public class P110_DevicesController implements Initializable {

    @FXML
    private TreeView<String> devicesTree;
    @FXML
    private TableView<Map<String, String>> propertyTable;
    
    private String selectedDevice = "";
    @FXML
    private TableView<Map<String, String>> commandTable;

    /**
     * Initializes the controller class.
     */
    @Override
    public void initialize(URL url, ResourceBundle rb) {

        List<ConfXmlObject.Connection> connectionsList = Configuration.getXMLObj().getConnectionsList();
        List<ConfXmlObject.Device> devicesList = Configuration.getXMLObj().getDevicesList();
        TreeItem<String> root = new TreeItem<>();
        
        for (ConfXmlObject.Connection c: Configuration.getXMLObj().getConnectionsList()){
            TreeItem<String> connection = new TreeItem<>(c.name);
            root.getChildren().add(connection);    
            connection.setExpanded(true);
            
            for(ConfXmlObject.Device d :devicesList)
            {
                if(d.connection.equals(c.name))
                {
                    TreeItem<String> item = new TreeItem<>(d.name);
                    connection.getChildren().add(item);    
                }   
            }
        }
        devicesTree.setShowRoot(false);
        devicesTree.setRoot(root);
        
        devicesTree.getSelectionModel().selectedItemProperty().addListener((observable, oldValue, newValue) -> {
            if (newValue != null) {
                // Handle the selection event
                selectedDevice = newValue.getValue();
                updateSelectedPropertyTable(newValue.getValue(),true);
            }
        });

        // test listener
        // Listen for changes in the outer map
        GuiModels.getInstance().getDeviceMap().addListener((MapChangeListener<String, ObservableMap<String, String>>) change -> {
            if (change.wasAdded() && change.wasRemoved()) {
                //System.out.println("Updated: " + change.getKey() + " -> " + change.getValueAdded());
                // Add listener to the updated inner map
                addInnerMapListener(change.getKey(), change.getValueAdded());
            } else if (change.wasAdded()) {
                //System.out.println("Added: " + change.getKey() + " -> " + change.getValueAdded());
                // Add listener to the newly added inner map
                addInnerMapListener(change.getKey(), change.getValueAdded());
            } else if (change.wasRemoved()) {
                System.out.println("Removed: " + change.getKey() + " -> " + change.getValueRemoved());
                // Optionally handle removed inner maps if needed
            }
        });

        // Add listeners to already existing inner maps
        GuiModels.getInstance().getDeviceMap().forEach((key, innerMap) -> addInnerMapListener(key, innerMap));
        
        // init table
        TableColumn<Map<String, String>, String> propertyCol = new TableColumn<>("Property");
        TableColumn<Map<String, String>, String> valueCol = new TableColumn<>("Value");
        
        propertyCol.setPrefWidth(150); // Set preferred width for the Property column
        valueCol.setPrefWidth(150); // Set preferred width for the Value column

        // Set up cell value factories manually
        propertyCol.setCellValueFactory(cellData -> new SimpleStringProperty(cellData.getValue().get("Property")));
        valueCol.setCellValueFactory(cellData -> new SimpleStringProperty(cellData.getValue().get("Value")));

        // Add columns to the TableView
        propertyTable.getColumns().add(propertyCol);
        propertyTable.getColumns().add(valueCol);
        
        
        // Initialize columns
        TableColumn<Map<String, String>, String> cmdProperty = new TableColumn<>("Property");
        TableColumn<Map<String, String>, String> cmdValue = new TableColumn<>("Value");
        
        cmdProperty.setPrefWidth(150); // Set preferred width for the Property column
        cmdValue.setPrefWidth(150); // Set preferred width for the Value column

        // Set up cell value factories
        cmdProperty.setCellValueFactory(cellData -> new SimpleStringProperty(cellData.getValue().get("Property")));
        cmdValue.setCellValueFactory(cellData -> new SimpleStringProperty(cellData.getValue().get("Value")));

        // Make the second column editable
        cmdValue.setCellFactory(TextFieldTableCell.forTableColumn());

        // Enable editing in the TableView
        commandTable.setEditable(true);

        // Add listener to handle changes when editing
        cmdValue.setOnEditCommit(event -> {
            DeviceInterface.getInstance().getDeviceByName(selectedDevice).cmdToDevice(event.getRowValue().get("Property"), event.getNewValue());
            //System.out.println(event.getRowValue().get("Property") +":"+ event.getNewValue());
        });

        // Add columns to the TableView
        commandTable.getColumns().add(cmdProperty);
        commandTable.getColumns().add(cmdValue);
        
        
        
    }
    
    private void addInnerMapListener(String deviceName, ObservableMap<String, String> innerMap) {
        innerMap.addListener((MapChangeListener<String, String>) change -> {
            if (change.wasAdded() && change.wasRemoved()) {
                //System.out.println("Updated property: " + change.getKey() + " -> " + change.getValueAdded());
                updateSelectedPropertyTable(deviceName, false);
            } else if (change.wasAdded()) {
                System.out.println("Added property: " + change.getKey() + " -> " + change.getValueAdded());
            } else if (change.wasRemoved()) {
                System.out.println("Removed property: " + change.getKey() + " -> " + change.getValueRemoved());
            }
        });
    }
    
    private void updateSelectedPropertyTable(String deviceName, boolean clearAll )
    {
        // clear table
        if(clearAll) {
            propertyTable.getItems().clear();
            commandTable.getItems().clear();

            // fill cmd table
            ObservableList<Map<String, String>> commandsItems = FXCollections.observableArrayList();
            for(String cmd : DeviceInterface.getInstance().getDeviceByName(deviceName).getCmdList())
            {
                Map<String, String> map = new HashMap<>();
                map.put("Property", cmd);
                map.put("Value", "");
                commandsItems.add(map);
            }
            commandTable.setItems(commandsItems);
            
        }
        // is changed in this device?
        if(!selectedDevice.equals(deviceName))
            return;
        
        
        // add property map
        // TODO - make it more effective
        ObservableMap<String,String> p = GuiModels.getInstance().getDeviceMap().valueAt(deviceName).get();
        ObservableList<Map<String, String>> items = FXCollections.observableArrayList();
        p.forEach((key, value) -> {
            Map<String, String> map = new HashMap<>();
            map.put("Property", key);
            map.put("Value", value);
            items.add(map);
        });
        propertyTable.setItems(items);
       
    }
    
    

}
