package IU;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.geom.Point2D;
import java.awt.geom.Point2D.Float;
import java.util.LinkedList;
import java.util.List;

import javax.swing.JPanel;


import Listener.GraphListener;
import Notifier.AbstractNotifier;
import Notifier.InternalEdgeNotifier;

public class JGraph extends JPanel {

	/**
	 * 
	 */
	private static final long serialVersionUID = 6814155611177643107L;
	private List<JNode> jnodes;
	private List<JEdge> jedges;
	private GraphListener gl;
	@SuppressWarnings("unused")
	private PopUpJNode pujn;
	@SuppressWarnings("unused")
	private PopUpJEdge puje;
	private Dimension d;
	private JIncompleteEdge jie;
	private boolean paintingEdge;
	private Point2D.Float pointer;
	private JNode jNodeToMove;
	private Point2D.Float dotToMove;
	private ColorManager color;
	private int zoomLevel=0;
	private static int GLOBAL_NUMBER = 0;
	private int id = 0;
	private AbstractNotifier an; 
	
	public JGraph(Dimension d) {
		this.d = d;
		this.jnodes = new LinkedList<JNode>();
		this.jedges = new LinkedList<JEdge>();
		this.color = new ColorManager(this);
		setBackground(java.awt.Color.WHITE);
		setPreferredSize(d);
		paintingEdge=false;
		
		JNode.setGLOBAL_NUMBER(0);
		JEdge.setGLOBAL_NUMBER(0);
		
		this.gl = new GraphListener(this);
		
		addKeyListener(gl);
		addMouseListener(gl);
		addMouseMotionListener(gl);
		
		wrapID();
		
		setVisible(true);
		validate();
	}
	/*
	 * wrap graph id
	 */
	public void wrapID() {
		this.id = GLOBAL_NUMBER;
		GLOBAL_NUMBER++;
	}
	/*
	 * create jnode from a float point
	 */
	public void createJNode(Point2D.Float p) {
		JNode jn = new JNode(p,color.getJNodeBackground()
				,color.getJNodeBound(),color.getJNodeLabel(),getZoomLevel());
		this.setPreferredSize(this.getMaxDimension());
		revalidate();
		jnodes.add(jn);
		repaint();
		add(jn);
		validate();
		
	}
	/*
	 * check if exits jnode in this point
	 */
	public boolean existJNode(Point2D.Float point) {
		boolean flag = false;
		if(this.getJNodeToPoint(point)!=null)
			flag = true;
		return flag;
		
	}
	/*
	 * delete last drawed line of jincompleteedge
	 */
	public void deleteLine(){
		if(jie==null){
			//System.out.println("jie == null");
		}else{
			repaint();
			if(jie.deleteLine()==1){
				jie=null;
				paintingEdge=false;
			}
		}
	}
	/*
	 * delete jnode
	 */
	public void deleteJNode(JNode jn){
		if(jn.getID()==JNode.getGLOBAL_NUMBER())
			JNode.setGLOBAL_NUMBER(JNode.getGLOBAL_NUMBER()-1);
		List<JEdge> l = jn.getIncidentJEdges();
		for(JEdge je: l)
			this.deleteJEdge(je,jn);
		remove(jn);
		jnodes.remove(jn);
		repaint();
		
	}
	/*
	 * delete jedge knowing also jnode
	 */
	public void deleteJEdge(JEdge je,JNode jn){
		if(je.getID()==JEdge.getGLOBAL_NUMBER())
			JEdge.setGLOBAL_NUMBER(JEdge.getGLOBAL_NUMBER()-1);
		if(!je.getSource().equals(jn))
			je.getSource().getIncidentJEdges().remove(je);
		if(!je.getDest().equals(jn))
			je.getDest().getIncidentJEdges().remove(je);
		remove(je);
		jedges.remove(je);
		repaint();
		
	}
	/*
	 * delete jedge
	 */
	public void deleteJEdge(JEdge je){
		if(je.getID()==JEdge.getGLOBAL_NUMBER())
			JEdge.setGLOBAL_NUMBER(JEdge.getGLOBAL_NUMBER()-1);
		je.getSource().getIncidentJEdges().remove(je);
		je.getDest().getIncidentJEdges().remove(je);
		remove(je);
		jedges.remove(je);
		repaint();
	}
	/*
	 * create a new jedge
	 */
	public void createJEdge(Point2D.Float p1, Point2D.Float p2, List<Point2D.Float> points) {
		JNode source = this.getJNodeToPoint(p1);
		JNode dest = this.getJNodeToPoint(p2);
		
		if(source.equals(dest) && jie.getCurTransitDots().size()==2) {
			jie=null;
			paintingEdge=false;
			an = new InternalEdgeNotifier();
			an.showMessage();
			return;
		}
		
		int last = jie.getCurTransitDots().size()-1;
		
		Point2D.Float anchorSource = source.getAnchorPoint(jie.getCurTransitDots().get(0), jie.getCurTransitDots().get(1));
		Point2D.Float anchorDest = dest.getAnchorPoint(jie.getCurTransitDots().get(last), jie.getCurTransitDots().get(last-1));
		
		jie.getCurTransitDots().remove(last);
		jie.getCurTransitDots().add(last,anchorDest);
		
		jie.getCurTransitDots().remove(0);
		jie.getCurTransitDots().add(0,anchorSource);
		
		
		JEdge je = new JEdge(source,dest,jie.getCurTransitDots(),jie.getCurrentLines()
						,color.getJEdge(),color.getJEdgeLabel());
		source.addIncidentJEdge(je);
		
		dest.addIncidentJEdge(je);
		jie=null;
		paintingEdge=false;
		jedges.add(je);
		
		add(je);
		validate();
		
	}
	/*
	 * check if exist jedge from jnode jn1 and jnode jn2
	 */	
	public boolean existJEdge(JNode jn1,JNode jn2) {
		boolean flag = false;
		for(JEdge ej : jedges) {
			if(ej.getSource()==jn1 && ej.getDest()==jn2)
				flag = true;
		}
		return flag;
	}
	/*
	 * create a new jincompleteedge
	 */
	public void manageLine(Point2D.Float p){
		if(jie==null && !this.existJNode(p))
		return;
			
		if(jie==null){
			this.jie = new JIncompleteEdge(this);
			paintingEdge = true;
			add(jie);
			validate();
			
		}
		repaint();
		jie.manageLine(p);		
	}
	/*
	 * check if exist jedge near this point
	 */
	public boolean existJEdge(Point2D.Float p){
		if(getJEdgeToPoint(p)==null)
			return false;
		return true;
	}
	/*
	 * standard JComponent method
	 */
	public void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2 = (Graphics2D)g;
        
