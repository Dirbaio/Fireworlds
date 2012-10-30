package net.dirbaio.dsvideo;

import java.awt.image.BufferedImage;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.zip.GZIPOutputStream;
import javax.imageio.ImageIO;
import javax.swing.JOptionPane;

public class Main
{
    public static void main(String[] args) throws IOException
    {

        String path = JOptionPane.showInputDialog("Please enter the path to where the image sequence is.");
        if(path == null) return;
        
        if(!(path.endsWith("/")) ||path.endsWith("\\"))
            path += "/";

        File f = new File(path);
//        File f = new File("C:/Users/Dario/Documents/3dsmax/renderoutput/");
        File[] files = f.listFiles(new FilenameFilter() {

            public boolean accept(File dir, String name) {
                return name.toLowerCase().endsWith(".png") ||name.toLowerCase().endsWith(".jpg");
            }
        });

        FileOutputStream ofs = new FileOutputStream(path + "out.vds");
        DSVideoEncoder out = new DSVideoEncoder(ofs);
        int fr = 300000;
        int tfr = 0;
        for(File ff : files)
        {
            if(fr-- == 0) break;
            tfr++;
            BufferedImage bi = ImageIO.read(ff);
            boolean resize = false;
            if(bi.getWidth() != 256 ||bi.getHeight() != 192)
            {
                BufferedImage bi2 = new BufferedImage(256, 192, BufferedImage.TYPE_INT_RGB);
                bi2.getGraphics().drawImage(bi, 0, 0, 256, 192, null);
                bi = bi2;
                resize = true;
            }
            out.writeFrame(bi);
            System.out.println(ff.getName() + " "+ tfr+"/"+files.length + (resize?" [R]":""));
        }
        out.close();
        JOptionPane.showMessageDialog(null, "Finished! You can close the window now.");

    }

}
