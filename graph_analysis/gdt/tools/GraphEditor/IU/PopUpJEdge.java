package IU;

import java.awt.geom.Point2D;

import javax.swing.JComponent;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;

import Listener.PopUpJEdgeListener;

public class PopUpJEdge extends JComponent {

	/**
	 * 
	 */
	private static final long serialVersionUID = 4283780383531263407L;
	private JGraph jg;
	private JEdge je;
	private Point2D.Float showPoint;
	
	private PopUpJEdgeListener jpel;
	
	private JPopupMenu jpm;

	private JMenuItem label;
	private JMenu colorProperty;
	private JMenuItem labelColor;
	private JMenuItem edgeColor;
	private JMenuItem showHideLabel;
	private JMenuItem orient;
	private JMenu restore;
	private JMenuItem restoreColor;
	
	private JMenuItem delete;
	
	public PopUpJEdge(JGraph jg,JEdge je,Point2D.Float p) {
		this.jg = jg;
		this.je = je;
		this.showPoint = p;
		this.jpel = new PopUpJEdgeListener(this);
		
		jpm = new JPopupMenu();
		
		label = new JMenuItem("Label\n");
		colorProperty = new JMenu("Color");
		labelColor = new JMenuItem("Label");
		edgeColor = new JMenuItem("Edge");
		showHideLabel = new JMenuItem("Show/Hide Label");
		orient = new JMenuItem("Orient");
		restore = new JMenu("Restore");
		restoreColor = new JMenuItem("Color\n");
		delete = new JMenuItem("Delete");
		
		label.addActionListener(jpel);
		labelColor.addActionListener(jpel);
		edgeColor.addActionListener(jpel);
		showHideLabel.addActionListener(jpel);
		orient.addActionListener(jpel);
		restoreColor.addActionListener(jpel);
		delete.addActionListener(jpel);
		
		jpm.add(label);
		jpm.add(colorProperty);
		colorProperty.add(labelColor);
		colorProperty.add(edgeColor);
		jpm.add(showHideLabel);
		jpm.add(orient);
		jpm.addSeparator();
		
		jpm.add(restore);
		restore.add(restoreColor);
		
		jpm.add(delete);
		
		jpm.show(jg,(int)showPoint.getX(),(int)showPoint.getY());
		
	}
	/*
	 * getter methods
	 */
	public JEdge getJEdge() {
		return this.je;
	}
	
	public JGraph getJGraph() {
		return this.jg;
	}
		
	public Point2D.Float getShowPoint() {
		return this.showPoint;
	}
}
