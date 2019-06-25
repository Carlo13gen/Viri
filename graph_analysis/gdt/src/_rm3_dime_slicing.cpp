
#include <assert.h>

#include <string>
#include <GDT/gdtmap.h>
#include <GDT/gdtlist.h>

#include <GDT/rm3_global.h>
#include <GDT/rm3_undi_graph.h>
#include <GDT/rm3_plan_undi_graph.h>
#include <GDT/rm3_orth_plan_undi_graph.h>
#include <GDT/rm3_dime_orth_plan_undi_graph.h>

#include <GDT/rm3_interval_set.h>
#include <GDT/gdt_error.h>

using std::cout;
using std::endl;
using std::flush;
using std::ostream;



#define min(a,b) ((a)>(b) ? (b) : (a))
#define max(a,b) ((a)>(b) ? (a) : (b))


//////////////////////////////////////////////////////////////////////////




class dime_rect_utils
       {
	private:

	  dime_orth_plan_undi_graph& dopug;

	public:
	  dime_rect_utils(dime_orth_plan_undi_graph& g):
	    dopug(g)
	    {}



		// ------------------------------------------------------------------------------------ 
		// build horizontal/vertical flow network associated to the rectangularized orthogonal
		// representation, that is the dual directed graph for computing a compaction algorithm
		// based on flow techniques.
		//
		// fn 			= flow network associated to the orthogonal representation;
		// fn_edge 		= mapping from orthogonal gdtedge of "dopug" to "fn" gdtedge;
		// L			= list of orthogonal edges on a longest path;
		// scan_edge_slope	= vertical or horizontal
		// ------------------------------------------------------------------------------------ 

	  //   If (slp== horizontal), the flow go from left to right, each gdtedge of the flow net is associated to a dopug's vertical gdtedge.
	  // If (slp == vertical) the flow go from bottom to top, each gdtedge of the flow net is associated to a dopig's horizontal gdtedge.


	// ----------------------------------------------------------------------
	// create the nodes of "fn" each of them is associate to a face in dopug
	// no gdtnode i screated for the external face.
	// the id's of the created nodes are the same of the corresponding face 
	// the balance of the flow at such gdtnode is set to zero 
	// ----------------------------------------------------------------------


	  void build_rect_flownet (
				   slope_type slp,   // horizontal or vertical (directio of the flow
				   const gdt::gdtmap<int, int>& min,   // bounds  (doupg's edges to int, we need them only for the gdtedge that have the opposite slope of slp) 
				   const gdt::gdtmap<int, int>& max,
				   const gdt::gdtmap<int, int>& cost,  // costs (doupg's edges to int, we need them only for the gdtedge that have the opposite slope of slp)
				   flow_dire_graph& fn // (output) the computed flownet
				   ) const;

	private:
	
	
	
	
	//////////////////////////////////////////////////////////////////////////

	/* CLASS _slice_lowlev
	   An object of this class acts as a small interface to two graphs:
	   src(): graph that we want to rectangularize
   	   trg(): a copy of the source automatically created by the object itself.

   	   All the modification are performed on trg while src is kept unchanged.
   	   The object allows to split an gdtedge of trg and to perform a vertical ray shooting.
	   
	   The class provides a new  interface which filters the methods of trg() that rely on metrics
	   	   
	      trg_new_node( gdtnode n, heading_type d, int x)
              trg_initial_heading_of_border_step(border_step s)
	      trg_find_edge_leaving_node_with_heading(gdtnode n, heading_type h)
	      trg_position_of_node(gdtnode n)

	   In the case that the _slice_lowlev is initialized with "vertical", 
	   the above methods behave exactly as the dime counterpart.
	   
	   In the case that the _slice_lowlev is initialized with "horizontal"
	   the above methods let the client object view the trg() graph clockwise rotated 
	   of 90 degrees with respect to the internal representation.
         */


	
	  class _slice_lowlev
	    {
	    public:
	      typedef gdt::gdtlist<gdtnode>* shrink;

	    private:
	      typedef GDT_interval_set::interval interval;	      
	      slope_type dir;
	      
	    private:
	      const dime_orth_plan_undi_graph* _src;
	      dime_orth_plan_undi_graph* _trg;

	      /* maps vertices from _src to _trg */
	      //	      h_array<gdtnode,node> st_vertices;
	      
	      //       /* maps edges from _trg to _src */
	      //h_array<gdtedge,edge> ts_edges;
	      
	      
	      /* the interval set. One for each orizontal gdtedge in _trg */
	      
	      GDT_interval_set intrvls;

	      gdt::gdtlist< shrink > shrink_set;
	      
	      gdt::gdtmap<gdtnode, shrink> node2shrink;


	      /* map each interval to the corrisponding shrink in _trg
		 The gdtnode(s) gives the left coordinate. Remember that the
		 intervals are closed on the left and open on the
		 right. */

	      gdt::gdtmap<interval, shrink> intrvls2shrink;

	      /* opposite of the above */
	      gdt::gdtmap<shrink, interval> shrink2intrvls;




	    private:
	      /* Split an gdtedge _trg at a distance "dist" from his left gdtnode.
		 The gdtedge is identified by the associated interval.
		 Both interval and gdtedge are splitted and the mappings are updated.
		 This is intended to be complitely safe.
		 Precondition: dist must be greater than 0 and less than the length of the interval 
	      */
	      gdtnode split_edge_interval(interval i, int dist);
	      
	      /* This method computes representatives of the extremal nodes of the target dime graph.
		 The result is returned modifing the parameters.  */
	      void target_bounding_box( gdtnode& highest,
					gdtnode& lowest,
					gdtnode& leftmost,
					gdtnode& rightmost
					);
	      
	      // get the extremes of an horizontal gdtedge. Result is placed in "l" and "r".
	      void get_left_right(gdtedge e, gdtnode& l, gdtnode& r ) const;
	      
	      void ray_shoot_generic( shrink, bool up ); 
	      
	      bool is_horizontal(gdtedge e) const;


	      
	    public:
	      
	      /* CATEGORY constructors and destructors
	       */
	      
	      
	      ~_slice_lowlev()
	      {
	      //cout << "Destructor of _slice_lowlev" << endl;
	      shrink s;
	      forall(s,shrink_set) delete s;
	      }
	      
	      /* METHOD _slice_lowlev
		 Make a copy of the graph that Take the graph that is passed as
		 parameter (the source) and make a copy creating another graph (the
		 target).
	      */
	      
	      _slice_lowlev(const dime_orth_plan_undi_graph& _src, slope_type dir );
	      


	      gdtnode               trg_new_node( gdtnode n, heading_type d, int x)
		 {
		   gdtnode rslt;
		   switch( dir )
		     {
		     case horizontal:
		       {
		       heading_type d2= dime_orth_plan_undi_graph::heading_after_rotation(d,_270);;
		       rslt= _trg->new_node( n, d2, x);
		       }
		       break;
		     case vertical:
		       rslt= _trg->new_node( n, d, x);
		       break;
		     default:
		       gdt_error("unexpected slope.");
		       break;
		     }
		   return rslt;
		 }      


	      /* 
		 CATEGORY target accessors (orientation dependent)
	       */






	      heading_type       trg_initial_heading_of_border_step(border_step s) const 
		 {
		   heading_type rslt;
		   switch( dir )
		     {
		     case horizontal:
		       {
		       heading_type h= _trg->initial_heading_of_border_step(s);
		       rslt= dime_orth_plan_undi_graph::heading_after_rotation(h,_090);
		       }
		       break;
		     case vertical:
		       rslt= _trg->initial_heading_of_border_step(s);
		       break;
		     default:
		       gdt_error("unexpected slope.");
		       break;
		     }
		   return rslt;
		 }      

	      gdtedge               trg_find_edge_leaving_node_with_heading(gdtnode n, heading_type h) const 
		 {
		   gdtedge r;
		   switch( dir )
		     {
		     case horizontal:
		       {
		       heading_type h1= dime_orth_plan_undi_graph::heading_after_rotation(h,_270);
		       r= _trg->find_edge_leaving_node_with_heading(n,h1);
		       break;
		       }
		     case vertical:   
		       r= _trg->find_edge_leaving_node_with_heading(n,h);
		       break;
		     default:
		       gdt_error("unexpected slope.");
		       break;
		     }
		   return r;
		 }

	      face              trg_face_visible_from_node_looking_with_heading(gdtnode v, heading_type h) const
		 {
		   face r;
		   switch( dir )
		     {
		     case horizontal:
		       {
		       heading_type h1= dime_orth_plan_undi_graph::heading_after_rotation(h,_270);
		       r= _trg->face_visible_from_node_looking_with_heading(v,h1);
		       break;
		       }
		     case vertical:   
		       r= _trg->face_visible_from_node_looking_with_heading(v,h);
		       break;
		     default:
		       gdt_error("unexpected slope.");
		       break;
		     }
		   return r;
		 }



	      gdt::gdtpoint trg_position_of_node(gdtnode n) const ;


	      shrink node_to_shrink(gdtnode v_trg) const
		{ return node2shrink[v_trg]; }

	      /* CATEGORY target modifiers
	       */


	      /* METHOD ray_shooting_up
	      */
	      void ray_shooting_up( shrink );

	      /* METHOD ray_shooting_down
	      */

	      void ray_shooting_down( shrink );

	      /* METHOD make_box_around_target
	      */



	      /*  e_left,e_right are outputs.
		  They are setted to the left and the right gdtedge of the box */

	      void make_box_around_target(gdtedge& e_left, gdtedge& e_right);

	      void init_topoSort(gdt::gdtnode_array<int>&);

	      void init_intervals();

	      void init_shrinks();


	      bool shrink_has_up_edge(shrink) const;
	      bool shrink_has_down_edge(shrink) const;

	      /*
		 returns that graph that we want to rectangularize.
	      */
	      const dime_orth_plan_undi_graph& src() const
		{ return *_src; };

	      /* METHOD trg
		 returns that graph that we want to rectangularize.
	      */
	      const dime_orth_plan_undi_graph& trg() const
		{ return *_trg; };


    	      //gdtnode to_trg(gdtnode v) const
	      // {
	      // assert( st_vertices.defined(v) );
              // return st_vertices[v];
	      // }


	  void print_shrinks() const;

#ifndef NDEBUG
	      /*
		return always true or calls "gdt_error()"
	      */
	      bool check_consistency() const;
              bool check_shrink_consistency() const;
#endif
	    };



	public:
	  dime_orth_plan_undi_graph* _slice(slope_type slp, face& f_left, face& f_right, int& width);

	};


	      gdt::gdtpoint
	      dime_rect_utils::_slice_lowlev::
