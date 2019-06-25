package Listener;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.text.NumberFormat;

import javax.swing.JButton;
import javax.swing.JColorChooser;
import javax.swing.JFormattedTextField;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JTextField;

import IU.ColorManager;
import IU.PopUpJNode;

public class PopUpJNodeListener implements ActionListener {

	private PopUpJNode pujn;
	private static String Label = "Label";
	private static String Height = "Height";
	private static String Width = "Width";
	private static String ShowHideLabel = "Show/Hide Label";
	private static String BgC = "Background";
	private static String BC = "Bound";
	private static String ShapeOwal = "Owal";
	private static String ShapeRect = "Rect";
	
	private static String RestoreColor = "Color";
	private static String RestoreShapeProperty = "Shape Property";
	private static String Delete = "Delete";
	private JFormattedTextField jftf;
	private JTextField jtf;
	private JPopupMenu jpm;
	private JButton jb;
	private JLabel jl;
	private JColorChooser jcc;
	private ColorManager cm;
	private NumberFormat format;
	private int zoomLevel;
	
	public PopUpJNodeListener(PopUpJNode pujn,int zoomLevel) {
		this.pujn = pujn;
		this.zoomLevel = zoomLevel;
	}
	/*
	 * standard method, for each menu item define an action
	 */
	public void actionPerformed(ActionEvent e) {
		
		String com = e.getActionCommand();
		
		if(com.equals(Label)) {
			jpm = new JPopupMenu();
			
			jl = new JLabel("Label:");
			jb = new JButton("modify");
			jb.setActionCommand("modLabel");
			jb.addActionListener(this);
			jtf = new JTextField(pujn.getJNode().getLabel());
			
			jpm.add(jl);
			jpm.add(jtf);
			jpm.add(jb);
			
			jpm.show(pujn.getJGraph(),(int)pujn.getJNode().getPoint().getX(),(int)pujn.getJNode().getPoint().getY());
			
		}
		
		if(com.equals("modLabel")) {
			pujn.getJNode().setLabel(jtf.getText());
			pujn.getJGraph().repaint();
		}
		
		if(com.equals(Height)) {
			jpm = new JPopupMenu();
			
			jb = new JButton("modify");
			jl = new JLabel("Height:");
			jb.setActionCommand("modHeight");
			jb.addActionListener(this);
			jftf = new JFormattedTextField(format);
			jftf.setValue(pujn.getJNode().getCurrentHeight());
			
			jpm.add(jl);
			jpm.add(jftf);
			jpm.add(jb);
			
			
			jpm.show(pujn.getJGraph(),(int)pujn.getJNode().getPoint().getX(),(int)pujn.getJNode().getPoint().getY());
			
		}
		
		
		if(com.equals("modHeight")) {
			Integer value = (Integer)jftf.getValue();
			int maxHeight = 300;
			if(value<maxHeight) {
				pujn.getJNode().setCurrentHeight(value);
				pujn.getJNode().setOriginalHeight(this.zoomedValue(value));
			}
			else {
				pujn.getJNode().setCurrentHeight(maxHeight);
				pujn.getJNode().setOriginalHeight(this.zoomedValue(maxHeight));
			}
			pujn.getJGraph().repaint();
			
			
		}
		
		if(com.equals(Width)) {
			
			jpm = new JPopupMenu();
			jl = new JLabel("Width:");
			jb = new JButton("modify");
			jb.setActionCommand("modWidth");
			jb.addActionListener(this);
			jftf = new JFormattedTextField(format);
			jftf.setValue(pujn.getJNode().getCurrentWidth());
			
			jpm.add(jl);
			jpm.add(jftf);
			jpm.add(jb);
			
			jpm.show(pujn.getJGraph(),(int)pujn.getJNode().getPoint().getX(),(int)pujn.getJNode().getPoint().getY());
			
		}
		
		if(com.equals("modWidth")) {
			Integer value = (Integer)jftf.getValue();
			int maxWidth = 300;
			if(value<maxWidth) {
				pujn.getJNode().setCurrentWidth(value);
				pujn.getJNode().setOriginalWidth(this.zoomedValue(value));
			}
			else {
				pujn.getJNode().setCurrentWidth(maxWidth);
				pujn.getJNode().setOriginalWidth(this.zoomedValue(maxWidth));
			}
			pujn.getJGraph().repaint();
		}
		
		if(com.equals(ShowHideLabel)) {
			if(pujn.getJNode().getShowLabelFlag()==true)
				pujn.getJNode().setShowLabelFlag(false);
			else 
				pujn.getJNode().setShowLabelFlag(true);
			pujn.getJGraph().repaint();
		}
		
		if(com.equals(BgC)) {
			jpm = new JPopupMenu();
			
			jcc = new JColorChooser();
			cm = new ColorManager(pujn.getJGraph(),pujn.getJNode());
			jcc.getSelectionModel().addChangeListener(new ColorListener("BACKNODE",cm));
			jcc.setPreviewPanel(new JPanel());
	        
			jpm.add(jcc);
			
			jpm.show(pujn.getJGraph(),(int)pujn.getJNode().getPoint().getX(),(int)pujn.getJNode().getPoint().getY());
			
		}
		
		if(com.equals(BC)) {
			jpm = new JPopupMenu();
			jcc = new JColorChooser();
			cm = new ColorManager(pujn.getJGraph(),pujn.getJNode());
			jcc.getSelectionModel().addChangeListener(new ColorListener("BOUNDNODE",cm));
			jcc.setPreviewPanel(new JPanel());
			
			jpm.add(jcc);
			
			jpm.show(pujn.getJGraph(),(int)pujn.getJNode().getPoint().getX(),(int)pujn.getJNode().getPoint().getY());
			
			
		}
	
		if(com.equals(ShapeRect)) {
			pujn.getJNode().setFlagShape(1);
			pujn.getJGraph().repaint();
		}
		
		if(com.equals(ShapeOwal)) {
			pujn.getJNode().setFlagShape(0);
			pujn.getJGraph().repaint();
		}
		
		if(com.equals(RestoreColor)) {
			pujn.getJNode().restoreColorProperty();
			pujn.getJGraph().repaint();
		}
		
		if(com.equals(RestoreShapeProperty)) {
			pujn.getJNode().restoreShapeProperty();
			pujn.getJGraph().repaint();
		}
		
		if(com.equals(Delete)) {
			if(!pujn.getJGraph().isPaintingEdge()){
				pujn.getJGraph().deleteJNode(pujn.getJNode());
				pujn.getJGraph().repaint();
			}
		}
	}
	
	private int zoomedValue(int value){
		double valueD = (double)value;
		//System.out.println("ZoomLevel = "+this.zoomLevel);
		if(zoomLevel<0)
			for(int i=zoomLevel;i<=0;i++)
				valueD = valueD*1.2;
		else if(zoomLevel>0)
			for(int i=zoomLevel;i>=0;i--)
				valueD = valueD/1.2;
		value = (int)valueD;
		//System.out.println("Value = "+value);
		return value;
	}

}
