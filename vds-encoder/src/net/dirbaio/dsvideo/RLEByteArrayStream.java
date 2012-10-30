package net.dirbaio.dsvideo;

import java.io.ByteArrayOutputStream;
import java.io.IOException;


public class RLEByteArrayStream
{
    LEDataOutputStream os;
    ByteArrayOutputStream os2;

    public RLEByteArrayStream()
    {
        os2 = new ByteArrayOutputStream();
        os = new LEDataOutputStream(os2);
    }

    boolean similar(int a, int b)
    {
        if(a == b) return true;
        if(a == b+1 || a == b-1) return true;
        if(a == b+1<<5 || a == b-1<<5) return true;
        if(a == b+1<<10 || a == b-1<<10) return true;
        return false;
    }

    void writeRun() throws IOException
    {
        if(ct == 1)
            os.writeShort(run|0x8000);
        else
        {
            os.writeShort(run);
            os.writeShort(ct);
        }
    }
    int run = -1;
    int ct = 0;
    public void writeShort(int s) throws IOException
    {
        if(run != -1 && similar(s, run))
            ct++;
        else
        {
            if(run != -1)
                writeRun();

            run = s;
            ct = 1;
        }
    }

    public void end() throws IOException
    {
        if(run != -1)
            writeRun();
    }

    public byte[] getCompressed()
    {
        return os2.toByteArray();
    }
}
