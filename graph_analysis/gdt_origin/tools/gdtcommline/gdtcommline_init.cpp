
#include "gdtcommline_init.h"   
#include <GDT/gdt_error.h>
#include <cstdio>
#include <string>
#include <cstdlib>

#define DEFAULT_DELTA 2.0


void usage(char* progname);
void help(char* progname);
void help_actions(char* progname);
void help_draw_action(char* progname);
void help_stat_action(char* progname);
void help_stat_type(char* progname);
void help_compaction_alg(char* progname);
void help_orthogonalization_alg(char* progname);
void help_drawing_approach(char* progname);
void rcs_alg_help(char* progname);
void file_needed_help(char* progname);


std::string ifile;
std::string action;
std::string stat_type;
std::string ofile;
std::string drawing_approach;
std::string rcs_alg;     // relative coordinates scenario algorithm
bool verbose;
algorithm_type orthogonalization_alg;
algorithm_type compaction_alg;

// for action points

bool points_nodes_only;
bool points_bends_only;
float points_delta;



void init(int argc, char** argv) {

  std::string x;
  bool help_flag;
  
  // initialize the default values
  //
  help_flag = false;
  std::string orthogonalization_alg_string = "PLAN_ORTH_OPTIMAL";
  std::string compaction_alg_string = "FAST_COMPACTION";
  rcs_alg = "ORTH_SIMPLE_PT00";
  action =                         "noaction";
  drawing_approach = "TSM";
  points_nodes_only = false;
  points_bends_only = false;
  points_delta = DEFAULT_DELTA;

  if (argc == 1) usage(argv[0]);

  //  1 - take the input values 
  //
  for (int n = 1 ; n < argc ; n++) {
    x = argv[n];
         if ( x == "-i" )                   ifile = argv[++n];
    else if ( x == "-o" )                   ofile = argv[++n];
    else if ( x == "-v" )                  verbose = true;
    else if ( x == "-a" )                   action = argv[++n];
    else if ( x == "-da"   || x == "--drawing-approach" )         drawing_approach = argv[++n];
    else if ( x == "-om"   || x == "--orthogonalization_method" ) orthogonalization_alg_string = argv[++n];
    else if ( x == "-cm"   || x == "--compaction_method" )        compaction_alg_string = argv[++n];
    else if ( x == "-rcsm" || x == "--rcs_method" )               rcs_alg = argv[++n];
    else if ( x == "-n"    || x == "--nodes_only" )               points_nodes_only = true;
    else if ( x == "-b"    || x == "--bends_only" )               points_bends_only = true;
    #ifdef WIN32
    else if ( x == "-d"    || x == "--delta" )                    sscanf_s(argv[++n],"%f", &points_delta);
    #else
    else if ( x == "-d"    || x == "--delta" )                    sscanf(argv[++n],"%f", &points_delta);
    #endif
    else if ( x == "-st"   || x == "--stat-type" )                stat_type = argv[++n];
    else if ( x == "-h"    || x == "--help"  )                    help_flag = true;
    else usage(argv[0]);
  }

  if ( action == "noaction" && help_flag ) help(argv[0]);

  if ( action != "draw" &&
       action != "points" && 
       action != "stat"  ) help_actions(argv[0]);  // either action=help or action not known

  if ( action == "draw" ) {
    
    if( help_flag ) help_draw_action(argv[0]);
    
    if ( drawing_approach != "TSM" &&
         drawing_approach != "POLYLINE" &&
         drawing_approach != "RCS" &&
         drawing_approach != "OFV") help_drawing_approach(argv[0]);  // either == "-h" or illegal 
  
  }

  if ( action == "stat" ) {
    
    if( help_flag ) help_stat_action(argv[0]);

    if ( stat_type != "nodes_and_edges" &&
         stat_type != "node_degree" &&
         stat_type != "bends" &&
         stat_type != "bends_per_edge" &&
         stat_type != "face_degree" &&
         stat_type != "nn" &&
         stat_type != "bb") help_stat_type(argv[0]);  // either == "-h" or illegal 
  
  }

  /* 
     orthogonalization_alg_string, compaction_alg_string, rcs_alg, and drawing_approach were 
     correctly initialized. If the user changed them, we check that the new values are
     legitimate ones.
  */

  if ( orthogonalization_alg_string == "PLAN_ORTH_OPTIMAL" )   orthogonalization_alg = PLAN_ORTH_OPTIMAL;
  else if ( orthogonalization_alg_string == "PLAN_ORTH_SLOW" ) orthogonalization_alg = PLAN_ORTH_SLOW;
  else                                                         help_orthogonalization_alg(argv[0]);

  if ( compaction_alg_string == "FAST_COMPACTION" )                        compaction_alg = FAST_COMPACTION;
  else if ( compaction_alg_string == "FAST_COMPACTION_REFINED" )           compaction_alg = FAST_COMPACTION_REFINED;
  else if ( compaction_alg_string == "SLOW_COMPACTION" )                   compaction_alg = SLOW_COMPACTION;
  else if ( compaction_alg_string == "SLOW_COMPACTION_REFINED" )           compaction_alg = SLOW_COMPACTION_REFINED;
  else if ( compaction_alg_string == "FAST_REGULAR_COMPACTION_1" )         compaction_alg = FAST_REGULAR_COMPACTION_1;
  else if ( compaction_alg_string == "FAST_REGULAR_COMPACTION_1_REFINED" ) compaction_alg = FAST_REGULAR_COMPACTION_1_REFINED;
  else if ( compaction_alg_string == "SLOW_REGULAR_COMPACTION_1" )         compaction_alg = SLOW_REGULAR_COMPACTION_1;
  else if ( compaction_alg_string == "SLOW_REGULAR_COMPACTION_1_REFINED" ) compaction_alg = SLOW_REGULAR_COMPACTION_1_REFINED;
  else if ( compaction_alg_string == "FAST_REGULAR_COMPACTION_2" )         compaction_alg = FAST_REGULAR_COMPACTION_2;
  else if ( compaction_alg_string == "FAST_REGULAR_COMPACTION_2_REFINED" ) compaction_alg = FAST_REGULAR_COMPACTION_2_REFINED;
  else if ( compaction_alg_string == "SLOW_REGULAR_COMPACTION_2" )         compaction_alg = SLOW_REGULAR_COMPACTION_2;
  else if ( compaction_alg_string == "SLOW_REGULAR_COMPACTION_2_REFINED" ) compaction_alg = SLOW_REGULAR_COMPACTION_2_REFINED;
  else                                                                     help_compaction_alg(argv[0]);

  if ( rcs_alg != "ORTH_SIMPLE_PT00" &&
       rcs_alg != "ORTH_SIMPLE_PT00_DEBUG" ) rcs_alg_help(argv[0]);



  /* 
     If you are here you want me to perform some action.
     If you want me to perform some action I need an input file
  */

  if( ifile == "" ) file_needed_help(argv[0]);

  if ( ofile == "" ) ofile = ifile + "." + action + ".gdt";
}

