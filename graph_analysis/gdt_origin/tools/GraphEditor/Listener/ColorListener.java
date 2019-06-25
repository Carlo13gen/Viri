package Listener;

import java.awt.Color;

import javax.swing.colorchooser.DefaultColorSelectionModel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import IU.ColorManager;
import Notifier.AbstractNotifier;
import Notifier.ColorListenerNotifier;

public class ColorListener implements ChangeListener {

	private ColorManager cm;
	private String s;
	private AbstractNotifier cln;
	
	public ColorListener(String s,ColorManager cm){
		this.cm = cm;
		this.s = s;
	}
	/*
	 * standard method
	 */
	public void stateChanged(ChangeEvent e) {
		
		Color c =  ((DefaultColorSelectionModel)e.getSource()).getSelectedColor();
		
		if(this.s.equals("BACKNODE")){
			cm.changeJNodeBackground(c);
		} else if(this.s.equals("BOUNDNODE")) {
			cm.changeJNodeBound(c);
		} else if(this.s.equals("LABELNODE")) {
			cm.changeJNodeLabel(c);
		} else if(this.s.equals("BACKNODES")) {
			cm.changeJNodesBackground(c);
		} else if(this.s.equals("BOUNDNODES")) {
			cm.changeJNodesBound(c);
		} else if(this.s.equals("LABELNODES")) {
			cm.changeJNodesLabel(c);
		} else if(this.s.equals("EDGE")) {
			cm.changeJEdge(c);
		} else if(this.s.equals("LABELEDGE")) {
			cm.changeJEdgeLabel(c);
		} else if(this.s.equals("EDGES")) {
			cm.changeJEdges(c);
		} else if(this.s.equals("LABELEDGES")) {
			cm.changeJEdgesLabel(c);
		} else {
			cln = new ColorListenerNotifier();
			cln.showMessage();
		}
	}
	

}
