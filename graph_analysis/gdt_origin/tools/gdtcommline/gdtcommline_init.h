
#ifndef _INIT_H
#define _INIT_H

#include <string>
#include <GDT/rm3_global.h>  // for algorithm_type

// general  

extern bool verbose;
extern std::string ifile;
extern std::string ofile;
extern std::string action;

// for action "stat"

extern std::string stat_type;

// for action "draw"

extern std::string drawing_approach;
extern std::string rcs_alg;   // relative coordinates scenario algorithm
extern algorithm_type orthogonalization_alg;
extern algorithm_type compaction_alg;

// for action "points"

extern bool points_nodes_only;
extern bool points_bends_only;
extern float points_delta;


void  init(int, char**);

#endif  // _INIT_H