void usage(char* progname) {

  std::cout << "\nType: \"" << progname << " -h\" for help";

  std::cout << "\n\nReport bugs to: gdt@dia.uniroma3.it\n\n";
      
  std::cout << std::flush;
  exit(0);
}

void help(char* progname) {

  std::cout << "\nUsage: " << progname << " [OPTIONS] ";
  
  std::cout << "\n\n\t-h            Obtain this help screen.";

  std::cout << "\n\n\t-v            Print verbose comments to stderr.";

  std::cout << "\n\n\t-a <action>   Execute the specified action.";

  std::cout << "\n\nType: \"" << progname << " -a help\" for help on actions";

  std::cout << "\n\nReport bugs to: gdt@dia.uniroma3.it\n\n";
      
  std::cout << std::flush;
  exit(0);
}


void help_actions(char* progname) {

  std::cout << "\nUsage: " << progname << " [OPTIONS] ";

  std::cout << "\n\n\t-a help";
  std::cout << "  \n\t         show this help screen ";

  std::cout << "\n\n\t-a <action> ";
  std::cout << "  \n\t         Execute the specified action.";
  std::cout << "  \n\t         \"draw\"";
  std::cout << "  \n\t           Draws the input graph by using a specified drawing approach.";
  std::cout << "  \n\t         \"stat\"";
  std::cout << "  \n\t           Prints statistics for the input graph.";
  std::cout << "  \n\t         \"points\"";
  std::cout << "  \n\t           Transforms a drawing of a graph into a collection of points.";

  std::cout << "\n\nType: \t\"" << progname << " -a draw -h\" for help on draw action ";
  std::cout <<         "\n\t\"" << progname << " -a stat -h\" for help on stat action ";

  std::cout << "\n\nReport bugs to: gdt@dia.uniroma3.it\n\n";
      
  std::cout << std::flush;
  exit(0);
}

