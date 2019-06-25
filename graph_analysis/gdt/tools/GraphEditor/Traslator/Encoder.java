package Traslator;

import java.awt.geom.Point2D;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

import IU.JEdge;
import IU.JGraph;
import IU.JNode;

public class Encoder {
	
	private JGraph jg;
	private FileWriter fileToWrite;
	
	public Encoder(JGraph jg) {
		this.jg = jg;
		jg.toZoom(0);
		
	}
	/*
	 * Save a JGraph in a file with the .gdt format
	 */
	public void save(String filename) {
		try {
			if(!filename.endsWith(".gdt")) {
				filename = new String(filename.concat(".gdt"));
			}
			fileToWrite = new FileWriter(new File(filename));
			
			fileToWrite.write("<UNDISECTION>\n");
			fileToWrite.write("<NODELIST>\n");
			
				for(JNode jn : jg.getJNodes()) {
					fileToWrite.write("<NODE>\n");
					fileToWrite.write(jn.getID()+"\n");
					jn.cleanIncidentJEdges();
					for(JEdge je : jn.getIncidentJEdges()) {
						fileToWrite.write("<EDGE> "+je.getID()+" ");
						if(!je.isVisibleOrientation()){
							fileToWrite.write("--"+"\n");
						}else{ 
							if(jn.equals(je.getSource()))
								fileToWrite.write("->"+"\n");
							
						else if(jn.equals(je.getDest()))
							fileToWrite.write("<-"+"\n");
						}	
					}
	
						fileToWrite.write("</NODE>\n");
					
				}
			
			
			fileToWrite.write("</NODELIST>\n");
			fileToWrite.write("</UNDISECTION>\n");
			
			fileToWrite.write("<DRAWSECTION>\n");
			
			fileToWrite.write("<NODELIST>\n");
			
			for(JNode jn : jg.getJNodes()) {
				fileToWrite.write("<NODE>\n ");
				fileToWrite.write(jn.getID()+" ");
				fileToWrite.write("(");
				fileToWrite.write(String.valueOf(jn.getPoint().getX()/5));
				fileToWrite.write(",");
				fileToWrite.write(String.valueOf(jn.getPoint().getY()/5));
				fileToWrite.write(") ");
				fileToWrite.write("1 ");
				fileToWrite.write(jn.getOriginalWidth()/5+" "+jn.getOriginalHeight()/5+" ");
				fileToWrite.write("0.5 5 1 1 ");
				fileToWrite.write(jn.getLabel()+" ");
				fileToWrite.write("(");
				fileToWrite.write(String.valueOf(jn.getPoint().getX()/5));
				fileToWrite.write(",");
				fileToWrite.write(String.valueOf(jn.getPoint().getY()/5));
				fileToWrite.write(")\n");
				fileToWrite.write("</NODE>\n");
			}
			fileToWrite.write("</NODELIST>\n");
			
			fileToWrite.write("<EDGELIST>\n");
			
			for(JEdge je : jg.getJEdges()) {
				fileToWrite.write("<EDGE>\n");
				fileToWrite.write(je.getID()+" ");
				fileToWrite.write(je.getSource().getID()+" ");
				fileToWrite.write("1 1 ");
				fileToWrite.write(je.getLabel()+" ");
				Point2D.Float point = je.getLabelPoint();
				Point2D.Float anchorSource = this.getAnchorForSource(je);
				Point2D.Float anchorDest = this.getAnchorForDest(je);
				fileToWrite.write("(");
				fileToWrite.write(String.valueOf(point.getX()/5.));
				fileToWrite.write(",");
				fileToWrite.write(String.valueOf(point.getY()/5.));
				fileToWrite.write(") ");
				fileToWrite.write("0 ");
				fileToWrite.write("(");
				fileToWrite.write(String.valueOf(anchorSource.getX()/5.));
				fileToWrite.write(",");
				fileToWrite.write(String.valueOf(anchorSource.getY()/5.));
				fileToWrite.write(") ");
				fileToWrite.write("(");
				fileToWrite.write(String.valueOf(anchorDest.getX()/5.));
				fileToWrite.write(",");
				fileToWrite.write(String.valueOf(anchorDest.getY()/5.));
				fileToWrite.write(") \n");
				if(je.getTransitDots().size()>2) {

					for(Point2D p : je.getTransitDots()) 
						if(!p.equals(je.getTransitDots().get(0)) 
								&& !p.equals(je.getTransitDots().get(je.getTransitDots().size()-1))){
						fileToWrite.write("<BEND> ");
						fileToWrite.write("(");
						fileToWrite.write(String.valueOf(p.getX()/5));
						fileToWrite.write(",");
						fileToWrite.write(String.valueOf(p.getY()/5));
						fileToWrite.write(") \n");
						
					}
				}
				
				fileToWrite.write("</EDGE>\n");
			}
			
			fileToWrite.write("</EDGELIST>\n");
			fileToWrite.write("</DRAWSECTION>\n");
			
			fileToWrite.close();
			
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	/*
	 * Get the position of the anchor point of the source node
	 */
	private Point2D.Float getAnchorForSource(JEdge je){
		Point2D.Float result=new Point2D.Float();
		result.setLocation(0,0);
		float currentHeight = je.getSource().getCurrentHeight()/2;
		float currentWidth = je.getSource().getCurrentWidth()/2;
		
		Point2D.Float last=je.getTransitDots().get(0);
		Point2D.Float prelast=je.getTransitDots().get(1);
		
		if(last.getX()==prelast.getX()){
			if(!(last.getX()==je.getSource().getPoint().getX())){
				if(last.getY()<je.getSource().getPoint().getY())
					currentHeight = -currentHeight;
				result.setLocation(-(je.getSource().getPoint().getX()-last.getX()),
						currentHeight);
			}
				
		}
		else if(last.getY()==prelast.getY()){
			if(!(last.getY()==je.getSource().getPoint().getY())){
				if(last.getX()<je.getSource().getPoint().getX())
					currentWidth = -currentWidth;
				result.setLocation(currentWidth,
						-(je.getSource().getPoint().getY()-last.getY()));
			}
		
		}
		return result;
	}
	/*
	 * Get the position of the anchor point of the destination node
	 */
	private Point2D.Float getAnchorForDest(JEdge je){
		Point2D.Float result=new Point2D.Float();
		result.setLocation(0,0);
		float currentHeight = je.getDest().getCurrentHeight()/2;
		float currentWidth = je.getDest().getCurrentWidth()/2;
		
		Point2D.Float last=je.getTransitDots().get(je.getTransitDots().size()-1);
		Point2D.Float prelast=je.getTransitDots().get(je.getTransitDots().size()-2);
		
		if(last.getX()==prelast.getX()){
			if(!(last.getX()==je.getDest().getPoint().getX())){
				if(last.getY()<je.getDest().getPoint().getY())
					currentHeight = -currentHeight;
				result.setLocation(-(je.getDest().getPoint().getX()-last.getX()),
						currentHeight);
			}
				
		}
		else if(last.getY()==prelast.getY()){
			if(!(last.getY()==je.getDest().getPoint().getY())){
				if(last.getX()<je.getDest().getPoint().getX())
					currentWidth = -currentWidth;
				result.setLocation(currentWidth,
						-(je.getDest().getPoint().getY()-last.getY()));
			}
		
		}
		return result;
	}
}	
