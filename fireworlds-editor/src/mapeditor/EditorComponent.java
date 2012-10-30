package mapeditor;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;

public class EditorComponent extends JComponent implements MouseListener, MouseMotionListener, KeyListener
{
    Level l;
    File f = null;
    int blocktype, data;
    int drawMode;
    
    JFileChooser chooser = new JFileChooser();
    File fl = null;

    private static final int TOOL_OBJ = 0;
    private static final int TOOL_ACT = 1;

    int tool = 0;
    
    public JLabel infoLabel;
    public JTextField spriteLabel;
    JLabel status;

    public EditorComponent(Level l, JLabel status)
    {
        this.status = status;
        setSize(32*256, 32*256);
        setPreferredSize(new Dimension(32*256, 32*256));
        this.l = l;
        addMouseListener(this);

        addMouseMotionListener(this);
        setFocusable(true);
        addKeyListener(this);
    }

    @Override
    public void paint(Graphics g)
    {

        g.setColor(Color.BLACK);
        g.fillRect(0, 0, 32*256, 32*256);
        for(LevelObj o : l.objs)
            o.render(g);
        for(LevelActorSpawn o : l.acts)
            o.render(g);


        if(tool == TOOL_OBJ)
        {
            g.setColor(Color.BLUE);
            for(int i = 0; i <= createPointNum; i++)
            {
                g.fillOval(x[i]-4, y[i]-4, 8, 8);
                if(i != 0)
                    g.drawLine(x[i-1], y[i-1], x[i], y[i]);
            }
        }
        
        g.setColor(Color.RED);
        g.drawRect(xp-1, yp-1, 3, 3);
    }

    public void mouseClicked(MouseEvent e) {
    }

    int xd, yd;

    int xp, yp;

    int[] x = new int[4];
    int[] y = new int[4];
    int createPointNum = 0;
    
    public void mousePressed(MouseEvent e)
    {
        xd = e.getX();
        yd = e.getY();
        
        xd -= xd%8;
        yd -= yd%8;
        if(e.getButton() == e.BUTTON1)
        {
            if(tool == TOOL_OBJ)
            {
                x[createPointNum] = xd;
                y[createPointNum] = yd;

                if(createPointNum == 3 && drawMode == 0)
                {
                    LevelObj o = new LevelObj(x, y);
                    o.type = blocktype;
                    if(o.isValid())
                        l.objs.add(o);
                    createPointNum = 0;
                }
                else if (createPointNum == 1 && drawMode == 1)
                {
                    int[] xx = new int[4];
                    int[] yy = new int[4];
                    xx[0] = x[0];
                    xx[1] = x[0];
                    xx[2] = x[1];
                    xx[3] = x[1];
                    yy[0] = y[0];
                    yy[1] = y[1];
                    yy[2] = y[1];
                    yy[3] = y[0];
                    LevelObj o = new LevelObj(xx, yy);
                    o.type = blocktype;
                    if(o.isValid())
                        l.objs.add(o);
                    createPointNum = 0;
                }
                else if (createPointNum == 2 && drawMode == 2)
                {
                    double centerX = x[0];
                    double centerY = y[0];

                    double minRad = Math.sqrt((centerX-x[1])*(centerX-x[1]) + (centerY-y[1])*(centerY-y[1]));
                    double maxRad = Math.sqrt((centerX-x[2])*(centerX-x[2]) + (centerY-y[2])*(centerY-y[2]));

                    int count = 24;
                    for(int i = 0; i < count; i++)
                    {
                        double ang1 = i * 2 * Math.PI / count;
                        double ang2 = (i+1) * 2 * Math.PI / count;
                        if(i == count-1) ang2 = 0; //To avoid a "gap" due to rounding errors.
                        
                        int[] xx = new int[4];
                        int[] yy = new int[4];

                        xx[0] = (int)(centerX + minRad*Math.sin(ang1));
                        yy[0] = (int)(centerY + minRad*Math.cos(ang1));
                        xx[1] = (int)(centerX + minRad*Math.sin(ang2));
                        yy[1] = (int)(centerY + minRad*Math.cos(ang2));
                        xx[2] = (int)(centerX + maxRad*Math.sin(ang2));
                        yy[2] = (int)(centerY + maxRad*Math.cos(ang2));
                        xx[3] = (int)(centerX + maxRad*Math.sin(ang1));
                        yy[3] = (int)(centerY + maxRad*Math.cos(ang1));

                        for(int j = 0; j < 4; j++)
                        {
                            xx[j] -= xx[j]%8;
                            yy[j] -= yy[j]%8;
                        }
                        
                        LevelObj o = new LevelObj(xx, yy);
                        o.type = blocktype;
                        if(o.isValid())
                            l.objs.add(o);
                    }
                    createPointNum = 0;
                }
                else
                    createPointNum++;

                x[createPointNum] = xd;
                y[createPointNum] = yd;
            }
            else
            {
                LevelActorSpawn a = new LevelActorSpawn();
                a.x = xd;
                a.y = yd;
                a.type = blocktype;
                a.data = data;
                l.acts.add(a);
            }
        }

        else
        {
            Object del = null;
            for(LevelObj o : l.objs)
                if(o.collision(xd, yd))
                    del = o;

            for(LevelActorSpawn o : l.acts)
                if(o.collision(xd, yd))
                    del = o;
            l.objs.remove(del);
            l.acts.remove(del);
            createPointNum = 0;
        }
        

        repaint();
    }

