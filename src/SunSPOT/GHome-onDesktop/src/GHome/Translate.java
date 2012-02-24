//package GHome;
//
//import java.util.HashMap;
//import java.util.Map;
//
//public abstract class Translate {
//    private static Map<Byte, String> bts = new HashMap<Byte, String>();
//    private static Map<String, Byte> stb = new HashMap<String, Byte>();
//    
//    private static byte checkIntegrity() {
//        int btss = bts.size();
//        int stbs = stb.size();
//        
//        // Bad integrity => clear all
//        if(btss != stbs || btss >= Byte.MAX_VALUE) {
//            bts.clear();
//            stb.clear();
//        }
//        
//        return (byte) stb.size();
//    }
//    
//    private static byte newSensor(String newAdress) {
//        byte newId = checkIntegrity();
//        stb.put(newAdress, newId);
//        bts.put(newId, newAdress);
//        return newId;
//    }
//    
//    public static byte toId(String address) {
//        if(stb.containsKey(address))
//            return stb.get(address);
//        else
//            return newSensor(address);
//    }
//    
//    public static String toAddress(byte id) {
//        if(bts.containsKey(id))
//            return bts.get(id);
//        else
//            return null;
//    }
//    
//    public static byte[] hexStringToByteArray(String s) {
//        int len = s.length();
//        byte[] data = new byte[len / 2];
//        for (int i = 0; i < len; i += 2) {
//            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
//                                    + Character.digit(s.charAt(i+1), 16));
//        }
//        return data;
//    }    
//}