        for(JEdge je : jedges)
			je.paintComponent(g2);
        
        for(JNode jn : jnodes)
			jn.paintComponent(g2);
        
        if(jie!=null)
        	jie.paintComponent(g2);
        
        if(isPaintingEdge() && pointer!=null)
        	this.drawLine(g);
        
        Dimension minDim = this.getMinDimension();
        if(minDim.height<0 || minDim.width<0)
        	this.moveAll(minDim);
        
        Dimension maxDim = this.getMaxDimension();
		this.setPreferredSize(maxDim);
					
		revalidate();
        
	}
	/*
	 * draw jincompleteedege
	 */
	public void drawLine(Graphics g){
		int x1 = (int)this.jie.getInterDot().getX();
		int y1 = (int)this.jie.getInterDot().getY();
		int x2 = (int)pointer.getX();
		int y2 = (int)pointer.getY();
		Graphics2D g2 = (Graphics2D)g;
		g2.setColor(color.getJEdge());
		g2.drawLine(x1, y1, x2, y2);
	}
	/*
	 * move jnode to this point
	 */
	public void moveJNodeTo(Point2D.Float p) {
		this.jNodeToMove.moveTo(p);
		repaint();
	}
	/*
	 * get jnode in this point, store it to move 
	 */
	public void storeJNodeToMove(Point2D.Float p) {
		this.jNodeToMove = getJNodeToPoint(p);
	}
	/*
	 * unload jnode to move
	 */
	public void unStoreJNodeToMove() {
		this.jNodeToMove = null;
	}
	/*
	 * check if exist transitDot near this point
	 */
	public boolean existTransitDot(Point2D.Float point) {
		boolean flagDot = false;
		if(this.getDotToPoint(point)!=null)
			flagDot = true;
		return flagDot;
		
	}
	/*
	 * get transitDot near this point
	 */
	private Point2D.Float getDotToPoint(Point2D.Float point) {
		for(JEdge je:jedges)
			if(je.existTransitDot(point)!=null)
				return (Float) je.existTransitDot(point);
		return null;
	}
	/*
	 * move dot to this point
	 */
	public void moveDotTo(Point2D.Float p) {
		this.dotToMove.setLocation(p);
		repaint();
	}
	/*
	 * get dot in this point, store it to move
	 */
	public void storeDotToMove(Point2D.Float p) {
		this.dotToMove = getDotToPoint(p);
	}
	/*
	 * unload dot to move
	 */
	public void unStoreDotToMove() {
		this.dotToMove = null;
	}
	/*
	 * orient this jedge
	 */
	public void orientJEdge(JEdge je) {
		if(je.isVisibleOrientation())
			if(!je.isInverse()){
				je.invert();
				je.setInverse(true);
			} else {
				je.setVisibleOrientation(false);
				je.setInverse(false);
				je.invert();
			}
		else
			je.setVisibleOrientation(true);
		repaint();
	}
	
	public void toZoom(int i){
		nodeZoom(i);
		int temp = zoomLevel;
		for(JEdge je:this.getJEdges()){
			zoomLevel = temp;
			while(zoomLevel>0){
				je.zoomOut(zoomLevel);
				setZoomLevel(zoomLevel-1);
			}
			while(zoomLevel<0){
				je.zoomIn(zoomLevel);
				setZoomLevel(zoomLevel+1);
			}
			je.repaint();
		}
		this.setZoomLevel(0);
	}
	
	private void nodeZoom(int i){
		int zoomLevel1 = zoomLevel;
		int temp = zoomLevel1;
		
		for(JNode jn:this.getJNodes()){
			zoomLevel1 = temp;
			while(zoomLevel1>0){
				jn.zoomOut(i);
				zoomLevel1--;
			}
			while(zoomLevel1<0){
				jn.zoomIn(i);
				zoomLevel1++;
			}
		}
	}
	/*
	 * open jnode popup menu 
	 */
	public void openJPopUpJNodeMenu(JGraph jg,JNode nodeToPoint) {
		pujn = new PopUpJNode(jg,nodeToPoint);
	}
	/*
	 * open jedge popup menu
	 */
	public void openJPopUpJEdgeMenu(JGraph jg,JEdge edgeToPoint,Point2D.Float p) {
		puje = new PopUpJEdge(jg,edgeToPoint,p);
	}
	/*
	 * move all components in resized jgraph
	 */
	private void moveAll(Dimension dim){
		int x=0;
		int y=0;
		if(dim.height<0)
			y = -dim.height;
		if(dim.width<0)
			x = -dim.width;
		
		for(JNode jn : jnodes)
			jn.getPoint().setLocation(jn.getPoint().getX()+x,jn.getPoint().getY()+y);
		
		for(JEdge je : jedges) {
			for(Point2D.Float p : je.getTransitDots()) {
				p.setLocation(p.getX()+x,p.getY()+y);
			}
		}
	}
	/*
	 * replace  first and last transitDots with anchor dots of source and destination jnode 
	 */
	public List<Point2D.Float> createNewTransitDots(JNode source, JNode dest,List<Point2D.Float> transitDots){
		int last = transitDots.size()-1;
		
		Point2D.Float anchorSource = source.getAnchorPoint(transitDots.get(0), transitDots.get(1));
		Point2D.Float anchorDest = dest.getAnchorPoint(transitDots.get(last), transitDots.get(last-1));
		
		transitDots.remove(last);
		transitDots.add(last,anchorDest);
		
		transitDots.remove(0);
		transitDots.add(0,anchorSource);
		
		return transitDots;
	}
	/*
	 * getter and setter methods
	 */
	public int getID() {
		return this.id;
	}
	
	public void setID(int id) {
		this.id = id;
	}
	
	public int getZoomLevel() {
		return zoomLevel;
	}

	public void setZoomLevel(int zoomLevel) {
		this.zoomLevel = zoomLevel;
		
	}
	
	public List<JNode> getJNodes() {
		return this.jnodes;
	}
	
	public List<JEdge> getJEdges() {
		return this.jedges;
	}
	
	public JEdge getJEdgeToPoint(Point2D.Float p){
		for(JEdge je: this.getJEdges())
			if(je.contains(p))
				return je;
		return null;
	}
	
	public JNode getJNodeToPoint(Point2D.Float p) {
		for(JNode jn : this.getJNodes()) {
			if(jn.getShape().getBounds2D().contains(p))
				return jn;
		}
		return null;
	}
	
	public boolean isPaintingEdge() {
		return paintingEdge;
	}

	public void setPaintingEdge(boolean paintingEdge) {
		this.paintingEdge = paintingEdge;
	}

	public Point2D.Float getPointer() {
		return pointer;
	}

	public void setPointer(Point2D.Float pointer) {
		this.pointer = pointer;
	}
	
	public ColorManager getColor() {
		return color;
	}
	
	public void setColor(ColorManager color) {
		this.color = color;
	}
	
	public JEdge getJEdge(int id){
		for(JEdge je:this.getJEdges())
			if(je.getID()==id)
				return je;
		return null;
	}
	
	public JNode getJNode(int id){
		for(JNode jn:this.getJNodes())
			if(jn.getID()==id)
				return jn;
		return null;
	}
	
	public Dimension getMinDimension(){
		Dimension d=null;
		
		double minX = 0;
		double minY = 0;
		
		for(JNode jn : jnodes) {
			if((jn.getPoint().getX()-jn.getCurrentWidth()/2)+10<minX)
				minX = jn.getPoint().getX()-jn.getCurrentWidth();
			if((jn.getPoint().getY()-jn.getCurrentHeight()/2)+10<minY)
				minY = jn.getPoint().getY()-jn.getCurrentHeight();
		}
		
		d = new Dimension((int)minX,(int)minY);
		
		return d;
	}
	
	public Dimension getMaxDimension() {
		
		Dimension d = null;
		
		double maxX = 0;
		double maxY = 0;
		
		for(JNode jn : jnodes) {
			if(jn.getShape().getBounds2D().getMaxX()>maxX)
				maxX = jn.getShape().getBounds2D().getMaxX();
			if(jn.getShape().getBounds2D().getMaxY()>maxY)
				maxY = jn.getShape().getBounds2D().getMaxY();
		}
		
		for(JEdge je : jedges) {
			for(Point2D.Float p : je.getTransitDots()) {
				if(p.getX()>maxX)
					maxX = p.getX();
				if(p.getY()>maxY)
					maxY = p.getY();
			}
		}
		
		if(maxX<this.d.getWidth())
			maxX = 1500;
		if(maxY<this.d.getHeight())
			maxY = 1500;
		
		d = new Dimension((int)maxX+10,(int)maxY+10);
		
		return d;
	}
}