trg_position_of_node(gdtnode n) const
                 {
		 gdt::gdtpoint r;
		 gdt::gdtpoint p= _trg->position_of_node(n);
		 switch( dir )
		   {
		   case horizontal:
		     {
		     r=gdt::gdtpoint(p.ycoord(), -p.xcoord());
		       break;
		     }
		   case vertical:
		     {
		     r=p;
		       break;
		     }
		   default:
		     {
		     gdt_error("unexpected slope.");
		       break;
		     }
		   }
		 return r;
		 }



// ------------------------------------------------------------

	void
	dime_rect_utils::
build_rect_flownet
	(
	slope_type slp,
	const gdt::gdtmap<int, int>& min,
	const gdt::gdtmap<int, int>& max,
	const gdt::gdtmap<int, int>& cost,
	flow_dire_graph& fn
	) const
	{
	// ----------------------------------------------------------------------
	// create the nodes of "fn" each of them is associate to a face in dopug
	// ----------------------------------------------------------------------
	
	face f;
	forall_faces (f,dopug)
		if (f != dopug.external_face())
			{
			gdtnode ffn = fn.new_node (dopug.id(f));
			fn.set_balance(ffn,0);
			}
	
	/* ------------------------------------
	   Create the edges in "fn", each of them is assocate to an gdtedge in "dopug"
	   ----------------------------------- */

	// this is a constant during the build of the net. it is the heading that conrresponds to the left-to-right direction 
	// looking in the direction of the flow.
	const heading_type left_to_right = ( slp == horizontal )? south: east;

	gdtedge e;
	forall_edges (e,dopug)
	        {

		/*
		  HORIZONTAL FLOW ( slp==horizontal in this case)
		            |
		  ----------|----> 
	       (from)       |   (to)
		            v  left_to_right

                  VERTICAL FLOW (slp==vertical)
			  ^(to)
                  	  |
			------> left_to_right
                  	  |
                  	  |
                        (from)
		 */

		if ( dopug.slope_of_edge(e)== slp ) continue; // consider just edges that are orthogonal to the flow direction


		gdtnode from, to; // gdtnode of the flow net
		border_step s = dopug.border_step_moving_along_edge ( e, dopug.source(e) );

		// the two faces incident to the gdtedge "e" are assigned to "from" and "to"
		// according to the direction of the flow net

		//this is the direction 90 deg. clockwise with respect to "slp"
		int id_from = ( dopug.initial_heading_of_border_step(s) == left_to_right ) 
		                                                                    ? dopug.id(dopug.face_of_border_step(s))
		                                                                    : dopug.id(dopug.face_of_border_step(dopug.opposite_border_step(s)));
		int id_to = ( dopug.initial_heading_of_border_step(s) == left_to_right ) 
		                                                                    ? dopug.id(dopug.face_of_border_step(dopug.opposite_border_step(s)))
		                                                                    : dopug.id(dopug.face_of_border_step(s));
		from = fn.find_node( id_from );
		to   = fn.find_node( id_to );

		if (from==NULL_NODE || to==NULL_NODE ) continue;  // one of the faces is the external face, skip it

		int ide= dopug.id(e);
		gdtedge efn = fn.new_edge (from,to,ide);

		fn.set_edge_info (efn, // the gdtedge
				  max[ide], // lower bound
				  min[ide], // upper bound
				  cost[ide], // cost
				  0   // flow
				  );	


		//cout.form("setting %d: min %d, max %d, cost %d\n", 
		//	  fn.id(efn),
		//	  fn.get_lower_capacity(efn),
		//	  fn.get_upper_capacity(efn),
		//	  fn.get_cost(efn)
		//	  );


		}
     
	
	};


