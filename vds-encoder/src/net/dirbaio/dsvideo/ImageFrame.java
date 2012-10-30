package net.dirbaio.dsvideo;

import java.awt.*;
import javax.swing.*;

public class ImageFrame extends JFrame
{
    int w = 256;
    int h = 192;

    public Image img;
    
    public ImageFrame()
    {
        super("Dirbaio's Video Encoder");
        setSize(w, h);
        setVisible(true);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }

    @Override
    public void paint(Graphics g)
    {
        g.drawImage(img, 0, 0, null);
    }
}
