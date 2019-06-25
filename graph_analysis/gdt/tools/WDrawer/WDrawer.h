#include<sstream>
#include<GDT/rm3_undi_graph.h>

/* ------------------------------------------
   WDrawer class include file
   Author: Walter Didimo
   Date: July 2003
   Revision: August 2004 (porting for gdt4b)
   ------------------------------------------ */ 

/* WARNING: The current implementation of this class does not take
into account labels on edges. Information about edge labels will be 
correctly stored but ignored throughout the execution of the drawing algorithms. 
*/

class WDrawer
	{
	// ---------
	// variables
	// ---------
	
	private:  
	
	/* Informs if the current graph is changed from the last drawing
	*/
	bool graphChanged;
	
	/* represents the whole graph 
	*/
	undi_graph ug;
	
	/* represents the graph with dummy cluster-nodes;
	each cluster-node groups all non-expansible degree-one nodes adjacent
	to the same node.
	*/
	undi_graph clustUg;
	
	/* represents the orthogonal shape of clustUg
	*/
	orth_plan_undi_graph *clustOpug; 
		
	/* representing the drawing of ug
	*/
	draw_undi_graph dug;
	
	/* represents the drawing of (*clustOpug)
	*/
	draw_undi_graph *clustDug;

	/* stores information about node widths	
	*/
	gdt::gdtmap<int,int> nodeWidth;
	
	/* stores information about node heights
	*/
	gdt::gdtmap<int,int> nodeHeight;
	
	/* stores information about the possibility of expanding a node;
	if a node cannot be expanded, the drawing algorithm will 
	ignore its dimensions, and assumes them equal to zero.
	*/
	gdt::gdtmap<int,bool> nodeExpansionAllowed;
	
	/* clusterOfNode[i] informs what is the identifier of the 
	node representing the cluster of node i in graph clustUg; 
	it must be either the identifier of a dummy cluster-node 
	or -1 if node i is not inside a dummy cluster-node. 
	The domain is the set of identifiers of nodes of clustUg before
	removing all non-expansible degree-one nodes. In paricular,
	clusterOfNode[i] > 0 only if i is also an identifier of a node
	of ug.
	*/
	gdt::gdtmap<int,int> clusterOfNode;
	
	/* isDummyCluster[i] is true if node i is a dummy cluster-node.
	The domain is the set of identifiers of nodes of clustUg.
	*/
	gdt::gdtmap<int,bool> isDummyCluster;		
	
	/* informs what is the heading of the edge incident on a dummy cluster-node;
           headingOfEdgeOnCluster[i] can be in the set {south, north, east, west}
	*/
	gdt::gdtmap<int,heading_type> headingOfEdgeOnCluster;
			
	/* 
	edgeLabel[i] is true if there is a label associated with the edge having i as identifier 	
	edgeLabelWidth[i] gives the width of the label associated with the edge having i as identifier 	
	edgeLabelHeight[i] gives the height of the label associated with the edge having i as identifier
        */
	gdt::gdtmap<int,bool> edgeLabel;
	gdt::gdtmap<int,int>  edgeLabelWidth;
	gdt::gdtmap<int,int>  edgeLabelHeight;	      
	
	// ----------------
	// public interface
	// ----------------
	
	public:
	
	/* constructor: create a new WDrawer object, representing an empty graph	
	*/
	WDrawer ();
	
	/* denstrucor: deallocate dynamic variables
	*/
	~WDrawer ();
	
	/* Creates a new node with identifier id.
        The new node can be expanded as default.
        PRECONDITIONS: 
	  - there is no other vertex with id as identifier 
	  - identifier id must be non-negative
        */ 
        void newNode (int id);

        /* 
        If exp=true, node nodeId can be expanded (default).
        Else, this node cannot be expanded, that is, its dimensions will be the minimum.   
        PRECONDITIONS: there exists a node with identifier nodeId.
        */ 
        void setNodeExpansion (int nodeId, bool exp);

        /* 
        Creates a new edge with identifier id and that has
        end-vertices with identifiers fromId ed toId, respectively.
        PRECONDITIONS: - fromId and toId identify vertices already created
                       - there is no other edge with id as identifier 
		       - identifier id must be non-negative
        */ 
        void newEdge (int id, int fromId, int toId);	

        /*
        Assignes new dimensions for node nodeId
        PRECODITIONS: there exists a node with identifier nodeId      		     
        */	
        void setNodeDim (int nodeId, int w, int h);

     
	/* 
	Creates a new edge-label with height h and width w, which is 
	associated with the edge having identifier edgeId
	PRECONDITIONS: there exists an edge with identifier edgeId	   
	*/

	void newEdgeLabel (int edgeId, int w, int h);


	/* 
	Updates the drawing for the current labeled graph
	*/
	void updateDrawing();


	/* Provide information about the position and the size of the node having 
	identifier nodeId. The node is represented as a rectangle.
	Namely, x and y are the coordinates of the center
	of the node, and w and h are its width and its height.
	PRECONDITIONS: there exists a node with identifier nodeId 	
	*/
	void getNodeInfo (int nodeId, double& x, double& y, double& w, double& h);

	/* Provide information about the polyline representing the edge with identifier
	edgeId. Namely, numberOfPoints is the number of points of the polyline, 
	x[] and y[] are the arrays of x- and y-coordinates of the sequence of points of the
	polyline, xL and yL are the coordinates of the center of the label associated with the
	edge (if any), and wL and hL are the width and the height of such a label. 
	The sequence of points starts from the end-node startingNode.
	PRECONDITIONS: there exists an edge with identifier edgeId and its end-node with identifier
	startingNode
	*/
	void getEdgeInfo (int edgeId, int startingNode, int& numberOfPoints, double*& x, double*& y, 
	                  double& xL, double& yL, double& wL, double& hL);

	/* Returns the number of points of edgeId 
	PRECONDITIONS: there exists an edge with identifier edgeId
	*/
	int getEdgeNumberOfPoints (int edgeId);

	/* save the current drawing in a file with the specified name 
	*/
	void saveDrawing (std::string name);
	
	 
	/* print the drawing information on the standard output
	*/
	void printDrawing ();


	/** 
	Returns the point of edge edgeId that is in position pos starting form node nodeId. 
	PRECONDITIONS: (i) there exists an edge with identifier edgeId
     		       (ii) node nodeId exists and is an end-vertex of edge edgeId.
		       (iii) pos must range from 1 to getEdgeNumberOfPoints (edgeId).
	*/

	void getEdgePoint (int pos, int edgeId, int nodeId, double& x, double& y);

	// ---------------
	// private methods
        // ---------------
	
	private:

	/* modify the current drawing dug in order to reinsert all vertices
	of each cluster 
	*/
	void expandClusters ();

	/* Returns the list of nodes adjacent to v such that: (i) have degree one, (ii) are not
	expansible.
	*/	
	gdt::gdtlist<gdtnode> adjDegOneNotExpansibleNodes (gdtnode v);
			
	/* Re-Compute clustUg and clustOpug 	
	*/		
	void computeClusteredGraphs ();
	
	/* Set dimension constraints for those nodes that are not dummy cluster-nodes
	*/
	void setConstOnNonClusterNodes ();
	
	/* Set dimension and edge pins constraints for those nodes that are dummy cluster-nodes 
	*/
        void setConstOnClusterNodes ();		
	
	/* Compute and set mapping HeadingOfEdgeOnCluster on dummy cluster-node idu
	*/
	void computeHeadingOfEdgeOnCluster(dime_orth_plan_undi_graph& tempDopug, int idu);
	
       };

	
	
	