// -------------------------------------------------- METRIC PROPAGATION
     void  
     dime_orth_plan_undi_graph::
one_dimensional_metric_propagation( gdtnode v, const gdt::gdtedge_array<int>& len, slope_type dir )
     {
     if (dir != vertical && dir != horizontal ) 
        gdt_error("dir is neither horizontal nor vertical" );

     dime_orth_plan_undi_graph& dopug = *this;
     heading_type positive_dir=  (dir == horizontal) ? east : north;
     
     // compute a dfs tree
          gdt::gdtnode_array<gdtedge> father( dopug.nodes_and_edges(), NULL_EDGE);
          gdt::gdtlist<gdtnode> order; 
          {
	    gdt::gdtnode_array<bool> dummy( dopug.nodes_and_edges(), false);
	    dopug.simple_DFS( v,   	// IN	: the starting gdtnode 
			      dummy,   	// OUT	: true when a gdtnode is reached (we do not need it)
			      father, 	// OUT	: for each gdtnode the father in the DFS tree
			      order 	// OUT	: the ordered list according to which the gdtnode are visited
			      );
	  }
     
     // propagate according to the dfs tree
     order.pop();  	// erase the first element (the root of the dfs tree)
			// (we assume that such gdtnode has already right coordinates)
     gdtnode n;
     forall(n, order)
       {
	 gdtedge e_fthr= father[n];

	 gdtnode n_fthr= dopug.opposite( n, e_fthr);
	 border_step fthr2child= dopug.border_step_moving_along_edge ( e_fthr, n_fthr );
	 gdt::gdtpoint pos_fthr = dopug.position_of_node( n_fthr );
	 heading_type dir_tochild= initial_heading_of_border_step( fthr2child );
	 
	 gdt::gdtpoint n_pos;
	 
	 if ( dopug.slope_of_edge(e_fthr) == dir )
	   {    // the length is taken from the parameter len
	     int length= len[e_fthr];
	     if (dir== horizontal)
	       {
	       double x_fthr = dopug.position_of_node(n_fthr).xcoord();
	       double y = dopug.position_of_node(n_fthr).ycoord();
	       if ( dir_tochild == positive_dir )
		 n_pos= gdt::gdtpoint( x_fthr+length , y );
	       else
		 n_pos= gdt::gdtpoint( x_fthr-length , y );
	       }
	     else  // vertical
 	       {
	       double y_fthr = dopug.position_of_node(n_fthr).ycoord();
	       double x = dopug.position_of_node(n_fthr).xcoord();
	       if ( dir_tochild == positive_dir )
		 n_pos=  gdt::gdtpoint( x, y_fthr+length );
	       else
		 n_pos= gdt::gdtpoint( x, y_fthr-length );
	       }
	   }
	 else
	   {   // the length should not change
	     double x,y;
	     if (dir== horizontal) // that is "e_fthr" is vertical
	       {
	       x = dopug.position_of_node(n_fthr).xcoord();
	       y = dopug.position_of_node(n).ycoord(); // y is unchanged
	       }
	     else  // that is "e_fthr" is horizontal
 	       {
	       x = dopug.position_of_node(n).xcoord(); // x is unchanged
	       y = dopug.position_of_node(n_fthr).ycoord();
	       }
	       n_pos = gdt::gdtpoint( x , y );
	   }

	 // this is the only plance where we must access the private part of *this object.
	 //cout << " propagate: setting " << dopug.id(n) << " from "<< dopug.id(e_fthr)<< " at " << n_pos  << endl; 
	 this->set_position_of_node( n, n_pos);
       }
     }



// -------------------------------------------------- PACKING
        void
        dime_orth_plan_undi_graph::
