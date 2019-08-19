
				//---------------------------------------------//
				//	 	   blag 		       //
				//					       //
				//	 GDToolkit-application (1999)	       //
				//					       // 
				//					       //
				// (Dip. Informatica e Automazione - Roma Tre) //	
				//---------------------------------------------//



// --------------------------------------------- DOCUMENTATION ------------------------------------------------------
//
// blag is a simple application for applying the main layout algorithms present in the GDToolkit library.
// It takes in input two files:
//
// 1 - argv[1] : a file that is the topology  'ug'  of a graph, in GDToolkit format;
// 2 - argv[2] : a file that specifies the layout algorithm to apply on 'ug' and, eventually, other options
//		 for applying that algorithm. (e.g., choice of external face, constraints, ...).
//
// The algorithm can be chosen in the following set, assuming that the graph is always connected:
// 
//    ---------------------------------------------------------------------------------------------------------------------
//   |       Name        | Code |               Description                |   Preconditions on graphs         | Time-Com. |
//    ---------------------------------------------------------------------------------------------------------------------
//   |                   |      |                                          |                                   |           |
//   | QUICK ORTHOGONAL  |  0   | Orthogonal, bend-min. heur., fix.embed.  | 4-deg.+ biconn.                   |    O(n)   |   
//   |                   |      |                                          |			               |           |
//   | OPTIMAL ORTHOGONAL|  1   | Orthogonal, bend-min. optim., fix.embed. | 		-	               | O(n^2logn)|
//   |                   |      |                                          |			               |           |
//   | SLOW ORTHOGONAL   |  2   | Orthogonal, bend-min. optim., var.embed. | biconn. + planar    	       | O(EXP(n)) |
//   |                   |      |                                          |			               |           |
//   | OPTIMAL UPWARD    |  3   | Quasi-Upward, bend-min optim., fix.embed.| direct.                  	       | O(n^2logn)|
//   |                   |      |                                          |			               |           |
//   | SLOW UPWARD	 |  4   | Quasi-Upward, bend-min optim., var.embed.| biconn. + direct. + bimod. + plan.| O(EXP(n)) |
//   |                   |      |                                          |			               |           |
//   | OPTIMAL UPWARD VI |  5	| Like 3, but with visibility representa.  | direct.			       | O(n^2logn)|
//   |                   |      |                                          |			               |           |
//   | SLOW UPWARD VI	 |  6   | Like 4, but with visibility representa.  | biconn. + direct. + bimod. + plan.| O(EXP(n)) |
//   |                   |      |                                          |			               |           |
//   | POLYLINE		 |  7   | Polyline based on visibility, fix.embed. | biconn.	                       |    O(n)   |
//   |                   |      |                                          |			               |           |
//   | VISIBILITY	 |  8   | Visibility, fix.embed.	           | biconn.	                       |    O(n)   |
//   |			 |	|					   |				       |	   |
//   | TREE CENTERSONS   |  9   | root. tree, nodes cent. w.r. their sons  | acyclic (undirected)	       |    O(n)   |	
//   |			 |	| 					   |				       |    	   |
//   | TREE CENTERSUBTREE|  10  | root. tree, nodes cent. w.r. their subtr.| acyclic (undirected)	       |    O(n)   |
//   |			 |	| 					   |				       |    	   |
//   | PLANAR EMBEDDING  |  11  | Computes a planar embedding of the graph | connected         	       	       |    O(n)   |
//    ---------------------------------------------------------------------------------------------------------------------
//
// Preconditions are checked by the application itself, and an error message will be generated if they are not verified.
// If the computation is correctly executed, a layout will be printed on three different files. These files
// are in "exp", "gdt", and "fig" formats, respectively. Namely, the "exp" format is a very simple format
// that can be used to extract all basic-drawings information (node and bends coordinates, and node
// dimensions) and redraw the graph in any other specific graphical interface. The "gdt" file format is
// the own GDToolkit format. Both the "exp" and the "gdt" formats are documented in gdt's web pages.
// Finally, the "fig" file format is the well known format used by the "xfig" application under Unix/Linux.
// The "fig" format is very useful, because many drivers are freely provided for exporting a fig file to
// most the well known formats, like "ps", "Latex", "gif", and so on.
// The format of the file argv[2], for choosing layout options, is the following:
// 
// [<BOFF VERSION="1.1"/>]
// <BEGIN_OPTIONS>
// 
// <ALGORITHM>	   code_a			</ALGORITHM>
// <CHECKING>	   code_c			</CHECKING>
// <PLANARIZATION> code_p 			</PLANARIZATION>
// <COMPACTION>	   code_co			</COMPACTION>			
// <EXTERNAL_FACE> node_ext_id, edge_ext_id	</EXTERNAL_FACE>
// <TREE_ROOT>	   node_r_id			</TREE_ROOT>
// <CONSTRAINTS>    
//		<BENDS>
//			<ANY>	    <EDGE> edge_id_1  .... <EDGE> edge_id_m2 </ANY>
//			<NONE>	    <EDGE> edge_id_1  .... <EDGE> edge_id_m3 </NONE>
//			<DIRECTION> <DIR>  node_id edge_id <DIR>..	     </DIRECTION>
//		</BENDS>
//
//		<ANGLES>
//			<ANGLE_SWEEP> node_id edge_id angle_sweep	<--- only ver 1.1
//			...
//		</ANGLES>
//
//		<UNCROSSABLE_EDGES>
//			<EDGE> edge_id_1
//			<EDGE> edge_id_2 ..
//		</UNCROSSABLE_EDGES>
//
//		<SAME_FACE_NODES>
//			<NODE> node_id_1 label_1
//			<NODE> node_id_2 label_2
//			....
//		</SAME_FACE_NODES>
//
//		<SAME_FACE_ORDERED_NODES>
//			<SAME_FACE>
//				<NODE> node_id_1 .... <NODE> node_id_n
//			</SAME_FACE>
//			....
//		</SAME_FACE_ORDERED_NODES>
//
// 		<NODE_DIM  NODE_ID="node_id" WIDTH="double_1" HEIGHT="double_2" />     <--- only ver 1.1
//		....
//			
//
// </CONSTRAINTS>
//
// <NODE_DIMENSION> 				<----- only ver 1.0
//		<NODE> idv1 wv1 hv1 		<----- only ver 1.0
//		<NODE> idv2 wv2 hv2		<----- only ver 1.0
//		...				<----- only ver 1.0
//						<----- only ver 1.0
// </NODE_DIMENSION>				<----- only ver 1.0
//
// <END_OPTIONS>
//
//
// Notes about the options-file:
// 
//	  1 - 'code_a'  must be chosen in the set {1,2,3,4,5,6,7,8,9,10} (see above);
//	  2 - 'code_c'  must be chosen in the set {'t', 'l'}, where t=TEST only, l=TEST+LAYOUT;
//	  3 - 'code_p'  must be chosen in the set { 0 (DOES NOT PRESERVE EMBEDDING), 1 (PRESERVES EMBEDDING) };
//	  4 - 'code_co' must be chosen in the set 
//			{ 
//			0 (RECTANGULARIZE FAST), 
//			1 (RECTANGULARIZE FAST AND REFINE),
//			2 (RECTANGULARIZE SLOW),
//			3 (RECTANGULARIZE SLOW AND REFINE),
//			4 (REGULARIZE 	  FAST),
//			5 (REGULARIZE 	  FAST AND REFINE),
//			6 (REGULARIZE 	  SLOW),
//			7 (REGULARIZE 	  SLOW AND REFINE)
//			}
//	       The options 0,1,2, and 3 specify a standard compaction based on rectangularization techniques.
//	       The options 4,5,6, and 7 specify a new compaction based on regularization techniques, 
//	       which works better then the standard one in several cases (especially for biconnected graphs).
//	       A compaction algorithm of type FAST runs in linear time but it takes in consideration only optimizations of the area. 
//	       A compaction algorithm of type SLOW runs in polynomial time but it takes in consideration both optimazions of the area 
//	       and the total edge length.
//	       Finally, the REFINE attribute specifies that a post-processing algorithm will be executed that improves
//	       the drawing in some cases. We suggest to use it for graphs with like-trees substructures. It takes polynomial time.  
//        5 - 'node_ext_id', 'edge_ext_id' specifying that the external face will be assumed as the right face
//	       of the edge 'edge_ext_id', starting from node 'node_ext_id';
//	  6 - 'edge_id_1,..., edge_id_mk' (k=1,..,3) denote the lists of the edges with the specified constraints; 		 
//	    - 'node_id' , 'edge_id' denote that the specified edge can spiralize only from its left-face to its right-face,
//             starting from the specified node;
//	  7 - 'angle_sweep' specify an angle at vertex 'node_id' formed by an edge 'edge_id' (incident
//	       to that vertex) with its right adjacenty edge (possibly itself). Allowed values are {0, 90, 180, 270, 360} 
//	  8 - 'idvk', 'wvk', 'hvk' indicate a node, its width and its height, respectively;
//	  9 - if the selected algorithm does not handle some options or it is in contrast with them, they will be ignored;
//	  10 - the different sections can be put in any order, and one or more of them can be omitted.
//
// ------------------------------------------------------------------------------------------------------------------