void help_draw_action(char* progname) {

  std::cout << "\nUsage: " << progname << " -a draw [OPTIONS] ";

  std::cout << "\n\n\tDraw the graph using the specified drawing approach.";
  std::cout << "  \n\tEnables the following flags:";

  std::cout << "\n\n\t-h show this help screen.";

  std::cout << "\n\n\t-da --drawing-approach <approach> ";
  std::cout << "  \n\t         Use the specified approach to draw the graph.";

  std::cout << "\n\n\t-i --input <input_filename> ";
  std::cout << "  \n\t         the input filename (.gdt format, contains an undi_graph).";
  
  std::cout << "\n\n\t-o --output <output_filename> ";
  std::cout <<   "\n\t         the output filename (.gdt format, contains a draw_undi_graph)";
  std::cout <<   "\n\t         default value is <input_filename>.draw.gdt";

  std::cout << "\n\nType: \"" << progname << " -a draw -da -h\" for help on drawing approaches";

  std::cout << "\n\nReport bugs to: gdt@dia.uniroma3.it\n\n";
      
  std::cout << std::flush;
  exit(0);
}


void help_stat_action(char* progname) {

  std::cout << "\nUsage: " << progname << " -a stat [OPTIONS] ";

  std::cout << "\n\n\tCompute the specified statistics on the input graph.";
  std::cout << "  \n\tEnables the following flags:";

  std::cout << "\n\n\t-h show this help screen.";

  std::cout << "\n\n\t-st --stat-type <stat_type> ";
  std::cout << "  \n\t         Perform the specified statistics type on the graph.";

  std::cout << "\n\n\t-i --input <input_filename> ";
  std::cout << "  \n\t         the input filename (.gdt format, contains an undi_graph).";

  std::cout << "\nType: ";

  std::cout << "\n\n\t" << progname << " -a stat -st -h";
  std::cout << "  \n\t         for help on statistics types";

  std::cout << "\n\nReport bugs to: gdt@dia.uniroma3.it\n\n";
      
  std::cout << std::flush;
  exit(0);
}

void help_stat_type(char* progname) {

  std::cout << "\nUsage: " << progname << " -a stat [OPTIONS] ";

  std::cout << "\n\n\t-st --stat-type -h";
  std::cout << "  \n\t         show this help screen ";

  std::cout << "\n\n\t-st <stat_type>";
  std::cout << "  \n\t         \"nodes_and_edges\"";
  std::cout << "  \n\t           Prints the number of nodes and the number of edges (in ";
  std::cout << "  \n\t           this order) of the input graph (a line for each pair).";
  std::cout << "  \n\t         \"node_degree\"";
  std::cout << "  \n\t           Prints the degree of each node (a line for each number).";
  std::cout << "  \n\t         \"bends\"";
  std::cout << "  \n\t           Prints the total number of bends";
  std::cout << "  \n\t         \"bends_per_edge\"";
  std::cout << "  \n\t           Prints number of bends of each edge (a line for each";
  std::cout << "  \n\t           number).";
  std::cout << "  \n\t         \"bb\"";
  std::cout << "  \n\t           Prints the bounding box (dx and dy) of the drawing.";
  std::cout << "  \n\t         \"face_degree\"";
  std::cout << "  \n\t           Prints the degree of each face (a line for each number).";
  std::cout << "  \n\t           The degree of the face is the number of (distinct) edges";
  std::cout << "  \n\t           that are on the boundary of the face.";
  std::cout << "  \n\t         \"nn\"";
  std::cout << "  \n\t           Prints the neighbor number, that is, a list of pairs (a line";
  std::cout << "  \n\t           for each pair) where the first number is a distance (starting";
  std::cout << "  \n\t           from 1 and ending with the maximum minimum topologic distance";
  std::cout << "  \n\t           between any two nodes) and the second number is the total number";
  std::cout << "  \n\t           of pairs at distance less or equal than r divided by the number";
  std::cout << "  \n\t           of nodes square.";

  std::cout << "\n\nReport bugs to: gdt@dia.uniroma3.it\n\n";
      
  std::cout << std::flush;
  exit(0);
}

