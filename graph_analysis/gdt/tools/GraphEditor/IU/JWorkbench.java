package IU;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Graphics;
import java.io.File;
import java.util.LinkedList;
import java.util.List;

import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JScrollPane;

import Drawer.OrthoDrawer;
import Notifier.CreditsNotifier;
import Notifier.JWorkbenchNotifier;
import Notifier.AbstractNotifier;
import Traslator.Decoder;
import Traslator.Encoder;

public class JWorkbench extends JFrame {
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 9110558829222648177L;
	private int heightJGraph = 1500;
	private int widthJGraph = 1500;
	private Dimension dimJGraph;
	private String title = "graph editor";
	private int HSize = 800;
	private int WSize = 600;
	private MenuBar menuBar;
	private List<JGraph> jgraphs;
	private JGraph currentJGraph;
    private Encoder ec;
	private Decoder dc;
	private ToolBar toolBar;
	private OrthoDrawer od;
	private AbstractNotifier an;
	
	public JWorkbench(String fileName) {
		setTitle(title);
		setSize(HSize,WSize);
		setLocation(100,100);
		this.jgraphs = new LinkedList<JGraph>();
        this.setDefaultCloseOperation(EXIT_ON_CLOSE);
		
		//toolBar
		toolBar = new ToolBar(this);
		//menuBar
		menuBar = new MenuBar(this);
		
		add(toolBar);
		this.getContentPane().add(toolBar,BorderLayout.PAGE_START);
		add(menuBar);
		this.getContentPane().add(toolBar,BorderLayout.PAGE_START);
		setJMenuBar(menuBar.getJMenuBar());
		setVisible(true);
		////////////////////////////////
		createJGraph();
		if (fileName != "") 
		{
			try {
				File f = new File(fileName);	
				this.dc = new Decoder(currentJGraph);
				dc.load(f);
			}
			catch(Exception e) {
				e.printStackTrace();
			}
		}
	}
	/*
	 * create a new jgraph
	 */
	public void createJGraph() {
		
		if(currentJGraph!=null)
			this.getContentPane().remove(menuBar.getScrool());
		
		dimJGraph = new Dimension(heightJGraph,widthJGraph);
		this.currentJGraph = new JGraph(dimJGraph);
		JNode.setGLOBAL_NUMBER(0);
		JEdge.setGLOBAL_NUMBER(0);
		
		menuBar.setJScrollPane(new JScrollPane(this.currentJGraph));
		menuBar.getScrool().setAutoscrolls(true);
		menuBar.getScrool().setSize(HSize, WSize);
		this.getContentPane().add(menuBar.getScrool());
		setVisible(true);
		
	}
	/*
	 * load a jgraph
	 */
	public void loadJGraph() {
		add(menuBar.getJFileChooser());
		setVisible(true);
		int response = menuBar.getJFileChooser().showOpenDialog(this);
		if(response == JFileChooser.APPROVE_OPTION) {
			try {
				File f = menuBar.getJFileChooser().getSelectedFile();
				this.createJGraph();
				this.dc = new Decoder(currentJGraph);
				dc.load(f);
			}
			catch(Exception e) {
				//System.out.println("ciao");
			}
		}
		
	}
	/*
	 * save jgraph
	 */
	public void saveJGraph() {
		add(menuBar.getJFileChooser());
		setVisible(true);
		int response = menuBar.getJFileChooser().showSaveDialog(this);
		if(response == JFileChooser.APPROVE_OPTION) {
			try {
				
				File f = menuBar.getJFileChooser().getSelectedFile();
				
				if(this.currentJGraph==null) {
					this.createJGraph();
					
				}
				this.ec = new Encoder(this.currentJGraph);
				
				ec.save(f.getPath());
				
			}
			catch(Exception e) {
				e.printStackTrace();
			}
		}
		
	}
	/*
	 * load a jframe to change color of various components
	 */
	public void changeColor(){
		if(this.currentJGraph==null)
			return;
		JFrame color = new JFrame("Color");

		color.setLocation(300,300);
        JComponent newContentPane = new JColor(this.currentJGraph.getColor());
        newContentPane.setOpaque(true); 
        color.setContentPane(newContentPane);
        color.setSize(newContentPane.getPreferredSize());
		
		color.setVisible(true);
	}
	/*
	 * standard jcomponent method
	 */
	public void paintComponents(Graphics g) {
        super.paintComponents(g);
        this.currentJGraph.paintComponents(g);
    }
	/*
	 * make jgraph orthogonal
	 */
	public void makeOrthogonal(JGraph g) {
		this.od = new OrthoDrawer(g);
		od.makeOrthogonal();
	}
	/*
	 * print help
	 */
	public void printHelp() {
		an = new JWorkbenchNotifier();
		an.showMessage();
	}
	/*
	 * print credits
	 */
	public void printCredits() {
		an = new CreditsNotifier();
		an.showMessage();
	}
	/*
	 * getter and setter methods
	 */
	public JGraph getCurrentJGraph() {
		return currentJGraph;
	}
	
	public void setCurrentJGraph(JGraph currentJGraph) {
		this.currentJGraph = currentJGraph;
	}

	public List<JGraph> getJGraphs() {
		return jgraphs;
	}

	public void setJGraphs(List<JGraph> jgraphs) {
		this.jgraphs = jgraphs;
	}
	
	public ToolBar getToolBar() {
		return this.toolBar;
	}
}
