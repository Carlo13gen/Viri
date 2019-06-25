#include <GDT/rm3_undi_graph.h>
#include <GDT/rel_coord_orth.h>
#include <GDT/gdt_error.h>


	grid_line
	rel_coord_orth::
new_grid_line
	(
	int axis, 
	bool forward
	) 
	{
	grid_line gl = NULL;
        gdt::list_item li;

        if( _grid_line_list[axis].size() == 0 )   // this is the first grid line
		{
		gl = new _grid_line_struct(axis, 0.0);
		gl->_position_into_list = _grid_line_list[axis].push_front(gl);
  		}
	else 
		{ 

		switch(forward) 
			{

		    	case true:
				li = _grid_line_list[axis].last();    // this points to the last grid line
    				gl = new _grid_line_struct(axis, _grid_line_list[axis].inf(li)->coord()+10.0);
				gl->_position_into_list = _grid_line_list[axis].push_back(gl);
				break;

		    	case false:
				li = _grid_line_list[axis].first();    // this points to the first grid line
    				gl = new _grid_line_struct(axis, _grid_line_list[axis].inf(li)->coord()-10.0);
				gl->_position_into_list = _grid_line_list[axis].push_front(gl);
				break;
			}
  		}
	return gl;
	}


	grid_line
	rel_coord_orth::
new_grid_line
	(
	grid_line prev_gl, 
	bool forward
	) 
	{
	int axis = prev_gl->_axis;

	grid_line return_gl;

	switch(forward) 
		{
	    	case true:
		if( prev_gl->_position_into_list == _grid_line_list[axis].last() )
			return new_grid_line(axis,true);
		else 
			{
			grid_line next_gl = next_grid_line(prev_gl, true);
			if( next_gl->_coord - prev_gl->_coord < 0.001 ) _redistribution_needed = true;
			double my_coord = (prev_gl->_coord + next_gl->_coord)/2.0;
			return_gl = new _grid_line_struct(axis, my_coord);
			return_gl->_position_into_list = 
				_grid_line_list[axis].insert(return_gl,prev_gl->_position_into_list,AFTER);
			}
		break;
	    	default:  // forward == false
		if( prev_gl->_position_into_list == _grid_line_list[axis].first() )
			return new_grid_line(axis,false);
		else 
			{
			grid_line next_gl = next_grid_line(prev_gl, false);
			if( prev_gl->_coord - next_gl->_coord < 0.001 ) _redistribution_needed = true;
			double my_coord = (prev_gl->_coord + next_gl->_coord)/2.0;
			return_gl = new _grid_line_struct(axis, my_coord);
			return_gl->_position_into_list = 
				_grid_line_list[axis].insert(return_gl,prev_gl->_position_into_list,BEFORE);
			}
		break;
  		}
	return return_gl;
	}

	void
	rel_coord_orth::
