/*******************************************************************************
+
+  rm3_simple_graph.h
+
+  This file is part of GDToolkit. It can be
+  used free of charge in academic research and teaching.
+  Any direct or indirect commercial use of this software is illegal
+  without a written authorization of
+  the Dipartimento di Informatica e Automazione
+  Universita' di Roma Tre, Roma, ITALIA
+
+
*******************************************************************************/
/*! \file */

#ifndef SIMPLE_GRAPH_H
#define SIMPLE_GRAPH_H


#include<iostream>
#include<GDT/gdtlist.h>
#include<GDT/gdt_error.h>
#include<GDT/rm3_undi_graph.h>

using namespace std;

class struct_node;
class struct_edge;

typedef struct_node* node;
typedef struct_edge* edge;

class simple_graph;

/////////////////////////////////////////////////////
// Class Node
/////////////////////////////////////////////////////

class struct_node
{
	friend class simple_graph;
	friend class undi_graph;
	
	private:
	gdt::gdtlist<edge> incident_edges;
	int index;
	gdt::list_item position_in_nodelist;

	public:
	struct_node()
	{
		position_in_nodelist=NULL;
	};
	
	struct_node(int id)
	{
		index=id;
		position_in_nodelist=NULL;
	};
	
	int get_index() const {return index;}
	int degree() const {return incident_edges.length();}

}; // end of class node

ostream& operator<<(ostream& oo, const struct_node* n);



/////////////////////////////////////////////////////
// Class Edge
/////////////////////////////////////////////////////

class struct_edge
{
	friend class simple_graph;
	friend class undi_graph;
	
	private:
	int index;
	node _start;
	node _end;
	gdt::list_item position_in_edgelist;
	gdt::list_item position_in_startnode;
	gdt::list_item position_in_endnode;
	
	int weight;

	public:
	struct_edge(int id) 
	{
		index=id;
		_start=NULL;
		_end=NULL;
		position_in_edgelist=NULL;
		position_in_startnode=NULL;
		position_in_endnode=NULL;
	};
	
	struct_edge()
	{
		_start=NULL;
		_end=NULL;
		position_in_edgelist=NULL;
		position_in_startnode=NULL;
		position_in_endnode=NULL;
	}

	struct_edge(node n1, node n2, int id);
	
	int get_index() const {return index;};
	int get_weight(){return weight;};
	void set_weight(int w) {weight=w;}
	node start() const {return _start;}
	node end() const {return _end;};

};  // end of class edge


ostream& operator<<(ostream& oo, const struct_edge* n);


/////////////////////////////////////////////////////
// Class simple_graph
/////////////////////////////////////////////////////


class simple_graph
{
	
	friend class undi_graph;
	private:
	
	int _max_node_id;
	int _max_edge_id;
	
	bool check();
	
	public:
	
	gdt::gdtlist<node> _nodes;
	gdt::gdtlist<edge> _edges;
	
	simple_graph();
	~simple_graph();

	simple_graph(undi_graph& G);

	node new_node(int id=-1);
	


	edge new_edge(node n1, node n2, int id=-1);
	
	void del_node(node& n);
	void del_edge(edge& e);
	
	int number_of_nodes() const;
	int number_of_edges() const;

	void contract(edge&,node);
	bool is_empty();
};


ostream& operator<<(ostream& oo, const simple_graph& g);



#endif
