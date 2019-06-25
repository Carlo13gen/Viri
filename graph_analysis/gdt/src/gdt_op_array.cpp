#include<GDT/gdtlist.h>
#include<GDT/gdt_graph_array.h>
#include<GDT/rm3_undi_graph.h>


gdt::gdtlist<struct_gdtedge*>  gdt::EDGE_OP::
			all(const undi_graph *g) 
	{return g->all_edges();};

int gdt::EDGE_OP::number(const undi_graph *g) {
		return g->number_of_edges();
}

gdt::gdtlist<gdtnode> gdt::NODE_OP::all(const undi_graph *g) {
	return g->all_nodes();
}

int gdt::NODE_OP::number(const undi_graph *g) {
	return g->number_of_nodes();
}

// gdtlist<face> FACE_OP::all(const undi_graph *g) {	return g->all_faces();}

// int FACE_OP::number(const undi_graph *g) {	return g->number_of_faces();}