new_box
	(
	gdtnode V,
	int V_id,
	grid_line col, 
	grid_line row,
	gdt::gdtlist<gdtnode>& attached_to_left_border,
	gdt::gdtlist<gdtnode>& attached_to_right_border,
	gdt::gdtlist<gdtedge>&                                  exiting_EDGES,
        gdt::gdtmap<gdtnode,gdtnode>&                           node_2_NODE,
	gdt::gdtmap<gdtnode, gdt::gdtmap< gdtedge, gdtnode > >& NODE_EDGE_2_node,
	gdt::gdtmap<grid_line, int>&                            grid_line_2_NODE_id
	) 
	{
	// create the first node
	// 
	gdtnode n_start = new_node(col, row);
	node_2_NODE[n_start] = V;

	/*
	           col
	            .    the box will be 
		    .    built in this
		    .    quarter
	            .
            . . . . o . . . . row
	            .
		    .
	*/

        // if there is an adjacency, then attach it with a bend
	//
	gdtnode adj = NULL_NODE;
	if( attached_to_left_border.size() > 0 )
		adj = attached_to_left_border.pop_front();
	if( adj != NULL_NODE ) 
		{
		/* 
			  c_adj    col
			   .       .    
			   .       .
			   .       .      
		      bend o-------o. . . . . row
		           |       .   
			   |       .   
                	   |       .  
		*/

		grid_line c_adj = _grid_line_of_node[1][adj];
		gdtnode bend  = new_node(c_adj, row);
		new_edge(bend,adj);
		new_edge(bend,n_start);
		}
	
	// create the second node above the first
	//
	grid_line prev_row = new_grid_line(row,true); // true = forward
	gdtnode n_prev = new_node(col, prev_row);
	node_2_NODE[n_prev] = V;
	new_edge(n_start,n_prev);
	
		/* 
			  c_adj    col
			   .       .    
			   .       .
			   .       .
			   .       .
			   .       o . . . . . prev_row
			   .       |
			   .       |      
		      bend o-------o . . . . . row
		           |       .   
			   |       .   
                	   |       .  
		*/
	
        // if there is a second adjacency, then attach it with a bend
	//
	adj = NULL_NODE;
	if( attached_to_left_border.size() > 0 )
		adj = attached_to_left_border.pop_front();
	if( adj != NULL_NODE ) 
		{
		/* 
			  c_adj     col
			   .         .    
			   .         .
		     bend  o---------o . . . . prev_row
			   |         |    
			   |    o----o . . . . row
			   |    |    .   
	               	   |    |    .  
		*/

		grid_line c_adj = _grid_line_of_node[1][adj];
		gdtnode bend  = new_node(c_adj, prev_row);
		new_edge(bend,adj);
		new_edge(bend,n_prev);
		}
	
	// we could stop here, but pehaps we have more adjacencies into attached_to_left_border
	//
	forall(adj,attached_to_left_border)
		{
		/* 
			            col
			             .    
			             o . . . . . curr_row
			             |
		           o---------o . . . . . prev_row
			   |         |      
			   |    o----o . . . . . row
			   |    |    .   
	               	   |    |    .  
		*/
		
		// insert a new row
		//
		grid_line curr_row = new_grid_line(prev_row, true);  // forward, that is, up		
		gdtnode n_curr = new_node(col,curr_row);
		node_2_NODE[n_curr] = V;
		new_edge(n_prev,n_curr);

		/* 
			   c_adj          col
			    .              .    
			    .              .    
		      bend  o--------------o . . . . . curr_row
			    |              |
		            |    o---------o . . . . . prev_row
			    |    |         |      
			    |    |    o----o . . . . . row
			    |    |    |    .   
	               	    |    |    |    .  
		*/
		
		// link to adj node with a bend
		//
		grid_line c_adj = _grid_line_of_node[1][adj];
		gdtnode bend = new_node(c_adj, curr_row);
		new_edge(bend,adj);
		new_edge(bend,n_curr);
		
		prev_row = curr_row;
		n_prev = n_curr;	
		}
	
	// we finished building the left side (n_prev is the node at the corner).
	
	// if there is an exiting edge then associate it with the left-top corner
	//
	gdtedge E = NULL_EDGE;
	if( exiting_EDGES.size() > 0 )
		E = exiting_EDGES.pop_front(); 
	if( E != NULL_EDGE )
		NODE_EDGE_2_node[V][E] = n_prev;

        gdtnode n_corner = n_prev;
	
	// create a second node on the right of n_corner
	//
	grid_line prev_col = new_grid_line(col,true);  // forward
	grid_line_2_NODE_id[prev_col] = V_id;
	n_prev = new_node(prev_col, prev_row);
	node_2_NODE[n_prev] = V;
	assert(n_corner);
	assert(n_prev);
	new_edge(n_corner,n_prev);

		/* 
			                  col   prev_col
					   .    .
		       o-------------------o----o . . . prev_row    
		       |                   |    
		       |    o--------------o 
		       |    |              |
		       |    |    o---------o 
		       |    |    |         |      
		       |    |    |    o----o . . . . . row
		       |    |    |    |    .   
	               |    |    |    |    .  
		*/

	// if there is a second exiting edge then associate it with n_prev
	//
	E = NULL_EDGE;
	if( exiting_EDGES.size() > 0 )
		E = exiting_EDGES.pop_front(); 
	if( E != NULL_EDGE )
		NODE_EDGE_2_node[V][E] = n_prev;
	
	// we could stop here, but pehaps we have more edges into exiting_EDGES
	//
	forall(E,exiting_EDGES)
		{
		
		// insert a new column
		//
		grid_line curr_col = new_grid_line(prev_col, true);  // forward, that is, to the right
		grid_line_2_NODE_id[curr_col] = V_id;
		gdtnode n_curr = new_node(curr_col,prev_row);
		node_2_NODE[n_curr] = V;
		NODE_EDGE_2_node[V][E] = n_curr;
		new_edge(n_prev,n_curr);

		/* 
		                               prev  curr
					  col  col   col
			                   .    .    .
					   .    .    .
		       o-------------------o----o----o . . . prev_row    
		       |                   |        
		       |    o--------------o 
		       |    |              |
		       |    |    o---------o 
		       |    |    |         |      
		       |    |    |    o----o . . . . . row
		       |    |    |    |    .   
	               |    |    |    |    .  
		*/
				
		prev_col = curr_col;
		n_prev = n_curr;	
		}

        // if there is a vertex to attach to the right border, then attach it with a bend
	//
	adj = NULL_NODE;
	if( attached_to_right_border.size() > 0 )
		adj = attached_to_right_border.pop_front();
	if( adj != NULL_NODE ) 
		{

		/* 
		                                          prev 
					  col             col   
			                   .              .    
					   .              .                 bend
		       o-------------------o----o----o----o------------------o . . . prev_row    
		       |                   |              .                  |
		       |    o--------------o              .                  |
		       |    |              |              .                  |
		       |    |    o---------o                                 |
		       |    |    |         |                                 |
		       |    |    |    o----o . . . . . row                   |
		       |    |    |    |    .   
	               |    |    |    |    .  
		*/


		grid_line c_adj = _grid_line_of_node[1][adj];
		gdtnode bend  = new_node(c_adj, prev_row);
		new_edge(bend,adj);
		new_edge(bend,n_prev);
		}
	// while there are other vertices attach them and go down
	//
	forall( adj, attached_to_right_border )
		{
		
		// find the right row
		//
		grid_line curr_row = next_grid_line(prev_row, false);  // backward, that is, downward		
		gdtnode n_curr = new_node(prev_col,curr_row);
		node_2_NODE[n_curr] = V;
		new_edge(n_prev,n_curr);

		/* 
		                                          prev 
					  col             col          c_adj
			                   .              .             .
					   .              .             .   
		       o-------------------o----o----o----o------------------o . . . prev_row    
		       |                   |              |             .    |
		       |    o--------------o              o-------------o . .| . . . curr_row
		       |    |              |              .             |    |
		       |    |    o---------o                            |    |
		       |    |    |         |                            |    |
		       |    |    |    o----o . . . . . row              |    |
		       |    |    |    |    .   
	               |    |    |    |    .  
		*/
		
		// link to adj node with a bend
		//
		grid_line c_adj = _grid_line_of_node[1][adj];
		gdtnode bend = new_node(c_adj, curr_row);
		new_edge(bend,adj);
		new_edge(bend,n_curr);
		
		prev_row = curr_row;
		n_prev = n_curr;	
		}	
	while( prev_row != row )
		{
		// find the right row
		//
		grid_line curr_row = next_grid_line(prev_row, false);  // backward, that is, downward		
		gdtnode n_curr = new_node(prev_col,curr_row);
		node_2_NODE[n_curr] = V;
		new_edge(n_prev,n_curr);

		/* 
		                                          prev 
					  col             col          
			                   .              .             
					   .              .                
		       o-------------------o----o----o----o------------------o  
		       |                   |              |                  |
		       |    o--------------o              o-------------o    | 
		       |    |              |              |             |    |
		       |    |    o---------o              o--------o    |    |
		       |    |    |         |              |        |    |    |
		       |    |    |    o----o . . . . . . .o . . . .| . .| . .| . . . row
		       |    |    |    |    .                       |    |    |
	               |    |    |    |    .  
		*/
		
		prev_row = curr_row;
		n_prev = n_curr;	
		}
	
	while( prev_col != next_grid_line(col,true) )
		{
		// find the right row
		//
		grid_line curr_col = next_grid_line(prev_col, false);  // backward		
		gdtnode n_curr = new_node(curr_col,prev_row);
		node_2_NODE[n_curr] = V;
		new_edge(n_prev,n_curr);

		/* 
		                                          prev 
					  col             col          
			                   .              .             
					   .              .                
		       o-------------------o----o----o----o------------------o  
		       |                   |              |                  |
		       |    o--------------o              o-------------o    | 
		       |    |              |              |             |    |
		       |    |    o---------o              o--------o    |    |
		       |    |    |         |              |        |    |    |
		       |    |    |    o----o . . . . o----o . . . .| . .| . .| . . . row
		       |    |    |    |    .                       |    |    |
	               |    |    |    |    .  
		*/
		
		prev_col = curr_col;
		n_prev = n_curr;	
		}
	
	// close the box
	//
	new_edge(n_prev,n_start);
	}


	rel_coord_orth::
