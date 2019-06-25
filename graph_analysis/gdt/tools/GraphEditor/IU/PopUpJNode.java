package IU;

import javax.swing.JComponent;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;

import Listener.PopUpJNodeListener;

public class PopUpJNode extends JComponent {
	
	/**
	 * 
	 */
	private static final long serialVersionUID = -5280406576749892943L;
	private JGraph jg;
	private JNode jn;
	
	private PopUpJNodeListener jpul;
	
	private JPopupMenu jpm;

	private JMenuItem height;
	private JMenuItem width;
	private JMenuItem label;
	private JMenu colorProperty;
	private JMenuItem backgroundColor;
	private JMenuItem boundColor;
	private JMenu shape;
	private JMenuItem shapeOwal;
	private JMenuItem shapeRect;
	private JMenu restore;
	private JMenuItem restoreShapeProperty;
	private JMenuItem restoreColor;
	private JMenuItem delete;
	private JMenuItem showHideLabel;
	
	public PopUpJNode(JGraph jg,JNode jn) {
		this.jg = jg;
		this.jn = jn;
		this.jpul = new PopUpJNodeListener(this,jg.getZoomLevel());
		jpm = new JPopupMenu();
		
		label = new JMenuItem("Label");
		height = new JMenuItem("Height");
		width = new JMenuItem("Width");
		showHideLabel = new JMenuItem("Show/Hide Label");
		
		colorProperty = new JMenu("Color");
		backgroundColor = new JMenuItem("Background");
		boundColor = new JMenuItem("Bound");
		
		shape = new JMenu("Shape");
		shapeOwal = new JMenuItem("Owal");
		shapeRect = new JMenuItem("Rect");
		
		restore = new JMenu("Restore");
		restoreShapeProperty = new JMenuItem("Shape Property");
		restoreColor = new JMenuItem("Color");
		
		delete = new JMenuItem("Delete");
		
		label.addActionListener(jpul);
		height.addActionListener(jpul);
		width.addActionListener(jpul);
		showHideLabel.addActionListener(jpul);
		
		backgroundColor.addActionListener(jpul);
		boundColor.addActionListener(jpul);
		
		shapeOwal.addActionListener(jpul);
		shapeRect.addActionListener(jpul);
		
		restoreShapeProperty.addActionListener(jpul);
		restoreColor.addActionListener(jpul);
		
		delete.addActionListener(jpul);
		
		
		
		jpm.add(label);
		jpm.add(height);
		jpm.add(width);
		jpm.add(showHideLabel);
		jpm.add(colorProperty);
		colorProperty.add(backgroundColor);
		colorProperty.add(boundColor);
		jpm.add(shape);
		shape.add(shapeRect);
		shape.add(shapeOwal);
		jpm.addSeparator();
		jpm.add(restore);
		jpm.add(delete);
		
		restore.add(restoreShapeProperty);
		restore.add(restoreColor);
		
		jpm.show(jg,(int)jn.getPoint().getX(),(int)jn.getPoint().getY());
		
	}
	/*
	 * getter methods
	 */
	public JNode getJNode() {
		return this.jn;
	}
	
	public JGraph getJGraph() {
		return this.jg;
	}
	
}