void help_drawing_approach(char* progname) {

  std::cout << "\nUsage: " << progname << " -a draw [OPTIONS] ";

  std::cout << "\n\n\t-da --drawing_approach -h";
  std::cout << "  \n\t         show this help screen ";

  std::cout << "\n\n\t-da --drawing_approach <approach>";
  std::cout << "  \n\t         Use the specified approach to draw the graph.";
  std::cout << "  \n\t         Possible values are:";
  std::cout << "  \n\t           TSM [default]";
  std::cout << "  \n\t             Topology-Shape-Metrics.";
  std::cout << "  \n\t             Triggers -om and -cm flags.";
  std::cout << "  \n\t           RCS";
  std::cout << "  \n\t             Relative-Coordinates-Scenario.";
  std::cout << "  \n\t             Triggers -rcsm flag.";
  std::cout << "  \n\t           OFV";
  std::cout << "  \n\t             Orthogonal-From-Visibility.";
  std::cout << "  \n\t           POLYLINE";
  std::cout << "  \n\t             Only for biconnected graphs.";

  std::cout << "\n\nReport bugs to: gdt@dia.uniroma3.it\n\n";
      
  std::cout << std::flush;
  exit(0);
}


void help_compaction_alg(char* progname) {

  std::cout << "\nUsage: " << progname << " [OPTIONS] ";

  std::cout << "\n\n\t-cm --compaction_method help";
  std::cout << "  \n\t         show this help screen ";

  std::cout << "\n\n\t-cm --compaction_method <algorithm> apply the specified algorithm";
  std::cout << "  \n\t         FAST_COMPACTION [default algorithm]";
  std::cout << "  \n\t           execute a linear-time compaction algorithm,";
  std::cout << "  \n\t           based on a DFS visit (area-minimiz. after";
  std::cout << "  \n\t           rectangularization)";
  std::cout << "  \n\t         FAST_COMPACTION_REFINED";
  std::cout << "  \n\t           execute a FAST_COMPACTION and then improve";
  std::cout << "  \n\t           the drawing by reducing bridge-lengths, if";
  std::cout << "  \n\t           it is possible";
  std::cout << "  \n\t         SLOW_COMPACTION";
  std::cout << "  \n\t           execute a polynomial-time compaction";
  std::cout << "  \n\t           algorithm, based on flow techniques (length";
  std::cout << "  \n\t           and area-minimiz. after rectangularization)";
  std::cout << "  \n\t         SLOW_COMPACTION_REFINED";
  std::cout << "  \n\t           execute a SLOW_COMPACTION and then improve";
  std::cout << "  \n\t           the drawing by reducing bridge-lengths, if";
  std::cout << "  \n\t           it is possible.";
  std::cout << "  \n\t         FAST_REGULAR_COMPACTION_1";
  std::cout << "  \n\t           execute a linear-time compaction algorithm,";
  std::cout << "  \n\t           based on a DFS visit (area-minimiz. after";
  std::cout << "  \n\t           regularization with \"heuristic 1\")";
  std::cout << "  \n\t         FAST_REGULAR_COMPACTION_1_REFINED";
  std::cout << "  \n\t           execute a FAST_REGULAR_COMPACTION_1 and then improve";
  std::cout << "  \n\t           the drawing by reducing bridge-lengths, if";
  std::cout << "  \n\t           it is possible";
  std::cout << "  \n\t         SLOW_REGULAR_COMPACTION_1";
  std::cout << "  \n\t           execute a polynomial-time compaction";
  std::cout << "  \n\t           algorithm, based on flow techniques (length";
  std::cout << "  \n\t           and area-minimiz. after regularization with";
  std::cout << "  \n\t           \"heuristic 1\")";
  std::cout << "  \n\t         SLOW_REGULAR_COMPACTION_1_REFINED";
  std::cout << "  \n\t           execute a SLOW_COMPACTION_REFINED_1 and then improve";
  std::cout << "  \n\t           the drawing by reducing bridge-lengths, if";
  std::cout << "  \n\t           it is possible";
  std::cout << "  \n\t         FAST_REGULAR_COMPACTION_2";
  std::cout << "  \n\t           execute a linear-time compaction algorithm,";
  std::cout << "  \n\t           based on a DFS visit (area-minimiz. after";
  std::cout << "  \n\t           regularization with \"heuristic 2\")";
  std::cout << "  \n\t         FAST_REGULAR_COMPACTION_2_REFINED";
  std::cout << "  \n\t           execute a FAST_REGULAR_COMPACTION_2 and then improve";
  std::cout << "  \n\t           the drawing by reducing bridge-lengths, if";
  std::cout << "  \n\t           it is possible";
  std::cout << "  \n\t         SLOW_REGULAR_COMPACTION_2";
  std::cout << "  \n\t           execute a polynomial-time compaction";
  std::cout << "  \n\t           algorithm, based on flow techniques (length";
  std::cout << "  \n\t           and area-minimiz. after regularization with";
  std::cout << "  \n\t           \"heuristic 1\")";
  std::cout << "  \n\t         SLOW_REGULAR_COMPACTION_2_REFINED";
  std::cout << "  \n\t           execute a SLOW_COMPACTION_REFINED_2 and then improve";
  std::cout << "  \n\t           the drawing by reducing bridge-lengths, if";
  std::cout << "  \n\t           it is possible";

  std::cout << "\n\nReport bugs to: gdt@dia.uniroma3.it\n\n";
      
  std::cout << std::flush;
  exit(0);
}

