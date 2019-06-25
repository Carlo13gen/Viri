package Listener;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.geom.Point2D;

import javax.swing.event.MouseInputAdapter;

import IU.JGraph;

public class GraphListener extends MouseInputAdapter implements KeyListener {
	private JGraph jg;
	private boolean flag = false;
	private boolean movingDot = false;
	private Point2D.Float onJEdge;
	
	
	public GraphListener(JGraph jg) {
		this.jg = jg;
	}
	
	public void mouseClicked(MouseEvent e) {
		
		jg.requestFocusInWindow();
		Point2D.Float p = new Point2D.Float(e.getPoint().x,e.getPoint().y);
		if(e.getButton()==MouseEvent.BUTTON1) {
			
			if(!jg.existJNode(p)){
				if(!jg.existJEdge(p)){
					if(!e.isControlDown() && !jg.isPaintingEdge()){
						jg.createJNode(p);
					}
					else if(jg.isPaintingEdge())
						jg.manageLine(p);
				}else if(e.isControlDown())
					jg.deleteJEdge(jg.getJEdgeToPoint(p));
				else{ 
					onJEdge = new Point2D.Float(p.x,p.y);
					jg.orientJEdge(jg.getJEdgeToPoint(onJEdge));
					
				}
			}else if(!e.isControlDown()){
				jg.manageLine(p);
			}
				
			else if(e.isControlDown() && !jg.isPaintingEdge())
				jg.deleteJNode(jg.getJNodeToPoint(p));
				
			
		}
		else if(e.getButton()==MouseEvent.BUTTON3) {
			if(jg.existJEdge(p)) {
				jg.openJPopUpJEdgeMenu(jg,jg.getJEdgeToPoint(p),p);
			}
			else if(jg.existJNode(p))
				jg.openJPopUpJNodeMenu(jg,jg.getJNodeToPoint(p));
		}	
	}

	public void mousePressed(MouseEvent e) {
		Point2D.Float p = new Point2D.Float(e.getPoint().x,e.getPoint().y);
		if(jg.existJNode(p)) {
			jg.storeJNodeToMove(p);
			this.flag = true;
			
		}else if(jg.existTransitDot(p)){
			jg.storeDotToMove(p);
			this.movingDot = true;
		}
		
		
	}

	public void mouseReleased(MouseEvent e) {
		this.flag = false;
		jg.unStoreJNodeToMove();
		this.movingDot = false;
		jg.unStoreDotToMove();
	}

	public void mouseDragged(MouseEvent e) {
		jg.setPointer(new Point2D.Float(e.getPoint().x,e.getPoint().y));
		if(jg.isPaintingEdge()){
			jg.repaint();
		}
		Point2D.Float p = new Point2D.Float(e.getPoint().x,e.getPoint().y);
		if(flag){
			jg.moveJNodeTo(p);
		}
		if(movingDot){
			jg.moveDotTo(p);
		}
	}


	public void mouseMoved(MouseEvent arg0) {
		jg.setPointer(new Point2D.Float(arg0.getPoint().x,arg0.getPoint().y));
		if(jg.isPaintingEdge()){
			jg.repaint();
		}
		
	}

	public void keyPressed(KeyEvent e) {
		if(e.getKeyCode()==127){
			if(e.getComponent().equals(jg)){
				jg.deleteLine();
			}
			
		}
		
	}

	public void keyReleased(KeyEvent e) {
		// TODO Auto-generated method stub
		
	}

	public void keyTyped(KeyEvent e) {
		// TODO Auto-generated method stub
		
	}
}