#define	UNDEFINED -1	
			// integer constant for undefined parameters
#define error(E)\
{rep_out << "\nERROR: " << E << "\n"; std::cout << "\nERROR: " << E << "\n" << std::flush; exit(1);} 

#define warning(E)\
{rep_out << "\nWARNING: " << E << "\n" << flush; cout << "\nWARNING: " << E << "\n" << flush;}


#include <GDT/rm3_draw_undi_graph.h>
#include <GDT/gdtgeometry.h>
using namespace std;

//-----------------------------
// 	FUNCTION PROTOTYPES
//-----------------------------

	void	
read_option_file_ver_1_0	
	(
	std::ifstream& , 		// input  : input stream 
	std::ofstream& , 		// input  : stream for output report
	undi_graph& ,		// input  : the graph
	gdt::gdtnode_array<double>& ,  	// output : width of each node
	gdt::gdtnode_array<double>& , 	// output : height of each node
	gdt::gdtlist<gdtnode>& 		// output : list of node for which a new dimension is specified
	);

	void	
read_option_file_ver_1_1
	(
	std::ifstream& ifs, 		// input  : input stream 
	std::ofstream& , 		// input  : stream for output report
	undi_graph &		// input  : the graph
	);

	void	
read_option_file_ver_1_2
	(
	std::ifstream& ifs, 		// input  : input stream 
	std::ofstream& , 		// input  : stream for output report
	undi_graph &		// input  : the graph
	);

	bool
write_embedding
	(
	plan_undi_graph& pug, 		// input  : the planar embedding to be printed out
	std::string file_name		// input  : the name of the file that will be written
	);


//------------------------------
//	GLOBAL VARIABLES
//------------------------------

bool	only_test;	// execute only a layout-test if true
int	po;		// planarization options
int	co;		// compaction options
int	alg;		// layout algorithm 
gdtnode	root;		// root node for rooted trees
gdtnode	nr;		// reference node for the external face
gdtedge	er;		// reference edge for the external face
std::string 	BOFF("1.0");	// Blag Option File Format

	int
write_check_error
	(std::string s)
	{
	std::ofstream test_out ("gdt_test_false");
        std::cout << "\n" << s << std::flush;
        test_out << s;
        return 1;
	}


	int
