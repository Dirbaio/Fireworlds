package mapeditor;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;

public class LevelActorSpawn
{
    public static final int ACTOR_SPAWN = 0;
    public static final int ACTOR_BALL = 1;
    public static final int ACTOR_ENDING = 2;
    public static final int ACTOR_GRAVITY = 3;

    public static final String[] names =
    {
        "unk",
        "Ball",
        "Exit",
        "Gravity",
        "Energy",
        "Music",
        "Ice Block",
        "Switch",
        "Iron Block",
        "Gravity Switch",
   };

    public static final Color[] colors =
    {
        Color.BLACK,
        Color.ORANGE,
        Color.CYAN,
        Color.GRAY,
        Color.WHITE,
        Color.BLUE,
        Color.CYAN,
        Color.GREEN,
        Color.DARK_GRAY,
        Color.PINK,
        Color.BLACK,
        Color.BLACK,
        Color.BLACK,
        Color.BLACK,
        Color.BLACK,
    };

    int type;
    int data;
    int x, y;

    void render(Graphics g)
    {
        g.setColor(colors[type]);
        g.fillRect(x-8, y-8, 16, 16);
        g.setColor(Color.RED);
        g.drawRect(x-8, y-8, 16, 16);
        g.setColor(Color.BLACK);
        g.setFont(new Font("Dialog", Font.BOLD, 10));
        g.drawString(data+"", x-5, y+5);

        if(type == 6)
        {
            g.setColor(Color.WHITE);
            g.drawRect(x-32, y-32, 64, 64);
        }
    }

    boolean collision(int xx, int yy)
    {
        return xx >= x-8 && xx <= x+8 &&
               yy >= y-8 && yy <= y+8;
    }
}
