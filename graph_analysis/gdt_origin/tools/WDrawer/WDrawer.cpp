#include <sstream>
#include <GDT/rm3_draw_undi_graph.h>
#include "WDrawer.h"	
#include <math.h>

/* -----------------------------------------
   WDrawer class source file
   Author: Walter Didimo
   Date: July 2003
   Revision: August 2004 (porting for gdt4b)
   ----------------------------------------- */ 

/* WARNING: The current implementation of this class does not take
into account labels on edges. Information about edge labels will be 
correctly stored but ignored through the execution of drawing algorithms. 
*/


WDrawer::WDrawer (){
 clustOpug = NULL;
 clustDug = NULL;
 graphChanged = false;  
}

WDrawer::~WDrawer (){
 if (clustOpug)
    delete(clustOpug);
 if (clustDug)	
    delete(clustDug);
 clustOpug = NULL;
 clustDug = NULL;   
}


void WDrawer::newNode (int id){
  if (id < 0){
    std::cout << "\nError: identifier id must be non-negative" << std::flush;
    exit (1);
  }
  ug.new_node (id);
  nodeWidth[id] = 0;
  nodeHeight[id] = 0;
  nodeExpansionAllowed[id] = true;
  graphChanged = true;
}

void WDrawer::setNodeExpansion (int nodeId, bool exp){
  if (!ug.find_node (nodeId)){
        std::cout << "\nNode " << nodeId << " does not exist!" << std::flush;
  	exit (1);	
  }
  if (nodeExpansionAllowed[nodeId]!=exp){
    nodeExpansionAllowed[nodeId] = exp;
    graphChanged = true;
  }
}

void WDrawer::newEdge (int id, int fromId, int toId){
  if (id < 0){
    std::cout << "\nError: identifier id must be non-negative" << std::flush;
    exit (1);
  }
  if ( (!ug.find_node (fromId)) || (!ug.find_node (toId)) ){
        std::cout << "\nInvalid end-nodes for edge " << id << std::flush;
  	exit (1);	
  }
  gdtnode from = ug.find_node (fromId);
  gdtnode to = ug.find_node (toId);
  ug.new_edge (from, to, id);
  edgeLabel[id]= false; 
  graphChanged = true;
}

void WDrawer::setNodeDim (int nodeId, int w, int h){
  if (!ug.find_node(nodeId)){
        std::cout << "\nNode " << nodeId << " does not exist!" << std::flush;
  	exit (1);	
  }
  if (w < 0 || h < 0){
        std::cout << "\nInvalid node dimensions; they must be non-negative" << std::flush;
	exit (1);
  }
  nodeWidth[nodeId] = w;
  nodeHeight[nodeId] = h;  
}

void WDrawer::newEdgeLabel (int edgeId, int w, int h){
  if (!ug.find_edge(edgeId)){
        std::cout << "\nEdge " << edgeId << " does not exist!" << std::flush;
  	exit (1);	
  }
  if (w < 0 || h < 0){
        std::cout << "\nInvalid edge label dimensions; they must be non-negative" << std::flush;
	exit (1);
  }
  edgeLabel[edgeId]=true;
  edgeLabelWidth[edgeId] = w;
  edgeLabelHeight[edgeId] = h;
  std::cout << "\nWarning: this setting will be ignored by the current implementation, since it is not yet supported " << std::flush;
}

