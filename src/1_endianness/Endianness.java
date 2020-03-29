import java.nio.ByteOrder;

public class Endianness
{
    public static void main(String argv[])
    {
        if (ByteOrder.nativeOrder().equals(ByteOrder.BIG_ENDIAN))
            System.out.println("Big Endian");
        else
            System.out.println("Little Endian");
    }
}
