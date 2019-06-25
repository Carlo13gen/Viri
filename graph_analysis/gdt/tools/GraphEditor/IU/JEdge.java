package IU;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import javax.swing.JComponent;

public class JEdge extends JComponent {

	/**
	 * 
	 */
	private static final long serialVersionUID = 342055870531000461L;
	private static Color Sharedcolor = Color.black;
	private static Color SharedlabelColor = Color.red;
	private Color color = Sharedcolor;
	private Color labelColor = SharedlabelColor;
	private JNode source;
	private JNode dest;
	private List<Point2D.Float> transitDots;
	private List<Line2D.Float> lines;
 	private static int GLOBAL_NUMBER = 0;
	private String label;
	private int id;
	private boolean inverse=false;
	private boolean visibleOrientation=false;
	private String direction="--";
	int zoomLevel;
	private boolean showLabelFlag = true;

	/*
	 * constructor used if JEdge is load from a GDT file
	 */
	public JEdge(int id){
		this.id = id;
		setMaxID(id);
		this.transitDots = new LinkedList<Point2D.Float>();
		this.lines = new LinkedList<Line2D.Float>();
		this.label = String.valueOf(id);
		
	}
	/*
	 * constructor used if JEdge is create from user
	 */
	public JEdge(JNode source,JNode dest,List<Point2D.Float> transitDots,List<Line2D.Float> lines,
										Color color,Color labelColor) {
		this.source = source;
		this.dest = dest;
		this.transitDots = transitDots;
		this.lines = lines;
		wrapID();
		this.color = color;
		this.labelColor = labelColor;
		
	}
	/*
	 *  wrap jedge id and assign it to value default label
	 */
	public void wrapID() {
		this.id = GLOBAL_NUMBER;
		this.label = String.valueOf(GLOBAL_NUMBER);
		GLOBAL_NUMBER++;	
	}
	/*
	 * draw a label on edge
	 */
	public void putLabelOn(Graphics g) {
        Graphics2D g2 = (Graphics2D)g;
        g2.setColor(this.getLabelColor());
        g2.setFont(new Font("Dialog",3,10));
        
        
        if(this.showLabelFlag)
        	g2.drawString(this.label,(int)this.getLabelPoint().getX(),(int)this.getLabelPoint().getY());
    }
    
	public Point2D.Float getLabelPoint(){
		Point2D.Float p1 = this.getTransitDots().get(0);
        Point2D.Float p2 = this.getTransitDots().get(1);
        
        int size= this.getTransitDots().size()/2;
        if(this.inverse && this.getTransitDots().size()%2!=0)
        	size++;
        
        for(int i=0;i<size;i++){
        	p1 = this.getTransitDots().get(i);
        	p2 = this.getTransitDots().get(i+1);
        }
		Point2D.Float labelPoint = new Point2D.Float((int)((p1.getX()+p2.getX())/2),(int)((p1.getY()+p2.getY())/2));
        return labelPoint;
	}
	
	/*
	 * standard JComponent method
	 */
    public void paintComponent(Graphics g) {
       
    	Graphics2D g2 = (Graphics2D)g;
    	
    	if(this.getTransitDots().size()==0)
        	return;
    	
    	if(this.source.equals(dest) && this.transitDots.size()==3) 
    		drawLoop(g2);
    	
    	recreateLines();
    	
    	for(Point2D.Float point : this.transitDots) {  
        	g2.setColor(this.getColor());
            g2.fillOval ((int)point.getX(),(int)point.getY(),1,1);
        }
    	
        if(this.visibleOrientation){
        	Point2D.Float last = this.transitDots.get(this.transitDots.size()-1);
        	Point2D.Float prelast = this.transitDots.get(this.transitDots.size()-2);
        	g2.setColor(this.getColor());
        	drawArrowhead(g2, (int)prelast.x, (int)prelast.y, (int)last.x, (int)last.y);
        
        }
        for(Line2D line : this.lines) {
        		g2.setColor(this.getColor());
        		g2.draw(line);
        }
        this.putLabelOn(g);
        g2.setColor(this.getColor());
        g2.drawLine((int)this.getSource().getPoint().getX(),(int)this.getSource().getPoint().getY(),
        		(int)this.getTransitDots().get(0).getX(),(int)this.getTransitDots().get(0).getY());
        int last = this.getTransitDots().size()-1;
        g2.drawLine((int)this.getDest().getPoint().getX(),(int)this.getDest().getPoint().getY(),
        		(int)this.getTransitDots().get(last).getX(),(int)this.getTransitDots().get(last).getY());
    }

    /*
     * if source is equals dest draw loop on JNode
     */
    public void drawLoop(Graphics2D g2) {
    	
    	Point2D.Float first = this.getTransitDots().get(0);
    	Point2D.Float dot = this.getTransitDots().get(1);
    	Point2D.Float last = this.getTransitDots().get(2);
    	
    	Point2D.Float dotNextFirst = new Point2D.Float(first.x,dot.y);
	    Point2D.Float dotPreLast = new Point2D.Float(dot.x,last.y);
	    
	    this.getTransitDots().clear();
	    	
	    this.getTransitDots().add(first);
	    this.getTransitDots().add(dotNextFirst);
	    this.getTransitDots().add(dot);
	    this.getTransitDots().add(dotPreLast);
	    this.getTransitDots().add(last);
	    
	    this.getSource().moveTo(this.getSource().getPoint());
	}
    
