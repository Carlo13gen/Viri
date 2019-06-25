#include <GDT/gdt_graph_array.h>
#include <GDT/rm3_undi_graph.h>

namespace gdt {

gdtlist<struct_gdtedge*> EDGE_OP::all(const undi_graph *g) {
	return g->all_edges();
}
int EDGE_OP::number(const undi_graph *g){
	return g->number_of_edges();
} 

gdtlist<struct_gdtnode*> NODE_OP::all(const undi_graph *g) {
	return g->all_nodes();
}

int NODE_OP::number(const undi_graph *g) {
	return g->number_of_nodes();
}

}