void WDrawer::updateDrawing(){
  // ---------------------------------
  // Step 1
  // if the graph is changed, then
  // re-compute clustUg, clustOpug, 
  // and set constraints on dummy 
  // cluster-nodes of clustOpug;
  // else go to Step2.
  // ---------------------------------
  
  if (graphChanged){
     computeClusteredGraphs (); // private method
     setConstOnClusterNodes (); // private method   
  }
    
  // ---------------------------------
  // Step 2 
  // set constraints on non 
  // cluster-nodes of clustOpug.
  // ---------------------------------
    
  setConstOnNonClusterNodes (); // private method
    
  // ---------------------------------
  // Step 3 
  // compute clustDug by applying a 
  // compaction algorithm on clustOpug
  // ---------------------------------
  
  clustDug = new draw_undi_graph (*clustOpug,SLOW_REGULAR_COMPACTION_2_REFINED);
  //clustDug->write ("clustDug.gdt"); //saving for debug
  
  // ---------------------------------
  // Step 4
  // compute dug from clustDug
  // ---------------------------------
  
  dug = *clustDug; // initialize the final drawing as the copy the clustered drawing
  
  // remove all dummy edges previously added 
  // in order to connect the graph
  //
  gdt::gdtlist<gdtedge> dummyEdges;
  gdtedge e;
  forall_edges (e,dug)
  	if (dug.is_marked (e,RM3_ADDED_BY_MAKE_CONNECTED))
		dummyEdges.append(e);
  forall (e,dummyEdges)
  	dug.del_edge (e);
  //
  
  // rescale the drawing
  //
  double k = dug.scale();
  dug.rescale(1); // rescale the coordinates of the drawing
  gdtnode u;
  forall_nodes (u,dug){
     dug.set_width (u,dug.width(u)/k);
     dug.set_height (u,dug.height(u)/k);
  }  	  
  //saveDrawing ("quasi-final-dug.gdt"); // saving for debug
  expandClusters (); // private method    
  //saveDrawing ("final-dug.gdt");  // saving for debug
}


// public 

void WDrawer::getNodeInfo (int nodeId, double& x, double& y, double& w, double& h){
    gdtnode u = dug.find_node (nodeId);
    if (!u){
      std::cout << "\nERROR: node not found " << std::flush;
      exit (1);
    }
    x = dug.center(u).xcoord();
    y = dug.center(u).ycoord();
    w = dug.width(u);
    h = dug.height(u);
}

int WDrawer::getEdgeNumberOfPoints (int edgeId){
   gdtedge e = dug.find_edge (edgeId);
   if (!e){
     std::cout << "\nERROR: edge not found" << std::flush;
     exit (1);
   }   
   gdtnode u = dug.source(e);
   int n=dug.bends(e,u).size() + 2; // bends + 2 anchor points   
   return n;
}
	
void WDrawer::getEdgeInfo (int edgeId, int startingNode, int& numberOfPoints, double*& x, double*& y, 
	                  double& xL, double& yL, double& wL, double& hL){

   gdtedge e = dug.find_edge (edgeId);
   if (!e){
     std::cout << "\nERROR: edge not found" << std::flush;
     exit (1);
   }   
   gdtnode u,v;  
   if (dug.id(dug.source(e)) == startingNode)
   	u = dug.source(e);
   else if (dug.id(dug.target(e)) == startingNode)
   	u = dug.target(e);
   else {
     std::cout << "\nERROR: invalid pair edge, node" << std::flush;
     exit (1);
   }   
   v = dug.opposite(u,e);

   gdt::gdtlist<gdt::gdtpoint> P = dug.bends(e,u);
   gdt::gdtpoint au = dug.anchor_point (e,u);
   gdt::gdtpoint av = dug.anchor_point (e,v);
   P.push (au);
   P.append (av);  
   numberOfPoints = P.size();			    
   gdt::gdtpoint p;
   int i=0;  
   x = new double[numberOfPoints];
   y = new double[numberOfPoints];
   forall (p,P){
     x[i]=p.xcoord();
     y[i]=p.ycoord();
     i++;
   }      
   xL = -1;
   yL = -1;
   wL = -1;
   hL = -1;   			  
}


void WDrawer::getEdgePoint (int pos, int edgeId, int nodeId, double& x, double& y){
   int numberOfPoints;
   double *xA, *yA;
   double xL,yL,wL,hL; // not used
   getEdgeInfo (edgeId, nodeId, numberOfPoints, xA, yA, xL, yL, wL, hL);
   x = xA[pos-1];
   y = yA[pos-1];
}


void WDrawer::saveDrawing (std::string name){
  dug.write (name);  
} 