    public void mouseReleased(MouseEvent e) {
        /*
        if(e.getButton() == e.BUTTON1)
        {
            down = false;
            int xm = xd<xu?xd:xu;
            int ym = yd<yu?yd:yu;
            int xx = xd>xu?xd:xu;
            int yx = yd>yu?yd:yu;

            LevelObj o = new LevelObj();
            o.x = xm;
            o.y = ym;
            o.tx = xx-xm+1;
            o.ty = yx-ym+1;
            if(o.tx > 16) o.tx = 16;
            if(o.ty > 16) o.ty = 16;

            if(o.x < 0) o.x = 0;
            if(o.y < 0) o.y = 0;

            l.objs.add(o);
            repaint();
        }*/
    }

    public void mouseEntered(MouseEvent e) {
    }

    public void mouseExited(MouseEvent e) {
    }

    public void mouseDragged(MouseEvent e) {
    }

    public void mouseMoved(MouseEvent e) {
        xp = e.getX();
        yp = e.getY();
        xp -= xp%8;
        yp -= yp%8;

        x[createPointNum] = xp;
        y[createPointNum] = yp;
        repaint();
    }

    public void keyTyped(KeyEvent e) {
    }

    public void keyPressed(KeyEvent e) {
        if(e.getKeyChar() == 's')
        {
            FileNameExtensionFilter filter = new FileNameExtensionFilter("Level files", "dgl");
            chooser.setFileFilter(filter);

            int returnVal = chooser.showSaveDialog(this);
            if(returnVal == JFileChooser.APPROVE_OPTION)
            {
                try {
                    FileOutputStream out = new FileOutputStream(fl = chooser.getSelectedFile());
                    l.save(out);
                    out.close();
                } catch (IOException ex) {
                    ex.printStackTrace();
                }
            }
        }

        if(e.getKeyChar() == 'a' && fl != null)
            try {
                FileOutputStream out = new FileOutputStream(fl);
                l.save(out);
                out.close();
            } catch (IOException ex) {
                ex.printStackTrace();
            }


        if(e.getKeyChar() == 'd')
        {
            FileNameExtensionFilter filter = new FileNameExtensionFilter("Level files", "dgl");
            chooser.setFileFilter(filter);

            int returnVal = chooser.showOpenDialog(this);
            if(returnVal == JFileChooser.APPROVE_OPTION)
            {
                try {
                    FileInputStream in = new FileInputStream(fl = chooser.getSelectedFile());
                    Level nl = Level.read(in);
                    setLevel(nl);
                    in.close();
                } catch (IOException ex) {
                    ex.printStackTrace();
                }
            }
        }
        if(e.getKeyChar() == 'f')
            if(JOptionPane.showConfirmDialog(this.status, "Are you sure you want to erase ALL the level?", "TROLL", JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION)
            {
                setLevel(new Level());
                fl = null;
            }

        char c = e.getKeyChar();

        if(c >= '1' && c <= '9')
        {
            blocktype = c - '1';
            tool = TOOL_OBJ;
            updateStatus();
        }

        if(c == 'j') setDrawMode(0);
        if(c == 'k') setDrawMode(1);
        if(c == 'l') setDrawMode(2);


        if(c == 'q') setActorNum(0);
        if(c == 'w') setActorNum(1);
        if(c == 'e') setActorNum(2);
        if(c == 'r') setActorNum(3);
        if(c == 't') setActorNum(4);
        if(c == 'y') setActorNum(5);
        if(c == 'u') setActorNum(6);
        if(c == 'i') setActorNum(7);
        if(c == 'o') setActorNum(8);
        if(c == 'p') setActorNum(9);

        if(c == 'z') setData(0);
        if(c == 'x') setData(1);
        if(c == 'c') setData(2);
        if(c == 'v') setData(3);
        if(c == 'b') setData(4);
        if(c == 'n') setData(5);
        if(c == 'm') setData(6);

        if(c == 'h')
            l.helpText = JOptionPane.showInputDialog("Help text", l.helpText);

        createPointNum = 0;
    }

    public void setDrawMode(int m)
    {
        drawMode = m;
        updateStatus();
    }
    public void setData(int d)
    {
        data = d;
        updateStatus();
    }
    public void setActorNum(int act)
    {
        tool = TOOL_ACT;
        blocktype = act;
        updateStatus();
    }

    public void updateStatus()
    {
        String dm = "?";
        if(drawMode == 0) dm = "QUAD";
        if(drawMode == 1) dm = "RECT";
        if(drawMode == 2) dm = "CIRCLE";
        
        if(tool == TOOL_ACT)
            status.setText("Actor: "+LevelActorSpawn.names[blocktype]+":"+data);
        else
            status.setText("Block: "+LevelObj.names[blocktype] + "  " + dm);
    }
    public void keyReleased(KeyEvent e) {
    }

    public void setLevel(Level l)
    {
        this.l = l;
        repaint();
    }
}