    /*
     * recreate lines passing through transit dots each time is invoked paintComponents method
     */
	private void recreateLines() {
		
		this.lines.clear();
		Point2D.Float p = this.transitDots.get(0);
		
		for(int i=1; i<=this.transitDots.size()-1; i++) {
			Line2D.Float line = new Line2D.Float(p,this.transitDots.get(i));
			this.lines.add(line);
			p = this.transitDots.get(i);
			
		}	
		
	}
    /*
     * check if lines contains Point p 
     */
	public boolean contains(Point2D.Float p){
		for(Line2D l:lines)
			if(l.ptSegDist(p)<3)
				return true;
		return false;
	}
	
	/*
	 * check if exits a transitDot is near clicked point 
	 */
	public Point2D existTransitDot(Point2D.Float point) {
		Point2D.Float dot = null;
		for(int i=1; i<=this.getTransitDots().size()-2; i++) {
			dot = this.getTransitDots().get(i);
			
			if(dot.distance(point)<3)
				return dot;
		}
		return null;
	}
	
	/*
	 * zoom++ of JEdge
	 */
	public void zoomIn(int zoom) {
		zoomLevel = zoom;
		for(Point2D.Float p : transitDots){ 
			if(!p.equals(this.getSource().getPoint()) && !p.equals(this.getDest().getPoint()))
				p.setLocation(p.getX()*1.2,p.getY()*1.2);
		}
	}	
	
	/*
	 * zoom-- of JEdge
	 */
	public void zoomOut(int zoom) {
		zoomLevel = zoom;
		for(Point2D.Float p : transitDots) 
			if(!p.equals(this.getSource().getPoint()) && !p.equals(this.getDest().getPoint()))
				p.setLocation(p.getX()/1.2,p.getY()/1.2);
	}

	/*
	 * reverse transitDots list 
	 */
	public void invert(){
		Collections.reverse(this.getTransitDots());
		JNode temp;
		temp = this.getSource();
		this.setSource(this.getDest());
		this.setDest(temp);
	}
	
	/*
	 * draw an arrow on JEdge
	 */
	public void drawArrowhead(Graphics2D g2, int x1, int y1, int x2, int y2) {
		 
		double ArrowAngle = Math.PI/6;
		double ArrowLenght = 10;  
		double theta, side = 1;
		Point2D.Float tip, side1, side2;
		
		theta = Math.atan((double)(y2-y1)/(double)(x2-x1));
		
		if (x2 < x1)
			side = -1;

			tip = new Point2D.Float((float)(x2 + side * Math.cos(theta)), (float)(y2 + side * Math.sin(theta)));
			side1 = new Point2D.Float((float)(tip.x - side * ArrowLenght * Math.cos(theta+ArrowAngle)), (float)(tip.y - side * ArrowLenght * Math.sin(theta+ArrowAngle)));
			side2 = new Point2D.Float((float)(tip.x - side * ArrowLenght * Math.cos(theta-ArrowAngle)), (float)(tip.y - side * ArrowLenght * Math.sin(theta-ArrowAngle)));
			Line2D.Float l1 = new Line2D.Float(tip.x,tip.y,side1.x,side1.y);
			Line2D.Float l2 = new Line2D.Float(tip.x,tip.y,side2.x,side2.y);
			
			g2.draw(l1);
			g2.draw(l2);
			
	}	
	
	/*
	 * getter and setter methods
	 */
	public void restoreColorProperty() {
		this.color = JEdge.Sharedcolor;
		this.labelColor = JEdge.SharedlabelColor;
	}

	public static Color getSharedcolor() {
		return Sharedcolor;
	}

	public static void setSharedcolor(Color sharedcolor) {
		Sharedcolor = sharedcolor;
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
	
	public static int getGLOBAL_NUMBER() {
		return JEdge.GLOBAL_NUMBER;
	}
	
	public static void setGLOBAL_NUMBER(int i) {
		JEdge.GLOBAL_NUMBER = i;
	}
	
	public int getID() {
		return this.id;
	}
	
	public void setID(int id) {
		this.id = id;
	}
	
	public JNode getSource() {
		return this.source;
	}
	
	public JNode getDest() {
		return this.dest;
	}
	
	public void setSource(JNode source) {
		this.source = source;
	}
	
	public void setDest(JNode dest) {
		this.dest = dest;
	}
	
	public List<Point2D.Float> getTransitDots() {
		return this.transitDots;
	}
	
	public void setTransitDots(List<Point2D.Float> transitDots) {
		this.transitDots = transitDots;
	}
	
	public List<Line2D.Float> getLines() {
		return this.lines;
	}
	
	public String getLabel() {
		return this.label;
	}
	
	public void setLabel(String l) {
		this.label = l;
	}
	
	public Color getColor() {
		return this.color;
	}

	public void setColor(Color c) {
		this.color = c;
	}
	
	public Color getLabelColor() {
		return labelColor;
	}

	public void setLabelColor(Color labelColor) {
		this.labelColor = labelColor;
	}
	
	public String getDirection() {
		return this.direction;
	}
	
	public void setDirection(String direction) {
		this.direction = direction;
	}
	
	public boolean isVisibleOrientation() {
		return visibleOrientation;
	}
	
	public void setVisibleOrientation(boolean visibleOrientation) {
		this.visibleOrientation = visibleOrientation;
	}
	
	public boolean isInverse() {
		return inverse;
	}
	
	public void setInverse(boolean inverse) {
		this.inverse = inverse;
	}
	
	private void setMaxID(int id){
		if(GLOBAL_NUMBER<=id)
			GLOBAL_NUMBER=id+1;
	}
}