main (int argc, char* argv[])
	{
    	
	undi_graph       ug;			// input graph-topology
	draw_undi_graph* dugp = NULL;		// output graph-layout
	plan_undi_graph* pugp = NULL;
	
	if (argc < 3) {
		std::cout << "\nERROR: SYNTAX ERROR  -  MISSING A PARAMETER!" << "\n" << std::flush; 
		exit(1); 
	}

	std::string graph_file_name   = argv[1];
	std::string options_file_name = argv[2];
    std::string gdt_report_file_name = graph_file_name + ".gdt_report";
    std::ofstream rep_out (gdt_report_file_name.c_str());    	// report file

	
	if (!ug.read(graph_file_name)) error ("FILE-ERROR LOADING " << graph_file_name)
	//if (!ug.is_connected()) error ("Graph must be connected")
	gdt::gdtlist<gdtedge> added_edges;	
	if (!ug.is_connected())
		{
		added_edges=ug.make_connected();
		//cout << "Non-connected graph, added " << added_edges.length() <<  " dummy edges" << endl;
		} 
	ifstream in(options_file_name.c_str());
	if (!in) error ("FILE-ERROR LOADING " << options_file_name)
	
	
	// ------------------------------------------
	// Step 0 - Read and Traduce the option file
	// ------------------------------------------

	cout    << "\nSTEP 0 - Reading Option File ..." << flush;
	rep_out << "\nSTEP 0 - Reading Option File ..." << flush;

	gdt::gdtnode_array<double>	     w (ug.nodes_and_edges(),UNDEFINED);	// width of a node;
	gdt::gdtnode_array<double>	     h (ug.nodes_and_edges(),UNDEFINED);	// height of a node;
	gdt::gdtlist<gdtnode>		     L_node_dimension;				// list of node to assign a new dimension

	only_test = false;
	po   =  0;		// planarization options
	co   =  7;		// compaction options
	alg  =  1;		// layout algorithm 
	root = NULL_NODE;	// root node for rooted trees
	nr   = NULL_NODE;	// reference node for the external face
	er   = NULL_EDGE;	// reference edge for the external face

	string s;

	in >> s;
	if ( s == "<BEGIN_OPTIONS>" )				// is the beginning of Blag Option File Format 1.0
		{
		cout    << "\n       - (1.0 Blag Opt. File Form. recognized)" << flush;
		rep_out << "\n       - (1.0 Blag Opt. File Form. recognized)" << flush;
		read_option_file_ver_1_0(in, rep_out, ug, w, h, L_node_dimension);
		}
	else if ( s == "<BOFF" ) 		 		// is the beginning of Blag Option File Format declaration
		{
		in >> s;
		if( s ==  "VERSION=\"1.1\"/>" ) 		// version 1.1
			{
			BOFF = "1.1";				// setting the global variable
			cout    << "\n       - (1.1 Blag Opt. File Form. recognized)" << flush;
			rep_out << "\n       - (1.1 Blag Opt. File Form. recognized)" << flush;

			read_option_file_ver_1_1(in, rep_out, ug);
			}
		else if( s ==  "VERSION=\"1.2\"/>" ) 		// version 1.1
			{
			BOFF = "1.2";				// setting the global variable
			cout    << "\n       - (1.2 Blag Opt. File Form. recognized)" << flush;
			rep_out << "\n       - (1.2 Blag Opt. File Form. recognized)" << flush;

			read_option_file_ver_1_2(in, rep_out, ug);
			}
		else error("Unrecognized file format: unknown BOFF version declaration");
		}
	else 	error("Unrecognized file format");
		
	in.close();

	cout    << "\n       - Normally Executed" << flush;
	rep_out << "\n       - Normally Executed" << flush;

	cout    << "\nSTEP 1 - Checking Feasibility and Planarizing.." << flush;
	rep_out << "\nSTEP 1 - Checking Feasibility and Planarizing.." << flush;

	// specific case for trivial graph (graph with only one node)
	// ----------------------------------------------------------
	//
	if (ug.number_of_nodes() == 1)
		{
		gdtnode v = ug.first_node ();
		ofstream out ("gdt_test_true");
		cout    << "\n       - Normally Executed" << flush;
		rep_out << "\n       - Normally Executed" << flush;
		if (only_test) return 0;
		cout    << "\nSTEP 2 - Applying Layout Algorithm .." << flush;
		rep_out << "\nSTEP 2 - Applying Layout Algorithm .." << flush;
		
		dugp = new draw_undi_graph();
		gdtnode w = dugp->new_node (gdt::gdtpoint(0,0));
		dugp->assign_id (w,ug.id(v));
		cout    << "\n       - Normally Executed" << flush;
		rep_out << "\n       - Normally Executed" << flush;
		}
	 else
	 	{
		// ----------------------------------------
		// Step 1 - Check Feasibility and planarize
		// ----------------------------------------
		
		

		planarize_options plan_op = DO_NOT_PRESERVE_EMBEDDING;
		switch (po)
			{
			case 0: plan_op = DO_NOT_PRESERVE_EMBEDDING; break;
			case 1: plan_op = PRESERVE_EMBEDDING	   ; break;
			default: break;
			}

		switch (alg)
			{
			case 0: // QUICK ORTHOGONAL
				{
				// check preconditions
				// -------------------
				if (ug.max_degree() > 4 || !ug.is_biconnected())
                        		return write_check_error ("preconditions not verified - graph is not 4-deg. or not biconnected");

                		// planarize
                		// ---------      
				if (!only_test) pugp = new plan_undi_graph (ug,plan_op);
				}
				break;

			case 1: // OPTIMAL ORTHOGONAL 
				{
				// planarize
				// ---------
				if (!only_test) pugp = new plan_undi_graph (ug,plan_op);
				}
				break;

			case 2: // SLOW ORTHOGONAL 
				{
				// check preconditions
				// -------------------
                		if (!ug.make_embedding_planar())
                        		return write_check_error ("preconditions not verified - graph is not planar");
                		if (!ug.is_biconnected()) 
                        		return write_check_error ("preconditions not verified - graph is not biconnected");// planarize
                		// --------- 
				if (!only_test) 
					{
					pugp = new plan_undi_graph (ug); 
					if (plan_op == PRESERVE_EMBEDDING) warning ("planarization options ignored")
					}
				}
				break;

			case 3: // OPTIMAL UPWARD
			case 5: // OPTIMAL UPWARD VI
				{
				// check preconditions
				// -------------------
				if (!ug.all_edges_are_directed()) 
					return write_check_error ("preconditions not verified - graph is not directed");

				// planarize
                		// --------- 
				if (!only_test)
					{
					gdtedge e;
					undi_graph ug_temp (ug);
					gdt::gdtlist<gdtedge> Le = ug_temp.make_embedding_cand_expanded();
					forall (e,Le) ug_temp.new_constraint_uncrossable_edge (e);
					pugp = new plan_undi_graph (ug_temp); 
					pugp->contract();
					}
				}
				break;

			case 4: // SLOW UPWARD
			case 6: // SLOW UPWARD VI
				{
				// check preconditions
				// -------------------
				if (!ug.all_edges_are_directed()) 
					return write_check_error ("preconditions not verified - graph is not directed");
				if (!ug.is_biconnected())
                        		return write_check_error ("preconditions not verified - graph is not biconnected");
                        	
                        	undi_graph ug_temp (ug);
                        	if (!ug_temp.make_embedding_cand_planar ()) 
                        		return write_check_error ("preconditions not verified - graph is not bimodal planar");
				else ug.make_embedding_as(ug_temp);
				
				
                		// planarize
                		// ---------         
                		if (!only_test)
                        		{
					pugp = new plan_undi_graph (ug);
					if (plan_op == PRESERVE_EMBEDDING) warning ("planarization options ignored")
					}
				}
				break;

			case 7:	// POLYLINE
				{
				// check preconditions
				// -------------------
				if (!ug.is_biconnected())
                        		return write_check_error ("preconditions not verified - graph is not biconnected");

                		// planarize
                		// ---------  
				if (!only_test) pugp = new plan_undi_graph (ug,plan_op);
				}
				break;

			case 8:	// VISIBILITY
				{
				// check preconditions
				// -------------------
				if (!ug.is_biconnected())
                        		return write_check_error ("preconditions not verified - graph is not biconnected");

                		// planarize
				// -------------------
				if (!only_test) pugp = new plan_undi_graph (ug,plan_op);
				}
				break;

			case 9 : // TREE CENTERSONS
			case 10: // TREE CENTERSUBTREE
				{
				// check preconditions
				// -------------------
				if (!ug.is_acyclic())
                        		return write_check_error ("preconditions not verified - graph is not acyclic");

                		// planarize
                		// ---------
				if (!only_test) pugp = new plan_undi_graph (ug,plan_op);
				}
				break;
			case 11: // PLANAR EMBEDDING
				{
				// check preconditions
				// -------------------
				if (!ug.is_connected())
                        		return write_check_error ("preconditions not verified - graph is not connected");

                		// planarize
                		// ---------      
				if (!only_test) pugp = new plan_undi_graph (ug,plan_op);
				}
				break;
			}

		cout    << "\n       - Normally Executed" << flush;
		rep_out << "\n       - Normally Executed" << flush;

		ofstream test_out ("gdt_test_true");
		if (only_test) return 0;

		// -------------------------------------
		// Step 2 - Compute the Layout Algorithm
		// ------------------------------------- 

		cout    << "\nSTEP 2 - Applying Layout Algorithm .." << flush;
		rep_out << "\nSTEP 2 - Applying Layout Algorithm .." << flush;

		plan_undi_graph& pug = *pugp;

		// choose external face
		//
		face ef;
		if (!nr || !er) ef = pug.first_face();
		else
			{ 
			if ((alg == 2 || alg == 4 || alg == 6) && (nr || er)) warning ("<EXTERNAL_FACE> section will be ignored")

			gdtnode nrc = pug.corresponding_node (nr,ug);
			gdtedge erc = pug.corresponding_edge (er,ug);
			if (pug.node_belongs_to_edge (nrc,erc)) ef = pug.right_face_moving_along_edge (erc,nrc);
			else
				{
				gdtnode vs = pug.source(erc);
				gdtnode vt = pug.target(erc);
				if (pug.is_marked(vs,RM3_CROSS_ON_REAL_EDGE)) ef = pug.left_face_moving_along_edge (erc,vt);
				else					      ef = pug.left_face_moving_along_edge (erc,vs);
				}
			}
		
		// select the compaction algorithm
		//
		algorithm_type compaction = SLOW_REGULAR_COMPACTION_2_REFINED;
		bool	       fast = false;
		switch (co)
			{
			case 0: compaction = FAST_COMPACTION	    		; fast = true ; break;
			case 1: compaction = FAST_COMPACTION_REFINED		; fast = true ; break;
			case 2: compaction = SLOW_COMPACTION			; fast = false; break;
			case 3: compaction = SLOW_COMPACTION_REFINED		; fast = false; break;
			case 4: compaction = FAST_REGULAR_COMPACTION_2		; fast = true ; break;
			case 5: compaction = FAST_REGULAR_COMPACTION_2_REFINED	; fast = true ; break;
			case 6: compaction = SLOW_REGULAR_COMPACTION_2		; fast = false; break;
			case 7: compaction = SLOW_REGULAR_COMPACTION_2_REFINED	; fast = false; break;
			}
					
		// select and apply the layout algorithm
		//
		switch (alg)
			{
			case 0: // QUICK ORTHOGONAL
				{
				orth_plan_undi_graph opug (pug,ef,PLAN_ORTH_QUICK);
				dugp = new draw_undi_graph(opug,compaction);
				}
				break;

			case 1: // OPTIMAL ORTHOGONAL
				{
				orth_plan_undi_graph opug (pug,ef,PLAN_ORTH_OPTIMAL);
				dugp = new draw_undi_graph(opug,compaction);
				}
				break;

			case 2: // SLOW ORTHOGONAL
				{
				orth_plan_undi_graph opug (pug,ef,PLAN_ORTH_SLOW);
				dugp = new draw_undi_graph(opug,compaction);
				}
				break;

			case 3: // OPTIMAL UPWARD
			case 5: // OPTIMAL UPWARD VI
				{
				upwa_plan_undi_graph upug (pug,ef,PLAN_UPWARD_OPTIMAL);
				if (alg == 3) 	
					if (fast) dugp = new draw_undi_graph(upug,POLYLINE);
					else	  dugp = new draw_undi_graph(upug,POLYLINE_COMPACTED);
				else		
					if (fast) dugp = new draw_undi_graph(upug,VISIBILITY);
					else	  dugp = new draw_undi_graph(upug,VISIBILITY_COMPACTED);
				}
				break;

			case 4: // SLOW UPWARD
			case 6: // SLOW UPWARD VI
				{
				upwa_plan_undi_graph upug (pug,ef,PLAN_UPWARD_SLOW);
				if (alg == 4) 	
					if (fast) dugp = new draw_undi_graph(upug,POLYLINE);
					else	  dugp = new draw_undi_graph(upug,POLYLINE_COMPACTED);
				else		
					if (fast) dugp = new draw_undi_graph(upug,VISIBILITY);
					else	  dugp = new draw_undi_graph(upug,VISIBILITY_COMPACTED);							
				}
				break;

			case 7: // POLYLINE
				{
				if (fast) dugp = new draw_undi_graph(pug,POLYLINE);
				else	  dugp = new draw_undi_graph(pug,POLYLINE_COMPACTED);
				}
				break;

			case 8: // VISIBILITY
				{
				if (fast) dugp = new draw_undi_graph(pug,VISIBILITY);
				else	  dugp = new draw_undi_graph(pug,VISIBILITY_COMPACTED);
				}
				break;

			case 9 : // TREE CENTERSONS
			case 10: // TREE CENTERSUBTREE
				{
				if (!root) root = ug.first_node();
				tree tr (ug,root);
				if (alg == 7) dugp = new draw_undi_graph(tr,TREE_DRAW_CENTERSONS);
				else	      dugp = new draw_undi_graph(tr,TREE_DRAW_CENTERSUBTREE);
				}
				break;
                        case 11: // PLANAR EMBEDDING

                                // nothing to do, bu remember that you don't have a dugp, only a pugp
                                break;
			}

		cout    << "\n       - Normally Executed .." << flush;
		rep_out << "\n       - Normally Executed .." << flush;

		// assign the specified node-dimensions 
		// and rescale the drawing
		//
                switch (alg)
			{
			case 11: // PLANAR EMBEDDING
				break; // nothing to do
			default:
				if (!L_node_dimension.empty())
				{
				gdtnode v;
				forall (v,L_node_dimension)
					{
					gdtnode u = dugp -> corresponding_node(v,ug);
					dugp -> set_width (u,w[v]);
					dugp -> set_height(u,h[v]);
					}
				dugp -> rescale();
				}
				break;
			}
		}

	// ---------------------------------------------------------------
	// STEP 3 - Write the Files EXP-format, GDT-format, and fig-format
	// ---------------------------------------------------------------
	
	cout    << "\nSTEP 3 - Writing output file.." << flush;
	rep_out << "\nSTEP 3 - Writing output file.." << flush;
	
	string  exp_graph_file_name = graph_file_name + ".exp",
		gdt_graph_file_name = graph_file_name + ".gdt",
		fig_graph_file_name = graph_file_name + ".fig",
		xml_graph_file_name = graph_file_name + ".xml";

        plan_undi_graph& pug = *pugp;

        switch (alg)
		{
		case 11: // PLANAR EMBEDDING

			if (!write_embedding(pug, gdt_graph_file_name)) error("File error writing in gdt format")

			break;

		default:

			//Remove the edges added by make_connected
			//
			gdtedge temp_edge;
			forall(temp_edge,added_edges)
				{
				dugp->del_edge(dugp->find_edge(ug.id(temp_edge)));
				//cout << "edge " << ug.id(temp_edge) << " deleted" << endl;
				}

			if (!dugp->draw (exp_graph_file_name)) error("File error writing in exp format")
			if (!dugp->write(gdt_graph_file_name)) error("File error writing in gdt format")

			dugp->export_to_fig_file_format(fig_graph_file_name,false,0.6);
			dugp->export_to_xml_file_format(xml_graph_file_name);

			break;
		}
	
	cout 	<< "\n       - Normally Executed\n\n" << flush;
	rep_out << "\n       - Normally Executed\n\n" << flush;
	
	
	/* COMMENT TO AVOID DISPLAYING OF THE OUTPUT
	// --------------------------------------------
	// STEP 4 - File is read and viewed on a window
	// --------------------------------------------
	*/
	/*
	cout << "\nSTEP 4 - Reloading and visualizing layout.. " << flush;
	
	in.open(exp_graph_file_name);
	if (!in) 
		{
		cout << "\nFILE-ERROR LOADING " << exp_graph_file_name << flush;
		return 1;
		}	
	
	window W;
	W.open ();
	
	// draw all the nodes as rectangles
	//
	in >> s;
	if (s != "<NODELIST>") error("Missing Symbol <NODELIST>");
	in >> s;
	while (s == "<NODE>") 
		{
		int    idv;
		point  cv;
		double wv;
		double hv;
		
		in >> idv >> cv >> wv >> hv;
		
		point  pl (cv.xcoord() - wv/2, cv.ycoord() - hv/2);
		point  pu (cv.xcoord() + wv/2, cv.ycoord() + hv/2);		
		
		W.draw_box   	 (pl,pu,16);
		W.draw_rectangle (pl,pu);
		string sidv  ("%d",idv);
		W.draw_ctext (cv,sidv,black);
		 
		in >> s;
		if (s != "</NODE>") error("Missing Symbol </NODE>")
		in >> s;
		}
	if (s != "</NODELIST>") error("Missing Symbol </NODELIST>")
	
	// draw all the edges as polygonal lines
	//
	in >> s;
	if (s != "<EDGELIST>") error("Missing Symbol <EDGELIST>")
	in >> s;
	while (s == "<EDGE>")
		{
		int ide;
		list<point> bl;
		enum {FORW, BACK, UNDI} dir;
		in >> ide;
		in >> s;
		if (s != "<DIRECTION>") error("Missing Symbol <DIRECTION>")
		in >> s;
		if 	(s == "--") dir = UNDI;
		else if (s == "->") dir = FORW;
		else if (s == "<-") dir = BACK;
		else error("Unknown symbol for direction")
		
		in >> s;
			
		while (s == "<BEND>")
			{
			point bp;
			in >> bp;
			bl.append(bp);
			in >> s;
			}
			
		if (s != "</EDGE>") error("Missing Symbol </EDGE>")
		
		list_item ip1,ip2;
		
		forall_items (ip1,bl)
			{
			ip2 = bl.succ(ip1);
			if (dir != UNDI)
				{
				if 	(dir==BACK && ip1 == bl.first_item()) W.draw_arrow (bl.inf(ip2), bl.inf(ip1));
				else if (dir==FORW && ip2 == bl.last_item())  W.draw_arrow (bl.inf(ip1), bl.inf(ip2));
				else if (ip2) W.draw_segment (bl.inf(ip1), bl.inf(ip2));
				}
			else if (ip2) W.draw_segment (bl.inf(ip1), bl.inf(ip2));
			}
		
		in >> s;
		}
	if (s != "</EDGELIST>") error("Missing Symbol </EDGELIST>")
	in.close();
	
	cout << "\nSTEP 4 - Normally Executed" << flush;
	
	int i;
	cin >> i;
	*/
	return 0;
	}




