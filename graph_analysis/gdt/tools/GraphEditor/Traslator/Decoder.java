package Traslator;

import java.awt.geom.Point2D;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.StringTokenizer;
import Notifier.GenericNotifier;
import IU.JEdge;
import IU.JGraph;
import IU.JNode;

public class Decoder {

	private BufferedReader bfr;
	private JGraph jg;
	String next;
	
	public Decoder(JGraph jg) {
		this.jg = jg;
	}
	/*
	 * Load a JGraph saved in a file
	 */
	@SuppressWarnings("hiding")
	public void load(File f) {
	
		try {
			bfr = new BufferedReader(new FileReader(f));
			this.isWellFormed(f);
			
			
		} catch (Exception e) {
			e.printStackTrace();
		} 
		
		
			
	}
	
	/*
	 * Read the section UNDISECTION in the file
	 * */
	private boolean readUndisection()throws IOException{
		boolean ris = false;
		next = nextToken();
		if(next.startsWith("<UNDISECTION>"))
			ris = readNodelist();
		else
			throw new IOException();
		if(next.startsWith("</UNDISECTION>")){
			next = nextToken();
			return ris;
			}
		return false;
	}
	
	/*
	 * Read the section NODELIST in the UNDISECTION
	 * */
	private boolean readNodelist()throws IOException{
		boolean ris = false;
		next = nextToken();
		if(next.startsWith("<NODELIST>"))	
			while(!next.startsWith("</NODELIST>")){
				ris = readNode();
			}
		else
			throw new IOException();
		next = nextToken();
		return ris;
	}
	/*
	 * Read the section NODE in the UNDISECTION
	 * */
	private boolean readNode()throws IOException{
		boolean ris = false;
		next = nextToken();
		
		if(!next.startsWith("<NODE>")){
			return true;
			
		}else{
			next = nextToken();
			JNode jn = new JNode(Integer.parseInt(next));
			jg.getJNodes().add(jn);
			ris = readEdge(jn);
			if(!next.startsWith("</NODE>"))
				throw new IOException();
		}	
			
		return ris;
	}
	
	/*
	 * Read the section EGDE in the UNDISECTION
	 * */
	private boolean readEdge(JNode jn)throws IOException{
		boolean ris;
		next = nextToken();
		if(!next.startsWith("<EDGE>"))
			
			return true;
		else {
			StringTokenizer token = new StringTokenizer(next);
			token.nextToken();
			String id = token.nextToken();
			String direction = token.nextToken();
			JEdge je;
			
			if(jg.getJEdge(Integer.parseInt(id))==null){
				je = new JEdge(Integer.parseInt(id));
				jg.getJEdges().add(je);
				}
			else
				je = jg.getJEdge(Integer.parseInt(id));
			
				if(je.getSource()==null){
					je.setSource(jn);
				}
				else if(je.getDest()==null){
					je.setDest(jn);
				}
				else 
					throw new IOException();
				
			if(je.getSource()!=null && je.getDest()!=null){
			if(!direction.equals("--")){
				je.setVisibleOrientation(true);
				if(direction.equals("->")){
					if(jn.equals(je.getDest()))
						je.invert();
				}
				else
					if(jn.equals(je.getSource()))
						je.invert();
				}
			}
			ris = readEdge(jn);
		}
		return ris;
	}
	
	/*
	 * Read the section DRAWSECTION in the file
	 * */
	
	private boolean readDrawsection() throws IOException{
		boolean ris = false;
		if(next.startsWith("<DRAWSECTION>"))
			ris = readNodelist2();
		else
			throw new IOException();
		ris = readEdgelist();
		if(next.startsWith("</DRAWSECTION>")){
			return ris;
			}
		return false;
	}
	
	/*
	 * Read the section NODELIST in the DRAWSECTION
	 * */
	
	private boolean readNodelist2()throws IOException{
		boolean ris = false;
		next = nextToken();
		if(next.startsWith("<NODELIST>"))	
			while(!next.startsWith("</NODELIST>")){
				ris = readNode2();
			}
		else
			throw new IOException();
		next = nextToken();
		return ris;
	}
	
	/*
	 * Read the section NODE in the DRAWSECTION
	 * */
	
