package GHome;

import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.microedition.io.Datagram;

public class Message {
    private byte idProbe;  // Identifiant de la sonde (1 octet)
    private double value;  // Valeur de la sonde (8 octets)
    private byte[] address = new byte[8]; // Addresse en hexa du capteur (8 octets)
    
    public Message(Datagram datagram) {
        try {
            parseAddress(datagram.getAddress().substring(10));
            this.idProbe = datagram.readByte();
            this.value = datagram.readDouble();
        } catch (IOException ex) {
            Logger.getLogger(Message.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    private void parseAddress(String str) {
        address[0] = (byte) (((char) Integer.parseInt(String.valueOf(str.charAt(0)), 16)) & 0x0F);
        address[1] = (byte) (((char) Integer.parseInt(String.valueOf(str.charAt(1)), 16)) & 0x0F);
        address[2] = (byte) (((char) Integer.parseInt(String.valueOf(str.charAt(2)), 16)) & 0x0F);
        address[3] = (byte) (((char) Integer.parseInt(String.valueOf(str.charAt(3)), 16)) & 0x0F);
        address[4] = (byte) (((char) Integer.parseInt(String.valueOf(str.charAt(5)), 16)) & 0x0F);
        address[5] = (byte) (((char) Integer.parseInt(String.valueOf(str.charAt(6)), 16)) & 0x0F);
        address[6] = (byte) (((char) Integer.parseInt(String.valueOf(str.charAt(7)), 16)) & 0x0F);
        address[7] = (byte) (((char) Integer.parseInt(String.valueOf(str.charAt(8)), 16)) & 0x0F);
    }    
    
    public byte[] getFormattedMessage() {
        long longValue = Double.doubleToRawLongBits(value);

        return new byte[] {
            (byte) 0xA5,
            (byte) 0x5A,
            (byte) 0x0B,
            (byte) idProbe,
            (byte)((longValue >> 56) & 0xff),
            (byte)((longValue >> 48) & 0xff),
            (byte)((longValue >> 40) & 0xff),
            (byte)((longValue >> 32) & 0xff),
            (byte)((longValue >> 24) & 0xff),
            (byte)((longValue >> 16) & 0xff),
            (byte)((longValue >> 8) & 0xff),
            (byte)((longValue >> 0) & 0xff),
            (byte)((address[0] << 4) | address[1]),
            (byte)((address[2] << 4) | address[3]),
            (byte)((address[4] << 4) | address[5]),
            (byte)((address[6] << 4) | address[7]),         
            (byte) 0x00,
            (byte) 0x00
        };
    }
}