one_dimensional_tieing_for_expanded_nodes (
					   const gdt::gdtmap<int, int>& min,
					   const gdt::gdtmap<int, int>& max,
					   const gdt::gdtmap<int, int>& cost,
					   slope_type slp,
					   const gdt::gdtmap<int, int>& extent,
					   unsigned int min_tieing_dist
					  )
        {
	dime_orth_plan_undi_graph dopug(*this);

	gdt::gdtmap<int, int> min1(0); //(min);
	gdt::gdtmap<int, int> max1(0); //(max);
	gdt::gdtmap<int, int> cost1(0); //(cost);

	gdtedge e;
	forall_edges (e,dopug)
		{
		int ide = dopug.id(e);		
		min1[ide] = min[ide];
		max1[ide] = max[ide];
		cost1[ide] = cost[ide];
		}

	switch (slp)
	  {
	  case vertical:
	       {
	       gdtnode v;
	       forall_nodes(v, dopug)
		     {
		     int v_id = dopug.id(v);
              	     if( extent[v_id]!=0 )
		          {

			    /*
			         n_high
			      o---o-----
			      |   |
			      |	  |
			      o	  |
			      |	  e
			      o	  |
			      |	  |
			      o	  |
			      |   |                 
			      o---o-----
			     v  ^  n_low
			        |
				+--zero length!
			     
			    */


			  gdtedge _e=dopug.find_edge_leaving_node_with_heading( v, east );
			  gdtnode n_low= dopug.new_node(_e, v, 0);

			  //dopug.print(COMPLETE);
			  border_step s= dopug.border_step_moving_along_edge( dopug.find_edge_leaving_node_with_heading( v, north ), v );
			  
			  while( dopug.initial_heading_of_border_step(s)!=east )
			     s=dopug.cyclic_succ_border_step(s);
			  
			  gdtnode n_high= dopug.new_node(dopug.edge_of_border_step(s),dopug.start_of_border_step(s), 0);
			  
			  gdtedge e= dopug.new_straight_edge(n_low, n_high);

			  min1[dopug.id(e)]= max1[dopug.id(e)]= extent[v_id];
			  cost1[dopug.id(e)]= 0;
		          }
		     }
 
	       }
	       break;

	  case horizontal:
	       {
	       gdtnode v;
	       forall_nodes(v, dopug)
		     {
		     int v_id = dopug.id(v);
              	     if( extent[v_id]!=0 )
		          {
			    /*

			      |                    |
			      |                    |
		      n_left  o---------e----------o n_right
			      |                    | <-------- zero length!
			      o-o-o----o-o--o-o----o
                             v

			     */



			  gdtedge _e=dopug.find_edge_leaving_node_with_heading( v, north );
			  gdtnode n_left= dopug.new_node(_e, v, 0);

			  border_step _s= dopug.border_step_moving_along_edge( dopug.find_edge_leaving_node_with_heading( v, east ), v );
			  border_step s= dopug.opposite_border_step(_s); // this points west toward v
			  
			  while( dopug.initial_heading_of_border_step(s)!=south )
			     s=dopug.cyclic_pred_border_step(s);
			  
			  gdtnode n_right= dopug.new_node(dopug.edge_of_border_step(s),dopug.stop_of_border_step(s), 0);
			     
			  gdtedge e= dopug.new_straight_edge(n_left, n_right);

			  min1[dopug.id(e)]= max1[dopug.id(e)]= extent[v_id];
			  cost1[dopug.id(e)]= 0;
		          }
		     }
 
	       }
	       break;

	  default:
               gdt_error("wrong direction");
	       break;
	  }

        // debug
        /*
            {
            draw_undi_graph dug(0, dopug);
            dug.rescale(1);
            cout << " -------- btieing.gdt" << endl;
            gdtnode n1;
            forall_nodes(n1,dug)
              {
              gdt::gdtpoint p= dug.center(n1);
              cout << dug.id(n1) << ' ' << p << endl;
              }
            cout << " -------- end btieing.gdt" << endl;
            dug.write("btieing.gdt");
            }
        */

	dopug.one_dimensional_tieing ( min1, max1, cost1, slp, min_tieing_dist);


	gdtnode n;
	forall_nodes( n, *this )
	  {
	    int id=this->id(n);
	    //cout << "assigning coord to gdtnode "<< this->id(n) << endl;
	    gdtnode n_dopug= dopug.find_node(id);
	    gdt::gdtpoint p= dopug.position_of_node( n_dopug );
	    set_position_of_node(n, p );
	  }
        }



        void
        dime_orth_plan_undi_graph::
one_dimensional_tieing ( const gdt::gdtmap<int, int>& min,
			 const gdt::gdtmap<int, int>& max,
			 const gdt::gdtmap<int, int>& cost,
			 slope_type slp,
			 unsigned int min_tieing_dist
			 )
        {
	
	// -----------------------
	// check the preconditions
	// -----------------------
	
	if (number_of_bends()>0)
		gdt_error("graph not linearized." );

	
	// --------------------------------------------
	// execute a copy "dg" of the current dime, and 
	// unmark all the nodes and edges of "dg"
	// --------------------------------------------
	
	dime_orth_plan_undi_graph dg(*this); 
	    
	    {
	    gdtedge e_temp;
	    gdtnode v_temp;
	    forall_edges (e_temp,dg) dg.unmark_all(e_temp);
	    forall_nodes (v_temp,dg) dg.unmark_all(v_temp);
	    }
	
	 
	face f_source, f_sink;
	int width;


	dime_orth_plan_undi_graph* sliced_graph= dime_rect_utils(dg)._slice(slp, f_source, f_sink, width); 


	{
	  //draw_undi_graph dug(0, *sliced_graph);
	  //dug.write("sliced.gdt");
	}
		
	//sliced_graph->orth_plan_undi_graph::print();
	
	// --------------------------------
	// complete the specifications of 
	// bounds and costs for dummy edges
	// --------------------------------
	
	gdt::gdtmap<int, int> min1 (-1);  //  = min;
	gdt::gdtmap<int, int> max1 (-1);  //= max;
	gdt::gdtmap<int, int> cost1 (-1); //= cost;
	  {
	  gdtedge e_tmp;
      	  forall_edges(e_tmp, *sliced_graph)
	    {
	    int ide_tmp = sliced_graph->id(e_tmp);
	    if ( sliced_graph->is_marked(e_tmp, RM3_ADDED_BY_RECTANGULARIZATION) )
	        {
	        //cout << "RM3_ADDED_BY_RECTANGULARIZATION: " << ide_tmp << endl ;
	        min1[ide_tmp]  = min_tieing_dist;
	        max1[ide_tmp]  = INFINITE;
	        cost1[ide_tmp] = 0;
	        }
	    else
	    	{
		min1[ide_tmp]  = min[ide_tmp];
	        max1[ide_tmp]  = max[ide_tmp];
	        cost1[ide_tmp] = cost[ide_tmp];
		}
	    }	
	  }
	
	  
	// -------------------------------------------------------
	// build the flow network associated with the sliced graph
	// and compute a minimum cost flow on it
	// -------------------------------------------------------
	
	flow_dire_graph flownet;
	dime_rect_utils(*sliced_graph).build_rect_flownet(
							  ( slp == horizontal? vertical: horizontal ),
							  min1,
							  max1,
							  cost1,
							  flownet
							  );


	// debugging
        /*
	{
	  //draw_undi_graph dug(flownet, POLYLINE);
	  //dug.write("fn.gdt");
	}
        */

	gdtnode n_source = flownet.find_node( sliced_graph->id(f_source) );
	gdtnode n_sink = flownet.find_node( sliced_graph->id(f_sink) );
	flownet.set_balance(n_source, width);
	flownet.set_balance(n_sink, -width);
	
	flownet.min_cost_flow ();
	
	// debugging
	/*  
	cout << "flownet:" << endl;
	gdtedge efn;
	forall_edges(efn, flownet)
	  {
	    cout.form("gdtedge %d: min %d, max %d, cost %d, flow %d\n", 
		      flownet.id(efn),
		      flownet.get_lower_capacity(efn),
		      flownet.get_upper_capacity(efn),
		      flownet.get_cost(efn),
		      flownet.get_flow(efn)
		      );
	 }
	*/

	/*
	
	gdtnode nfn;
	forall_nodes(nfn, flownet)
	  {
	    cout.form("gdtnode %d: balance %d\n", 
		      flownet.id(nfn),
		      flownet.get_balance(nfn)
		      );
	  }
	}
	print(COMPLETE);
	-----------------------------------------------*/
	
	// ------------------------------------
	// assign the length of the edges "len"
	// according to the flow values
	// ------------------------------------
	
        gdt::gdtedge_array<int> len(nodes_and_edges(), 0);
	gdtedge e1;
	forall_edges(e1, *this)
	  	{
	   	if ( slope_of_edge(e1)==slp)
	     		{
	     		gdtedge fe = flownet.find_edge( id(e1) );
	     		len[e1]= flownet.get_flow( fe ); 	
	     		}
	  	}
	
	// -----------------------------------
	// assign the gdtnode-coordinates
	// according to the gdtedge-lengths "len"
	// -----------------------------------
	
	one_dimensional_metric_propagation( first_node(), len, slp );
	
	// try						
	delete sliced_graph;
	}






      bool
      dime_rect_utils::_slice_lowlev::
