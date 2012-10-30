package net.dirbaio.dsvideo;

import java.awt.Point;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public class DSVideoEncoder {

    LEDataOutputStream out;
    private static final int KEYFRAME_FREQ = 100;
    int framesUntilKeyframe = 0;
    private static final int IMG_TX = 256;
    private static final int IMG_TY = 192;
    private static final int BLOCK_SIZE = 8;
//    private static final int MAX_DIFF = 800;
    private static final int MAX_DIFF = 130;
    private static final int BLOCK_SEARCH_RES = 2;
    private static final int U_SUBSAMPLING = 4;
    private static final int V_SUBSAMPLING = 4;
    private static final int Y_BITS = 5;
    private static final int U_BITS = 4;
    private static final int V_BITS = 4;
    BufferedImage refImage;
    BufferedImage targetImage;
    int totalFrames = 0;
    int keyFrames = 0;
    int totalBlocks = 0;
    int keyBlocks = 0;
    ImageFrame imgf = new ImageFrame();

    public DSVideoEncoder(OutputStream fout) {
        out = new LEDataOutputStream(fout);
        refImage = new BufferedImage(IMG_TX, IMG_TY, BufferedImage.TYPE_INT_RGB);
        
    }

    int byteDiff(int a, int b) {
        a /= 8;
        b /= 8;
        int r = (a - b);
        if (r < 0) {
            return -r;
        } else {
            return r;
        }
    }

    int colorDiff(int a, int b) {
        return byteDiff((a >> 0) & 0xFF, (b >> 0) & 0xFF)
                + byteDiff((a >> 8) & 0xFF, (b >> 8) & 0xFF)
                + byteDiff((a >> 16) & 0xFF, (b >> 16) & 0xFF);
    }

    int blockDiff(int ax, int ay, int bx, int by) {
        if (ax < 0) {
            return Integer.MAX_VALUE;
        }
        if (ay < 0) {
            return Integer.MAX_VALUE;
        }
        if (ax + BLOCK_SIZE > IMG_TX) {
            return Integer.MAX_VALUE;
        }
        if (ay + BLOCK_SIZE > IMG_TY) {
            return Integer.MAX_VALUE;
        }

        int res = 0;

        for (int x = 0; x < BLOCK_SIZE; x++) {
            for (int y = 0; y < BLOCK_SIZE; y++) {
                res += colorDiff(refImage.getRGB(x + ax, y + ay), targetImage.getRGB(x + bx, y + by));
            }
        }

        return res;
    }

    //Find best match of xo, yo in target img
    //around x, y in ref img
    Point findBestMatch(int xo, int yo, int x, int y, int s) {
        if (s == 0) {
            return new Point(x, y);
        }

        int bestx = -1;
        int besty = -1;
        int bestd = Integer.MAX_VALUE;

        for (int xx = -BLOCK_SEARCH_RES; xx <= BLOCK_SEARCH_RES; xx++) {
            for (int yy = -BLOCK_SEARCH_RES; yy <= BLOCK_SEARCH_RES; yy++) {
                int d = blockDiff(x + xx * s, y + yy * s, xo, yo);
                if (d < bestd) {
                    bestx = x + xx * s;
                    besty = y + yy * s;

                    bestd = d;
                }
            }
        }

        if (bestx == -1) {
            throw new RuntimeException("LOL?");
        }


        return findBestMatch(xo, yo, bestx, besty, (int) (s * 0.3));
    }

    int yc(int col) {
        int r = (col >> 16) & 0xFF;
        int g = (col >> 8) & 0xFF;
        int b = (col) & 0xFF;

        int y = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
        return y;
    }

    int uc(int col) {
        int r = (col >> 16) & 0xFF;
        int g = (col >> 8) & 0xFF;
        int b = (col) & 0xFF;

        int u = ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
        return u;
    }

    int vc(int col) {
        int r = (col >> 16) & 0xFF;
        int g = (col >> 8) & 0xFF;
        int b = (col) & 0xFF;

        int v = ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;
        return v;
    }

    short colToShort(int col) {
        int y = yc(col) >> 3;
        int u = uc(col) >> 3;
        int v = vc(col) >> 3;

        return (short) (y << 10 | u << 5 | v);
    }

    short colToShort15(int col) {
        int r = (col >> 19) & 0x1F;
        int g = (col >> 11) & 0x1F;
        int b = (col >> 3) & 0x1F;

        return (short) (b << 10 | g << 5 | r);
    }

    int getMeanU(int xx, int yy)
    {
        int res = 0;
        for(int x = 0; x < 4; x++)
            for(int y = 0; y < 4; y++)
                res += uc(targetImage.getRGB(x+xx, y+yy));

        res /= 16;
        return res >> 4;
    }

    int getMeanV(int xx, int yy)
    {
        int res = 0;
        for(int x = 0; x < 4; x++)
            for(int y = 0; y < 4; y++)
                res += vc(targetImage.getRGB(x+xx, y+yy));

        res /= 16;
        return res >> 4;
    }

    int[][] dxs = new int[IMG_TX / BLOCK_SIZE][IMG_TY / BLOCK_SIZE];
    int[][] dys = new int[IMG_TX / BLOCK_SIZE][IMG_TY / BLOCK_SIZE];

    public void writeFrame(BufferedImage bi) throws IOException {
        ByteArrayOutputStream buffer = new ByteArrayOutputStream();
        LEDataOutputStream bufwriter = new LEDataOutputStream(buffer);


        targetImage = bi;
        BufferedImage newRefImage = new BufferedImage(IMG_TX, IMG_TY, BufferedImage.TYPE_INT_RGB);


        for (int by = 0; by < IMG_TY / BLOCK_SIZE; by++) {
            for (int bx = 0; bx < IMG_TX / BLOCK_SIZE; bx++) {
                boolean needsRawBlock = false;
                totalBlocks++;

                if (framesUntilKeyframe == 0) {
                    needsRawBlock = true;
                } else {
                    Point p = findBestMatch(bx * BLOCK_SIZE, by * BLOCK_SIZE, bx * BLOCK_SIZE, by * BLOCK_SIZE, 6);
                    int dif = blockDiff(bx * BLOCK_SIZE, by * BLOCK_SIZE, p.x, p.y);
                    if (dif > MAX_DIFF) //Block has to be new
                        needsRawBlock = true;
                    else {
                        int dx = p.x - bx * BLOCK_SIZE;
                        int dy = p.y - by * BLOCK_SIZE;
                        int offs = dx+dy*256;
                        if ((offs & 0xFF00) == 0x8000) {
                            offs += 0x100; //0x80xx is a special magic code
                        }
                        bufwriter.writeShort(offs);
                        
                        for (int y = 0; y < BLOCK_SIZE; y++) {
                            for (int x = 0; x < BLOCK_SIZE; x++) {
                                int col = refImage.getRGB(bx * BLOCK_SIZE + x + dx, by * BLOCK_SIZE + y + dy);
                                newRefImage.setRGB(bx * BLOCK_SIZE + x, by * BLOCK_SIZE + y, col);
                            }
                        }
                    }
                }

                if (needsRawBlock) {

                    bufwriter.writeShort(0x8080);
                    for (int y = 0; y < BLOCK_SIZE; y++) {
                        for (int x = 0; x < BLOCK_SIZE; x++) {
                            int col = targetImage.getRGB(bx * BLOCK_SIZE + x , by * BLOCK_SIZE + y);
                            bufwriter.writeShort(colToShort15(col));
                            newRefImage.setRGB(bx * BLOCK_SIZE + x, by * BLOCK_SIZE + y, col);
                        }
                    }

                    /*
                    BitStream bs = new BitStream();

                    for (int y = 0; y < BLOCK_SIZE; y++) {
                        for (int x = 0; x < BLOCK_SIZE; x++) {
                            int col = targetImage.getRGB(bx * BLOCK_SIZE + x , by * BLOCK_SIZE + y);
                            newRefImage.setRGB(bx * BLOCK_SIZE + x, by * BLOCK_SIZE + y, col);
                        }
                    }


                    bufwriter.writeShort(0x8080);
                    for (int y = 0; y < BLOCK_SIZE; y++) {
                        for (int x = 0; x < BLOCK_SIZE; x++) {
                            int col = targetImage.getRGB(bx * BLOCK_SIZE + x, by * BLOCK_SIZE + y);
                            bs.write5bit(yc(col)>>3);
                        }
                    }
                    
                    bufwriter.write(bs.end()); //Y data

                    int u = 0;
                    u |= getMeanU(bx * BLOCK_SIZE + 0, by * BLOCK_SIZE + 0)<<12;
                    u |= getMeanU(bx * BLOCK_SIZE + 4, by * BLOCK_SIZE + 0)<<8;
                    u |= getMeanU(bx * BLOCK_SIZE + 0, by * BLOCK_SIZE + 4)<<4;
                    u |= getMeanU(bx * BLOCK_SIZE + 4, by * BLOCK_SIZE + 4);
                    bufwriter.writeShort(u);
                    int v = 0;
                    v |= getMeanV(bx * BLOCK_SIZE + 0, by * BLOCK_SIZE + 0)<<12;
                    v |= getMeanV(bx * BLOCK_SIZE + 4, by * BLOCK_SIZE + 0)<<8;
                    v |= getMeanV(bx * BLOCK_SIZE + 0, by * BLOCK_SIZE + 4)<<4;
                    v |= getMeanV(bx * BLOCK_SIZE + 4, by * BLOCK_SIZE + 4);
                    bufwriter.writeShort(v);*/
                    keyBlocks++;

                }
            }
        }

        totalFrames++;
        if (framesUntilKeyframe == 0)
        {
            keyFrames++;
            framesUntilKeyframe = KEYFRAME_FREQ;
        }
        framesUntilKeyframe--;


        refImage = newRefImage;
            imgf.img = newRefImage;
           imgf.repaint();

        byte[] data = buffer.toByteArray();
        out.writeInt(data.length);
        out.write(data);
    }

    void close() throws IOException {
        out.write(0);
        out.close();

        System.out.println("Encoding finished!");
        System.out.println("Key frames: " + keyFrames + " / " + totalFrames);
        if (totalBlocks == 0) {
            System.out.println("No blocks written. What!?!");
        } else {
            System.out.println("Key blocks: " + keyBlocks + " / " + totalBlocks + "(" + (keyBlocks * 100 / totalBlocks) + "%)");
        }

    }
}
