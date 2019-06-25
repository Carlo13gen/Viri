/*! \file */

#include <GDT/rm3_undi_graph.h>
#include <GDT/gdt_error.h>

#ifndef RM3_REL_COORD_ORTH_H
#define RM3_REL_COORD_ORTH_H

class _grid_line_struct {

  friend class rel_coord_orth;

 private:
  
  
 public:

  int                       _axis;
  double                    _coord;
  gdt::gdtlist<gdtnode>*    _node_list;  // sequence of nodes met on the grid line
  gdt::list_item            _position_into_list;  // grid position into list
    
  
  _grid_line_struct(){
    _grid_line_struct::_node_list = new gdt::gdtlist<gdtnode>;
    _grid_line_struct::_axis = 0;
    _grid_line_struct::_coord = 0;
  }

  _grid_line_struct(int axis, double coord) {

    _grid_line_struct::_node_list = new gdt::gdtlist<gdtnode>;
    _grid_line_struct::_axis = axis;
    _grid_line_struct::_coord = coord;
  }

  ~_grid_line_struct(){ delete _node_list; }
  
  int get_axis() const { return _axis; }
  double coord() const { return _coord; }
  
  void set_axis(int axis) { _axis = axis; }  // 0 = horizontal; 1 = vertical
  void set_coord(double c) { _coord = c; }
};


typedef _grid_line_struct* grid_line;    


class rel_coord_orth {

  private:

  undi_graph ulug;    // underlying undi_graph

  gdt::gdtlist<grid_line>  _grid_line_list[2];           // list of vertical (1) and horizontal (0) grid lines
  gdt::gdtmap<gdtnode,grid_line> _grid_line_of_node[2];  // vertical (1) and horizontal (0) grid line of node
  gdt::gdtmap<gdtnode, gdt::list_item > _node_pos_into_gl_list[2];
  gdt::gdtmap<gdtedge, int> _axis_of_edge;               // 0 for horizontal and 1 for vertical
  gdt::gdtmap<int, int> _aux_id;                         // record the id of the node of the undi graph
  							 // from which the node with this id has been generated
  bool _redistribution_needed; 
    

			/* 
			PRIVATE_METHOD new_grid_line
    			Insert a new last grid line parallel to the axis and return it.
     			If "forward" is true the new grid line is inserted as the last grid line of 
     			the list; if "forward" is false the grid line is inserted at the 
     			beginning of the list. 
			*/

  			grid_line 
		new_grid_line
			(
			int axis, 
			bool forward=true
			);  


			/* 
			PRIVATE_METHOD new_grid_line
    			Insert a new grid line near the input gl and return it.
     			If "forward" is true the new grid line is inserted after gl,
			if "forward" is false the new grid line is inserted before gl. 
			*/

  			grid_line 
		new_grid_line
			(
			grid_line gl, 
			bool forward=true
			);  


			/* 
			PRIVATE_METHOD new_box
    			Build a box in the specified position. 
			*/