	private boolean readNode2()throws IOException{
		boolean ris = false;
		next = nextToken();
		
		if(!next.startsWith("<NODE>")){
			return true;
			
		}else{
			next = nextToken();
			StringTokenizer nodeinfo = new StringTokenizer(next);
			String id = nodeinfo.nextToken();
			String point = nodeinfo.nextToken();
			String shape = nodeinfo.nextToken();
			String width = nodeinfo.nextToken();
			String height = nodeinfo.nextToken();
			@SuppressWarnings("unused")
			String zoom = nodeinfo.nextToken();
			@SuppressWarnings("unused")
			String nodeColor = nodeinfo.nextToken();
			@SuppressWarnings("unused")
			String borderColor = nodeinfo.nextToken();
			@SuppressWarnings("unused")
			String labelColor = nodeinfo.nextToken();
			String label = nodeinfo.nextToken();
			
			JNode jn = jg.getJNode(Integer.parseInt(id));
			jn.setPoint(stringToPoint(point));
			jn.setOriginalWidth(Integer.parseInt(width)*5);
			jn.setCurrentWidth(Integer.parseInt(width)*5);
			jn.setOriginalHeight(Integer.parseInt(height)*5);
			jn.setCurrentHeight(Integer.parseInt(height)*5);
			jn.setFlagShape(Integer.parseInt(shape));
			jn.setLabel(label);
			next = nextToken();
			if(!next.startsWith("</NODE>"))
				throw new IOException();
		}	
			
		return ris;
	}
	
	/*
	 * Read the section EDGELIST in the DRAWSECTION
	 * */
	private boolean readEdgelist()throws IOException{
		boolean ris = false;
		if(next.startsWith("<EDGELIST>"))	
			while(!next.startsWith("</EDGELIST>")){
				ris = readEdge2();
			}
		else
			throw new IOException();
		next = nextToken();
		return ris;
	}
	/*
	 * Read the section EDGE in the DRAWSECTION
	 * */
	
	private boolean readEdge2()throws IOException{
		next = nextToken();
		
		if(!next.startsWith("<EDGE>")){
			return true;
			
		}else{
			next = nextToken();
			StringTokenizer edgeinfo = new StringTokenizer(next);
			String id = edgeinfo.nextToken();
			String source = edgeinfo.nextToken();
			@SuppressWarnings("unused")
			String color = edgeinfo.nextToken();
			@SuppressWarnings("unused")
			String labelColor = edgeinfo.nextToken();
			String label = edgeinfo.nextToken();
			edgeinfo.nextToken();
			edgeinfo.nextToken();
			Point2D.Float anchorSource = this.stringToPoint(edgeinfo.nextToken());
			Point2D.Float anchorDest = this.stringToPoint(edgeinfo.nextToken());
			
			JEdge je = jg.getJEdge(Integer.parseInt(id));
			
			if(je!=null){
			je.setLabel(label);
			if(!source.equals(String.valueOf(je.getSource().getID())))
				invert(je);
			
			je.getTransitDots().add(je.getSource().getPoint());
			}
			
			next = nextToken();
			
			if(next.startsWith("<BEND>"))
				readBend(je);
			else if(!next.equals("</EDGE>")){
				throw new IOException();
			}
			je.getTransitDots().add(je.getDest().getPoint());
			je.setTransitDots(jg.createNewTransitDots(je.getSource(), je.getDest(), je.getTransitDots()));
			je.getSource().getIncidentJEdges().add(je);
			je.getDest().getIncidentJEdges().add(je);
			if(anchorSource.getX()!=0 || anchorSource.getY()!=0){
				
				if(Math.abs(je.getTransitDots().get(0).getX()-je.getSource().getPoint().getX())
							<=je.getSource().getCurrentWidth()){
					je.getTransitDots().get(0).setLocation(je.getSource().getPoint().getX()+anchorSource.getX(),
							je.getSource().getPoint().getY()+anchorSource.getY());
				}else if(Math.abs(je.getTransitDots().get(0).getY()-je.getSource().getPoint().getY())
						<=je.getSource().getCurrentHeight()){
					je.getTransitDots().get(0).setLocation(je.getSource().getPoint().getX()+anchorSource.getX(),
							je.getSource().getPoint().getY()+anchorSource.getY());
				}
			}
				int width = je.getSource().getCurrentWidth();
				int height = je.getSource().getCurrentHeight();
				
				if(je.getSource().getPoint().getY()>je.getDest().getPoint().getY())
					height = -height;
				if(je.getSource().getPoint().getX()>je.getDest().getPoint().getX())
					width = -width;
				
				
				if(!this.straightJEdge(je)){
					}
			
			Point2D.Float last=je.getTransitDots().get(je.getTransitDots().size()-1);
			Point2D.Float prelast=je.getTransitDots().get(je.getTransitDots().size()-2);
			if(anchorDest.getX()!=0 || anchorDest.getY()!=0){
				if(Math.abs(last.getX()-je.getDest().getPoint().getX())
						<=je.getDest().getCurrentWidth()){
					last.setLocation(je.getDest().getPoint().getX()+anchorDest.getX(),
							je.getDest().getPoint().getY()+anchorDest.getY());
				}else if(Math.abs(last.getY()-je.getDest().getPoint().getY())
						<=je.getDest().getCurrentHeight()){
					last.setLocation(je.getDest().getPoint().getX()+anchorDest.getX(),
							je.getDest().getPoint().getY()+anchorDest.getY());
				}
			}
				int width2 = je.getDest().getCurrentWidth();
				int height2 = je.getDest().getCurrentHeight();
				if(je.getDest().getPoint().getY()>je.getSource().getPoint().getY())
					width2 = -width2;
				if(je.getDest().getPoint().getX()>je.getSource().getPoint().getX())
					height2 = -height2;
				if(!this.straightJEdge(je)){
					
				}
			
				
			if(!this.straightJEdge(je)){
			if(je.getTransitDots().get(1).getX()==je.getSource().getPoint().getX())
				je.getTransitDots().get(0).setLocation(je.getSource().getPoint().getX(),
						je.getSource().getPoint().getY()+height/2);
			else if(je.getTransitDots().get(1).getY()==je.getSource().getPoint().getY())
				je.getTransitDots().get(0).setLocation(je.getSource().getPoint().getX()+width/2,
						je.getSource().getPoint().getY());
			
			if(prelast.getX()==je.getDest().getPoint().getX())
				last.setLocation(je.getDest().getPoint().getX(),
						je.getDest().getPoint().getY()+height2/2);
			else if(prelast.getY()==je.getDest().getPoint().getY())
				last.setLocation(je.getDest().getPoint().getX()+width2/2,
						je.getDest().getPoint().getY());
			}
			return true;
			
			}
		
	}
	
