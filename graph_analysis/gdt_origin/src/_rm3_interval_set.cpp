/*******************************************************************************
+
+  _rm3_interval_set.c
+
+  This file is part of GDToolkit. It can be
+  used free of charge in academic research and teaching.
+  Any direct or indirect commercial use of this software is illegal
+  without a written authorization of
+  the Dipartimento di Informatica e Automazione
+  Universita' di Roma Tre, Roma, ITALIA

+  All rights reserved.
+
*******************************************************************************/
#include <iostream>
#include <fstream>
#include <GDT/rm3_interval_set.h>
#include <GDT/gdt_error.h>

 
     GDT_interval_set::interval 
     GDT_interval_set::
ray_shooting_generic( double x, double z, bool up)
       {
	 interval i;
	 interval current_nearest=NULL; // the current optimum
	 
	 forall( i, the_set )
	   {
	   if ( (x >= i->from && x < i->to ) ||
                (x == i->to && i->to == i->from) 
	     )
	     if( up ) // shoot up
	       {
	       if (i->z > z) 
		 {
		 if (!current_nearest)
		   current_nearest=i;
		 else
		   if (i->z < current_nearest->z)
		     current_nearest=i;
		 }
	       }
	     else // shoot down
	       {	       
		 if (i->z < z) 
		 {
		 if (!current_nearest)
		   current_nearest=i;
		 else
		   if (i->z > current_nearest->z)
		     current_nearest=i;
		 }
	       }
	   }
	 return current_nearest; // possibly NULL
       }


     GDT_interval_set::interval 
     GDT_interval_set::
new_interval( double f, double t, double z )
       {
	 if ( f > t ) gdt_error("new_interval inverted endpoints");
	 
	 interval i= new _interval(f,t,z);

#ifndef NDEBUG
	 interval j;
	 forall( j, the_set )
	   {
           if ( i->z == j->z &&  // same altitude
                !( i->from >= j->to || i->to <= j->from )
		)
	     gdt_error("new_interval overlapping segments");
	   }
#endif
	 the_set.push_back(i);
	 return i;
       }



     void 
     GDT_interval_set::
del_interval ( interval i )
       {
	 gdt::list_item it=NULL;
	 
	 forall_items (it, the_set)
	   {
	     if (the_set.inf(it)==i) break;
	   }
	 if (!it)
	   gdt_error("the interval is not contained in the set");
	 delete the_set.inf(it);
	 the_set.erase(it);
       }



     GDT_interval_set::
~GDT_interval_set()
       {
	 interval j;
	 forall( j, the_set )
	   { del_interval( j ); }
       }



std::ostream& operator<< (std::ostream& o, GDT_interval_set::interval i)
   {
     if (i)
       {
	   o << i->from << ","<< i->to<< "," << i->z;
	   o.flush();
	   }
     else
       {
	   o << "(null interval)";
	   o.flush();
	   }
   return o;
   }

   void GDT_interval_set::
print(std::ostream& o) const
       {
	 interval i;
	 o << "size: " << the_set.size() << std::endl;
	 forall(i, the_set)
	     o << i << std::endl;
       }


std::ostream& operator<< (std::ostream& o, const GDT_interval_set& s)
   { s.print(o); return o; }