rel_coord_orth 
	() : ulug()	
	{
	_redistribution_needed = false;
	}


	rel_coord_orth::
~rel_coord_orth 
	()	
	{
	grid_line gl;
	for (int i=0; i<=1; i++)
		forall(gl,_grid_line_list[i])
			delete gl;   
	}

	rel_coord_orth::
rel_coord_orth 
	(
	undi_graph ug,
	algorithm_type alg
	) : ulug()
	{
	if(alg != ORTH_SIMPLE_PT00)
		gdt_error("algorithm_type not handled");

	_redistribution_needed = false;


        gdt::gdtmap<gdtnode,gdtnode> node_2_NODE;  // from node of rel_coord_orth to node of input ug
	

	/* for each node of ug and each edge adjacent to it
	   the following mapping gives me the small_node of rel_coord_orth
	   I am building 

	    e1    e2    e3          e1   e2   e3
	     \    |    /            |    |    |
	      \   |   /             |    |    |
	       \  |  /              |    |    |
	        \ | /               o----o----o
		 \|/                |         |
		  O NODE N          |         |
	 */

        gdt::gdtmap<gdtnode, gdt::gdtmap< gdtedge, gdtnode > > NODE_EDGE_2_node;
	
	gdt::gdtmap<grid_line, int> grid_line_2_NODE_id;  // columns are marked with the NODE they intersect
	
	// first perform an st-numbering of the nodes
	// (graph ug must be biconnected)
	//
	gdtedge e_st = ug.first_edge();
	gdtnode s = ug.source(e_st);
	gdt::gdtnode_array<int> st_num(ug.nodes_and_edges(),0); 
	ug.st_number(e_st, s, st_num); 	// st_num[node] gives a number in [1,n] 

        gdtnode* nodes_in_order;
	nodes_in_order = new gdtnode[ug.number_of_nodes()+1];  // nodes_in_order[0] is not used
	gdtnode n;
	forall_nodes(n,ug) 
		{
		nodes_in_order[st_num[n]] = n;	
		}

        // I will insert a NODE v at a time in the order
	// given by the st-numbering contained into nodes_in_order[int]
	//
        for( int V_st=1; V_st <= ug.number_of_nodes(); V_st++) 
	//for( int V_st=1; V_st <= 4; V_st++) 
		{
		redistribute_coords(10.0);
		//redistribute_coords_if_needed(10.0);
		gdtnode V = nodes_in_order[V_st];       // now V is the current node of ug
		assert(V);
		
		int V_id = ug.id(V);

                /* build lists of entering and exiting edges of V */

		gdt::gdtlist<gdtedge>  entering_EDGES;  // entering into V, i.e., going to a vertex with lower st-number
		gdt::gdtlist<gdtedge>  exiting_EDGES;   // exiting from V, i.e., going to a vertex with higher st-number
		
		assert(ug.degree(V) != 0);
		gdtedge E = NULL_EDGE;
		forall_edges_adjacent_node(E,V,ug)
			{
			gdtnode opposite_V = ug.opposite(V,E);
			// assert(opposite_V);
			if( st_num[opposite_V] < V_st ) entering_EDGES.push_front(E);
			else exiting_EDGES.push_front(E);  // st_num[opposite_V] < V_st
			}

		gdt::gdtlist<gdtnode> attached_to_left_border;
		gdt::gdtlist<gdtnode> attached_to_right_border;
			
		// find the "median incoming edges" of V 
		//		
		while( entering_EDGES.size() > 2 ) 
			{
			// find the leftiest and rightiest
			//
			gdt::list_item li = entering_EDGES.first();
			gdtedge E = entering_EDGES.inf(li);
			gdtnode opposite_V = ug.opposite(V,E);
			gdtnode n = NODE_EDGE_2_node[opposite_V][E];

			gdt::list_item e_min_li = li;
			gdt::list_item e_max_li = li;
			gdtnode n_min = n;
			gdtnode n_max = n;
			assert(n);
			double min_x = (_grid_line_of_node[1][n])->coord(); // take the vertical coord of n
			double max_x = min_x;
			
			forall_items(li,entering_EDGES) 
				{
				gdtedge E = entering_EDGES.inf(li);
				gdtnode opposite_V = ug.opposite(V,E);
				gdtnode n = NODE_EDGE_2_node[opposite_V][E];
				double ncoord = (_grid_line_of_node[1][n])->coord();       // take the vertical coord of n
				
				if( ncoord < min_x ) 
					{
					e_min_li = li;
					n_min = n;
					min_x = ncoord;
					}
				if( ncoord > max_x )
					{
					e_max_li = li;
					n_max = n;
					max_x = ncoord;
					}
				}
			
			// remember the nodes attached to left and right
			//
			attached_to_left_border.push_front(n_min);
			attached_to_right_border.push_back(n_max);
			
			// remove the leftiest and rightiest
			//
			entering_EDGES.del(e_min_li);			
			entering_EDGES.del(e_max_li);	
			}

		// now entering_EDGES has 0, 1, or 2 elements
		//
		if( entering_EDGES.size() == 0 )    // this is the first node we draw
			{
			grid_line gl_row = new_grid_line(0,true);    // 0=hor, true=forward
			grid_line gl_col = new_grid_line(1,true);    // 1=ver, true=forward
			grid_line_2_NODE_id[gl_col] = V_id;  // columns are marked with the id of their only node
			
		        new_box(V,
			        V_id,
			        gl_col,  // col
				gl_row,  // row
	                        attached_to_left_border,
	                        attached_to_right_border,
				exiting_EDGES,
                                node_2_NODE,
                                NODE_EDGE_2_node,
				grid_line_2_NODE_id); 	
			
			}
		else if ( entering_EDGES.size() == 1 ) // there was an odd number of incoming edges and
		                                       // the one left into the list is the median one
			{
			// find the column of entering edge
			//
			gdtedge E = entering_EDGES.head();
			gdtnode opposite_V = ug.opposite(V,E);
			gdtnode n = NODE_EDGE_2_node[opposite_V][E];  // n is the node from which the edge comes
			grid_line column = _grid_line_of_node[1][n];
			
			// find the column c_good after that you want to insert your node
			//
			grid_line nc = column; // nc = next column (one step behond the one i'm searching for)
			while( nc != NULL && grid_line_2_NODE_id[nc] == ug.id(opposite_V) )
				nc = next_grid_line(nc,true);
			grid_line c_good = column; 
			c_good = ( nc == NULL ) ? 
				_grid_line_list[1].tail() : next_grid_line(nc,false);

		        grid_line gl_row = new_grid_line(0,true); 
			grid_line gl_col = new_grid_line(c_good,true);    // true=forward
			grid_line_2_NODE_id[gl_col] = V_id;
			attached_to_left_border.push_front(n);
			
		        new_box(V,
			        V_id,
			        gl_col,  // col
				gl_row,  // row
                        	attached_to_left_border,
	             	        attached_to_right_border,
				exiting_EDGES,
       	                        node_2_NODE,
               	                NODE_EDGE_2_node,
				grid_line_2_NODE_id); 	
			}
		else 		// entering_EDGES.size() == 2
				// there were an even number of entering edges and
				// the two left into the list are the median ones
			{
			// find the columns of the two entering edges
			//
			gdtedge E_min;
			gdtedge E_max;
			gdtnode n_min;
			gdtnode n_max;
			grid_line c_min;
			grid_line c_max;
			grid_line c_med;
			
			assert(entering_EDGES.size() == 2);
			
			/*
			        c_min       c_med         c_max
			         .           .             .
			         .           .             .
			  ..........................................
			         .           .             .
			         .           .             .
                             o---o---o       .             .
			     |       |       .             .
			     o       o                     .
			     |       |                     .
                             o---o---o                     .
			                           o---o---o---o---o---o
						   |                   |
			                           o---o---o---o---o---o
			*/
			
			
			// assume the first of the list is the min (we will know soon)
			//
			E_min = entering_EDGES.head();
			n_min = NODE_EDGE_2_node[ug.opposite(V,E_min)][E_min];  
			c_min = _grid_line_of_node[1][n_min];
			
			E_max = entering_EDGES.tail();
			n_max = NODE_EDGE_2_node[ug.opposite(V,E_max)][E_max];  
			c_max = _grid_line_of_node[1][n_max];
			
                        if( c_max->_coord < c_min->_coord )   // it was the other way around
				{
				gdtedge   E_tmp = E_min;
				gdtnode   n_tmp = n_min;
				grid_line c_tmp = c_min;
				
				E_min = E_max;
				n_min = n_max;
				c_min = c_max;
				
				E_max = E_tmp;
				n_max = n_tmp;
				c_max = c_tmp;
				}

			// compute c_med (that may be the same as c_max)
                        //
			int steps = 0;
			c_med = c_min;
			while ( c_med != c_max ) 
				{
				c_med = next_grid_line(c_med, true);  // one step forward
				steps++;
				}
			steps = steps/2;
			while ( steps > 0 )
				{
				c_med = next_grid_line(c_med, false);  // one step backward
				steps--;
				}
/*
assert(c_med);
std::cout << "c_min->_coord = " << c_min->_coord << " (node=" << grid_line_2_NODE_id[c_min] << ")\n";
std::cout << "c_med->_coord = " << c_med->_coord << " (node=" << grid_line_2_NODE_id[c_med] << ")\n";
std::cout << "c_max->_coord = " << c_max->_coord << " (node=" << grid_line_2_NODE_id[c_max] << ")\n";
std::cout.flush();
*/
			// find c_good, the column after that you want to insert the node
			//
			grid_line c_good;   
			grid_line l_gl = c_med;
			grid_line r_gl = c_med;
			int med_mark = grid_line_2_NODE_id[c_med];  // the id of the intersected node
			
			while( true )
				{
				
				// try a further step to the left
				//
				grid_line c_tmp = next_grid_line(l_gl,false);  // try backward
				int tmp_mark = grid_line_2_NODE_id[c_tmp];
				if( tmp_mark != med_mark ) 
					{
					c_good = c_tmp;
					break;
					}
				else
					{
					l_gl = c_tmp;
					}	
				
				
				// try a further step to the right
				//
				c_tmp = next_grid_line(r_gl,true);  // try forward
				tmp_mark = grid_line_2_NODE_id[c_tmp];
				if( tmp_mark != med_mark ) 
					{
					c_good = r_gl;
					break;
					}
				else
					{
					r_gl = c_tmp;
					}	
				}
			
			// now c_good has the right value and you can insert the node
			// after inserting two grid lines for its bottom-left corner
/*			//
grid_line titto;
if( titto = next_grid_line(c_good,true) ) 
	{
	assert( grid_line_2_NODE_id.defined(titto) );
	assert( grid_line_2_NODE_id.defined(c_good) );
	assert( grid_line_2_NODE_id[titto] != grid_line_2_NODE_id[c_good] );
	}
*/
			grid_line gl_top = new_grid_line(0,true);       // 0=hor, true=forward
			grid_line gl_ver = new_grid_line(c_good,true);  // true = forward
			grid_line_2_NODE_id[gl_ver] = V_id;
			attached_to_left_border.push_front(n_min);
			attached_to_right_border.push_back(n_max);
		        new_box(V,
			        V_id,
			        gl_ver,  // col
				gl_top,  // row
	                        attached_to_left_border,
	                        attached_to_right_border,
				exiting_EDGES,
                                node_2_NODE,
                                NODE_EDGE_2_node,
				grid_line_2_NODE_id); 	
			}
		}
	
	// set the aux_id to remember which nodes generated which boxes	
	//
	forall_nodes(n,ulug)
		{
		if( node_2_NODE.defined(n) )
			_aux_id[ulug.id(n)] = ug.id(node_2_NODE[n]);
		else
			_aux_id[ulug.id(n)] = -1;   // it is a bend
		}
	
	delete [] nodes_in_order;
	}

	
	undi_graph 
	rel_coord_orth::
