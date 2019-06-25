//#include<GDT/rm3_draw_undi_graph.h>

#include<GDT/rm3_draw_undi_graph.h>
#include<GDT/rm3_orth_plan_undi_graph.h>
#include<GDT/rm3_upwa_plan_undi_graph.h>
#include<GDT/rm3_dime_orth_plan_undi_graph.h>
#include<GDT/stopwatch.h>
#include<string>

using namespace std;



    int
main
    (int argc, char* argv[])
    {
    // global variables
    //
    undi_graph ug;

    draw_undi_graph* dugp = NULL;


    // read the file "table_test" and executes test
    //
    ifstream in ("test_table");
	if (!in)
		{
		cout << "\nfile test_table not found" << flush;
		return 1;
		}

    string graph_name;		// graph to test
    int	   alg;			// algorithm to apply on graph_name
    int	   times; 		// number of times to apply alg



    float CPU_time;
    stopwatch* T=new stopwatch();
	T->start();
	//used_time(T);	// initialize the timer T;
    cout << "\n\nSTARTING TEST" << flush;

    in >> graph_name;

    while (graph_name != "END")
    	{
	in >> alg;
	in >> times;

	if (!ug.read(graph_name))
		{
		cout << "\nGRAPH " << graph_name << "NOT FOUND" << flush;
		delete T;
		return 1;
		}

	cout << "\n\nCHECKING GRAPH " << graph_name << ".." << flush;
	algorithm_type algorithm;
	switch (alg)
		{
		case 0 : algorithm = PLAN_ORTH_QUICK	; break;
		case 1 : algorithm = PLAN_ORTH_OPTIMAL	; break;
		case 2 : algorithm = PLAN_ORTH_SLOW	; break;
		case 3 : algorithm = PLAN_UPWARD_OPTIMAL; break;
		case 4 : algorithm = PLAN_UPWARD_SLOW	; break;
		case 5 : algorithm = POLYLINE		; break;
		case 6 : algorithm = VISIBILITY		; break;
		default: cout << "\nINVALID ALGORITHM" << flush; return 1; break;
		}
	
	switch (algorithm)
		{
		case PLAN_ORTH_QUICK:
		case PLAN_ORTH_OPTIMAL:
		case PLAN_ORTH_SLOW:
			{
			int i;
			for (i = 1; i<=times; i ++)
				{
				cout << "\nsub-computation n." << i << ".." << flush;
				// planarize
				//
				cout << "\n  computing topology.." << flush;
				plan_undi_graph pug (ug);
				if (!pug.internal_consistency_check())
					{
					cout << "\nERROR IN THE PLANAR STRUCTURES" << flush;
					return 1;
					}
				cout << "\n  planar topology ok!" << flush;
				
				// apply algorithm
				//
				cout << "\n  computing orthogonal shape.." << flush;
				face ef = pug.first_face();
				orth_plan_undi_graph opug (pug,ef,algorithm,false);
				if (!opug.internal_consistency_check())
					{
					cout << "\nERROR IN THE PLANAR STRUCTURES" << flush;
					return 1;
					}
				cout << "\n  orthogonal shape ok!" << flush;
				
				cout << "\n  computing orthogonal drawings (6 different compactions).." << flush;
				dugp = new draw_undi_graph (opug,FAST_COMPACTION_REFINED); delete (dugp);
				dugp = new draw_undi_graph (opug,SLOW_COMPACTION_REFINED); delete (dugp);
				dugp = new draw_undi_graph (opug,FAST_REGULAR_COMPACTION_1_REFINED); delete (dugp);
				dugp = new draw_undi_graph (opug,FAST_REGULAR_COMPACTION_2_REFINED); delete (dugp);		     
				dugp = new draw_undi_graph (opug,SLOW_REGULAR_COMPACTION_1_REFINED); delete (dugp);
				dugp = new draw_undi_graph (opug,SLOW_REGULAR_COMPACTION_2_REFINED); delete (dugp);
								
				cout << "\n  orthogonal drawings ok!" << flush;
			
				// ------------------------------------
				// test for nodes with prescribed sizes
				// ------------------------------------
    				// read and set the node constraints
        			// ------------------------------------
				//

				cout << "\n  computing orthogonal drawing with expanded nodes and pins.." << flush;
				    {				    
				    string config_name = graph_name + ".conf";

				    ifstream     in_conf (config_name.c_str());
   				    string 	     symbol;
    				    int          idv,w,h;
    				    h=0; w=0;
				    gdt::gdtmap<int,int> width;
				    gdt::gdtmap<int,int> height;
				    int          max_w = 0;
				    int          max_h = 0;
				    gdtnode         v;

				    forall_nodes (v,opug)
	   				 {
					 idv = opug.id(v);
					 width[idv] = 0;
					 height[idv]= 0;
					 }

    				    in_conf >> symbol;
    				    if (symbol != "<NODE_DIMENSIONS>")
    	        			 {
    	        			 cout << "\nMissing <NODE_DIMENSIONS> symbol" << flush;
	        			 return 1;
    	        			 }

				    in_conf >> symbol;

				    while (symbol == "<NODE>" || symbol == "<PIN_ON_NODE>")
    					{
					if (symbol == "<NODE>")
						{
						in_conf >> idv;
						in_conf >> w;
						in_conf >> h;

						v = opug.find_node(idv);
						if (!v) {cout << "\nNode " << idv << " does not exist " << flush; return 1;}
						if (w > 0) opug.new_constraint_node_width (v,w);
						if (h > 0) opug.new_constraint_node_height(v,h);

						width[idv]  = w;
						height[idv] = h;

						max_w = max_w > w ? max_w : w; 
						max_h = max_h > h ? max_h : h; 

						in_conf >> symbol;
						}
					else // symbol=="<PIN_ON_NODE>"
						{
						in_conf >> idv;
						int min_dim =  width[idv] < height[idv] ? width[idv] : height[idv];
						v = opug.find_node(idv);						
						//int pinv = random(0,min_dim);
						int pinv = 0;
						pinv = min_dim;
						gdtedge ev;
						forall_edges_adjacent_node (ev,v,opug)
							opug.new_constraint_edge_attachment_wrt_previous_corner(v,ev,pinv);	
						//cout << "\nSet pin = " << pinv << " on node " << idv << flush; 						
						in_conf >> symbol;
						}
					}// end while	

				    if (symbol != "</NODE_DIMENSIONS>")
    					{
    					cout << "\nMissing </NODE_DIMENSIONS> symbol" << flush;
					return 1;
    					}
				    
				    
				    in_conf.close();

				    dugp = new draw_undi_graph (opug);
				    // for debugging 
				    double k = dugp->scale();
				    dugp->rescale(1); // rescale the coordinates of the drawing
				    gdtnode u;
				    forall_nodes (u,*dugp){
				       dugp->set_width (u,dugp->width(u)/k);
				       dugp->set_height (u,dugp->height(u)/k);
				    }  	 
				    dugp->write("dug.gdt"); 
				    // for debugging
				    delete (dugp);
				    }				  			 
				cout << "\n  orthogonal drawing ok!" << flush;
				
				cout << "\nsub-computation OK!" << flush;				
				}					
			}
			break;
			
		
		case POLYLINE:
			{
			int i;
			for (i = 1; i<=times; i ++)
				{
				cout << "\nsub-computation n." << i << ".." << flush;
				// planarize
				//
				cout << "\n  computing topology.." << flush;
				plan_undi_graph pug (ug);
				if (!pug.internal_consistency_check())
					{
					cout << "\nERROR IN THE PLANAR STRUCTURES" << flush;
					return 1;
					}
				cout << "\n  planar topology ok!" << flush;
				
				// apply algorithm
				//
				cout << "\n  computing polyline drawing.." << flush;
				face ef = pug.first_face();
				dugp = new draw_undi_graph (pug, POLYLINE, ef); delete (dugp);
				#ifdef DEBUG
                		cout << "Primo costruttore OK" << endl;
				#endif
				dugp = new draw_undi_graph (pug, POLYLINE_COMPACTED, ef); delete (dugp);
				cout << "\n  polyline drawing ok!" << flush;
				cout << "\nsub-computation OK!" << flush;
				}	
			}
			break;
		
		
		case VISIBILITY:
			{
			int i;
			for (i = 1; i<=times; i ++)
				{
				cout << "\nsub-computation n." << i << ".." << flush;
				// planarize
				//
				cout << "\n  computing topology.." << flush;
				plan_undi_graph pug (ug);
				if (!pug.internal_consistency_check())
					{
					cout << "\nERROR IN THE PLANAR STRUCTURES" << flush;
					return 1;
					}
				cout << "\n  planar topology ok!" << flush;

				// apply algorithm
				//
				cout << "\n  computing visibility drawing.." << flush;
				face ef = pug.first_face();
				dugp = new draw_undi_graph (pug, VISIBILITY, ef); delete (dugp);
				dugp = new draw_undi_graph (pug, VISIBILITY_COMPACTED, ef); delete (dugp);
				cout << "\n  visibility drawing ok!" << flush;
				cout << "\nsub-computation OK!" << flush;
				}
			}
			break;

		case PLAN_UPWARD_OPTIMAL:
		case PLAN_UPWARD_SLOW:
			{
			int i;
			for (i = 1; i<=times; i ++)
				{
				cout << "\nsub-computation n." << i << ".." << flush;

				// planarize
				//
				cout << "\n  computing upward shape.." << flush;
				gdtedge e;
				undi_graph ug_temp (ug);
				gdt::gdtlist<gdtedge> Le = ug_temp.make_embedding_cand_expanded();
				cout << "make_embedding_cand_expanded ok" << endl;

				#ifdef PJ
				plan_undi_graph pug2(ug_temp);
				cout << "Planarization without constraints OK" << endl;
				ug_temp.internal_consistency_check();
				#endif
				forall (e,Le) ug_temp.new_constraint_uncrossable_edge (e);
				#ifdef PJ
				if (ug_temp.is_connected())
					{
					cout << "Graph is connected" << endl;
					}
				else
					{
					cout << "Graph is not connected" << endl;
					}
				ug_temp.internal_consistency_check();

				#endif
				plan_undi_graph pug (ug_temp);
				cout << "planarization ok" << endl;
				pug.contract();
				cout << "contract ok" << endl;
				if (!pug.internal_consistency_check())
					{
					cout << "\nERROR IN THE PLANAR STRUCTURES" << flush;
					return 1;
					}
				cout << "\n  planar topology ok!" << flush;
				
				// apply algorithm
				//
				cout << "\n  computing quasi-upward shape.." << flush;
				face ef = pug.first_face();
				upwa_plan_undi_graph upug (pug,ef,algorithm,false);
				if (!pug.internal_consistency_check())
					{
					cout << "\nERROR IN THE PLANAR STRUCTURES" << flush;
					return 1;
					}
				cout << "\n  quasi-upward shape ok!" << flush;
				
				cout << "\n  computing quasi-upward drawing.." << flush;
				dugp = new draw_undi_graph (upug,POLYLINE); delete (dugp);
				dugp = new draw_undi_graph (upug,POLYLINE_COMPACTED); delete (dugp);
				dugp = new draw_undi_graph (upug,VISIBILITY); delete (dugp);
				dugp = new draw_undi_graph (upug,VISIBILITY_COMPACTED); delete (dugp);
				cout << "\n  quasi-upward drawing ok!" << flush;
				cout << "\nsub-computation OK!" << flush;
				}
			}
			break;	
		default: break;
		}
	cout << "\nGRAPH " << graph_name << " OK !" << flush;
	in >> graph_name;
	}
    T->pause();
    //CPU_time = used_time(T);
    CPU_time =T->get();
	cout << "\n\nTEST OK!" << flush;
    cout << "\nThe overall test required " << CPU_time << " CPU-time seconds)\n" << flush;
    delete T;
    return 0;
    }
