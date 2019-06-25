package IU;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.util.LinkedList;
import java.util.List;

import javax.swing.JComponent;

public class JIncompleteEdge extends JComponent{
	/**
	 * 
	 */
	private static final long serialVersionUID = -4649179805341535999L;
	private static Color color = JEdge.getSharedcolor();;
	private List<Point2D.Float> curTransitDots;
	private List<Line2D.Float> currentLines;
	private Point2D.Float source;
	private Point2D.Float dest;
	private Point2D.Float interDot;
	private JGraph jg;

	public JIncompleteEdge(JGraph jg){
		this.jg=jg;
		initialize();
	}
	/*
	 * initialize structures
	 */
	public void initialize(){
		this.clearCurrentLines();
		this.clearCurTransitDots();
		source = null;
		interDot = null;
		dest = null;
	}
	/*
	 * renew transitDots list
	 */
	public void clearCurTransitDots() {
		this.curTransitDots = new LinkedList<Point2D.Float>();
	}
	/*
	 * renew currentLines list
	 */
	public void clearCurrentLines() {
		this.currentLines = new LinkedList<Line2D.Float>();
	}
	/*
	 * add transitDot to jincompleteedge
	 */
	public void manageLine(Point2D.Float p){
		
		if(jg.existJNode(p)) {
			if(source==null) {
				source = jg.getJNodeToPoint(p).getPoint();
				interDot = jg.getJNodeToPoint(p).getPoint();
				this.getCurTransitDots().add(source);
			}
			else {
				dest = jg.getJNodeToPoint(p).getPoint();
				this.createLine(interDot,dest);
				jg.createJEdge(source,dest,this.getCurTransitDots());
				initialize();
			}
		}
		else {
			if(interDot!=null) {
				dest = p;
				this.createLine(interDot,dest);
				interDot = dest;
			}
		}
	}	
	/*
	 * create line between transitDots
	 */
	public void createLine(Point2D.Float p1,Point2D.Float p2) {
		this.getCurTransitDots().add(p2);
		Line2D.Float line = new Line2D.Float(p1,p2);
		this.getCurrentLines().add(line);
	}
	/*
	 * delete last line of jincompleteedge
	 */
	public int deleteLine(){
		if(this.getCurTransitDots().size()==1){
			initialize();
			return 1;
		}
		this.getCurrentLines().remove(this.getCurrentLines().size()-1);
		this.getCurTransitDots().remove(this.getCurTransitDots().size()-1);
		this.interDot = this.getCurTransitDots().get(this.getCurTransitDots().size()-1);
		return 0;
	}
	/*
	 * standard Jcomponent method
	 */
	public void paintComponent(Graphics g) {
		this.recreateLines();
		Graphics2D g2 = (Graphics2D)g;
		g2.setColor(getColor());
	    for(Line2D  line: currentLines)
	    	g2.draw(line);
	
	}
	/*
	 * recreate lines between transitDots
	 */
	private void recreateLines() {
		if(this.curTransitDots.isEmpty())
			return;
		this.currentLines.clear();
		Point2D.Float p = this.curTransitDots.get(0);
		for(int i=1; i<=this.curTransitDots.size()-1; i++) {
			Line2D.Float line = new Line2D.Float(p,this.curTransitDots.get(i));
			this.currentLines.add(line);
			p = this.curTransitDots.get(i);
		}		
	}
	/*
	 * getter and setter methods
	 */
	public List<Point2D.Float> getCurTransitDots() {
		return this.curTransitDots;
	}
	
	public List<Line2D.Float> getCurrentLines() {
		return this.currentLines;
	}
	
	public static Color getColor() {
		return JIncompleteEdge.color;
	}

	public static void setColor(Color color) {
		JIncompleteEdge.color = color;
	}

	public Point2D.Float getInterDot() {
		return interDot;
	}

	public void setInterDot(Point2D.Float interDot) {
		this.interDot.setLocation(interDot);
	}
	
}
