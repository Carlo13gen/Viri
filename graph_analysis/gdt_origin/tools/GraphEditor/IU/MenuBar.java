package IU;

import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JScrollPane;

import Listener.WorkbenchListener;

public class MenuBar extends JComponent {

	/**
	 * 
	 */
	private static final long serialVersionUID = -2955306927718277383L;

	@SuppressWarnings("unused")
	private JWorkbench jw;
	
	private JMenuBar menuBar;
	
	private JMenu fileMenu;
	private JMenuItem newMenuItem;
	private JMenuItem saveMenuItem;
	private JMenuItem loadMenuItem;
	private JMenuItem exitMenuItem;
	
	private JMenu editMenu;
	private JMenuItem colorMenuItem;
	private JMenuItem zoomInMenuItem;
	private JMenuItem zoomOutMenuItem;
	private JMenuItem zoomResetMenuItem;
	private JMenu nodesMenu;
	private JMenu edgesMenu;
	private JMenuItem showLabelJNodesMenuItem;
	private JMenuItem hideLabelJNodesMenuItem;
	private JMenuItem showLabelJEdgesMenuItem;
	private JMenuItem hideLabelJEdgesMenuItem;	
	
	private JMenu infoMenu;
	private JMenuItem helpMenuItem;
	private JMenuItem creditsMenuItem;
	
	private JFileChooser jfl;
	private WorkbenchListener wl;
	private JScrollPane scroll;
	
	
	public MenuBar(JWorkbench jw) {
		this.jw =jw;
		wl = new WorkbenchListener(jw);
		jfl = new JFileChooser();
		
		/*
		 * menuBar
		 */
		menuBar = new JMenuBar();
		
		/*
		 * fileMenu
		 */
		fileMenu = new JMenu("File");
		menuBar.add(fileMenu);
		/*
		 * fileMenu menuItem
		 */
		newMenuItem = new JMenuItem("New");
		saveMenuItem = new JMenuItem("Save");
		loadMenuItem = new JMenuItem("Load");
		exitMenuItem = new JMenuItem("Exit");
		
		/*
		 * editMenu
		 */
		editMenu = new JMenu("Edit");
		menuBar.add(editMenu);
		/*
		 * editMenu menuItem
		 */
		colorMenuItem = new JMenuItem("Color");
		zoomInMenuItem = new JMenuItem("Zoom ++");
		zoomOutMenuItem = new JMenuItem("Zoom --");
		zoomResetMenuItem = new JMenuItem("Default Zoom");
		nodesMenu = new JMenu("Nodes");
		edgesMenu = new JMenu("Edges");
		showLabelJNodesMenuItem = new JMenuItem("Show Labels");
		hideLabelJNodesMenuItem = new JMenuItem("Hide Labels");	
		showLabelJEdgesMenuItem = new JMenuItem("Show Labels\n");
		hideLabelJEdgesMenuItem = new JMenuItem("Hide Labels\n");	
		/*
		 * infoMenu
		 */
		infoMenu = new JMenu("Info");
		menuBar.add(infoMenu);
		
		helpMenuItem = new JMenuItem("Help");
		creditsMenuItem = new JMenuItem("Credits");
		
		/*
		 * add to listener menuItem
		 */
		newMenuItem.addActionListener(wl);
		saveMenuItem.addActionListener(wl);
		loadMenuItem.addActionListener(wl);
		exitMenuItem.addActionListener(wl);
		colorMenuItem.addActionListener(wl);
		zoomInMenuItem.addActionListener(wl);
		zoomOutMenuItem.addActionListener(wl);
		zoomResetMenuItem.addActionListener(wl);
		helpMenuItem.addActionListener(wl);
		creditsMenuItem.addActionListener(wl);
		showLabelJNodesMenuItem.addActionListener(wl);
		hideLabelJNodesMenuItem.addActionListener(wl);
		showLabelJEdgesMenuItem.addActionListener(wl);
		hideLabelJEdgesMenuItem.addActionListener(wl);
		/*
		 * add menuItem in fileMenu 
		 */
		fileMenu.add(newMenuItem);
		fileMenu.add(saveMenuItem);
		fileMenu.add(loadMenuItem);
		fileMenu.add(exitMenuItem);
		/*
		 * add menuItem in editMenu
		 */
		editMenu.add(colorMenuItem);
		editMenu.addSeparator();
		editMenu.add(zoomInMenuItem);
		editMenu.add(zoomOutMenuItem);
		editMenu.add(zoomResetMenuItem);
		editMenu.addSeparator();
		editMenu.add(nodesMenu);
		editMenu.add(edgesMenu);
		nodesMenu.add(showLabelJNodesMenuItem);
		nodesMenu.add(hideLabelJNodesMenuItem);
		edgesMenu.add(showLabelJEdgesMenuItem);
		edgesMenu.add(hideLabelJEdgesMenuItem);
		/*
		 * add menuItem in infoMenu
		 */
		infoMenu.add(helpMenuItem);
		infoMenu.add(creditsMenuItem);
	}
	
	/*
	 * getter and setter methods
	 */
	public JMenuBar getJMenuBar() {
		return this.menuBar;
	}
	
	public JScrollPane getScrool() {
		return this.scroll;
	}
	
	public void setJScrollPane(JScrollPane jsp) {
		this.scroll = jsp;
	}
	
	public JFileChooser getJFileChooser() {
		return this.jfl;
	}
}
