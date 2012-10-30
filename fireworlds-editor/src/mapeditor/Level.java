package mapeditor;

import java.io.*;
import java.util.ArrayList;
import java.util.LinkedList;
import org.omg.CORBA.INTERNAL;

public class Level
{
    private static final int blockCount = 8;
    private static final int magicHeader = 0x564C4744;
    
    LinkedList<LevelObj> objs = new LinkedList<LevelObj>();
    LinkedList<LevelActorSpawn> acts = new LinkedList<LevelActorSpawn>();
    String helpText = "";

    int getCoincidence(LevelObj o, int x1, int y1, int x2, int y2)
    {
        for(LevelObj p : objs)
        {
            if(p != o && p.type == o.type)
                if(p.hasVertex(x1, y1) && p.hasVertex(x2, y2)) return p.levelIdx;
        }

        return -1;
    }
/*
    int calcCoincidences(LevelObj o)
    {
        int res = 0;

        for(int i = 0; i < 4; i++)
        {
            if(getCoincidence(o, o.x[i], o.y[i], o.x[(i+1)%4], o.y[(i+1)%4]))
                res |= 1<<i;
        }

        return res;
    }
*/
    public void save(OutputStream out) throws IOException
    {
        LinkedList<LevelObj> invalidObjs = new LinkedList<LevelObj>();
        for(LevelObj o : objs)
            if(!o.isValid()) invalidObjs.add(o);

        objs.removeAll(invalidObjs);

        LEDataOutputStream idx = new LEDataOutputStream(out);
        ByteArrayOutputStream blocks = new ByteArrayOutputStream();

        int headerSize = 4+blockCount*8;

        //Magic Stamp
        idx.writeInt(magicHeader);

        byte[] blockData;

        //Block 0: Objects
        idx.writeInt(blocks.size()+headerSize);
        blockData = saveObjects();
        idx.writeInt(blockData.length);
        blocks.write(blockData);
        
        //Block 1: Actors
        idx.writeInt(blocks.size()+headerSize);
        blockData = saveActors();
        idx.writeInt(blockData.length);
        blocks.write(blockData);

        //Block 2: Help text
        idx.writeInt(blocks.size()+headerSize);
        blockData = saveHelp();
        idx.writeInt(blockData.length);
        blocks.write(blockData);

        for(int i = 0; i < 5; i++)
        {
            idx.writeInt(0);
            idx.writeInt(0);
        }
        idx.write(blocks.toByteArray());
    }

    public byte[] saveHelp()
    {
        if(helpText == null) helpText = "";
        byte[] res = new byte[helpText.length()+1];
        for(int i = 0; i< helpText.length(); i++)
            res[i] = (byte)helpText.charAt(i);
        res[res.length-1] = 0;
        return res;
    }

    public byte[] saveObjects() throws IOException
    {
        ByteArrayOutputStream o = new ByteArrayOutputStream();
        LEDataOutputStream od = new LEDataOutputStream(o);

        ByteArrayOutputStream o2 = new ByteArrayOutputStream();
        LEDataOutputStream xd = new LEDataOutputStream(o2);

        //First order objects! xD
        int objCt = 0;
        for(int x = 0; x < 32; x++)
            for(int y = 0; y < 32; y++)
                for(LevelObj ob : objs)
                {
                    ob.calcTopLeft();
                    if(ob.xt >= x*256 && ob.xt < x*256+256)
                        if(-ob.yt+32*256 >= y*256 && -ob.yt+32*256 < y*256+256)
                            ob.levelIdx = objCt++;
                }


        xd.writeInt(objCt);

        objCt  = 0;
        for(int x = 0; x < 32; x++)
            for(int y = 0; y < 32; y++)
            {
                int thisobj = objCt;

                for(LevelObj ob : objs)
                {
                    ob.calcTopLeft();
                    if(ob.xt >= x*256 && ob.xt < x*256+256)
                        if(-ob.yt+32*256 >= y*256 && -ob.yt+32*256 < y*256+256)
                        {
                            ob.reflip();
                            for(int i = 0; i < 4; i++)
                                od.writeInt(ob.x[i]);
                            for(int i = 0; i < 4; i++)
                                od.writeInt(-ob.y[i]+32*256);
                            od.writeInt(ob.type);
                            for(int i = 0; i < 4; i++)
                                od.writeInt(getCoincidence(ob, ob.x[i], ob.y[i], ob.x[(i+1)%4], ob.y[(i+1)%4]));

                            objCt++;
                        }
                }

                xd.writeInt(thisobj);
                xd.writeInt(objCt-thisobj);
                
            }

        xd.write(o.toByteArray());
        return o2.toByteArray();
    }


    public byte[] saveActors() throws IOException
    {
        ByteArrayOutputStream o2 = new ByteArrayOutputStream();
        LEDataOutputStream xd = new LEDataOutputStream(o2);

        xd.writeInt(acts.size());
        for(LevelActorSpawn ob : acts)
        {
            xd.writeInt(ob.x);
            xd.writeInt(-ob.y+32*256);
            xd.writeInt(ob.type);
            xd.writeInt(ob.data);
        }

        return o2.toByteArray();
    }

    public static  Level read(InputStream in) throws IOException
    {
        LEDataInputStream ind = new LEDataInputStream(in);

        if(ind.readInt() != magicHeader) throw new RuntimeException("Bad header magic. Not a good level file?");
        int[] blocksOffs = new int[blockCount];
        int[] blocksLen = new int[blockCount];
        for(int i = 0; i < blockCount; i++)
        {
            blocksOffs[i] = ind.readInt();
            blocksLen[i] = ind.readInt();
        }

        Level l = new Level();
        
        byte [] objectsBlock = new byte[blocksLen[0]];
        ind.readFully(objectsBlock);
        byte [] actorsBlock = new byte[blocksLen[1]];
        ind.readFully(actorsBlock);
        byte [] helpBlock = new byte[blocksLen[2]];
        ind.readFully(helpBlock);
        l.helpText = "";
        for(int i = 0; i < helpBlock.length; i++)
            if(helpBlock[i] != 0)
                l.helpText += (char) helpBlock[i];

        l.readObjects(new ByteArrayInputStream(objectsBlock));
        l.readActors(new ByteArrayInputStream(actorsBlock));
        return l;
    }

    public void readObjects(InputStream in) throws IOException
    {
        LEDataInputStream ind = new LEDataInputStream(in);
        int count = ind.readInt();
        ind.skipBytes(4*2*32*32); //useless index data :P

        for(int i = 0; i < count; i++)
        {
            int[] x = new int[4];
            int[] y = new int[4];
            for(int j = 0; j < 4; j++)
                x[j] = ind.readInt();
            for(int j = 0; j < 4; j++)
                y[j] = -(ind.readInt()-32*256);

            LevelObj obj = new LevelObj(x, y);
            obj.type = ind.readInt();
            for(int j = 0; j < 4; j++) ind.readInt(); //vtx coincidences
            objs.add(obj);
        }
        in.close();
    }

    public void readActors(InputStream in) throws IOException
    {
        LEDataInputStream ind = new LEDataInputStream(in);
        int count = ind.readInt();

        for(int i = 0; i < count; i++)
        {
            LevelActorSpawn act = new LevelActorSpawn();
            act.x = ind.readInt();
            act.y = -(ind.readInt()-32*256);
            act.type = ind.readInt();
            act.data = ind.readInt();
            acts.add(act);
        }
    }
}