shrink_has_up_edge(shrink sh) const
      {
      gdtnode n;
      forall(n, *sh )  // traverse the shrink left to right
	if ( trg_find_edge_leaving_node_with_heading ( n, north ) ) 
	  return true;

      return false;
      }


      bool
      dime_rect_utils::_slice_lowlev::
shrink_has_down_edge(shrink sh) const
      {
      gdtnode n;
      forall(n, *sh )  // traverse the shrink left to right
	if ( trg_find_edge_leaving_node_with_heading ( n, south ) ) 
	  return true;

      return false;
      }





      dime_rect_utils::_slice_lowlev::
_slice_lowlev(const dime_orth_plan_undi_graph& _src1, slope_type slp):
         dir(slp),
         _src(&_src1)
         { 
	   //	   gdtnode v_src; // a gdtnode of _src dime graph 
	   _trg = new dime_orth_plan_undi_graph(_src1);  
	   /*	   
	   forall_nodes(v_src, _src)
	      {
	      gdtnode v_trg = _trg->corresponding_node(v_src, _src);
	      st_vertices[v_src]= v_trg;
              }
	   */
	 }





/* this check the consistency between the gdtedge geometry, the intervals and the mappings. It assume that "is_horizontal()" and "get_left_right" are correct
 */
#ifndef NDEBUG


      bool
      dime_rect_utils::_slice_lowlev::
check_shrink_consistency() const
      {
      gdt::gdtnode_array<int> count( _trg->nodes_and_edges(), 0 );


      shrink shr;
      forall(shr, shrink_set)
	{
	  gdtnode n;
	  gdt::gdtpoint n_pos= trg_position_of_node( shr->front());
	  forall(n, *shr)
	    {
	      ++count[n];
	      if ( trg_position_of_node(n)!= n_pos )
		   gdt_error("shrinked cluster contains not all overlapped nodes." );

	      if ( node2shrink[n]!=shr) 
                   gdt_error("wrong node2shrink mapping." );

	      // WARNING: it does not verify the sequence. front() should be the bottom gdtnode while last() should be the top gdtnode.
	      
	    }

	}

      gdtnode n;
      forall_nodes(n,*_trg)
	{
	  if (count[n]!=1) 
	    {
	      cout << endl << _trg->id(n) << " counted " << count[n] << " times." << endl;
	      gdt_error("gdtnode is not a partition" );
	    }
	}

      return true;
      }


	  void 
	  dime_rect_utils::_slice_lowlev::
print_shrinks() const
      {

      cout << "Shrinks:" <<endl;
      shrink shr;
      forall( shr, shrink_set)
	{
	gdtnode n;
	forall(n, *shr)
	    cout << _trg->id(n) << " ";
        if ( shrink2intrvls.defined(shr) )
            {
            interval i= shrink2intrvls[shr];
            cout<< " -- "<< intrvls.left(i)<< " "<< intrvls.right(i)<< " "<< intrvls.altitude(i)<< " " << endl;
            }
        else
            cout<< " -- interval undefined" << endl;
	}

      }



      bool
      dime_rect_utils::_slice_lowlev::
check_consistency() const
      {

      check_shrink_consistency();

      shrink shr;
      forall(shr, shrink_set)
	{

	  // verfying mapping consistency interval<->shrink
          if ( !shrink2intrvls.defined(shr) && shrink_has_down_edge(shr) && shrink_has_up_edge(shr))
              continue; // this should not be necessarily mapped on intervals
	  if ( !shrink2intrvls.defined(shr) ) gdt_error("shrink is not mapped to an interval." );
	  interval i= shrink2intrvls[shr];
	  if (i==NULL) gdt_error("shrink is mapped to a NULL interval." );
	  if ( !intrvls2shrink.defined(i) ) gdt_error("interval is not mapped to a shrink." );
	  shrink shr_alias= intrvls2shrink[i];
	  if( shr_alias == NULL ) gdt_error("interval mapped to NULL shrink." );
	  if( shr_alias != shr ) gdt_error("intrvls2shrink[ shrink2intrvls[n] ]!=n" );

	  
	  shrink shr_left= shr;
	  gdtedge e = trg_find_edge_leaving_node_with_heading(shr->back(), east);
	  shrink shr_right= (e != NULL_EDGE)
	                                ? node2shrink[_trg->opposite(shr->back(), e)]
	                                : shr_left;

	  

	  
	  // verfying geometric consistency
	  gdt::gdtpoint pl= trg_position_of_node(shr_left->front());
	  gdt::gdtpoint pr= trg_position_of_node(shr_right->front());

	  //if ( pl.ycoord() != pr.ycoord() ) gdt_error("gdtedge is not horizontal even if detected so" );
	  double diff = max(pl.ycoord() - pr.ycoord(), pr.ycoord() - pl.ycoord());
	  if (diff > 0.005) gdt_error("gdtedge is not horizontal if detected so" );
	  
	  
	  
	  //	  int y= (int) pl.ycoord();
	  int x_left= (int) pl.xcoord();
	  int x_right= (int) pr.xcoord();
	  

	  //	  double ia= intrvls.altitude(i);
	  double il= intrvls.left(i);
	  double ir= intrvls.right(i);
	  
	  // if ( ia != y ) gdt_error("wrong altitude" );
	  if ( il != x_left ) gdt_error("wrong left end coordinate" );
	  if ( ir != x_right ) gdt_error("wrong right end coordinate" );
	}



      return true;
      }
#endif



      bool 
      dime_rect_utils::_slice_lowlev::
