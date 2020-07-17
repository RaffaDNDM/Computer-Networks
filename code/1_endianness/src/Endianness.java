import java.nio.ByteOrder;
import java.nio.ByteBuffer;

public class Endianness
{
    private boolean little_endian;

    public Endianness()
    {
        little_endian = isLittleEndian();
    }

    public boolean isLittleEndian()
    {
        return ByteOrder.nativeOrder().equals(ByteOrder.LITTLE_ENDIAN);
    }
    
    public short htons(short num)
    {
        if(little_endian)
        {
            ByteBuffer bbuf = ByteBuffer.allocate(2);
            bbuf.order(ByteOrder.LITTLE_ENDIAN);
            bbuf.putShort(num);
            bbuf.order(ByteOrder.BIG_ENDIAN);
        
            return bbuf.getShort(0);
        }
        else
            return num;
    }

    public int htonl(int num)
    {
        if(little_endian)
        {
            ByteBuffer bbuf = ByteBuffer.allocate(4);
            bbuf.order(ByteOrder.LITTLE_ENDIAN);
            bbuf.putInt(num);
            bbuf.order(ByteOrder.BIG_ENDIAN);
        
            return bbuf.getInt(0);
        }
        else
            return num;
    }

    public static void main(String argv[])
    {
        Endianness e = new Endianness();
        
        int n1 = 0x01020304;
        short n2 = 0x0102;
        System.out.println("0x0"+Integer.toString(n1,16));
        System.out.println("0x0"+Integer.toString(n2,16));
        
        n1 = e.htonl(n1);
        n2 = e.htons(n2);
        System.out.println("0x0"+Integer.toString(n1,16));
        System.out.println("0x0"+Integer.toString(n2,16));
    }
}
