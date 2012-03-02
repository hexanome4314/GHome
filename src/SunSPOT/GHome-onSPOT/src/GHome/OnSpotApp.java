package GHome;

import com.sun.spot.io.j2me.radiogram.RadiogramConnection;
import com.sun.spot.peripheral.IBattery;
import com.sun.spot.resources.Resources;
import com.sun.spot.resources.transducers.ILightSensor;
import com.sun.spot.resources.transducers.ITemperatureInput;
import com.sun.spot.resources.transducers.ITriColorLED;
import com.sun.spot.util.Utils;
import javax.microedition.io.Connector;
import javax.microedition.io.Datagram;
import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

public class OnSpotApp extends MIDlet {
    private static final int HOST_PORT = 67;
    private static final int SAMPLE_PERIOD = 3 * 1000;  // in milliseconds
    private static final int BUFFER_SIZE = 19 + 8 + 1; // Addresse + value + idProbe
    
    protected void startApp() throws MIDletStateChangeException {
        RadiogramConnection rCon = null;
        Datagram dg = null;
        String ourAddress = System.getProperty("IEEE_ADDRESS");
        ILightSensor lightSensor = (ILightSensor)Resources.lookup(ILightSensor.class);
        ITemperatureInput temperatureInput = (ITemperatureInput)Resources.lookup(ITemperatureInput.class);
        IBattery batteryLevel = (IBattery)Resources.lookup(IBattery.class);
        ITriColorLED led = (ITriColorLED)Resources.lookup(ITriColorLED.class, "LED7");
        
        System.out.println("Starting sensor sampler application on " + ourAddress + " ...");
        // Listen for downloads/commands over USB connection
        new com.sun.spot.service.BootloaderListenerService().getInstance().start();

        try {
            // Open up a broadcast connection to the host port
            // where the 'on Desktop' portion of this demo is listening
            rCon = (RadiogramConnection) Connector.open("radiogram://broadcast:" + HOST_PORT);
            dg = rCon.newDatagram(BUFFER_SIZE);  // only sending 50 bytes of data           
        } catch (Exception e) {
            System.err.println("Caught " + e + " in connection initialization.");
            notifyDestroyed();
        }
        
        while (true) {
            try {
                // Get the current time and sensor reading
                long now = System.currentTimeMillis();
                int light = lightSensor.getValue();
                double temperature = temperatureInput.getCelsius();
                int battery = 100;//batteryLevel.getBatteryLevel();

                // Flash an LED to indicate a sampling event
                led.setRGB(255, 255, 255);
                led.setOn();
                Utils.sleep(50);
                led.setOff();
                
                dg.reset();
                //dg.writeChar(0);
                dg.writeInt(light);// Send lightValue to the base
                dg.writeFloat(new Double(temperature).floatValue());// Send temperatureInput to the base
                dg.writeInt(battery);
                rCon.send(dg);
                
                System.out.println("New light value = " + light);                
                System.out.println("New temperature value = " + temperature);
                System.out.println("New battery value = " + battery);

                // Go to sleep to conserve battery
                Utils.sleep(SAMPLE_PERIOD - (System.currentTimeMillis() - now));
            } catch (Exception e) {
                System.err.println("Caught " + e + " while collecting/sending sensor sample.");
            }
        }
    }
    
    protected void pauseApp() {
        // This will never be called by the Squawk VM
    }
    
    protected void destroyApp(boolean arg0) throws MIDletStateChangeException {
        // Only called if startApp throws any exception other than MIDletStateChangeException
    }
}
