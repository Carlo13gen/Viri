package IU;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Shape;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Line2D;
import java.awt.geom.Rectangle2D;
import java.awt.geom.Point2D;
import java.util.LinkedList;
import java.util.List;

import javax.swing.JComponent;



public class JNode extends JComponent {
	
	/**
	 * 
	 */
	private static final long serialVersionUID = -960157070153530436L;
	private Point2D.Float p;
	private Shape shape;
	private static Color Sharedbackground = Color.red;
	private static Color Sharedbound = Color.black;
	private static Color SharedlabelColor = Color.black;
	private Color background = Sharedbackground;
	private Color bound = Sharedbound;
	private Color labelColor = SharedlabelColor;
	private static int GLOBAL_NUMBER = 0;
	private int id;
	private String label;
	private List<JEdge> incidentJEdges;
	private List<Point2D.Float> anchorPoints;
	private static int basicHeight = 20;
	private static int basicWidth = 20;
	private int originalHeight = basicHeight;
	private int originalWidth = basicWidth;
	private int currentHeight = originalHeight;
	private int currentWidth = originalWidth;
	private int flagShape = 1;
	private boolean showLabelFlag = true;
	
	/*
	 * constructor used if JEdge is load from a GDT file
	 */
	public JNode(int id){
		this.id=id;
		setMaxID(id);
		this.setSize(this.currentHeight,this.currentWidth);
		this.incidentJEdges = new LinkedList<JEdge>();
		this.anchorPoints = new LinkedList<Point2D.Float>();
		this.label= String.valueOf(id);
		
	}
	/*
	 * constructor used if JEdge is create from user
	 */
	public JNode(Point2D.Float p,Color background,Color bound,Color labelColor,int zoomLevel) {
		this.p = p;
		this.background = background;
		this.bound = bound;
		this.labelColor = labelColor;
		this.currentHeight = (int)(originalHeight*Math.pow(1.2,zoomLevel));
		this.currentWidth = (int)(originalWidth*Math.pow(1.2,zoomLevel));
		this.setSize(this.currentHeight,this.currentWidth);
		this.incidentJEdges = new LinkedList<JEdge>();
		this.anchorPoints = new LinkedList<Point2D.Float>();
		wrapID();
	}
	/*
	 * wrap graph id
	 */
	public void wrapID() {
		this.id = GLOBAL_NUMBER;
		this.label = String.valueOf(GLOBAL_NUMBER);
		GLOBAL_NUMBER++;
	}
	/*
	 * standard Jcomponent methods
	 */
	public void paintComponent(Graphics g) {
		super.paintComponent(g);
		Graphics2D g2 = (Graphics2D) g;
		
		if(this.flagShape==1)
			shape = new Rectangle2D.Float((float)(p.x-this.getCurrentWidth()/2),((float)p.y-this.getCurrentHeight()/2),this.getCurrentWidth(),this.getCurrentHeight());
		else if(this.flagShape==0)
			shape = new Ellipse2D.Float((float)(p.x-this.getCurrentWidth()/2),((float)p.y-this.getCurrentHeight()/2),this.getCurrentWidth(),this.getCurrentHeight());
		g2.setColor(this.getBackground());
		g2.fill(shape);
		g2.setColor(this.getBound());
		g2.setStroke(new BasicStroke(1));
		g2.setFont(new Font("Dialog",1,8));
		g2.draw(shape);
		g2.setColor(this.getLabelColor());
		if(this.showLabelFlag)
			g2.drawString(this.label,(int)((getShape().getBounds2D().getMinX()+getShape().getBounds2D().getCenterX())/2),(int)((getShape().getBounds2D().getMaxY()+getShape().getBounds2D().getCenterY())/2));		
	
	}
	/*
	 * move jnode to this point
	 */
	public void moveTo(Point2D.Float p) {
		if(!this.getPoint().equals(p))
			this.getPoint().setLocation(p);
		
		
		for(JEdge je : this.incidentJEdges) {
			this.setAnchorPoint(je);
			if(this.equals(je.getDest()))
				for(JEdge je2 : je.getSource().incidentJEdges)
					je.getSource().setAnchorPoint(je2);
			else if(this.equals(je.getSource()))
				for(JEdge je2 : je.getDest().incidentJEdges)
					je.getDest().setAnchorPoint(je2);
		}
	}
	/*
	 * move this jnode to x y coordinates
	 */
	public void moveTo(float x, float y) {
		this.moveTo(new Point2D.Float(x,y));
		
	}
	/*
	 * add this incident jedge to this node
	 */
	public void addIncidentJEdge(JEdge je) {
		this.incidentJEdges.add(je);
	}
	/*
	 * zoom in jnode
	 */
	public void zoomIn(double zoom) {
		
		this.setCurrentHeight((int)(this.originalHeight*Math.pow(1.2,zoom)));
		this.setCurrentWidth((int)(this.originalWidth*Math.pow(1.2,zoom)));
		this.getPoint().setLocation(this.getPoint().getX()*1.2,this.getPoint().getY()*1.2);
	}
	/*
	 * zoom out jnode
	 */
	public void zoomOut(double zoom) {
		
		this.setCurrentHeight((int)(this.originalHeight*(Math.pow(1.2,zoom))));
		this.setCurrentWidth((int)(this.originalWidth*(Math.pow(1.2,zoom))));
		this.getPoint().setLocation(this.getPoint().getX()/1.2,this.getPoint().getY()/1.2);
	}
	/*
	 * restore jnode color property
	 */
	public void restoreColorProperty() {
		this.background = JNode.Sharedbackground;
		this.bound = JNode.Sharedbound;
		this.labelColor = JNode.SharedlabelColor;
	}
	/*
	 * restore jnode shape property
	 */
	public void restoreShapeProperty() {
		this.originalHeight = JNode.basicHeight;
		this.originalWidth = JNode.basicWidth;
		this.currentHeight = JNode.basicHeight;
		this.currentWidth = JNode.basicWidth;
		this.flagShape = 1;
	}
	/*
	 * get intersect point between jnode and last line of jedge
	 */
	public Point2D.Float getAnchorPoint(Point2D.Float p1, Point2D.Float p2) {
		Point2D.Float intersectPoint = null;
			
		Point2D.Float phl = new Point2D.Float(p.x-this.currentWidth/2,p.y-this.currentHeight/2);
		Point2D.Float phr = new Point2D.Float(p.x+this.currentWidth/2,p.y-this.currentHeight/2);
		Point2D.Float pll = new Point2D.Float(p.x-this.currentWidth/2,p.y+this.currentHeight/2);
		Point2D.Float plr = new Point2D.Float(p.x+this.currentWidth/2,p.y+this.currentHeight/2);
		
		Line2D.Float lineTop = new Line2D.Float(phl.x,phl.y,phr.x,phr.y);
		Line2D.Float lineDown = new Line2D.Float(pll.x,pll.y,plr.x,plr.y);
		Line2D.Float lineRight = new Line2D.Float(phr.x,phr.y,plr.x,plr.y);
		Line2D.Float lineLeft = new Line2D.Float(phl.x,phl.y,pll.x,pll.y);
			
		Line2D.Float line = new Line2D.Float(p1.x,p1.y,p2.x,p2.y);
		
		if(Line2D.linesIntersect(line.getX1(), line.getY1(), line.getX2(), line.getY2(), lineTop.getX1(), lineTop.getY1(), lineTop.getX2(), lineTop.getY2())) 
			intersectPoint = this.getIntersectPoint(line,p.y-this.currentHeight/2,"y");
		else if(Line2D.linesIntersect(line.getX1(), line.getY1(), line.getX2(), line.getY2(), lineDown.getX1(), lineDown.getY1(), lineDown.getX2(), lineDown.getY2())) 
			intersectPoint = this.getIntersectPoint(line,p.y+this.currentHeight/2,"y");
		else if(Line2D.linesIntersect(line.getX1(), line.getY1(), line.getX2(), line.getY2(), lineRight.getX1(), lineRight.getY1(), lineRight.getX2(), lineRight.getY2()))
			intersectPoint = this.getIntersectPoint(line,p.x+this.currentWidth/2,"x");
		else if(Line2D.linesIntersect(line.getX1(), line.getY1(), line.getX2(), line.getY2(), lineLeft.getX1(), lineLeft.getY1(), lineLeft.getX2(), lineLeft.getY2()))
			intersectPoint = this.getIntersectPoint(line,p.x-this.currentWidth/2,"x");
		else {
			intersectPoint = this.p;
		}
		
		this.anchorPoints.add(intersectPoint);
		
		return intersectPoint;
	}
	/*
	 * app methods for getAnchorPoint method
	 */
	private Point2D.Float getIntersectPoint(Line2D.Float segment,float cost,String axis) {
		
		Point2D.Float intersect = null;
		float m,q;
		
		if(segment.x1==segment.x2) {
			intersect = new Point2D.Float(segment.x1,cost); 
			return intersect;
		}
		
		if(segment.y2>segment.y1) 
			m = ((segment.y2-segment.y1)/(segment.x2-segment.x1));
		else 
			m =((segment.y1-segment.y2)/(segment.x1-segment.x2));
		q = (segment.y1-(m*segment.x1));
		
		if(axis.equals("x")) {
			intersect = new Point2D.Float(cost,(q+(cost*m)));
		}
		if(axis.equals("y")) {
			intersect = new Point2D.Float(((cost-q)/m),cost);
		}
	
		return intersect;
	}
	/*
	 * check if this point belongs to jnode
	 */
	public boolean isMyAnchor(Point2D.Float anchor) {
		double d = Math.sqrt(Math.pow(this.currentHeight,2)+Math.pow(this.currentWidth,2));
		if(anchor.distance(this.p)<d+1)
			return true;
		return false;
	}
	/*
	 * clean list of incident jedges
	 */
	public void cleanIncidentJEdges(){
		List<JEdge> newIncidentJEdges=new LinkedList<JEdge>();
		
		for(JEdge je:this.getIncidentJEdges())
			if(!newIncidentJEdges.contains(je))
				newIncidentJEdges.add(je);
		this.setIncidentJEdges(newIncidentJEdges);
	}
	/*
	 * restore the basic size of jnode
	 */
	public void restoreBasicSize(){
		this.setCurrentHeight(basicHeight);
		this.setCurrentWidth(basicWidth);
		this.setOriginalHeight(basicHeight);
		this.setOriginalWidth(basicWidth);
		
	}
	/*
	 * getter and setter methods
	 */
	public List<Point2D.Float> getAnchorPoints() {
		return anchorPoints;
	}
	