is_horizontal(gdtedge e) const
      {
      border_step s=  _trg->border_step_moving_along_edge( e, _trg->source(e) );
      heading_type h= trg_initial_heading_of_border_step( s );
      return (h==east || h==west);
      }





      void 
      dime_rect_utils::_slice_lowlev::
init_topoSort( gdt::gdtnode_array<int>& theSort )
      { 

      {
	//draw_undi_graph dug(0,*_trg);
	//dug.write("startsort.gdt");
      }

      

      // create the st upward, contracting the chains
      plan_undi_graph* st_pug;
      plan_undi_graph pug_v, pug_h;
          {
      	  gdt::gdtnode_map<gdtnode>  dummyv(_trg->nodes_and_edges(), NULL_NODE);
      	  gdt::gdtnode_map<gdtnode>  dummyh(_trg->nodes_and_edges(), NULL_NODE);
      	  _trg->build_embedded_posets( pug_v, pug_h, dummyv, dummyh ); // here a source and a sink are inserted
	  /*
	  gdtnode n;
	  forall_nodes(n, *_trg)
	      cout << "nodo " << _trg->id(n) << " a cluster " << pug_h.id(dummyh[n]) << endl;
	  */
	  st_pug= dir==vertical ? &pug_v : &pug_h ;
          }
	  

      // get the source
	  
          int source_id;
          {
          gdtnode s,dummy_t;
          st_pug->is_st_digraph(s, dummy_t);
	  source_id= st_pug->id(s);
	  }


      // now the nodes of the st graph represent the horizontal chains of the _trg graph.

      // build a copy using the dire_graph class to use its topological sort
      dire_graph st_dg(*st_pug);

      //{ // debug
		//  st_dg.print(COMPLETE);
      //}

      // compute the topological sort
	  gdt::gdtlist<gdtnode> order;
      
	  gdt::gdtnode_array<bool> dummy_reached(st_dg.nodes_and_edges(),false);
	  gdtnode s= st_dg.find_node(source_id);
	  //cout << "after find_node the id is " << st_dg.id(s) <<endl;
      st_dg.topological_sort( s, dummy_reached, order );

      //{ // debug
	  //  cout << "order:" << order << endl;
      //}		  

	  order.pop_front(); // delete the fictitious s and t gdtnode
	  order.pop_back();

      // assign the numbering according to 'order' 
	  // reverse the in case the direction is horizontal because the particular rotation with which 
	  // the graph is viewed (rotated clockwise)
          if (dir==horizontal) order.reverse(); 

      int numbering=0;
      gdtnode n;
      forall(n, order)
        {
        
	int id= st_dg.id(n);
	gdtnode n_trg= _trg->find_node(id);

	assert(n_trg);

	gdt::gdtlist<gdtnode> L;
	if (dir== vertical)
	     L= _trg->nodes_reachable_moving_from_node_with_slope( n_trg, horizontal );
	else
	     L= _trg->nodes_reachable_moving_from_node_with_slope( n_trg, vertical );
	
        gdtnode n_iter;
	forall(n_iter, L)
	  theSort[n_iter]= numbering;
	++numbering;
	}
      }



      void 
      dime_rect_utils::_slice_lowlev::
init_shrinks()
      {
	gdt::gdtnode_map<bool> visited(_trg->nodes_and_edges(), false);
	gdtnode n;
	forall_nodes( n, *_trg )
          {
	    gdt::gdtpoint p= trg_position_of_node(n);
	    if (!visited[n])
	      {
		shrink theShrink= new gdt::gdtlist<gdtnode>();  // create the new shrink
		shrink_set.append( theShrink );
		

		// find the leftmost gdtnode of the chain
		gdtnode current=n;
		while(true)
                  {
		  gdtedge e_next= trg_find_edge_leaving_node_with_heading(current, west);
		  if ( e_next == NULL_EDGE ) break;
		  gdtnode next= _trg->opposite(current, e_next);
                  if ( trg_position_of_node(next) != p) break;
		  current= next;
		  }

		// current contains the leftmost gdtnode, now
		
		while(true)
                  {
                  visited[current]=true;
		  theShrink->append( current );
		  node2shrink[current]=theShrink;

		  gdtedge e_next=trg_find_edge_leaving_node_with_heading(current, east);
		  if ( e_next == NULL_EDGE ) break;
		  gdtnode next= _trg->opposite(current,e_next );
                  if ( trg_position_of_node(next) != p) break;
		  current= next;
		  }
	      }
	  }
	//assert(check_shrink_consistency());
      }
      



      void 
      dime_rect_utils::_slice_lowlev::
init_intervals()
      {
      gdt::gdtnode_array<int> topoSort(_trg->nodes_and_edges(), 0);
      init_topoSort(topoSort);
      double epsilon = 0.0001;

      shrink shr;
      forall(shr, shrink_set)
	      {
	      gdtedge e_trg= trg_find_edge_leaving_node_with_heading( shr->back(), east);

              if ( shrink_has_down_edge(shr) &&
                   shrink_has_up_edge(shr) && 
                   e_trg==NULL  // shr has no right gdtedge
                   )
                   continue; // no interval mapped to this since it can never be hit

	      shrink shr_left= shr;
	      gdtnode n_right= (e_trg != NULL_EDGE) 
                                  ? _trg->opposite( shr->back(), e_trg)
		                  : shr_left->back();
 
	      gdt::gdtpoint pl= trg_position_of_node(shr_left->front());
	      gdt::gdtpoint pr= trg_position_of_node(n_right);

	      //border_step s= 	_trg->border_step_moving_along_edge (e_trg, n_trg);
	      //heading_type h= _trg->initial_heading_of_border_step(s);
	      //cout << " init_intrv: gdtnode " << _trg->id(n_trg) << " gdtedge " << _trg->id(e_trg) << " right gdtnode " << _trg->id(n_right) << " heading " << h << endl ;

	      //double diff = max(pl.ycoord() - pr.ycoord(), pr.ycoord() - pl.ycoord());
	      //if (diff > 0.005) gdt_error("gdtedge is not horizontal if detected so" );


	      interval i = intrvls.new_interval(pl.xcoord(), pr.xcoord(), pl.ycoord()+epsilon*topoSort[shr_left->front()]);
	      intrvls2shrink[i]=shr;
	      shrink2intrvls[shr]= i;
              }

      //      intrvls.print(cout);
      //assert( check_consistency() );
      }  


      void
      dime_rect_utils::_slice_lowlev::
