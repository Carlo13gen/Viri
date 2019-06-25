package IU;

import java.awt.event.KeyEvent;

import javax.swing.BorderFactory;
import javax.swing.JColorChooser;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;

import Listener.ColorListener;

public class JColor extends JPanel {
	/**
	 * 
	 */
	private static final long serialVersionUID = -5010347838465994832L;
	private JColorChooser tcc0;
	private JColorChooser tcc1;
	private JColorChooser tcc2;
	private JColorChooser tcc3;
	private JColorChooser tcc4;
	@SuppressWarnings("unused")
	private ColorManager cm;
	
    public JColor(ColorManager cm) {
        this.cm = cm;

        tcc0 = new JColorChooser();
        tcc0.getSelectionModel().addChangeListener(new ColorListener("BACKNODES",cm));
        tcc0.setBorder(BorderFactory.createTitledBorder("Choose Background Node Color"));
        tcc0.setPreviewPanel(new JPanel());
        
        JTabbedPane tabbedPane = new JTabbedPane();

        tabbedPane.addTab("Node Background Color", tcc0);
        tabbedPane.setMnemonicAt(0, KeyEvent.VK_1);

        tcc1 = new JColorChooser();
        tcc1.getSelectionModel().addChangeListener(new ColorListener("BOUNDNODES",cm));
        tcc1.setPreviewPanel(new JPanel());
        tcc1.setBorder(BorderFactory.createTitledBorder("Choose Bound Node Color"));
        
        tabbedPane.addTab("Node Bound Color",tcc1);
        tabbedPane.setMnemonicAt(1, KeyEvent.VK_2);
        
        tcc2 = new JColorChooser();
        tcc2.getSelectionModel().addChangeListener(new ColorListener("LABELNODES",cm));
        tcc2.setPreviewPanel(new JPanel());
        tcc2.setBorder(BorderFactory.createTitledBorder("Choose Label Node Color"));
        
        tabbedPane.addTab("Node Label Color",tcc2);
        tabbedPane.setMnemonicAt(2, KeyEvent.VK_3);
        

        tcc3 = new JColorChooser();
        tcc3.getSelectionModel().addChangeListener(new ColorListener("EDGES",cm));
        tcc3.setPreviewPanel(new JPanel());
        tcc3.setBorder(BorderFactory.createTitledBorder("Choose Edge Color"));
        
        tabbedPane.addTab("Edge Color",tcc3);
        tabbedPane.setMnemonicAt(3, KeyEvent.VK_4);
        
        
        tcc4 = new JColorChooser();
        tcc4.getSelectionModel().addChangeListener(new ColorListener("LABELEDGES",cm));
        tcc4.setPreviewPanel(new JPanel());
        tcc4.setBorder(BorderFactory.createTitledBorder("Choose Edge Label Color"));
        
        tabbedPane.addTab("Edge Label Color",tcc4);
        tabbedPane.setMnemonicAt(4, KeyEvent.VK_5);
        
        add(tabbedPane);
    }

	

}
