#include <list>
#include <map>
#include <set>
#include <utility>
#include <string>


#include <GDT/rm3_undi_graph.h>
#include <GDT/rm3_draw_undi_graph.h>


using std::list;
using std::map;
using std::set;
using std::pair;
using std::make_pair;
using std::string;



/* An instance of Orthobox allows you to compute an orthogonal drawing
   of a graph. The drawing can be customized by applying some
   constraints.  Some of the nodes may be put on the box enclosing the
   drawing, no other node is placed outside such box.  The order of
   the nodes on the box is arbitrary choosen.

   Edges are considered oriented.  All dimensions are expressed in
   grid units. Nodes may have or both dimensions equal to zero.

   Refers to the GDT implementation of the simple podevsnef drawing
   standard for features of the generated drawings.

   Methods of this interface are partitioned into four classes.

   INPUT METHODs: prepare the graph and the constraints which are the data for to be drawn. 
   COMMIT METHODs: start computation
   OUTPUT METHODs: retrieve the information for the drawing.
   DEBUG METHODs: permits to interact with the internal state of the object

   Usage prototcol: (1)First call the input methods (2) call the
   commit method, and (3) call the output methods.  */


class Orthobox{

private: 
       undi_graph ug;

       orth_plan_undi_graph *opug;
       bool dir_is_valid; // this is true if the following info are valid
       int ref_node; // this encode the reference border step, by means of edge id and starting node id.
       int ref_edge;
       heading_type heading_dir; // this must is set according to the reference border step

       draw_undi_graph *dug;
       
       gdt::gdtmap<int, int> node_width;
       gdt::gdtmap<int, int> node_height;

       gdt::gdtlist<int> border_edges; //the set of the id's of the edge of the border

       set<int> on_the_box;  // id's of the nodes on the border
  
public :

       class IdError: public string
         {
         public: IdError(const char*x): string(x){}
         };

       Orthobox();
       ~Orthobox();


  /*  INPUT METHOD Inserts a node in the graph to be drawn. It is possible to
      specify width and height of the nodes in grid units.  'id'
      should be a unique identifier for each node.  Duplicated id's
      gives unpredictable results. */

       void insertNode(int id, int witdh, int height);

  /*  INPUT METHOD Inserts an edge in the graph to be drawn.  Edge are considered
      oriented. Orientation is kept unchanged and is maningful in
      retrieving the bends coordinates.  'id' should be a unique
      identifier for each edge.  Duplicated id's gives unpredictable
      results.  */

	void insertEdge(int id, int nodeIdSource, int nodeIdTarget);
    
      /* INPUT METHOD Constrains the node specified by means of idNode
         to be placed on the box that represent the boundary of the
         drawing.   */

	void putOnTheBox(int idNode);
    
	/* COMMIT METHOD
	  Computes the drawing and returns it in the draw_undi_graph ug.
	 */
	void computeDrawing(draw_undi_graph& ug);


        /* OUTPUT METHOD Returns the id's of the edges that has been
          introduced by the drawing engine in order to represent the
          boundary of the drawing.  Such edges may be treated as regular
          edges and the id's of contained in the returned list may be
          used as argument of getEdgeCoordinate(int edgeId). Such
          edges are connected to the nodes that are constrained to be
          on the border by means of setSideConstraints().  Note that
          the border frame has also four nodes at the corner. However,
          the edges attach to the center of such nodes, hence the id
          of such nodes are not needed.  */

        gdt::gdtlist<int> boxEdges(); 

	
	 /* OUTPUT METHOD Gets coordinates of the center of the node
	     specified by nodeId.  */
        pair<double,double> getNodeCoord(int nodeId);
    
	/* OUTPUT METHOD Gets coordinates of the bend points for each
	 edge (specified by edgeId).  */

	gdt::gdtlist< pair<double,double> > getEdgeCoord(int edgeId);

    

  //----------------------------

        /*  DEBUG METHOD
         *      save the input graph in GDT format 
         *      plus parameters in a distinct file
         *
         *      this method may be called before drawing
         *      computation to freeze input that make the system crash
         */
        void saveInput(const char *graphfile, const char *paramsfile);


        /*  DEBUG METHOD
         *      save in GDT format the computed drawing 
         */
        void saveDrawing(const char *filename);




};
