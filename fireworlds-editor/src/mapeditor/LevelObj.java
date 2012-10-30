package mapeditor;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Point;
import java.awt.Polygon;
import java.awt.Rectangle;

public class LevelObj
{
    int[] x, y;
    int xt, yt;
    int type;
    Polygon p;

    int levelIdx; //Temp var for saving, lol.

    public LevelObj()
    {
        x = new int[4];
        y = new int[4];
        p = new Polygon(x, y, 4);
    }

    public LevelObj(int[] x, int[] y)
    {
        this.x = new int[4];
        this.y = new int[4];

        for(int i = 0; i < 4; i++)
        {
            this.x[i] = x[i];
            this.y[i] = y[i];
        }
        p = new Polygon(x, y, 4);
    }
    
    public static final Color[] cols = {
        Color.WHITE,
        Color.PINK,
        Color.RED,
        Color.BLUE,
        Color.ORANGE,
        Color.CYAN,
        Color.GREEN,
        Color.BLUE,
        Color.YELLOW,
        Color.DARK_GRAY
    };

    public static final String[] names =
    {
        "Wall",
        "Bouncy",
        "Lava top",
        "Water top",
        "Lava",
        "Water",
        "Portal 1",
        "Portal 2",
        "unk",
        "unk",
    };

    void render(Graphics g)
    {
        p.invalidate();
        g.setColor(cols[type]);
        g.fillPolygon(p);
        g.setColor(Color.BLACK);
        g.drawPolygon(p);
        g.setColor(Color.WHITE);
        if(type > 1)
            g.drawLine(x[0], y[0], x[1], y[1]);
    }

    void calcTopLeft()
    {
        xt = x[0];
        yt = y[0];

        for(int i = 1; i < 4; i++)
        {
            if(xt>x[i]) xt=x[i];
            if(yt<y[i]) yt=y[i];
        }
    }

    boolean collision(int xx, int yy)
    {
        return p.contains(xx, yy);
    }
    
    boolean collisionRect(Rectangle r)
    {
        return p.intersects(r);
    }

    boolean isValid()
    {
        calcTopLeft();

        for(int i = 0; i < 4; i++)
            for(int j = 0; j < i; j++)
            {
                if(x[i] == x[j] && y[i] == y[j]) return false;
                if(x[i] > xt+256) return false;
                if(y[i] < yt-256) return false;
            }
        
        return true;
    }

    boolean hasVertex(int xx, int yy)
    {
        for(int i = 0; i < 4; i++)
        {
            if(x[i] == xx && y[i] == yy) return true;
        }

        return false;
    }

    boolean isReverse()
    {
        int a = 0;
        int b = 1;
        int xx = x[2];
        int yy = y[2];
        
        int s =  (x[b]-x[a])*(yy-y[a]) - (y[b]-y[a])*(xx-x[a]);
        return s<0;
    }
    
    void reflip()
    {
       if(isReverse())
       {
           int aux = x[0];
           x[0] = x[2];
           x[2] = aux;

           aux = y[0];
           y[0] = y[2];
           y[2] = aux;

           p.invalidate();
           System.out.println("flip");
       }
    }
}
