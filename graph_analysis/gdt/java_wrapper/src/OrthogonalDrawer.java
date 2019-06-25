//-----------------------------
//
//   GdtJavaWrapper / Java
//   (c)2005 by INTEGRA Sistemi                               
//   Luigi Del Negro          
//                               
//-----------------------------

package GdtJavaWrapper;

import java.awt.geom.*;

	public
	class 
OrthogonalDrawer 
	extends Drawer
	{
		
		public 
	OrthogonalDrawer
		(Graph g)
		{		
		super(g);
		wrapper.drawOrthogonalGraph(g.getId());
		};
		
		public 
	OrthogonalDrawer
		(OrthogonalGraph g)
		{		
		super(g);
		wrapper.drawOrthogonalGraphUsingOrthogonal(g.getId());
		};

		
		

		public
		Rectangle2D.Double
	getNodeRectangle
		(Node n)
		{			
		int nodeId  = n.getId();
		double x = wrapper.getNodeX(graphId, nodeId);
		double y = wrapper.getNodeY(graphId, nodeId);
		double w = wrapper.getNodeWidth(graphId, nodeId);
		double h = wrapper.getNodeHeight(graphId, nodeId);
		return new Rectangle2D.Double(x,y,w,h);
		};
		
		
		public
		Point2D.Double[] // port on source node, {bend point}, point on target node
	getEdgePolyline
		(Edge e)
		{
		int edgeId     = e.getId();
		int bendsCount = wrapper.getEdgeBendsCount(graphId,edgeId);
		
		Point2D.Double[] polyline;
		polyline = new Point2D.Double[bendsCount+2]; // include initial and final points
		
		for(int i=0; i<bendsCount; i++)
			{	
			double x = wrapper.getEdgeBendPointX(graphId,edgeId,i); 
			double y = wrapper.getEdgeBendPointY(graphId,edgeId,i);
			polyline[i+1] = new Point2D.Double(x,y);
			}
		
		polyline[0]            = getEdgeAnchorSource(graphId,edgeId);
		polyline[bendsCount+1] = getEdgeAnchorTarget(graphId,edgeId);	
		return polyline;
		}
		

		public
		double
	getEdgeLength
		(Edge e)
		{
		return wrapper.getEdgeLength(graphId, e.getId());
		}
		
		private
		Point2D.Double
	getEdgeAnchorSource
		(int graphId, int edgeId)
		{
		double x = wrapper.getEdgeAnchorSourceX(graphId,edgeId);
		double y = wrapper.getEdgeAnchorSourceY(graphId,edgeId);
		return new Point2D.Double(x,y);
		}
		
		
		private
		Point2D.Double
	getEdgeAnchorTarget
		(int graphId, int edgeId)
		{
		double x = wrapper.getEdgeAnchorTargetX(graphId,edgeId);
		double y = wrapper.getEdgeAnchorTargetY(graphId,edgeId);
		return new Point2D.Double(x,y);
		}		
		
	}	


                         
  	
	
	
		






    
