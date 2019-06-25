
#include<GDT/rm3_draw_undi_graph.h>
#include<GDT/rm3_orth_plan_undi_graph.h>
#include<GDT/rm3_upwa_plan_undi_graph.h>
#include<GDT/rm3_dime_orth_plan_undi_graph.h>
#include<GDT/stopwatch.h>

using namespace std;

    int
main
    (int argc, char* argv[])
    {
    // global variables
    //
    undi_graph ug; // input graph
    draw_undi_graph* dugp = NULL; // output drawing
        
    if (argc < 3)
      {
      // help     
      cout << "\nSYNTAX: simpletest <inputFile> <algo> <outputFile>" << flush;
      cout << "\n\tinputFile: must contain a graph in GDT format" << flush;
      cout << "\n\talgo: must be one of the following numbers:" << flush; 
      cout << "\n\t\t0 - PLAN_ORTH_QUICK" << flush; 
      cout << "\n\t\t1 - PLAN_ORTH_OPTIMAL" << flush; 
      cout << "\n\t\t2 - PLAN_ORTH_SLOW" << flush; 
      cout << "\n\t\t3 - PLAN_UPWARD_OPTIMAL" << flush; 
      cout << "\n\t\t4 - PLAN_UPWARD_SLOW" << flush; 
      cout << "\n\t\t5 - POLYLINE" << flush;
      cout << "\n\t\t6 - VISIBILITY" << flush;
      cout << "\n\toutputFile: contains the drawing in GDT format" << flush;
      return 0;
      }
    
    ug.read (argv[1]); 	 
            
    float CPU_time;
    //used_time(T);	// initialize the timer T;
    stopwatch* T=new stopwatch();

	cout << "\n\nSTARTING TEST" << flush;

    	{
	int alg = atoi(argv[2]);

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

			cout << "\n  computing orthogonal drawing by the best compaction algorithm.." << flush;
			dugp = new draw_undi_graph (opug,SLOW_REGULAR_COMPACTION_2_REFINED);
			dugp->write(argv[3]);
			delete (dugp);

			cout << "\n  orthogonal drawings ok!" << flush;
			}
			break;

		case POLYLINE:
			{
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
			dugp = new draw_undi_graph (pug, POLYLINE_COMPACTED, ef);
			dugp->write(argv[3]);
			delete (dugp);

			cout << "\n  polyline drawing ok!" << flush;

			}
			break;

		case VISIBILITY:
			{
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
			dugp = new draw_undi_graph (pug, VISIBILITY_COMPACTED, ef);
			dugp->write(argv[3]);
			delete (dugp);
			cout << "\n  visibility drawing ok!" << flush;
			}
			break;

		case PLAN_UPWARD_OPTIMAL:
		case PLAN_UPWARD_SLOW:
			{
			// planarize
			//
			cout << "\n  computing upward shape.." << flush;
			gdtedge e;
			undi_graph ug_temp (ug);
			gdt::gdtlist<gdtedge> Le = ug_temp.make_embedding_cand_expanded();
			cout << "make_embedding_cand_expanded ok" << endl;
			forall (e,Le) ug_temp.new_constraint_uncrossable_edge (e);
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
			dugp = new draw_undi_graph (upug,VISIBILITY_COMPACTED); 
			cout << "\n  quasi-upward drawing ok!" << flush;
			dugp->write(argv[3]);
			delete (dugp);
			}
			break;
		default: break;
		}
	}

    CPU_time = T->get();
    cout << "\n\nTEST OK!" << flush;
    cout << "\nThe overall test required " << CPU_time << " CPU-time seconds)\n" << flush;
    return 0;
    }