void WDrawer::printDrawing(){
  std::cout << "<Nodes>\n" << std::flush;
  gdtnode u;
  forall_nodes (u,dug){
    double x = dug.center(u).xcoord();
    double y = dug.center(u).ycoord();
    double w = dug.width(u);
    double h = dug.height(u);
    std::cout << dug.id(u) << " " << x << " " << y << " " << w << " " << h << "\n" << std::flush;
  }  
  std::cout << "</Nodes>\n" << std::flush;
  
  std::cout << "<Edges>\n" << std::flush;
  gdtedge e;
  forall_edges (e,dug){
    int edgeId = dug.id(e);
    int nodeId = dug.id(dug.source(e));
    int n = (*this).getEdgeNumberOfPoints (edgeId);
    std::cout << edgeId << " " << nodeId << " ";
    double x,y;
    for (int i=1; i<=n; i++){	 
	(*this).getEdgePoint (i, edgeId, nodeId, x, y);
	std::cout << x << " " << y << " ";	
    }
    std::cout << "\n" << std::flush;   
  }  
  std::cout << "</Edges>\n" << std::flush;    
}


// ---------------
// private methods
// ---------------

void WDrawer::expandClusters (){
  gdt::gdtmap<int,gdt::gdtpoint> upLeftCorner; // the upper left corner of each cluster-node
  gdt::gdtmap<int,int> reservedRow, reservedCol; // the row and column indexe reserved for each cluster-node       
  gdt::gdtmap<int,int> currRow, currCol; // current row and column index for each cluster-node
  const double dim = 0.25;
  gdt::gdtlist<gdtnode> toBeRemoved; // list of dummy cluster nodes 
  
  // initialize mappings
  //
  gdtnode u;
  forall_nodes (u,dug)
    if (isDummyCluster[dug.id(u)]){      
        toBeRemoved.append(u);
	int idu = dug.id(u);	
	gdt::gdtpoint cu = dug.center(u);		
	upLeftCorner[idu]= gdt::gdtpoint (cu.xcoord()-dug.width(u)/2, cu.ycoord()+dug.height(u)/2); 
	switch (headingOfEdgeOnCluster[idu]){
	  case south:
        	    reservedRow[idu] = nodeHeight[idu];
   		    reservedCol[idu] = nodeWidth[idu]/2; // nodeWidth is even				 
        	    break;
	  case north:	  	    
      		    reservedRow[idu] = 0;
		    reservedCol[idu] = nodeWidth[idu]/2; // nodeWidth is even
		    break;
	  case east: 	  	  
        	    reservedRow[idu] = nodeHeight[idu]/2; // nodeHeight is even
		    reservedCol[idu] = nodeWidth[idu];
		    break;
	  case west:	  	    
        	    reservedRow[idu] = nodeHeight[idu]/2; // nodeHeight is even
		    reservedCol[idu] = 0;
		    break;
	  default: 
      		    std::cout << "\nError: invalid heading for " << idu << std::flush;
		    exit(1);
		    break;		    
	} // end switch    	
		
	currRow[idu] = 0;
	currCol[idu] = 0;
			
	// redefine dimensions and center of the cluster node
	//
	gdt::gdtpoint newCenter = gdt::gdtpoint (upLeftCorner[idu].xcoord() + dim + reservedCol[idu],
                        	 upLeftCorner[idu].ycoord() - dim - reservedRow[idu]);
	gdtnode dug_u = dug.find_node (idu);
	dug.set_width (dug_u,dim/5);
	dug.set_height (dug_u,dim/5);
	dug.set_center (dug_u,newCenter);			   	
	dug.set_anchor_point (dug.first_adj_edge(dug_u),dug_u,dug.center(dug_u));
	dug.set_shape (dug_u,ELLIPSE);	
	dug.set_label_position (dug_u, gdt::gdtpoint (newCenter.xcoord () - dim/5, 
	                                      newCenter.ycoord () + dim/5));
			
		      
    } // end if (and end for)
  
  // reinsert the one-degree node inside the corresponding cluster-node
  //  
  gdtnode v;
  forall_nodes (v,ug){
    int idu = clusterOfNode[ug.id(v)]; // cluster of node v, if any
    bool validPosition = false;
    if (idu >= 0){
    	while (!validPosition){	
	  if (currRow[idu]!=reservedRow[idu] &&
	      currCol[idu]!=reservedCol[idu] &&
	      currRow[idu]<=nodeHeight[idu]  &&
	      currCol[idu]<=nodeWidth[idu] )
	      validPosition = true;
	  else{   
	    if (currRow[idu]==reservedRow[idu])
      	       currRow[idu]++;
            if (currCol[idu]==reservedCol[idu])
               currCol[idu]++;  	  
	    if (currCol[idu]>nodeWidth[idu]){
               // start with a new row
               currCol[idu] = 0; 
	       currRow[idu]++;
            }  
	    if (currRow[idu]>nodeHeight[idu]){      
               std::cout << "\nError: invalid row number " << idu << std::flush;
	       exit(1);
	    }
	  } 
	   
        }
    
      // reinsert and place the vertex 	 	 
      //           
      gdtedge ev = ug.first_adj_edge (v); // the only edge (v has degree one)      
      gdt::gdtpoint cv = gdt::gdtpoint (upLeftCorner[idu].xcoord() + dim + currCol[idu],
                        upLeftCorner[idu].ycoord() - dim - currRow[idu]);
      gdtnode dug_v = dug.new_node (cv,ug.id(v));
      
      gdtnode dug_u = dug.find_node(idu); // cluster node in dug (which has degree one)
      gdtedge dug_ewu = dug.first_adj_edge (dug_u); // edge incident on dug_u
      gdtnode dug_w = dug.opposite(dug_u,dug_ewu);  
      gdt::gdtlist<gdt::gdtpoint> b_w_v = dug.bends (dug_ewu,dug_w);
      b_w_v.append(dug.center(dug_u));
      gdtedge dug_ev = dug.new_edge (dug_w,dug_v,b_w_v,ug.id(ev));      
      dug.set_anchor_point (dug_ev,dug_w,dug.anchor_point(dug_ewu,dug_w)); 
      gdt::gdtpoint p1 = dug.center(dug_u);
      gdt::gdtpoint p2 = dug.center(dug_v);
      double m = (p2.ycoord()-p1.ycoord())/(p2.xcoord()-p1.xcoord());
      double q = p1.ycoord() - m*p1.xcoord();
      double x0 = (p1.xcoord() + p2.xcoord())/2;
      double y0 = m*x0 + q;
      dug.set_label_position(dug_ev,gdt::gdtpoint(x0,y0));     
            
      currCol[idu]++;
    }// end if 
  }//end forall
  
  // remove dummy cluster nodes
  //
  forall (u,toBeRemoved)
  	dug.del_node(u);
}