get_underlying_undi_graph
	() const 
	{
	return ulug;
	}

			
	gdt::gdtpoint
	rel_coord_orth::
center
	(int id) const
	{
	gdtnode n = ulug.find_node(id);
	double x = _grid_line_of_node[1][n]->coord();  // 1 stands for vertical
	double y = _grid_line_of_node[0][n]->coord();
	return gdt::gdtpoint(x,y);  
	}
	
			
	gdt::gdtpoint
	rel_coord_orth::
center
	(gdtnode n) const
	{
	double x = _grid_line_of_node[1][n]->coord();  // 1 stands for vertical
	double y = _grid_line_of_node[0][n]->coord();
	return gdt::gdtpoint(x,y);  
	}
	
			
	double
	rel_coord_orth::
xcoord
	(gdtnode n) const
	{
	return _grid_line_of_node[1][n]->coord();  // 1 stands for vertical
	}
	
	double
	rel_coord_orth::
xcoord
	(int id) const
	{
	return _grid_line_of_node[1][ulug.find_node(id)]->coord();  // 1 stands for vertical
	}
	

	double
	rel_coord_orth::
ycoord
	(gdtnode n) const
	{
	return _grid_line_of_node[0][n]->coord();  // 0 stands for horizontal
	}


	double
	rel_coord_orth::
