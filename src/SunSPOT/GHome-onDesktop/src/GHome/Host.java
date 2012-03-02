package GHome;

import com.sun.spot.io.j2me.radiogram.RadiogramConnection;
import com.sun.spot.peripheral.ota.OTACommandServer;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.microedition.io.Connector;
import javax.microedition.io.Datagram;


public class Host {
    // Broadcast port on which we listen for sensor samples
    private static final int HOST_PORT = 67;
    private static final int SERVER_PORT = 6666;    
    private RadiogramConnection rCon;
    private ServerSocket welcomeSocket;
    private Socket client;
    private Datagram dg;
    private OutputStreamWriter outStream;
    
    /*
     * Fonction de connection au en broadcast permettant la connexion des spots
     * Retourne un booléen de l'état de la connexion
     */
    private boolean connectToSensors(int remote_port) {
        try {
            // Open up a server-side broadcast radiogram connection
            // to listen for sensor readings being sent by different SPOTs
            rCon = (RadiogramConnection) Connector.open("radiogram://:" + remote_port);
            dg = rCon.newDatagram(rCon.getMaximumLength());
            return true;
        } catch (Exception e) {
             System.err.println("setUp caught " + e.getMessage());
        }
        return false;
    }
    
    /*
     * Fonction de connection au driver via le socket (interfacage C-Java)
     * Cette fonction initialise le socket welcomeSocket avec les paramètres
     * Retoure un booléen de l'état de la connexion
     */
    private boolean connectToDriver(InetAddress server_addr, int server_port) {
        try {
            welcomeSocket = new ServerSocket(server_port);
            welcomeSocket.setReuseAddress(true);
            
            reconnectToDriver();
            
            return true;
        } catch (IOException ex) {
            Logger.getLogger(Host.class.getName()).log(Level.SEVERE, null, ex);
        }
        return false;
    }
    
    /*
     * Fonction de reconnection au driver en cas de déconnexion
     * Le socket welcomeSocket doit être initialisé (les paramètres fournis par 
     * connectToDriver sont mémorisés)
     */
    private void reconnectToDriver() {
        try {
            client = welcomeSocket.accept();
            client.setReuseAddress(true);
            outStream = new OutputStreamWriter(client.getOutputStream());
            System.out.println("Listener connected on port "+SERVER_PORT);            
        } catch (IOException ex) {
            Logger.getLogger(Host.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    /*
     * Routine d'execution de la base
     */
    private void run() {
        try {
            if(connectToSensors(HOST_PORT) && connectToDriver(InetAddress.getLocalHost(), SERVER_PORT))
            {
                // Main data collection loop
                while (true) {
                    try {
                        // Appel bloquant scrutant les messages des spots connectés
                        rCon.receive(dg);
                        System.out.println("Nouveau message recu.");
                        Message msg = new Message(dg);
                        try
                        {
                            // Envoi dans le socket pour le driver du message formaté
                            outStream.write(msg.getFormattedMessage());
                            outStream.flush();
                            System.out.println("msg sent: "+msg.getFormattedMessage());
                        }
                        catch (IOException ex) // Reconnexion
                        {
                            System.out.println("Perte de la connexion client. Reconnexion en cours..."); 
                            reconnectToDriver();
                        }
                    }
                    catch (IOException ex)
                    {System.out.println("Erreur "+ex+" lors de la reception des capteurs.");}
                }
            }
        } catch (UnknownHostException ex) {
            Logger.getLogger(Host.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    public static void main(String[] args) throws Exception {
        // register the application's name with the OTA Command server & start OTA running
        OTACommandServer.start("GHome");

        Host app = new Host();
        app.run();
    }
}