gdt::gdtlist<gdtnode> WDrawer::adjDegOneNotExpansibleNodes (gdtnode v){
  gdt::gdtlist<gdtnode> L;
  gdtedge e;
  forall_edges_adjacent_node (e,v,clustUg){
  	gdtnode w = clustUg.opposite(v,e);
	if (clustUg.degree(w) == 1 && !nodeExpansionAllowed[clustUg.id(w)])
		L.append(w);
  }
  return L;
}

void WDrawer::computeClusteredGraphs (){      
  // ---------------
  // compute clustUg
  // ---------------
  gdtnode u;
  clustUg.clear();
  clustUg = ug;            
  forall_nodes (u,clustUg){
    isDummyCluster[clustUg.id(u)] = false;
    clusterOfNode[clustUg.id(u)] = -1; 
  }
  forall_nodes (u,ug){        
    if (ug.degree(u) > 1){
      gdtnode v = clustUg.find_node(ug.id(u));
      gdt::gdtlist<gdtnode> L = adjDegOneNotExpansibleNodes (v); // private method	
      if (L.size() > 1) {
	// add a dummy cluster-node and connect it to v
	//
	gdtnode vClust = clustUg.new_node ();
	int idvc = clustUg.id(vClust);
	isDummyCluster[idvc] = true;
	clusterOfNode[idvc] = -1; 
	nodeWidth[idvc] = 0;
	nodeHeight[idvc] = 0;
	headingOfEdgeOnCluster[idvc] = undefined_heading;
	clustUg.new_edge (v,vClust);	
	// remove all vertices that belong to this cluster, that is those in L
	//
	gdtnode w;
	forall (w, L){	  
	    int idw = clustUg.id(w);
	    clustUg.del_node(w);
	    clusterOfNode[idw]=idvc;	 
	}
      } 
    }
  }
  // -----------------
  // compute clustOpug
  // -----------------
  if (clustOpug){
     delete(clustOpug); // memory deallocation
     clustOpug = NULL;
  }
  
  // make the graph clustUg connected if needed
  //
  gdt::gdtlist<gdtedge> dummyEdges = clustUg.make_connected();
    
  // compute a planar embedding for clustUg
  //
  plan_undi_graph pugTemp (clustUg);	 
  clustOpug = new orth_plan_undi_graph (pugTemp, PLAN_ORTH_OPTIMAL);	 
}