	public void setAnchorPoints(List<Point2D.Float> anchorPoints) {
		this.anchorPoints = anchorPoints;
	}
	
	private void setMaxID(int id){
		if(GLOBAL_NUMBER<=id)
			GLOBAL_NUMBER=id+1;
	}
	
	public static int getGLOBAL_NUMBER() {
		return JNode.GLOBAL_NUMBER;
	}
	
	public static void setGLOBAL_NUMBER(int i) {
		JNode.GLOBAL_NUMBER = i;
	}
	
	public int getID() {
		return this.id;
	}
	
	public void setID(int id) {
		this.id = id;
	}
	
	public void setBackground(Color background) {
		this.background = background;
	}

	public void setBound(Color bound) {
		this.bound = bound;
	}
	
	public Shape getShape() {
		return this.shape;
	}
	
	public Point2D.Float getPoint() {
		return this.p;
	}
	
	public void setPoint(Point2D.Float p) {
		this.p = p;
	}
	
	public void setPoint(Float x,Float y) {
		this.p.x = x;
		this.p.y = y;
	}
	
	public Color getBackground() {
		return this.background;
	}
	
	public String getLabel() {
		return this.label;
	}

	public void setLabel(String label) {
		this.label = label;
	}
	
	public List<JEdge> getIncidentJEdges() {
		return this.incidentJEdges;
	}
	