void help_orthogonalization_alg(char* progname) {

  std::cout << "\nUsage: " << progname << " [OPTIONS] ";

  std::cout << "\n\n\t-om --orthogonalization_method help";
  std::cout << "  \n\t         show this help screen ";

  std::cout << "\n\n\t-om --orthogonalization_method <algorithm> ";
  std::cout << "  \n\t         Apply the specified algorithm.";
  std::cout << "  \n\t         PLAN_ORTH_OPTIMAL [default]";
  std::cout << "  \n\t           an optimal bend-minimization within the current";
  std::cout << "  \n\t           embedding (if it is planar). Takes polynomial time.";
  std::cout << "  \n\t           Variation of Tamassia's algorithm also handling";
  std::cout << "  \n\t           bend-constraints";
  std::cout << "  \n\t         PLAN_ORTH_SLOW";
  std::cout << "  \n\t           an optimal bend-minimization not preserving the embedding.";
  std::cout << "  \n\t           Takes exponential time. Based on a branch and bound technique.";
  std::cout << "  \n\t           Handles bend-constraints.";

  std::cout << "\n\nReport bugs to: patrigna@dia.uniroma3.it\n\n";
      
  std::cout << std::flush;
  exit(0);
}


void rcs_alg_help(char* progname) {

  std::cout << "\nUsage: " << progname << " [OPTIONS] ";

  std::cout << "\n\n\t-rcsm --rcs_method help";
  std::cout << "  \n\t         show this help screen ";

  std::cout << "\n\n\t-rcsm --rcs_method <algorithm> use the specified algorithm";
  std::cout << "  \n\t         ORTH_SIMPLE_PT00 [default algorithm]";
  std::cout << "  \n\t           The \"Simple Orthogonal\" algorithm described in:";
  std::cout << "  \n\t             A. Papakostas and I.G. Tollis,";
  std::cout << "  \n\t             \"Efficient Orthogonal Drawings of High Degree Graphs\"";
  std::cout << "  \n\t             Algorithmica (2000) 26: 100-125.";

  std::cout << "\n\nReport bugs to: patrigna@dia.uniroma3.it\n\n";
   
  std::cout << std::flush;
  exit(0);
}