// -------------------------------------------------------
// modules for parsing all possible configuration versions
// ------------------------------------------------------- 



	void
read_option_file_ver_1_0
	(
	ifstream & 		in, 
	ofstream & 		rep_out, 
	undi_graph & 		ug,
	gdt::gdtnode_array<double> & 	w,  			// output : width of each node
	gdt::gdtnode_array<double> & 	h, 			// output : height of each node
	gdt::gdtlist<gdtnode> &		L_node_dimension	// output : list of node for which a new dimension is specified
	)
	{
	int	idnr = UNDEFINED;	// id of the reference node for the external face
	int	ider = UNDEFINED;	// id of the reference edge for the external face
	int	ide, idn;	
	int	fl; 			// face_label for <SAME_FACE_NODES> constraint
	gdtnode 	n;
	gdtedge	e;

	string s;

	in >> s;  
	while (s != "<END_OPTIONS>")
		{
		if (s == "<CHECKING>")
			 {
			 char c; 
			 in >> c; 
			 if (c == 't') only_test = true;
                	 else if (c == 'l') only_test = false;
			 else error ("invalid symbol in <CHECKING> section")
			 in >> s; if (s != "</CHECKING>") error ("missing </ALGORITHM> symbol")
			 }

        	else if (s == "<ALGORITHM>")
			 {
			 in >> alg; if (alg < 0 || alg > 10)	error("invalid algorithm code")
			 in >> s  ; if (s != "</ALGORITHM>")	error("missing </ALGORITHM> symbol")
			 }

		else if (s == "<PLANARIZATION>")
			 {
			 in >> po; if (po < 0 || po > 1) 	 error("invalid planarization code")
			 in >> s ; if (s  != "</PLANARIZATION>") error("missing </PLANARIZATION> symbol")
			 }
		 
		else if (s == "<COMPACTION>")
		 	 {
			 in >> co; if (co < 0 || co > 7)	 error("invalid compaction code")
			 in >> s ; if (s  != "</COMPACTION>")	 error("missing </COMPACTION> symbol")
			 }
		 
		else if (s == "<EXTERNAL_FACE>")
			 {
			 in >> idnr; nr = ug.find_node(idnr); 
				     if (!nr)			error("invalid node in <EXTERNAL_FACE> section")
			 in >> ider; er = ug.find_edge(ider);
				     if (!er)			error("invalid edge in <EXTERNAL_FACE> section")

			 if (!ug.node_belongs_to_edge(nr,er))	error("invalid (node,edge) pair in <EXTERNAL_FACE> section")

			 in >> s ; if (s != "</EXTERNAL_FACE>")	error("missing </EXTERNAL_FACE> symbol")
			 }

		else if (s == "<TREE_ROOT>")
			 {
			 int idr;
			 in >> idr; root = ug.find_node(idr);
				 if (!root)			error("invalid node in <TREE_ROOT> section") 
			 in >> s;
			 if (s != "</TREE_ROOT>") error("missing </TREE_ROOT> symbol")
			 }

		else if (s == "<NODE_DIMENSION>")
			{
			int 	idv;
			double  wv, hv;
			in >> s;
			while (s == "<NODE>")
				{
				in >> idv >> wv >> hv;
				gdtnode v = ug.find_node(idv);
				if (!v) error("invalid node in <NODE_DIMENSION> section")
				w[v] = wv;
				h[v] = hv;
				L_node_dimension.append(v);
				in >> s; 
				}
			if (s != "</NODE_DIMENSION>") error("missing </NODE_DIMENSION> symbol")
			}

		else if (s == "<CONSTRAINTS>")
			 {
			 in >> s;
			 while (s != "</CONSTRAINTS>")
				 {
				 if (s == "<BENDS>")
					 {
					 in >> s;
					 while (s != "</BENDS>")
						 {
						 if (s == "<ANY>")
							 {
							 in >> s;
							 while (s == "<EDGE>")
								 {
								 in >> ide; 
								 e = ug.find_edge(ide);
								 if (!e)  error("invalid edge in <BENDS>/<ANY> section")
								 else 	  ug.new_constraint_number_of_bends_on_edge(e,ANY);
								 in >> s;
								 }
							 if (s != "</ANY>") error("missing </ANY> symbol")				
							 }
						 else if (s == "<NONE>")
							 {
							 in >> s;
							 while (s == "<EDGE>")
								 {
								 in >> ide; 
								 e = ug.find_edge(ide);
								 if (!e)  error("invalid edge in <BENDS>/<NONE> section")
								 else 	  ug.new_constraint_number_of_bends_on_edge(e,NONE);
								 in >> s;
								 }
							 if (s != "</NONE>") error("missing </NONE> symbol")				
							 }
						 else if (s == "<DIRECTION>")
						 	 {
						 	 in >> s;
						 	 while (s == "<DIR>")
						 	 	{
						 	 	in >> idn;
						 	 	n = ug.find_node(idn);
						 	 	if (!n)  error("invalid node in <BENDS>/<DIRECTION> section")
						 	 	in >> ide;
								e = ug.find_edge(ide);
								if (!e)  error("invalid edge in <BENDS>/<DIRECTION> section")
								if (!ug.node_belongs_to_edge(n,e))
									 error("invalid pair edge,node in <BENDS>/<DIRECTION> section")
								ug.new_constraint_bend_direction_on_edge (e,n);
								in >> s;
						 	 	}
						 	 if (s != "</DIRECTION>") error("missing </DIRECTION> symbol")
						 	 }	 
						
						 else error("unknown symbol in <BENDS> section")
						 in >> s; 
						 }	
					 }
					
				 else if (s == "<UNCROSSABLE_EDGES>")
				 	{
				 	in >> s;
				 	while (s == "<EDGE>")
				 		{
				 		in >> ide;
				 		e = ug.find_edge(ide);
				 		if (!e)  error("invalid edge in <UNCROSSABLE_EDGE> section")
				 		else	 ug.new_constraint_uncrossable_edge (e);
				 		in >> s;
				 		}
				 	if (s != "</UNCROSSABLE_EDGES>") error("missing </UNCROSSABLE_EDGES> symbol")
				 	}

				 else if (s == "<SAME_FACE_NODES>")
				 	{
				 	in >> s;
				 	while (s == "<NODE>")
				 		{
				 		in >> idn;
				 		n = ug.find_node(idn);
				 		if (!n)  error("invalid node in <SAME_FACE_NODES> section")
			 			in >> fl;
				 		ug.new_constraint_same_face_node (n,fl);
				 		in >> s;
				 		}
				 	if (s != "</SAME_FACE_NODES>") error("missing </SAME_FACE_NODES> symbol")
				 	}

				 else if (s == "<SAME_FACE_ORDERED_NODES>")
				 	{
				 	in >> s;
				 	while (s == "<SAME_FACE>")
				 		{
				 		gdt::gdtlist<gdtnode> Ln;
				 		in >> s;
				 		while (s == "<NODE>")
				 			{
				 			in >> idn;
				 		        n = ug.find_node(idn);
				 		        if (!n)  error("invalid node in <SAME_FACE_ORDERED_NODES> section")
				 		        Ln.append(n);
				 			in >> s;
				 			}
				 		if (s != "</SAME_FACE>")  error("missing </SAME_FACE> symbol")
				 		ug.new_constraint_same_face_ordered_nodes (Ln);
				 		in >> s;
				 		}
				 	if (s != "</SAME_FACE_ORDERED_NODES>") error("missing </SAME_FACE_ORDERED_NODES> symbol")
				 	}

				 else error("unknown symbol in <CONSTRAINTS> section")
				 in >> s;
				 }
			 }	
		 else error("unknown symbol in <BEGIN_OPTIONS> section")
		 in >> s;
		 }
	}

	void