			void
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
			); 



  public:

			/**
			Void constructor. 
			*/
			
		rel_coord_orth
			(void);


			/**
			Deletes the object. 
			*/
			
		~rel_coord_orth
			(void);


			/** 
			Builds a drawing starting from the given undi_graph.
			Graph will be drawn with the following algorithm: 
			
			<UL>
			<LI> ORTH_SIMPLE_PT00 (DEFAULT_ALGORITHM)<br>
			    	Implements the "simple algorithm" described in: 
				A. Papakostas and I.G. Tollis,
				"Efficient Orthogonal Drawings of High Degree Graphs",
				Algorithmica (2000) 26: 100-125.
			</UL>

			*/

		rel_coord_orth
			(
			undi_graph ug,
			algorithm_type alg=ORTH_SIMPLE_PT00
			);  


			/** 
			Return the underlying graph. 
			It makes a copy of it.
			*/
			
			
			undi_graph
		get_underlying_undi_graph
			(
			) const;
			

			/** 
			Return the x and y coordinates of a node.
			PRECONDITIONS: the node must belong to the underlying
			               undi_graph.
			*/
			
			gdt::gdtpoint
		center
			(
			gdtnode n
			) const;


			/**
			Return the x and y coordinates of a node.
			PRECONDITIONS: the node must belong to the underlying
			               undi_graph.
			*/
			
			gdt::gdtpoint
		center
			(
			int id
			) const;

			/**
			Return the x coordinate of a node.
			PRECONDITIONS: the node must belong to the underlying
			               undi_graph.
			*/
			
			double
		xcoord
			(
			gdtnode n
			) const;


			/**
			Return the x coordinate of a node.
			*/
			
			double
		xcoord
			(
			int id
			) const;


			/** 
			Return the y coordinate of a node.
			PRECONDITIONS: the node must belong to the underlying
			               undi_graph.
			*/
			
			double
		ycoord
			(
			gdtnode n
			) const;


			/**
			Return the y coordinate of a node.
			*/
			
			double
		ycoord
			(
			int id
			) const;


			/**
			Return the auxiliary id of a node whose id is the given one.
			This value is the id of the node of the
			undi_graph from which this node has been generated.
			Rationale: a node of the undi graph may correspond to a
			box of the rel_coord_orth. All nodes composing the box
			have the same aux_id as the original node.
			*/
			
			inline
			int
		aux_id
			(
			int id
			) const
			{
			return _aux_id[id];
			}


			/**
			Return the next grid line in the direction
			specified by "forward". If gl is the last grid
			line return NULL.
			*/
			
			grid_line
		next_grid_line
			(
			grid_line gl,
			bool forward
			);



			/**
			Create a new node and insert it 
			in the specified position.
			*/
			
			inline
			gdtnode
		new_node
			(
			grid_line l1,
			grid_line l2
			)
			{
			gdtnode n = ulug.new_node();
			insert_node(n,l1,l2);
			return n;
			}


			/**
			Create a new edge and remember
			the axis it is parallel to.
			PRECONDITION: the two input nodes must share a grid line.
			*/
			
			inline
			gdtedge
		new_edge
			(
			gdtnode n1,
			gdtnode n2
			)
			{
			gdtedge e = ulug.new_edge(n1,n2);
			if( _grid_line_of_node[0][n1] == _grid_line_of_node[0][n2] )
				_axis_of_edge[e]=0;
			else if ( _grid_line_of_node[1][n1] == _grid_line_of_node[1][n2] )
				_axis_of_edge[e]=1;
			else gdt_error("the two input nodes do not share a grid line");
			return e;
			}


			/**
			Insert the node in the given position.
			Grid lines l1 and l2 may be swapped without problems.
			*/

			void 
		insert_node
			(
			gdtnode n, 
			grid_line l1, 
			grid_line l2
			);

			/**
			Since to implement the relative coordinates scenario, this
     			class uses doble coords, after the drawing is fully obtained, you
     			should reassign coords to the planes in order to have grid lines
			equally spaced
			*/
			
			inline
			void 
		redistribute_coords
			(
			double gap
			) 
			{
			redistribute_coords(0,gap);
			redistribute_coords(1,gap);
			_redistribution_needed = false;
			}

			/**
			Since to implement the relative coordinates scenario, this
     			class uses doble coords, after the drawing is fully obtained, you
     			should reassign coords to the planes in order to have grid lines
			equally spaced
			*/
			
			inline
			void 
		redistribute_coords_if_needed
			(
			double gap
			) 
			{
			if( _redistribution_needed )
				{
				redistribute_coords(0,gap);
				redistribute_coords(1,gap);
				_redistribution_needed = false;
				}
			}


			/**
			Since to implement the relative coordinates scenario, this
     			class uses doble coords, after the drawing is fully obtained, you
     			should reassign coords to the planes in order to have grid lines
			equally spaced
			*/
			
			void 
		redistribute_coords
			(
			int axis, 
			double gap
			);


  
};

#endif // RM3_REL_COORD_ORTH_H