	public Color getBound() {
		return this.bound;
	}
	
	public int getCurrentHeight() {
		return this.currentHeight;
	}
	
	public int getCurrentWidth() {
		return this.currentWidth;
	}
	
	public void setCurrentHeight(int h) {
		this.currentHeight = h;
	}
	
	public void setCurrentWidth(int w) {
		this.currentWidth = w;
	}
	
	public Color getLabelColor() {
		return labelColor;
	}

	public void setLabelColor(Color labelColor) {
		this.labelColor = labelColor;
	}
	
	public int getOriginalHeight() {
		return this.originalHeight;
	}

	public void setOriginalHeight(int oh) {
		this.originalHeight = oh;
	}
	
	public int getOriginalWidth() {
		return this.originalWidth;
	}

	public void setOriginalWidth(int ow) {
		this.originalWidth = ow;
	}
	
	public int getFlagShape() {
		return this.flagShape;
	}
	
	public void setFlagShape(int fs) {
		this.flagShape = fs;
	}
	
	public static void setSharedbound(Color sharedbound) {
		Sharedbound = sharedbound;
	}

	public static Color getSharedlabelColor() {
		return SharedlabelColor;
	}

	public static void setSharedlabelColor(Color sharedlabelColor) {
		SharedlabelColor = sharedlabelColor;
	}
	
	public boolean getShowLabelFlag() {
		return this.showLabelFlag;
	}
	
	public void setShowLabelFlag(boolean flag) {
		this.showLabelFlag = flag;
	}
	
	public void setIncidentJEdges(List<JEdge> incidentJEdges) {
		this.incidentJEdges = incidentJEdges;
	}
	
	public static int getBasicHeight() {
		return JNode.basicHeight;
	}
	
	public static int getBasicWidth() {
		return JNode.basicWidth;
	}
	
	public Point2D.Float setAnchorPoint(JEdge je) {
		
		Point2D.Float oldAnchor = null;
		
		if(je.getSource().equals(this)) {
			
			oldAnchor = je.getTransitDots().get(0);
			oldAnchor.setLocation(getAnchorPoint(this.p,je.getTransitDots().get(1)));
		}
		if(je.getDest().equals(this)) {
			oldAnchor = je.getTransitDots().get(je.getTransitDots().size()-1);	
			oldAnchor.setLocation(getAnchorPoint(je.getTransitDots().get(je.getTransitDots().size()-2),this.p));
		}
		
		return oldAnchor;
	}
	
	public static Color getSharedbackground() {
		return Sharedbackground;
	}

	public static void setSharedbackground(Color sharedbackground) {
		Sharedbackground = sharedbackground;
	}

	public static Color getSharedbound() {
		return Sharedbound;
	}
}
