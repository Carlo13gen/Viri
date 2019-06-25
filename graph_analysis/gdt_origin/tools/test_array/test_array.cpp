
#include <GDT/gdt_graph_array.h>
#include <LEDA/node_array.h>
#include <LEDA/map.h>
#include <LEDA/graph_gen.h>
#include <GDT/rm3_undi_graph.h>

struct complex {
	int x;
	int y;
	complex() {}
	complex(int _x, int _y) : x(_x), y(_y) {}
	//LEDA_MEMORY(complex);
};

ostream& operator << (ostream& os, const complex&) {return os;}
istream& operator >> (istream& is, complex&) {return is;}

void main(int argc, char** argv) {

	//Generazione del grafo
	if (strcmp(argv[1], "GEN") == 0) {
		graph g;
		int n = atoi(argv[3]);
		int m = atoi(argv[4]);
		random_graph(g, n, m);
		g.make_undirected();
		undi_graph ug(g);
		ug.write(argv[2]);
		exit(0);
	}

	//Test
	undi_graph ug;
	ug.read(argv[2]);
	int max = atoi(argv[3]);
	float dti, dta, T;
	int x = 0;
	// gdt map
	if (strcmp(argv[1], "gdtmap") == 0) {
		//Initialization
		used_time(T);
		gdt::gdtmap<node, complex> m;
		node n;
		forall_nodes(n, ug) {
			m[n] = complex(1,1);
		}
		dti = used_time(T);
		//Access
		for (int i = 0; i < max; ++i) {
			forall_nodes(n, ug)
				x += m[n].x;
		}
		dta = used_time(T);
	}
	// leda map
	if (strcmp(argv[1], "map") == 0) {
		//Initialization
		used_time(T);
		map<node, complex> m;
		node n;
		forall_nodes(n, ug) {
			m[n] = complex(1,1);
		}
		dti = used_time(T);
		//Access
		for (int i = 0; i < max; ++i) {
			forall_nodes(n, ug)
				x += m[n].x;
		}
		dta = used_time(T);
	}

	// gdtnode_array
	if (strcmp(argv[1], "gdtnode_array") == 0) {
		//Initialization
		used_time(T);
		gdt::gdtnode_array<complex> m;
		m.init(ug.nodes_and_edges());
		node n;
		forall_nodes(n, ug) {
			m[n] = complex(1,1);
		}
		dti = used_time(T);
		//Access
		for (int i = 0; i < max; ++i) {
			forall_nodes(n, ug)
				x += m[n].x;
		}
		dta = used_time(T);
	}
	// leda node_array
	if (strcmp(argv[1], "node_array") == 0) {
		//Initialization
		used_time(T);
		node_array<complex> m;
		m.init(ug.nodes_and_edges());
		node n;
		forall_nodes(n, ug) {
			m[n] = complex(1,1);
		}
		dti = used_time(T);
		//Access
		for (int i = 0; i < max; ++i) {
			forall_nodes(n, ug)
				x += m[n].x;
		}
		dta = used_time(T);
	}

	// gdtnode_map
	if (strcmp(argv[1], "gdtnode_map") == 0) {
		//Initialization
		used_time(T);
		gdt::gdtnode_map<complex> m;
		m.init(ug.nodes_and_edges());
		node n;
		forall_nodes(n, ug) {
			m[n] = complex(1,1);
		}
		dti = used_time(T);
		//Access
		for (int i = 0; i < max; ++i) {
			forall_nodes(n, ug)
				x += m[n].x;
		}
		dta = used_time(T);
	}
	// leda node_map
	if (strcmp(argv[1], "node_map") == 0) {
		//Initialization
		used_time(T);
		node_map<complex> m;
		m.init(ug.nodes_and_edges());
		node n;
		forall_nodes(n, ug) {
			m[n] = complex(1,1);
		}
		dti = used_time(T);
		//Access
		for (int i = 0; i < max; ++i) {
			forall_nodes(n, ug)
				x += m[n].x;
		}
		dta = used_time(T);
	}
	cout << "Initialization " << dti << endl;
	cout << "Access         " << dta << endl;
	cout << x << endl;
}
