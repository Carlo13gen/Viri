package Drawer;

import java.awt.Graphics2D;
import java.awt.Shape;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

import GdtJavaWrapper.Edge;
import GdtJavaWrapper.Graph;
import GdtJavaWrapper.Node;
import GdtJavaWrapper.OrthogonalDrawer;
import IU.JEdge;
import IU.JGraph;
import IU.JNode;
import Notifier.AbstractNotifier;
import Notifier.OrthoDrawerNotifier;

public class OrthoDrawer {

	private JGraph jg;
	private Graph g;
	private List<Node> ns; 
	private List<Edge> es; 
	private OrthogonalDrawer ortho;
	private AbstractNotifier odf;
	
	public OrthoDrawer(JGraph jg) {
		this.jg = jg;
	}
	/*
	 * build GDT Graph from JGraph
	 */
	private void buildGraph() {
		this.jg.toZoom(0);
		this.restoreJNodeSize();
		this.g = new Graph();
		this.jg.setID(g.getId());
		
		
		this.ns = new LinkedList<Node>();
		this.es = new LinkedList<Edge>();
		
		List<JNode> jns = this.jg.getJNodes();
		List<JEdge> jes = this.jg.getJEdges();
		/*
		 * map JGraph JNode to GDT Node
		 */
		for(JNode jn : jns) {
			Node n = g.newNode();
			jn.setID(n.getId());
			ns.add(n);
		}
		/*
		 * map JGraph JEdge to GDT Edge
		 */
		for(JEdge je : jes) {
			JNode source = je.getSource();
			JNode dest = je.getDest();
			
			source.getIncidentJEdges().clear();
			dest.getIncidentJEdges().clear();
			
			Node n1 = null;
			Node n2 = null;
			
			for(Node n : ns) {
				if(n.getId()==source.getID()) {
					n1 = n;
					source.addIncidentJEdge(je);
				}
				if(n.getId()==dest.getID()) {
					n2 = n;
					dest.addIncidentJEdge(je);
				}
			}
			
			if(n1!=null && n2!=null) {
				Edge e = g.newEdge(n1, n2);
				je.setID(e.getId());
				es.add(e);
			}
		
		
		}
		
	}
	
	/*
	 * check if JGraph contains self-loop
	 */
	public boolean containsSelfLoop() {
		
		for(JEdge je : jg.getJEdges()) {
			if(je.getSource().equals(je.getDest()))
				return true;
		}
		
		return false;
	}
	/*
	 * build from GDT Graph a orthogonal JGraph using JNI 
	 */
	public void makeOrthogonal() {
		
		buildGraph();
		
		if(this.g.isConnected() && !containsSelfLoop()) {
			
			ortho = new OrthogonalDrawer(this.g);
			
			for(Node n : this.ns) {
			
				for(JNode jn : jg.getJNodes()) {
					if(n.getId()==jn.getID()) {
						
						Rectangle2D.Double r = ortho.getNodeRectangle(n);
						jn.moveTo(5*(float)(r.x+r.width/2),5*(float)(r.y-r.height/2));
					}
						
				}
			}
			
			for(Edge e : this.es) {
				
				Point2D.Double[] points = ortho.getEdgePolyline(e);
				List<Point2D.Float> transitDots = this.toListFloat(points);	
				
				for(JEdge je : jg.getJEdges()) {
					
					if(e.getId()==je.getID()) {
						
						JNode source = je.getSource();
						source.addIncidentJEdge(je);
						JNode dest = je.getDest();
						dest.addIncidentJEdge(je);	
						je.setTransitDots(transitDots);
						
						if(source.isMyAnchor(transitDots.get(0))){
							
							this.attachAnchorToNode(transitDots.get(0), source);
							this.attachAnchorToNode(transitDots.get(transitDots.size()-1), dest);
						
						}else if(dest.isMyAnchor(transitDots.get(0))){
							
							this.attachAnchorToNode(transitDots.get(0), dest);
							this.attachAnchorToNode(transitDots.get(transitDots.size()-1), source);
						}		
					}	
				}	
			}
				for(JEdge je:jg.getJEdges()){
					
					if(!je.getSource().isMyAnchor(je.getTransitDots().get(0)))
						Collections.reverse(je.getTransitDots());
				}	
			jg.repaint();
		}
		else {
			
			odf = new OrthoDrawerNotifier();
			odf.showMessage();
		}
	}
	/*
	 *  get a Point2D list from an array
	 */
	private List<Point2D.Float> toListFloat(Point2D.Double[] array) {
		List<Point2D.Float> list = new LinkedList<Point2D.Float>();
		for(int i=0; i<array.length; i++) {
			
			Point2D.Float p = new Point2D.Float(5*((float)array[i].x),(5*(float)array[i].y));
			list.add(p);
			Graphics2D g2 = (Graphics2D)jg.getGraphics();
			Shape s = new Ellipse2D.Float(p.x,p.y,1,1);
			g2.draw(s);
			
		}
		return list;
	}
	/*
	 * attach JEdge to JNode. JNI methods calculate a polyline of double Point2Ds, but
	 * JEdge use a float Point2Ds polyline
	 */
	private void attachAnchorToNode(Point2D.Float anchor,JNode jn) {
		Point2D.Float center = jn.getPoint();
		
		if(anchor.x<center.x+jn.getCurrentWidth()/2 && anchor.x>center.x-jn.getCurrentWidth()/2) {
			if(anchor.y<center.y)
				anchor.y = anchor.y+2;
			if(anchor.y>center.y)
				anchor.y = anchor.y-2;
		}
		if(anchor.y<center.y+jn.getCurrentHeight()/2 && anchor.y>center.y-jn.getCurrentHeight()/2){
			if(anchor.x<center.x)
				anchor.x = anchor.x+2;
			if(anchor.x>center.x)
				anchor.x = anchor.x-2;
		}	
	}
	/*
	 * getter Methods
	 */
	public Graph getGraph() {
		return this.g;
	}
	
	public List<Node> getNodes() {
		return this.ns;
	}
	
	public List<Edge> getEdges() {
		return this.es;
	}
	private void restoreJNodeSize(){
		
		for(JNode jn:jg.getJNodes())
			jn.restoreBasicSize();
	}
}
