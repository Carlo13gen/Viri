package Listener;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JColorChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JTextField;

import IU.ColorManager;
import IU.PopUpJEdge;

public class PopUpJEdgeListener implements ActionListener {

	private PopUpJEdge puje;
	
	private static String Label = "Label\n";
	private static String LabelColor = "Label";
	private static String EdgeColor = "Edge";
	private static String ShowHideLabel = "Show/Hide Label";
	private static String Orient = "Orient";
	private static String RestoreColor = "Color\n";
	private static String Delete = "Delete";
	
	private JTextField jtf;
	private JPopupMenu jpm;
	private JButton jb;
	private JLabel jl;
	
	private JColorChooser jcc;
	private ColorManager cm;
	
	public PopUpJEdgeListener(PopUpJEdge puje) {
		this.puje = puje;
		
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
			jtf = new JTextField(puje.getJEdge().getLabel());
			
			jpm.add(jl);
			jpm.add(jtf);
			jpm.add(jb);
			
			jpm.show(puje.getJGraph(),(int)puje.getShowPoint().getX(),(int)puje.getShowPoint().getY());
			
		}
		
		if(com.equals("modLabel")) {
			puje.getJEdge().setLabel(jtf.getText());
			puje.getJGraph().repaint();
		}
		
		if(com.equals(EdgeColor)) {
			jpm = new JPopupMenu();
			
			jcc = new JColorChooser();
			cm = new ColorManager(puje.getJGraph(),puje.getJEdge());
			jcc.getSelectionModel().addChangeListener(new ColorListener("EDGE",cm));
			jcc.setPreviewPanel(new JPanel());
	        
			jpm.add(jcc);
			
			jpm.show(puje.getJGraph(),(int)puje.getShowPoint().getX(),(int)puje.getShowPoint().getY());
			
		}
		
		if(com.equals(LabelColor)) {
			jpm = new JPopupMenu();
			
			jcc = new JColorChooser();
			cm = new ColorManager(puje.getJGraph(),puje.getJEdge());
			jcc.getSelectionModel().addChangeListener(new ColorListener("LABELEDGE",cm));
			jcc.setPreviewPanel(new JPanel());
	        
			jpm.add(jcc);
			
			jpm.show(puje.getJGraph(),(int)puje.getShowPoint().getX(),(int)puje.getShowPoint().getY());
			
		}
		
		if(com.equals(ShowHideLabel)) {
			if(puje.getJEdge().getShowLabelFlag()==true)
				puje.getJEdge().setShowLabelFlag(false);
			else 
				puje.getJEdge().setShowLabelFlag(true);
			puje.getJGraph().repaint();
		}
		
		if(com.equals(Orient)) {
			puje.getJGraph().orientJEdge(puje.getJEdge());
			puje.getJGraph().repaint();
		}
		
		if(com.equals(RestoreColor)) {
			puje.getJEdge().restoreColorProperty();
			puje.getJGraph().repaint();
		}
		
		if(com.equals(Delete)) {
			puje.getJGraph().deleteJEdge(puje.getJEdge());
			puje.getJGraph().repaint();
		}
	}
}