get_left_right(gdtedge e, gdtnode& l, gdtnode& r ) const
      {
	gdtnode v= _trg->source(e);
	border_step s= _trg->border_step_moving_along_edge(e,v);
	heading_type h= trg_initial_heading_of_border_step(s);
	if (h == west)
	  {
	  l=_trg->stop_of_border_step(s);
	  r=_trg->start_of_border_step(s);
	  }
	else
	  {
	  r=_trg->stop_of_border_step(s);
	  l=_trg->start_of_border_step(s);
	  }
      }

      gdtnode
      dime_rect_utils::_slice_lowlev::
split_edge_interval( interval i, int dist )
         {
	   //assert(check_consistency());
	   /* 
	      BEFORE

             v_left           
              O---------e---------O      (associated to interval "i")  


	      AFTER

             v_left         n_new
              O---e_left------O---e_right------O    (associated to intervals i_left and i_right)  
	      
	    */

	   /* get interval parameters */
	 double a= intrvls.altitude(i);
	 double l= intrvls.left(i);
	 double r= intrvls.right(i);
	 
	 if( dist <= 0 || dist >= r-l )
	      gdt_error("split position out of range");

	 shrink shr_left= intrvls2shrink[i];
	 gdtnode n_left= shr_left->back();
	 gdtedge e= trg_find_edge_leaving_node_with_heading(n_left, east);

	 // split the gdtedge
	 gdtnode n_new= _trg->new_node(e,n_left, dist);

	 // update the interval set
	 intrvls.del_interval(i);
	 interval i_left= intrvls.new_interval(l, l+dist, a);
	 interval i_right= intrvls.new_interval(l+dist, r, a);

	 // update shrinks:

	 shrink shr_new= new gdt::gdtlist<gdtnode>();	 
	 shrink_set.append( shr_new );
	 shr_new->append( n_new );

	 node2shrink[n_new]= shr_new;


	 // update mappings shr<>intrvls mappings
	 intrvls2shrink.undefine(i);

	 intrvls2shrink[i_left]= shr_left;
	 intrvls2shrink[i_right]= shr_new;

	 shrink2intrvls.undefine(shr_left);

	 shrink2intrvls[shr_left]= i_left;
	 shrink2intrvls[shr_new]= i_right;

	 //assert(check_consistency());

	 _trg->mark(n_new, RM3_ADDED_BY_RECTANGULARIZATION);
	 return n_new;
	 }



      void dime_rect_utils::_slice_lowlev::
ray_shoot_generic( shrink shr, bool up)
      {
        //gdtnode v = shr->front();
        //int v_id= _trg->id(v);
	gdt::gdtpoint pv=trg_position_of_node( shr->front() );

	//assert(check_consistency());

	/*
	draw_undi_graph dug(0,*_trg);
	dug.write("dbg1.gdt");

	{
	  cout << "--------" <<endl;
	  gdtnode n;
	  forall_nodes(n, *_trg)
	    {
	      cout << _trg->id(n) << " " << node2intrvls[n] << endl;
	    }
	}
	*/

	
	interval j=shrink2intrvls[shr];
        double altitude= intrvls.altitude(j);
	
	//cout << "shooting from " << _trg->id(shr->front()) << " at altitude" << altitude << endl;
	
	interval i=  up   ? intrvls.ray_shooting_up( pv.xcoord(), altitude )
	                  : intrvls.ray_shooting_down( pv.xcoord(), altitude );


	if ( i== NULL )
	  gdt_error("no interval hit, internal error, maybe the box is open?");

	double length= intrvls.right(i) - intrvls.left(i);


	double dist= (int)pv.xcoord()-intrvls.left(i);

	if ( dist<0 || ( length != 0 && dist >= length ) )
	  gdt_error("hit something out of the returned interval, internal error");
	
	// split the gdtnode or devise the gdtnode hit between the two extremes

	
	gdtnode n_hit= ( dist==0 ) ? intrvls2shrink[i]->front()
                                : split_edge_interval( i, dist);                                  
	


	// debug junk
	/*
       {
	 draw_undi_graph dug(0, trg());
	 dug.write("beforese.gdt");
	 cout << "==============" << flush;
	 cout << "\nstraight gdtedge from " << trg().id(shr->front()) << " to " <<trg().id(n_hit)  << endl;	 
	 _trg->print(COMPLETE);
       }
       */


	// the new gdtedge
		//Debug by Marcandalli
		//
		
       	gdtedge e_new=_trg->new_straight_edge(shr->front(),n_hit);
	_trg->mark(e_new, RM3_ADDED_BY_RECTANGULARIZATION);
 
 
 
 
      }



      void dime_rect_utils::_slice_lowlev::
ray_shooting_down( shrink shr)
      {
	if ( !shrink_has_down_edge(shr) )
	  ray_shoot_generic( shr, false); 
      }




      void dime_rect_utils::_slice_lowlev::
ray_shooting_up( shrink shr)
      {
	if ( !shrink_has_up_edge(shr) )
	  ray_shoot_generic( shr, true); 
      }



      void
      dime_rect_utils::_slice_lowlev::
target_bounding_box( gdtnode& highest,
		     gdtnode& lowest,
		     gdtnode& leftmost,
		     gdtnode& rightmost
		     )
	 {
	 highest= lowest= leftmost= rightmost= _trg->first_node();  // init everything with a gdtnode (wathever)

	 double maxX, maxY, minX, minY;
	 // init the above variable
	    {
	    gdt::gdtpoint p= trg_position_of_node(highest);
	    maxX= minX= p.xcoord();
	    maxY= minY= p.ycoord();
	    }

	 gdtnode v;
	 forall_nodes(v, *_trg)
	     {
	     gdt::gdtpoint p = trg_position_of_node(v);
	     if ( p.xcoord() >= maxX && trg_face_visible_from_node_looking_with_heading(v, east) ==_trg->external_face() ) { maxX= p.xcoord(); rightmost= v; }
	     if ( p.ycoord() >= maxY && trg_face_visible_from_node_looking_with_heading(v, north)==_trg->external_face() ) { maxY= p.ycoord(); highest= v; }
	     if ( p.xcoord() <= minX && trg_face_visible_from_node_looking_with_heading(v, west) ==_trg->external_face() ) { minX= p.xcoord(); leftmost= v; }
	     if ( p.ycoord() <= minY && trg_face_visible_from_node_looking_with_heading(v, south)==_trg->external_face() ) { minY= p.ycoord(); lowest=v; }
	     }
	 }



      void dime_rect_utils::_slice_lowlev::
