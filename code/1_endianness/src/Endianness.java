import java.nio.ByteOrder;

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
        return 0;
    }

    public int htonl(int num)
    {
        return 0;
    }

    public static void main(String argv[])
    {
        Endianness e = new Endianness();
         
    }
}
