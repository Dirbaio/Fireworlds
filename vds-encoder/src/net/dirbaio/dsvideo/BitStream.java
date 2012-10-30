package net.dirbaio.dsvideo;

import java.io.ByteArrayOutputStream;

public class BitStream
{
    ByteArrayOutputStream out;
    int buf = 0;
    int fill = 0;
    
    public BitStream() {
        out = new ByteArrayOutputStream();
    }

    public void write5bit(int n)
    {
//           System.out.println(n);
        buf = (buf << 5) | (n & 0x1F);
        fill += 5;

        if(fill >= 16)
        {
            int d = buf >> (fill-16);
            out.write(d);
            out.write(d>>8);
            fill -= 16;
        }
    }

    public byte[] end()
    {
        if(fill != 0) throw new RuntimeException("GRAAAAH");
        return out.toByteArray();
    }
}