make_box_around_target(gdtedge& e_left, gdtedge& e_right)
      { 
      gdtnode highest, rightmost;
      double maxX, maxY, minX, minY;
      
      // search the highest gdtnode
	 {
	 gdtnode lowest, leftmost;
	 target_bounding_box( highest, lowest, leftmost, rightmost );
	 maxX= trg_position_of_node(rightmost).xcoord();
	 maxY= trg_position_of_node(highest).ycoord();
	 minX= trg_position_of_node(leftmost).xcoord();
	 minY= trg_position_of_node(lowest).ycoord();
	 }



      double deltaX= maxX - minX;
      double deltaY= maxY - minY;

      // when this is different from zero create a lot of space in the box in order for the graph to expand his width
      // This expansion is useful for diminuishing the gdtedge length or to satisfy a constraint on the edges.
      const int pad= 10; 

      gdtnode x = trg_new_node( highest, north, 1);
      _trg->mark( x, RM3_ADDED_BY_RECTANGULARIZATION );
      _trg->mark( _trg->first_adj_edge(x), RM3_ADDED_BY_RECTANGULARIZATION );

      gdt::gdtpoint p_x= trg_position_of_node(x);
      gdt::gdtpoint p_rm= trg_position_of_node(rightmost);

      gdtnode v_top_right= trg_new_node( x, east, p_rm.xcoord()-p_x.xcoord()+1);
         _trg->mark( v_top_right, RM3_ADDED_BY_RECTANGULARIZATION );
         _trg->mark( _trg->first_adj_edge(v_top_right), RM3_ADDED_BY_RECTANGULARIZATION );

      gdtnode v_bottom_right= trg_new_node( v_top_right, south, deltaY + 2 +pad );
         _trg->mark( v_bottom_right, RM3_ADDED_BY_RECTANGULARIZATION );
      e_right= _trg->first_adj_edge(v_bottom_right);
         _trg->mark( e_right, RM3_ADDED_BY_RECTANGULARIZATION );

      gdtnode v_bottom_left= trg_new_node( v_bottom_right, west, deltaX + 2 );

         _trg->mark( v_bottom_left, RM3_ADDED_BY_RECTANGULARIZATION );
         _trg->mark( _trg->first_adj_edge(v_bottom_left), RM3_ADDED_BY_RECTANGULARIZATION );

      gdtnode v_top_left= trg_new_node( v_bottom_left, north, deltaY + 2 +pad);
         _trg->mark( v_top_left, RM3_ADDED_BY_RECTANGULARIZATION );
      e_left= _trg->first_adj_edge(v_top_left);
         _trg->mark( e_left, RM3_ADDED_BY_RECTANGULARIZATION );	 

      gdtedge e_top2 = _trg->new_straight_edge ( v_top_left, x);
          _trg->mark( e_top2, RM3_ADDED_BY_RECTANGULARIZATION );	 
	 
      }

      
//----------------------------------------------------------------------------------

    dime_orth_plan_undi_graph*
    dime_rect_utils::
_slice(slope_type dir, face& f_left, face& f_right, int& width)
   {
   const dime_orth_plan_undi_graph& src_dime = dopug; // alias for this
   _slice_lowlev M(src_dime,dir);
   
  
   gdtedge e_left, e_right;
   M.make_box_around_target(e_left,e_right); // make a box around the target dime graph

   M.init_shrinks();
   
   M.init_intervals();

   //M.src().print();

   gdtnode v_src;  // a gdtnode in g
   forall_nodes(v_src, M.src()) 
       {
	 //       gdtnode v_trg= M.to_trg(v_src);  // the corresponding gdtnode in M.trg() di v_src
       int v_id= M.src().id(v_src);

       //cout << " considering " << v_id <<endl;
       gdtnode v_trg= M.trg().find_node(v_id);
       _slice_lowlev::shrink shr= M.node_to_shrink(v_trg);

       //assert(M.check_consistency());

       // --------------- debug junk
       /*
       {
	 draw_undi_graph dug(0, M.trg());
	 dug.write("lastshoot.gdt");
	 cout << " shooting from gdtedge" << M.trg().id(v_trg) << endl;
	 M.print_shrinks();
       }
       */

       switch ( M.trg().degree(v_trg) )
	 {
         case 0:
	   gdt_error("degree zero detected, maybe graph is not connected?" );
	   break;
	 case 1:
	   {
	   border_step bs= M.trg().border_steps_starting_at_node(v_trg).front(); // the only border_step starting from v
           heading_type h= M.trg_initial_heading_of_border_step(bs);
	   
           switch ( h )
	     {
	     case 0: // north
                 M.ray_shooting_down(shr);
                 break;
	     case 2: // south
                 M.ray_shooting_up(shr); 
                 break;
	     case 1: // east
	     case 3: // west
                 M.ray_shooting_down(shr); 
                 M.ray_shooting_up(shr); 
                 break;
             default:
                 break;
	     }
	   }
           break;
         case 2:
	   {
	   gdt::gdtlist<border_step> lbs= M.trg().border_steps_starting_at_node(shr->front()); // lbs contains the two border step
           
           border_step bs1= lbs.front();
           border_step bs2= lbs.inf( lbs.succ(lbs.first()) );

           heading_type h1= M.trg_initial_heading_of_border_step(bs1);
           heading_type h2= M.trg_initial_heading_of_border_step(bs2);

	   angle_type delta_orientation= dime_orth_plan_undi_graph::angle_required_to_change_heading(h1,h2);

           if ( delta_orientation != _180 ) // the two edges are NOT on the same line that is 'v' is a corner.
	     {
	       if ( h1 != north && h2 != north ) // the UP direction is "empty" we can shoot up.
		 M.ray_shooting_up(shr); 
               else 
		 M.ray_shooting_down(shr);
             }
           else 
             {  /* do nothing, two edges on the same line cannot create a bend */ }
	   }
           break;
	   
         default:  // no action for higher degree
           break;
	 }
       }
   
   { // set f_left
     gdtnode n1=M.trg().source(e_left);
     gdtnode n2=M.trg().target(e_left);
     f_left =  ( M.trg_position_of_node(n1).ycoord() < M.trg_position_of_node(n2).ycoord() ) 
                     ? M.trg().right_face_moving_along_edge( e_left, n1 )
                     : M.trg().left_face_moving_along_edge( e_left, n1 );
   }

   { // set f_right & width
     gdtnode n1=M.trg().source(e_right);
     gdtnode n2=M.trg().target(e_right);
     f_right =  ( M.trg_position_of_node(n1).ycoord() < M.trg_position_of_node(n2).ycoord() ) 
                     ? M.trg().left_face_moving_along_edge( e_right, n1 )
                     : M.trg().right_face_moving_along_edge( e_right, n1 );

     width = abs( (int)M.trg_position_of_node(n1).ycoord() - (int)M.trg_position_of_node(n2).ycoord() );
   }


   return (dime_orth_plan_undi_graph*)&M.trg();
   }   



