

public class ToolSlove {

    private static int AddH,AddL,Channel;


    public final String getAddH() {
        return Integer.toHexString(AddH);
    }


    public final String getAddL() {
        return Integer.toHexString(AddL);
    }


    public final String getChannel() {
        return Integer.toHexString(Channel);
    }


    ToolSlove(String Mac1,String Mac2,String Mac3,String Mac4,String Mac5,String Mac6){
        if(Mac1 == null || Mac2 == null || Mac3 == null || Mac4 == null || Mac5 == null || Mac6 == null)
            return;
        int[]num = new int[6];

        num[0] = Integer.parseInt(Mac1,16);
        num[1] = Integer.parseInt(Mac2,16);
        num[2] = Integer.parseInt(Mac3,16);
        num[3] = Integer.parseInt(Mac4,16);
        num[4] = Integer.parseInt(Mac5,16);
        num[5] = Integer.parseInt(Mac6,16);

        AddH = (num[0]+ num[1]+num[2])&255;
        AddL = (num[3]+ num[4]+num[5])&255;
        Channel = (num[0]+ num[1]+num[2]+num[3]+ num[4]+num[5])&255;

        while(AddH > 255) AddH -= 256;
        while(AddL > 255) AddL -= 256;
        while(Channel > 31) Channel -= 32;

    }
}