ycoord
	(int id) const
	{
	return _grid_line_of_node[0][ulug.find_node(id)]->coord();  // 0 stands for horizontal
	}
	

	grid_line
	rel_coord_orth::
next_grid_line
	(grid_line gl,bool forward) 
	{
	gdt::gdtlist<grid_line>&  glL = _grid_line_list[gl->get_axis()];
	switch( forward ) 
		{
		case true:
			if (gl->_position_into_list == glL.last())
				return NULL;
			else 
				return glL.inf(glL.succ(gl->_position_into_list));
		break;
		default:
			if (gl->_position_into_list == glL.first())
				return NULL;
			else
				return glL.inf(glL.pred(gl->_position_into_list));
		break;
		}
	}


	void 
	rel_coord_orth::
insert_node
	(gdtnode n, grid_line l1, grid_line l2) 
	{
  	_grid_line_of_node[l1->get_axis()][n] = l1;
        _node_pos_into_gl_list[l1->get_axis()][n] = l1->_node_list->push_front(n);

        _grid_line_of_node[l2->get_axis()][n] = l2;
        _node_pos_into_gl_list[l2->get_axis()][n] = l2->_node_list->push_front(n);
	}


	void 
	rel_coord_orth::
redistribute_coords
	(int axis, double gap) 
	{

        double d = 0.0;
  	grid_line gl;
	forall(gl, _grid_line_list[axis]) 
		{
    		gl->_coord = d;
		d += gap;
  		}
	}