void file_needed_help(char* progname) {

  std::cout << "\n" << progname << ": you need an input file (flag -i) for action \"" << action << "\"\n\n";
  std::cout << std::flush;
  exit(0);
}


void points_help(char* progname) {

  std::cout << "\n\"" << progname << "\" transforms a gdtoolkit drawing into a collection";
  std::cout << "\n of points (a pair of coordinates for each line).";
  std::cout << "\n\nUsage: " << progname << " [OPTIONS] \n";
  std::cout << "\n\t-d <value> the maximum distance between points";
  std::cout << "\n\t           default is " << DEFAULT_DELTA << " (";
  std::cout <<                 10.0/DEFAULT_DELTA << " points for each grid segment";
  std::cout << "\n\t           if grid lines are 10 units apart as in GDToolkit)";
  std::cout << "\n\t-n output nodes coordinates only";
  std::cout << "\n\t-b output bends coordinates only";
  std::cout << "\n\t-v prints information to stderr";

  std::cout << "\n\nExample: " << progname << " -i 100-edges.draw.gdt -d 1.0 > output.txt";

  std::cout << "\n\nReport bugs to: patrigna@dia.uniroma3.it\n\n";
      
  std::cout << std::flush;
  exit(0);
}

void helpxxx(char* progname) {

  std::cout << "\nUsage: " << progname << " [OPTIONS] ";
  
  std::cout << "\n\n\t-h --help obtain this help screen";

  std::cout << "\n\n\t-v --verbose print verbose comments to stderr";

  std::cout << "\n\n\t-a --action <action> ";
  std::cout << "  \n\t         Execute the specified action.";
  std::cout << "  \n\t         Default value is \"draw\".";
  std::cout << "  \n\t         Specify \"help\" as <action> to have a list and a brief";
  std::cout << "  \n\t         description of all supported actions.";

  std::cout << "\n\n\t-da --drawing-approach <approach> ";
  std::cout << "  \n\t         Use the specified approach to draw the graph.";
  std::cout << "  \n\t         Triggered by \"-a draw\".";
  std::cout << "  \n\t         Default value is \"TSM\" (Topology-Shape-Metrics).";
  std::cout << "  \n\t         Specify \"help\" as <approach> to have a list and a brief";
  std::cout << "  \n\t         description of all supported drawing approaches.";


  std::cout << "\n\n\t-om --orthogonalization_method <algorithm>";
  std::cout << "  \n\t         Triggered by \"-da TSM\".";
  std::cout << "  \n\t         Apply the specified orthogonalization algorithm.";
  std::cout << "  \n\t         Default orthogonalization algorithm is PLAN_ORTH_OPTIMAL.";
  std::cout << "  \n\t         Specify \"help\" as <algorithm> to have a list and brief";
  std::cout << "  \n\t         description of all supported orthogonalization algorithms.";

  std::cout << "\n\n\t-cm --compaction_method <algorithm>";
  std::cout << "  \n\t         Triggered by \"-da TSM\".";
  std::cout << "  \n\t         Apply the specified compaction algorithm.";
  std::cout << "  \n\t         Default compaction algorithm is FAST_COMPACTION.";
  std::cout << "  \n\t         Specify \"help\" as <algorithm> to have a list and brief";
  std::cout << "  \n\t         description of all supported compaction algorithms.";
				 
  std::cout << "\n\n\t-i --input <input_filename> ";
  std::cout << "  \n\t         the input filename (.gdt format, contains an undi_graph).";
  
  std::cout << "\n\n\t-o --output <output_filename> ";
  std::cout <<   "\n\t         the output filename (.gdt format, contains a draw_undi_graph)";
  std::cout <<   "\n\t         default value is <input_filename>.draw.gdt";

  std::cout << "\n\nExamples: " << progname << " -h";
  std::cout <<   "\n          " << progname << " -a draw -i 100-nodes.gdt";
  std::cout <<   "\n          " << progname << " -a help";

  std::cout << "\n\nReport bugs to: patrigna@dia.uniroma3.it\n\n";
      
  std::cout << std::flush;
  exit(0);
}