void WDrawer::setConstOnNonClusterNodes (){
  // remove all previous constraints 
  // on the dimensions of non cluster-nodes
  // and set the current constraints
  gdtnode v;
  forall_nodes (v,*clustOpug){
    int idv = clustOpug->id(v);
    gdtnode u = ug.find_node (idv); 
    if (u && nodeExpansionAllowed[idv]){
       clustOpug->del_constraints_type_involving_node (NODE_WIDTH,v);
       clustOpug->del_constraints_type_involving_node (NODE_HEIGHT,v);
       clustOpug->new_constraint_node_width (v,nodeWidth[clustOpug->id(v)]);
       clustOpug->new_constraint_node_height(v,nodeHeight[clustOpug->id(v)]);
    }    
  }
}

void WDrawer::setConstOnClusterNodes (){
  // count the number of nodes belonging 
  // to each dummy cluster-node
  //  
  gdt::gdtmap<int,int> numberOfNodesOfCluster;
  gdtnode u;
  forall_nodes (u,ug){
    int idu = ug.id(u);
    if (clusterOfNode[idu] >= 0)
       numberOfNodesOfCluster[clusterOfNode[idu]]++;
  }
  // compute a temporary dime-orth representation to fix an orientation;
  // the final drawing will have the same orientation
  //
  dime_orth_plan_undi_graph tempDopug (*clustOpug,FAST_COMPACTION);
    
  // for each dummy cluster-node u of clustOpug
  // compute two integer numbers, d1 and d2, that 
  // represent the two dimensions of u in terms of grid points; 
  // they are computed as follows:
  // let n be the number of nodes in u
  // c1 = ceil(sqrt(n/2)) 
  // c2 = min{(c1-1),c1,(c1+1) : 2*(c1*c2) >= n}
  // d1 = (c1*2)+1 
  // d2 = c2+1
  
  // the n nodes of u will be arranged into two rectangles,
  // each of dimensions (c1*c2) = (d1/2)*(c2-1)
  // observer that d1 is always an odd number.
  
  int c1,c2;
  int d1,d2;
  forall_nodes (u,clustUg){
    int idu = clustUg.id(u);    
    if (isDummyCluster[idu]){   
      int n = numberOfNodesOfCluster[idu]; 
      computeHeadingOfEdgeOnCluster(tempDopug, idu);
      c1 = (int)ceil(sqrt((double)n/2));
      if (2*c1*(c1-1) >= n)
      	c2 = (c1-1);
      else if (2*c1*c1 >=n)
      	c2 = c1;
      else
      	c2 = c1+1;      
		            	
      // check of correctness 
      if (2*c1*c2 < n){
          std::cout << "\nError: dimensions not sufficient for cluster" << idu << std::flush;
          exit (1);	
      }
      d1 = (2*c1)+1;
      d2 = c2+1;
      	
      // set constraints on the width, the height, and the edge pin of node u according
      // to the values of HeadingOfEdgeOnCluster[idu], d1 and d2
      //
      gdtnode v = clustOpug->find_node(idu);
      gdtedge ev = clustOpug->first_adj_edge (v); // v has degree one  
      
      // deletion is not needed, because these contraints are computed just
      // immediately after the clustered graphs have been computed
      //
      //clustOpug->del_constraints_type_involving_node (NODE_WIDTH,v);
      //clustOpug->del_constraints_type_involving_node (NODE_HEIGHT,v);
      //clustOpug->del_constraints_type_involving_node (EDGE_ATTACHMENT_WRT_PREVIOUS_CORNER,v);    
      
      if (headingOfEdgeOnCluster[idu] == south || 
          headingOfEdgeOnCluster[idu] == north){
            nodeWidth[idu] = d1-1;
	    nodeHeight[idu] = d2-1;	        	    
	    clustOpug->new_constraint_edge_attachment_wrt_previous_corner(v,ev,nodeWidth[idu]/2);
      }
      else{ //east or west
            nodeWidth[idu] = d2-1;
	    nodeHeight[idu] = d1-1;
	    clustOpug->new_constraint_edge_attachment_wrt_previous_corner(v,ev,nodeHeight[idu]/2);
      }            	    
      clustOpug->new_constraint_node_width (v,nodeWidth[idu]);
      clustOpug->new_constraint_node_height(v,nodeHeight[idu]); 
                  
    }
    
  }
}

