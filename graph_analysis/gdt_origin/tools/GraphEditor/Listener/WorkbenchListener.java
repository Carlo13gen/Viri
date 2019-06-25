package Listener;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import IU.JNode;
import IU.JEdge;
import IU.JWorkbench;
import IU.ToolBar;

public class WorkbenchListener implements ActionListener{
	
	private final static String NEW = "New";
	private final static String SAVE = "Save";
	private final static String LOAD = "Load";
	private final static String EXIT = "Exit";
	private final static String COLOR = "Color";
	private final static String ZOOMIN = "Zoom ++";
	private final static String ZOOMOUT = "Zoom --";
	private final static String ZOOMRESET = "Default Zoom";
	private final static String SHOWLABELNODES = "Show Labels";
	private final static String HIDELABELNODES = "Hide Labels";
	private final static String SHOWLABELEDGES = "Show Labels\n";
	private final static String HIDELABELEDGES = "Hide Labels\n";
	private final static String HELP = "Help";
	private final static String CREDITS = "Credits";
	
	private JWorkbench jw;
	
	public WorkbenchListener(JWorkbench jw) {
		this.jw = jw;
	}
	/*
	 * standard method, for each menu item define an action
	 */
	public void actionPerformed(ActionEvent e) {
		String com = e.getActionCommand();
		if(com.equals(NEW)) {
			jw.createJGraph();
		}
		else if(com.equals(SAVE)) 
			jw.saveJGraph();
		else if(com.equals(LOAD))
			jw.loadJGraph();
		else if(com.equals(EXIT))
			System.exit(0);
		else if(com.equals(COLOR))
			jw.changeColor();
		else if(com.equals(ZOOMIN))
			jw.getToolBar().stringPerformed(ToolBar.getZoomIn());
		else if(com.equals(ZOOMOUT)) 
			jw.getToolBar().stringPerformed(ToolBar.getZoomOut());
		else if(com.equals(ZOOMRESET))
			jw.getToolBar().stringPerformed(ToolBar.getZoomReset());
		else if(com.equals(SHOWLABELNODES)) {
			for(JNode jn : jw.getCurrentJGraph().getJNodes())
				jn.setShowLabelFlag(true);
			jw.getCurrentJGraph().repaint();
		}
		else if(com.equals(HIDELABELNODES)) {
			for(JNode jn : jw.getCurrentJGraph().getJNodes())
				jn.setShowLabelFlag(false);
			jw.getCurrentJGraph().repaint();
		}
		else if(com.equals(SHOWLABELEDGES)) {	
			for(JEdge je : jw.getCurrentJGraph().getJEdges())
				je.setShowLabelFlag(true);
			jw.getCurrentJGraph().repaint();
		}
		else if(com.equals(HIDELABELEDGES)) {	
			for(JEdge je : jw.getCurrentJGraph().getJEdges())
				je.setShowLabelFlag(false);
			jw.getCurrentJGraph().repaint();
		}
		else if(com.equals(HELP))
			jw.printHelp();
		else if(com.equals(CREDITS))
			jw.printCredits();
	}
	
}