read_option_file_ver_1_1
	(
	ifstream & 		in, 
	ofstream & 		rep_out, 
	undi_graph & 		ug
	)
	{
	int	idnr = UNDEFINED;	// id of the reference node for the external face
	int	ider = UNDEFINED;	// id of the reference edge for the external face
	int	ide, idn;	
	int	fl; 			// face_label for <SAME_FACE_NODES> constraint
	gdtnode 	n;
	gdtedge	e;

	gdt::gdtnode_array<double>	n_width  (ug.nodes_and_edges(),UNDEFINED);	// width of each node in L_node_dimension
	gdt::gdtnode_array<double>	n_height (ug.nodes_and_edges(),UNDEFINED);	// height of each node in L_node_dimension
	gdt::gdtlist<gdtnode>		L_node_width;					// list of nodes to assign a width constraint
	gdt::gdtlist<gdtnode> 		L_node_height;					// list of nodes to assign a height constraint

	string s;

        in >> s; if (s != "<BEGIN_OPTIONS>") error("missing '<BEGIN_OPTIONS>' symbol in options-file")
	in >> s;  
	while (s != "<END_OPTIONS>")
		{
		if (s == "<CHECKING>")
			 {
			 char c; 
			 in >> c; 
			 if (c == 't') only_test = true;
                	 else if (c == 'l') only_test = false;
			 else error ("invalid symbol in <CHECKING> section")
			 in >> s; if (s != "</CHECKING>") error ("missing </ALGORITHM> symbol")
			 }

        	else if (s == "<ALGORITHM>")
			 {
			 in >> alg; if (alg < 0 || alg > 10)	error("invalid algorithm code")
			 in >> s  ; if (s != "</ALGORITHM>")	error("missing </ALGORITHM> symbol")
			 }

		else if (s == "<PLANARIZATION>")
			 {
			 in >> po; if (po < 0 || po > 1) 	 error("invalid planarization code")
			 in >> s ; if (s  != "</PLANARIZATION>") error("missing </PLANARIZATION> symbol")
			 }
		 
		else if (s == "<COMPACTION>")
		 	 {
			 in >> co; if (co < 0 || co > 7)	 error("invalid compaction code")
			 in >> s ; if (s  != "</COMPACTION>")	 error("missing </COMPACTION> symbol")
			 }
		 
		else if (s == "<EXTERNAL_FACE>")
			 {
			 in >> idnr; nr = ug.find_node(idnr); 
				     if (!nr)			error("invalid node in <EXTERNAL_FACE> section")
			 in >> ider; er = ug.find_edge(ider);
				     if (!er)			error("invalid edge in <EXTERNAL_FACE> section")

			 if (!ug.node_belongs_to_edge(nr,er))	error("invalid (node,edge) pair in <EXTERNAL_FACE> section")

			 in >> s ; if (s != "</EXTERNAL_FACE>")	error("missing </EXTERNAL_FACE> symbol")
			 }

		else if (s == "<TREE_ROOT>")
			 {
			 int idr;
			 in >> idr; root = ug.find_node(idr);
				 if (!root)			error("invalid node in <TREE_ROOT> section") 
			 in >> s;
			 if (s != "</TREE_ROOT>") error("missing </TREE_ROOT> symbol")
			 }

		else if (s == "<CONSTRAINTS>")
			 {
			 in >> s;
			 while (s != "</CONSTRAINTS>")
				 {
				 if (s == "<BENDS>")
					 {
					 in >> s;
					 while (s != "</BENDS>")
						 {
						 if (s == "<ANY>")
							 {
							 in >> s;
							 while (s == "<EDGE>")
								 {
								 in >> ide; 
								 e = ug.find_edge(ide);
								 if (!e)  error("invalid edge in <BENDS>/<ANY> section")
								 else 	  ug.new_constraint_number_of_bends_on_edge(e,ANY);
								 in >> s;
								 }
							 if (s != "</ANY>") error("missing </ANY> symbol")				
							 }
						 else if (s == "<NONE>")
							 {
							 in >> s;
							 while (s == "<EDGE>")
								 {
								 in >> ide; 
								 e = ug.find_edge(ide);
								 if (!e)  error("invalid edge in <BENDS>/<NONE> section")
								 else 	  ug.new_constraint_number_of_bends_on_edge(e,NONE);
								 in >> s;
								 }
							 if (s != "</NONE>") error("missing </NONE> symbol")				
							 }
						 else if (s == "<DIRECTION>")
						 	 {
						 	 in >> s;
						 	 while (s == "<DIR>")
						 	 	{
						 	 	in >> idn;
						 	 	n = ug.find_node(idn);
						 	 	if (!n)  error("invalid node in <BENDS>/<DIRECTION> section")
						 	 	in >> ide;
								e = ug.find_edge(ide);
								if (!e)  error("invalid edge in <BENDS>/<DIRECTION> section")
								if (!ug.node_belongs_to_edge(n,e))
									 error("invalid pair edge,node in <BENDS>/<DIRECTION> section")
								ug.new_constraint_bend_direction_on_edge (e,n);
								in >> s;
						 	 	}
						 	 if (s != "</DIRECTION>") error("missing </DIRECTION> symbol")
						 	 }	 
						
						 else error("unknown symbol in <BENDS> section")
						 in >> s; 
						 }	
					 }
					 
					 
				else if (s == "<ANGLES>")
				 	{
					in >> s;
					while (s == "<ANGLE_SWEEP>")
						{
						in >> idn;
						n = ug.find_node(idn);
						if (!n)  error("invalid node in <ANGLE_SWEEP>/<ANGLES> section")
						in >> ide;
						e = ug.find_edge(ide);
						if (!e)  error("invalid edge in <ANGLE_SWEEP>/<ANGLES> section")
						if (!ug.node_belongs_to_edge(n,e))
							 error("invalid pair edge,node in <ANGLE_SWEEP>/<ANGLES> section")
						int a;
						angle_type alpha;
						in >> a;
						switch (a)
						  {
						  case 0  : alpha = _000; break;
						  case 90 : alpha = _090; break;
						  case 180: alpha = _180; break;
						  case 270: alpha = _270; break;
						  case 360: alpha = _360; break;
						  default : error ("invalid angle-sweep value in <ANGLE_SWEEP>/<ANGLES> section"); break;
						  }
						
						ug.new_constraint_angle_sweep (n,e,alpha);
						in >> s;
						}
					if (s != "</ANGLES>") error("missing </ANGLES> symbol")
					}			
					 
				 else if (s == "<UNCROSSABLE_EDGES>")
				 	{
				 	in >> s;
				 	while (s == "<EDGE>")
				 		{
				 		in >> ide;
				 		e = ug.find_edge(ide);
				 		if (!e)  error("invalid edge in <UNCROSSABLE_EDGE> section")
				 		else	 ug.new_constraint_uncrossable_edge (e);
				 		in >> s;
				 		}
				 	if (s != "</UNCROSSABLE_EDGES>") error("missing </UNCROSSABLE_EDGES> symbol")
				 	}

				 else if (s == "<SAME_FACE_NODES>")
				 	{
				 	in >> s;
				 	while (s == "<NODE>")
				 		{
				 		in >> idn;
				 		n = ug.find_node(idn);
				 		if (!n)  error("invalid node in <SAME_FACE_NODES> section")
			 			in >> fl;
				 		ug.new_constraint_same_face_node (n,fl);
				 		in >> s;
				 		}
				 	if (s != "</SAME_FACE_NODES>") error("missing </SAME_FACE_NODES> symbol")
				 	}

				 else if (s == "<SAME_FACE_ORDERED_NODES>")
				 	{
				 	in >> s;
				 	while (s == "<SAME_FACE>")
				 		{
				 		gdt::gdtlist<gdtnode> Ln;
				 		in >> s;
				 		while (s == "<NODE>")
				 			{
				 			in >> idn;
				 		        n = ug.find_node(idn);
				 		        if (!n)  error("invalid node in <SAME_FACE_ORDERED_NODES> section")
				 		        Ln.append(n);
				 			in >> s;
				 			}
				 		if (s != "</SAME_FACE>")  error("missing </SAME_FACE> symbol")
				 		ug.new_constraint_same_face_ordered_nodes (Ln);
				 		in >> s;
				 		}
				 	if (s != "</SAME_FACE_ORDERED_NODES>") error("missing </SAME_FACE_ORDERED_NODES> symbol")
				 	}

				else if (s == "<NODE_DIM")
					{
					gdtnode 	v;
					int 	idv;
					double  wv = 0.0;
					double 	hv = 0.0;
					char*   dummy;   	// to give to strtod (anyway I will not make use of it)
				
					in >> s;
					while (s != "/>")
						{
						string 	ws;		// working string
						//if ( s.tail(2) == string("/>") ) 	// element terminator trailing
						if ( s.substr(s.length()-2,2) == string("/>") ) 	// element terminator trailing
							{
							//ws = s.head(s.length()-2);
							ws = s.substr(0,s.length()-2);
							// s = s.tail(2);			// preparing next string
							s = s.substr(s.length()-2,2);			// preparing next string
							}
						else	
							{
							ws = s;
							// 	cout << "\nread: " << ws;
							in >> s;			// preparing next string
							}
					
						//if( ws.head(9) == (string("NODE_ID=")+string('"') ) )	// example: 'NODE_ID="100"
						if( ws.substr(0,9) == (string("NODE_ID=")+string("\"") ) )	// example: 'NODE_ID="100"'
							{
							//string node_id = ws.tail(ws.length()-9);	// example: '100"'
							string node_id = ws.substr(9,ws.length()-9);	// example: '100"'
							//node_id = node_id.head(node_id.length()-1);	// example: '100'
							node_id = node_id.substr(0,node_id.length()-1);	// example: '100'
							idv = atoi(node_id.c_str());
							v = ug.find_node(idv);
							if (!v) error("invalid value for NODE_ID attribute in <NODE_DIM/> element")
							}
						//else if ( ws.head(7) == "WIDTH=\"" )			// example: 'WIDTH="5.45"'
						else if ( ws.substr(0,7) == "WIDTH=\"" )
							{
							//string wid = ws.tail(ws.length()-7);		// example: '5.45"'
							string wid = ws.substr(7,ws.length()-7);		// example: '5.45"'
							wv = strtod(wid.c_str(), &dummy);		// trailing stuff ignored
							}
						//else if ( ws.head(8) == "HEIGHT=\"" )			// example: 'HEIGHT="4.6789"'
						else if ( ws.substr(0,8) == "HEIGHT=\"" )			// example: 'HEIGHT="4.6789"'
							{						
							//string hei = ws.tail(ws.length()-8);		// example: '4.6789"'
							string hei = ws.substr(8,ws.length()-8);		// example: '4.6789"'
							hv = strtod(hei.c_str(), &dummy);			// trailing stuff ignored
							}
						else	error(ws <<  " invalid attribute in <NODE_DIM/> element")

						}
					if (s != "/>") error("missing \"/>\" terminator in <NODE_DIM/> symbol")

					if ( wv != 0.0 ) 
						{
						n_width[v] = wv;
						L_node_width.append(v);
						}
					if ( hv != 0.0 )
						{
						n_height[v] = hv;
						L_node_height.append(v);
						} 
					}
				
				else error("unknown symbol in <CONSTRAINTS> section")
				in >> s;
				}
			}	
		else error("unknown symbol in <BEGIN_OPTIONS> section")
		in >> s;
		}

	//-------------------------------------------------------------
	// operations to be performed at the end of the parsing process
	//-------------------------------------------------------------

	if (!L_node_width.empty())
		{
		gdtnode v;
		forall (v,L_node_width)
			{
			ug.new_constraint_node_width(v,n_width[v]);
			}
		}
	if (!L_node_height.empty())
		{
		gdtnode v;
		forall (v,L_node_height)
			{
			ug.new_constraint_node_height(v,n_height[v]);
			}
		}
	}