void WDrawer::computeHeadingOfEdgeOnCluster(dime_orth_plan_undi_graph& tempDopug, int idu){
   gdtnode v = tempDopug.find_node(idu);
   gdtedge e_south = tempDopug.find_edge_leaving_node_with_heading(v,south);
   gdtedge e_north = tempDopug.find_edge_leaving_node_with_heading(v,north);
   gdtedge e_east  = tempDopug.find_edge_leaving_node_with_heading(v,east);
   gdtedge e_west  = tempDopug.find_edge_leaving_node_with_heading(v,west);
   // look for the real edge leaving v (v has degree one)
   //
   if (e_south && !tempDopug.is_marked(e_south,RM3_ADDED_BY_RECTANGULARIZATION)) 
     headingOfEdgeOnCluster[idu] = south;
   else if (e_north && !tempDopug.is_marked(e_north,RM3_ADDED_BY_RECTANGULARIZATION))
     headingOfEdgeOnCluster[idu] = north;
   else if (e_east && !tempDopug.is_marked(e_east,RM3_ADDED_BY_RECTANGULARIZATION))
     headingOfEdgeOnCluster[idu] = east;
   else if (e_west && !tempDopug.is_marked(e_west,RM3_ADDED_BY_RECTANGULARIZATION))
     headingOfEdgeOnCluster[idu] = west;
   if (headingOfEdgeOnCluster[idu] == undefined_heading){
     std::cout << "\nError: edge incident on dummy-cluster node not found: " << idu << std::flush;
     exit (1);
   }
}





/* MAIN - COMMAND SHELL FOR TESTING PURPOSES */

