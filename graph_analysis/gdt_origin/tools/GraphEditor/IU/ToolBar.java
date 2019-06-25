package IU;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JToolBar;

public class ToolBar extends JPanel implements ActionListener {

	/**
	 * 
	 */
	private static final long serialVersionUID = 4709673189920675739L;
	private JWorkbench jw;
	private JToolBar toolBar;
	
	private static String zoomIn = "ZOOMIN";
	private static String zoomOut = "ZOOMOUT";
	private static String zoomReset = "ZOOMRESET";
	private static String makeOrthogonal = "ORTHO";
	
	
	public ToolBar(JWorkbench jw) {
		super();
		this.jw = jw;
		toolBar = new JToolBar();
		addButtons(this.toolBar);
		
		setLayout(new BorderLayout());
		setPreferredSize(new Dimension(50,50));
		
		add(toolBar);
		
		setVisible(true);
		validate();
	}
	/*
	 * add jbuttons in jtoolbar
	 */
	private void addButtons(JToolBar jtb) {
		
		JButton button = null;
		
		button = makeZoomUp(zoomIn,"ZOOMIN");
		jtb.add(button);
		
		button = makeZoomDown(zoomOut,"ZOOMOUT");
		jtb.add(button);
		
		button = makeZoomReset(zoomReset,"ZOOMRESET");
		jtb.add(button);
		
		button = makeOrthogonal(makeOrthogonal,"ORTHO");
		jtb.add(button);
		
	}
	/*
	 * initialize jbuttons
	 */
	private JButton makeZoomUp(String name,String actionCommand) {
		JButton b = new JButton(new ImageIcon("zoomIn.gif"));
		b.setActionCommand(actionCommand);
		b.addActionListener(this);
		return b;
	}

	private JButton makeZoomDown(String name,String actionCommand) {
		JButton b = new JButton(new ImageIcon("zoomOut.gif"));
		b.setActionCommand(actionCommand);
		b.addActionListener(this);
		return b;
	}
	
	private JButton makeZoomReset(String name, String actionCommand) {
		JButton b = new JButton(new ImageIcon("zoomReset.gif"));
		b.setActionCommand(actionCommand);
		b.addActionListener(this);
		return b;
	}

	private JButton makeOrthogonal(String name, String actionCommand) {
		JButton b = new JButton(new ImageIcon("ortho.gif"));
		b.setActionCommand(actionCommand);
		b.addActionListener(this);
		return b;
	}
	/*
	 * standard method
	 */
	public void actionPerformed(ActionEvent e) {
		String com = e.getActionCommand();
		stringPerformed(com);
	}
	/*
	 * define action performed for each jbutton
	 */
	public void stringPerformed(String com) {
		
		if(jw.getCurrentJGraph()==null)
			return;
		
		Dimension d = jw.getCurrentJGraph().getMaxDimension();
		jw.getCurrentJGraph().setPreferredSize(d);
		jw.getCurrentJGraph().revalidate();
		
		
		int zoomLevel = jw.getCurrentJGraph().getZoomLevel();
		
		
		if(com.equals(zoomIn) && zoomLevel<6) {	
			
			if(zoomLevel==-4){
				zoomLevel=-3;
				jw.getCurrentJGraph().setZoomLevel(-3);
			}
			else{
			jw.getCurrentJGraph().setZoomLevel(jw.getCurrentJGraph().getZoomLevel()+1);
			zoomLevel = jw.getCurrentJGraph().getZoomLevel();
			}
			
				
			for(JNode jn : jw.getCurrentJGraph().getJNodes()) {
				
					jn.zoomIn(zoomLevel);
				
				
			}
			for(JEdge je : jw.getCurrentJGraph().getJEdges()) {
				
					je.zoomIn(zoomLevel);
			}
			
		
		}
		
		else if(com.equals(zoomOut) && zoomLevel>-4) {
			
			if(zoomLevel==6){
				zoomLevel=5;
				jw.getCurrentJGraph().setZoomLevel(5);
			}
			else{
				jw.getCurrentJGraph().setZoomLevel(jw.getCurrentJGraph().getZoomLevel()-1);
				zoomLevel = jw.getCurrentJGraph().getZoomLevel();
			}
			for(JNode jn : jw.getCurrentJGraph().getJNodes()) {
				
					jn.zoomOut(zoomLevel);
		
			}
			
			for(JEdge je : jw.getCurrentJGraph().getJEdges()) {
					je.zoomOut(zoomLevel);
			}
			
			
		}
		
		else if(com.equals(zoomReset)) 
			jw.getCurrentJGraph().toZoom(0);
		
		else if(com.equals(makeOrthogonal)) {
			
			jw.makeOrthogonal(jw.getCurrentJGraph());
		
		}
	
		jw.getCurrentJGraph().repaint();
		
	}
	/*
	 * getter and setter methods
	 */
	public static String getMakeOrthogonal() {
		return makeOrthogonal;
	}

	public static void setMakeOrthogonal(String makeOrthogonal) {
		ToolBar.makeOrthogonal = makeOrthogonal;
	}

	public static String getZoomIn() {
		return zoomIn;
	}

	public static void setZoomIn(String zoomIn) {
		ToolBar.zoomIn = zoomIn;
	}

	public static String getZoomOut() {
		return zoomOut;
	}

	public static void setZoomOut(String zoomOut) {
		ToolBar.zoomOut = zoomOut;
	}

	public static String getZoomReset() {
		return zoomReset;
	}

	public static void setZoomReset(String zoomReset) {
		ToolBar.zoomReset = zoomReset;
	}

	public JWorkbench getJw() {
		return jw;
	}

	public JToolBar getToolBar() {
		return toolBar;
	}
	
}
