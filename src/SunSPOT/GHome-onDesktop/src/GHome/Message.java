package GHome;

import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.microedition.io.Datagram;

public class Message {
    private int light;
    private float temperature;
    private String address;
    private int battery;
    
    public Message(Datagram datagram) {
        try {
            this.address = parseAddress(datagram.getAddress());
            this.light = datagram.readInt();
            this.temperature = datagram.readFloat();
            this.battery = datagram.readInt();
        } catch (IOException ex) {
            Logger.getLogger(Message.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    //Translate 7F00.0001.0000.1001 to 00001001
    private String parseAddress(String str) {
        return str.substring(10).replaceAll("\\.", "");
    }    
    
    //3,402823e+38 max
    //1,401298e-45 min
    //0,000000e+00 0
    //6,666600e+01 
    public String getFormattedMessage() {
        return address+" "+String.format("%010d", light)+" "+String.format("%E", temperature).replace(",", ".")+" "+String.format("%03d", battery);
    }
}