int main (){

  WDrawer wD;
  bool exit = false;
  
  while (!exit){
	std::string command;
	//std::cout << "\n\ncommand>" << std::flush; 
	std::cin >> command;
		
	if (command == "newNode"){
	  int id;
	  std::cin >> id;
	  wD.newNode(id);
	  //std::cout << "\n Node added successfully" << std::flush;	  				
	}
	else if (command == "newEdge"){
	  int id, fromId, toId;
	  std::cin >> id >> fromId >> toId;
	  wD.newEdge(id, fromId, toId);
	  //std::cout << "\n Edge added successfully" << std::flush;	  				
	}		
	else if (command == "setNodeDim"){
	  int id, w, h;
	  std::cin >> id >> w >> h;
	  wD.setNodeDim(id, w, h);
	  //std::cout << "\n Dimensions assigned successfully" << std::flush;
	}
	else if (command =="setNodeExpansion"){
	  int id;
	  std::string exp;
	  std::cin >> id >> exp;	  
	  if (exp == "true"){
	  	wD.setNodeExpansion (id,true);
 	  	//std::cout << "\n Node expansion allowed" << std::flush;
	  }
	  else {
	  	wD.setNodeExpansion (id,false);
	        //std::cout << "\n Node expansion not allowed" << std::flush;	  
	  }
	}
	else if (command == "updateDrawing"){
	  wD.updateDrawing();
	  //std::cout << "\n Drawing updated successfully" << std::flush;
	}
	else if (command == "getEdgePoints"){
	  int edgeId, nodeId;
	  std::cin >> edgeId >> nodeId;
	  int n = wD.getEdgeNumberOfPoints (edgeId);
	  double x,y;
	  std::cout << "\n ";
	  for (int i=1; i<=n; i++){	 
	  	wD.getEdgePoint (i, edgeId, nodeId, x, y);
	  	std::cout << "(" << x << "," << y << ")-";
	  }
	  std::cout << std::flush;
	}	
	else if (command == "getNodeInfo"){
	  int nodeId;
	  std::cin >> nodeId;
	  double x,y,w,h;
	  wD.getNodeInfo (nodeId,x,y,w,h);
	  std::cout << "\n center = (" << x << "," << y << ")" << std::flush;  
	  std::cout << "\n width = " << w << std::flush;
	  std::cout << "\n height = " << h << std::flush;
	}
	else if (command == "saveDrawing"){
	  std::string fileName;
	  std::cin >> fileName;
	  wD.saveDrawing (fileName);
	  //std::cout << "\n Drawing saved successfully" << std::flush;
	}
	else if (command == "printDrawing"){
	  wD.printDrawing();
	}	
	else if (command == "help"){
	  std::cout << "\n List of commands: " << std::flush;
	  std::cout << "\n\t - newNode <id>" << std::flush;
	  std::cout << "\n\t\t add a new node having <id> as identifier" << std::flush;
	  std::cout << "\n\t - newEdge <id> <fromId> <toId>" << std::flush;
	  std::cout << "\n\t\t add a new edge with identifier <id> and that has end-vertices \n\t\t with identifiers <fromId> ed <toId>, respectively" << std::flush;
	  std::cout << "\n\t - setNodeDim <nodeId> <w> <h>" << std::flush;
	  std::cout << "\n\t\t set the with and the height of vertex <nodeId> with <w> and <h>, respectively" << std::flush;		
	  std::cout << "\n\t - setNodeExpansion <nodeId> <possible>" << std::flush;
	  std::cout << "\n\t\t if <possible> is true it allows node <nodeId> to be expanded" << std::flush;		
	  std::cout << "\n\t\t if <possible> is false <nodeId> is not expansible" << std::flush;		
	  std::cout << "\n\t - updateDrawing" << std::flush;	 
	  std::cout << "\n\t\t update the drawing for the current labeled graph" << std::flush;
	  std::cout << "\n\t - getNodeInfo <nodeId>" << std::flush;
	  std::cout << "\n\t\t list all information about <nodeId>" << std::flush;	  
	  std::cout << "\n\t - getEdgePoints <edgeId> <nodeId>" << std::flush;
	  std::cout << "\n\t\t list all points of edge <edgeId> from its end-node <nodeId>" << std::flush;
	  std::cout << "\n\t - saveDrawing <name>" << std::flush;
	  std::cout << "\n\t\t save the current drawing in a file called <name>" << std::flush;
	  std::cout << "\n\t - printDrawing" << std::flush;
	  std::cout << "\n\t\t print the drawing information on the standard output" << std::flush;
	  std::cout << "\n\t - exit" << std::flush;
	  std::cout << "\n\t\t exit from this shell" << std::flush;		  	  
	}	
	else if (command == "exit") exit = true;	
	else std::cout << "\nInvalid command - digit help to get instructions" << std::flush;
     }
     return 0;     
}

