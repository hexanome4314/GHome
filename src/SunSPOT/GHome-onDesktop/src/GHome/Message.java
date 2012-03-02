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
    
    /*
     * Créé un nouveau message depuis un datagram reçu par un spot
     * Contrat : Le spot doit envoyer une trame cohérente
     */
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
    
    /*
     * Convertit les addresses des spots vers le format retenu coté driver
     * Exemple : 7F00.0001.0000.1001 -> 00001001
     */
    private String parseAddress(String str) {
        return str.substring(10).replaceAll("\\.", "");
    }    
    
    /*
     * Retourne le message formaté afin qu'il soit acceptable par le driver
     */
    public String getFormattedMessage() {
        return address+" "+String.format("%010d", light)+" "+String.format("%E", temperature).replace(",", ".")+" "+String.format("%03d", battery);
    }
}