/*
   It is the same as read_option_file_ver_1_1() but allows for code 11: planarization only 
*/
	void
read_option_file_ver_1_2
	(
	ifstream & 		in, 
	ofstream & 		rep_out, 
	undi_graph & 		ug
	)
	{
	int	idnr = UNDEFINED;	// id of the reference node for the external face
	int	ider = UNDEFINED;	// id of the reference edge for the external face
	int	ide, idn;	
	int	fl; 			// face_label for <SAME_FACE_NODES> constraint
	gdtnode 	n;
	gdtedge	e;

	gdt::gdtnode_array<double>	n_width  (ug.nodes_and_edges(),UNDEFINED);	// width of each node in L_node_dimension
	gdt::gdtnode_array<double>	n_height (ug.nodes_and_edges(),UNDEFINED);	// height of each node in L_node_dimension
	gdt::gdtlist<gdtnode>		L_node_width;					// list of nodes to assign a width constraint
	gdt::gdtlist<gdtnode> 		L_node_height;					// list of nodes to assign a height constraint

	string s;

        in >> s; if (s != "<BEGIN_OPTIONS>") error("missing '<BEGIN_OPTIONS>' symbol in options-file")
	in >> s;  
	while (s != "<END_OPTIONS>")
		{
		if (s == "<CHECKING>")
			 {
			 char c; 
			 in >> c; 
			 if (c == 't') only_test = true;
                	 else if (c == 'l') only_test = false;
			 else error ("invalid symbol in <CHECKING> section")
			 in >> s; if (s != "</CHECKING>") error ("missing </ALGORITHM> symbol")
			 }

        	else if (s == "<ALGORITHM>")
			 {
			 in >> alg; if (alg < 0 || alg > 11)	error("invalid algorithm code")  // THIS IS THE ONLY CHANGE WITH RESPECT TO read_option_file_ver_1_1()
			 in >> s  ; if (s != "</ALGORITHM>")	error("missing </ALGORITHM> symbol")
			 }

		else if (s == "<PLANARIZATION>")
			 {
			 in >> po; if (po < 0 || po > 1) 	 error("invalid planarization code")
			 in >> s ; if (s  != "</PLANARIZATION>") error("missing </PLANARIZATION> symbol")
			 }
		 
		else if (s == "<COMPACTION>")
		 	 {
			 in >> co; if (co < 0 || co > 7)	 error("invalid compaction code")
			 in >> s ; if (s  != "</COMPACTION>")	 error("missing </COMPACTION> symbol")
			 }
		 
		else if (s == "<EXTERNAL_FACE>")
			 {
			 in >> idnr; nr = ug.find_node(idnr); 
				     if (!nr)			error("invalid node in <EXTERNAL_FACE> section")
			 in >> ider; er = ug.find_edge(ider);
				     if (!er)			error("invalid edge in <EXTERNAL_FACE> section")

			 if (!ug.node_belongs_to_edge(nr,er))	error("invalid (node,edge) pair in <EXTERNAL_FACE> section")

			 in >> s ; if (s != "</EXTERNAL_FACE>")	error("missing </EXTERNAL_FACE> symbol")
			 }

		else if (s == "<TREE_ROOT>")
			 {
			 int idr;
			 in >> idr; root = ug.find_node(idr);
				 if (!root)			error("invalid node in <TREE_ROOT> section") 
			 in >> s;
			 if (s != "</TREE_ROOT>") error("missing </TREE_ROOT> symbol")
			 }

		else if (s == "<CONSTRAINTS>")
			 {
			 in >> s;
			 while (s != "</CONSTRAINTS>")
				 {
				 if (s == "<BENDS>")
					 {
					 in >> s;
					 while (s != "</BENDS>")
						 {
						 if (s == "<ANY>")
							 {
							 in >> s;
							 while (s == "<EDGE>")
								 {
								 in >> ide; 
								 e = ug.find_edge(ide);
								 if (!e)  error("invalid edge in <BENDS>/<ANY> section")
								 else 	  ug.new_constraint_number_of_bends_on_edge(e,ANY);
								 in >> s;
								 }
							 if (s != "</ANY>") error("missing </ANY> symbol")				
							 }
						 else if (s == "<NONE>")
							 {
							 in >> s;
							 while (s == "<EDGE>")
								 {
								 in >> ide; 
								 e = ug.find_edge(ide);
								 if (!e)  error("invalid edge in <BENDS>/<NONE> section")
								 else 	  ug.new_constraint_number_of_bends_on_edge(e,NONE);
								 in >> s;
								 }
							 if (s != "</NONE>") error("missing </NONE> symbol")				
							 }
						 else if (s == "<DIRECTION>")
						 	 {
						 	 in >> s;
						 	 while (s == "<DIR>")
						 	 	{
						 	 	in >> idn;
						 	 	n = ug.find_node(idn);
						 	 	if (!n)  error("invalid node in <BENDS>/<DIRECTION> section")
						 	 	in >> ide;
								e = ug.find_edge(ide);
								if (!e)  error("invalid edge in <BENDS>/<DIRECTION> section")
								if (!ug.node_belongs_to_edge(n,e))
									 error("invalid pair edge,node in <BENDS>/<DIRECTION> section")
								ug.new_constraint_bend_direction_on_edge (e,n);
								in >> s;
						 	 	}
						 	 if (s != "</DIRECTION>") error("missing </DIRECTION> symbol")
						 	 }	 
						
						 else error("unknown symbol in <BENDS> section")
						 in >> s; 
						 }	
					 }
					 
					 
				else if (s == "<ANGLES>")
				 	{
					in >> s;
					while (s == "<ANGLE_SWEEP>")
						{
						in >> idn;
						n = ug.find_node(idn);
						if (!n)  error("invalid node in <ANGLE_SWEEP>/<ANGLES> section")
						in >> ide;
						e = ug.find_edge(ide);
						if (!e)  error("invalid edge in <ANGLE_SWEEP>/<ANGLES> section")
						if (!ug.node_belongs_to_edge(n,e))
							 error("invalid pair edge,node in <ANGLE_SWEEP>/<ANGLES> section")
						int a;
						angle_type alpha;
						in >> a;
						switch (a)
						  {
						  case 0  : alpha = _000; break;
						  case 90 : alpha = _090; break;
						  case 180: alpha = _180; break;
						  case 270: alpha = _270; break;
						  case 360: alpha = _360; break;
						  default : error ("invalid angle-sweep value in <ANGLE_SWEEP>/<ANGLES> section"); break;
						  }
						
						ug.new_constraint_angle_sweep (n,e,alpha);
						in >> s;
						}
					if (s != "</ANGLES>") error("missing </ANGLES> symbol")
					}			
					 
				 else if (s == "<UNCROSSABLE_EDGES>")
				 	{
				 	in >> s;
				 	while (s == "<EDGE>")
				 		{
				 		in >> ide;
				 		e = ug.find_edge(ide);
				 		if (!e)  error("invalid edge in <UNCROSSABLE_EDGE> section")
				 		else	 ug.new_constraint_uncrossable_edge (e);
				 		in >> s;
				 		}
				 	if (s != "</UNCROSSABLE_EDGES>") error("missing </UNCROSSABLE_EDGES> symbol")
				 	}

				 else if (s == "<SAME_FACE_NODES>")
				 	{
				 	in >> s;
				 	while (s == "<NODE>")
				 		{
				 		in >> idn;
				 		n = ug.find_node(idn);
				 		if (!n)  error("invalid node in <SAME_FACE_NODES> section")
			 			in >> fl;
				 		ug.new_constraint_same_face_node (n,fl);
				 		in >> s;
				 		}
				 	if (s != "</SAME_FACE_NODES>") error("missing </SAME_FACE_NODES> symbol")
				 	}

				 else if (s == "<SAME_FACE_ORDERED_NODES>")
				 	{
				 	in >> s;
				 	while (s == "<SAME_FACE>")
				 		{
				 		gdt::gdtlist<gdtnode> Ln;
				 		in >> s;
				 		while (s == "<NODE>")
				 			{
				 			in >> idn;
				 		        n = ug.find_node(idn);
				 		        if (!n)  error("invalid node in <SAME_FACE_ORDERED_NODES> section")
				 		        Ln.append(n);
				 			in >> s;
				 			}
				 		if (s != "</SAME_FACE>")  error("missing </SAME_FACE> symbol")
				 		ug.new_constraint_same_face_ordered_nodes (Ln);
				 		in >> s;
				 		}
				 	if (s != "</SAME_FACE_ORDERED_NODES>") error("missing </SAME_FACE_ORDERED_NODES> symbol")
				 	}

				else if (s == "<NODE_DIM")
					{
					gdtnode 	v;
					int 	idv;
					double  wv = 0.0;
					double 	hv = 0.0;
					char*   dummy;   	// to give to strtod (anyway I will not make use of it)
				
					in >> s;
					while (s != "/>")
						{
						string 	ws;		// working string
						//if ( s.tail(2) == string("/>") ) 	// element terminator trailing
						if ( s.substr(s.length()-2,2) == string("/>") ) 	// element terminator trailing
							{
							//ws = s.head(s.length()-2);
							ws = s.substr(0,s.length()-2);
							// s = s.tail(2);			// preparing next string
							s = s.substr(s.length()-2,2);			// preparing next string
							}
						else	
							{
							ws = s;
							// 	cout << "\nread: " << ws;
							in >> s;			// preparing next string
							}
					
						//if( ws.head(9) == (string("NODE_ID=")+string('"') ) )	// example: 'NODE_ID="100"
						if( ws.substr(0,9) == (string("NODE_ID=")+string("\"") ) )	// example: 'NODE_ID="100"'
							{
							//string node_id = ws.tail(ws.length()-9);	// example: '100"'
							string node_id = ws.substr(9,ws.length()-9);	// example: '100"'
							//node_id = node_id.head(node_id.length()-1);	// example: '100'
							node_id = node_id.substr(0,node_id.length()-1);	// example: '100'
							idv = atoi(node_id.c_str());
							v = ug.find_node(idv);
							if (!v) error("invalid value for NODE_ID attribute in <NODE_DIM/> element")
							}
						//else if ( ws.head(7) == "WIDTH=\"" )			// example: 'WIDTH="5.45"'
						else if ( ws.substr(0,7) == "WIDTH=\"" )
							{
							//string wid = ws.tail(ws.length()-7);		// example: '5.45"'
							string wid = ws.substr(7,ws.length()-7);		// example: '5.45"'
							wv = strtod(wid.c_str(), &dummy);		// trailing stuff ignored
							}
						//else if ( ws.head(8) == "HEIGHT=\"" )			// example: 'HEIGHT="4.6789"'
						else if ( ws.substr(0,8) == "HEIGHT=\"" )			// example: 'HEIGHT="4.6789"'
							{						
							//string hei = ws.tail(ws.length()-8);		// example: '4.6789"'
							string hei = ws.substr(8,ws.length()-8);		// example: '4.6789"'
							hv = strtod(hei.c_str(), &dummy);			// trailing stuff ignored
							}
						else	error(ws <<  " invalid attribute in <NODE_DIM/> element")

						}
					if (s != "/>") error("missing \"/>\" terminator in <NODE_DIM/> symbol")

					if ( wv != 0.0 ) 
						{
						n_width[v] = wv;
						L_node_width.append(v);
						}
					if ( hv != 0.0 )
						{
						n_height[v] = hv;
						L_node_height.append(v);
						} 
					}
				
				else error("unknown symbol in <CONSTRAINTS> section")
				in >> s;
				}
			}	
		else error("unknown symbol in <BEGIN_OPTIONS> section")
		in >> s;
		}

	//-------------------------------------------------------------
	// operations to be performed at the end of the parsing process
	//-------------------------------------------------------------

	if (!L_node_width.empty())
		{
		gdtnode v;
		forall (v,L_node_width)
			{
			ug.new_constraint_node_width(v,n_width[v]);
			}
		}
	if (!L_node_height.empty())
		{
		gdtnode v;
		forall (v,L_node_height)
			{
			ug.new_constraint_node_height(v,n_height[v]);
			}
		}
	}


	bool
write_embedding
	(plan_undi_graph& pug, std::string file_name)
	{
	// open a text file for output with name "file_name"
	//

	ofstream out (file_name.c_str());
	if (!out) return false;

	// write the undi_graph part, i.e. the adjacent lists
	// forall nodes and direction of their edges

	pug.undi_graph::append_section_to_fstream (out);  // write the undi_graph part
	out.close();
	return true;
	}