	/*
	 * Read the section BEND in the DRAWSECTION
	 * */
	private void readBend(JEdge je) throws IOException{
		
		StringTokenizer bend = new StringTokenizer(next);
		bend.nextToken();
		Point2D.Float point = this.stringToPoint(bend.nextToken());
		je.getTransitDots().add(point);
		next = nextToken();
		if(next.startsWith("<BEND>"))
			readBend(je);
		else if(!next.equals("</EDGE>")){
			throw new IOException();
		}
	}
	/*
	 * Verify if the file is well formed
	 * */
	private boolean isWellFormed(File f) {
		boolean flag = false;
				
		try{
			flag = this.readUndisection();
			flag = flag && this.readDrawsection();
		} catch (IOException e) {
			GenericNotifier gn=new GenericNotifier();
			gn.showMessage("Error: malformed file");
		}
		return flag;
	}
	
	/*
	 * Get the next line in the file
	 * */
	private String nextToken() throws IOException{
		String next = bfr.readLine();
		if(next==null)
			next = bfr.readLine();
		while(next.equals(""))
			next = bfr.readLine();
		
		while(next.charAt(0)==' ' || next.charAt(0)=='\t'){
			
			next = next.substring(1,next.length());
		}
		return next;
			
	}
	
	/*
	 * Get a Point2D from a String
	 * */
	private Point2D.Float stringToPoint(String point){
		StringTokenizer coordinates = new StringTokenizer(point,",");
		String x = coordinates.nextToken().substring(1);
		String ytemp = coordinates.nextToken();
		String y = ytemp.substring(0, ytemp.length()-1);
		Point2D.Float p = new Point2D.Float(Float.parseFloat(x)*5,Float.parseFloat(y)*5);
		return p;
	}
	
	/*
	 * Invert the direction of a JEdge
	 * */
	private void invert(JEdge je){
		JNode temp=je.getSource();
		je.setSource(je.getDest());
		je.setDest(temp);
		
	}
	private boolean straightJEdge(JEdge je){
		boolean result = false;
		int last = je.getTransitDots().size()-1;
		if(je.getTransitDots().size()==2){
			
			if(je.getTransitDots().get(0).getX()==je.getTransitDots().get(last).getX() || 
					je.getTransitDots().get(0).getY()==je.getTransitDots().get(last).getY()){
				return true;
			}
		}else{
			if((je.getTransitDots().get(0).getX()==je.getTransitDots().get(1).getX() || 
				je.getTransitDots().get(0).getY()==je.getTransitDots().get(1).getY() ))
					return true;
			else if((je.getTransitDots().get(last).getX()==je.getTransitDots().get(last-1).getX()  || 
					je.getTransitDots().get(last).getY()==je.getTransitDots().get(last-1).getY()))
					return true;
			
		}
		return result;
	}
	
}
