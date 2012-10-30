package mapeditor;

import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.*;

public class Main extends JFrame implements ActionListener
{
    EditorComponent ed;
    public Main()
    {
        super("DirbaioGame Map Editor");
//        this.getRootPane().setLayout(new BorderLayout());

        Level l = new Level();

        JPanel top = new JPanel(new BorderLayout());

        JLabel lab=new JLabel("Troll");
        top.add(lab, BorderLayout.CENTER);

        ed = new EditorComponent(l, lab);
        add(new JScrollPane(ed), BorderLayout.CENTER);
        add(top, BorderLayout.NORTH);

        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(1000, 500);
        
    }
    public static void main(String[] args)
    {
        new Main().setVisible(true);
    }

    public void actionPerformed(ActionEvent e) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

}
