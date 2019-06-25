/*******************************************************************************
+
+  rm3_interval_set.h
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

#ifndef RM3_INTERVAL_SET_H
#define RM3_INTERVAL_SET_H

#if !defined(ROOT_INCL_ID)
#define ROOT_INCL_ID 34015
#endif

#include <GDT/gdtlist.h>


/**
  CLASS GDT_interval_set

  this is a set of _closed_ intervals (integer coordinates) in one dimension.
  Insertion, deletion and ray shooting are supported operations.
  */

class GDT_interval_set
   {
   private:
     GDT_interval_set(const GDT_interval_set& );
     GDT_interval_set& operator=(const GDT_interval_set& );
   private:
     struct _interval
       {
           // end points
	 const double from;
         const double to;
	   // second coordinate
         const double z;

	 _interval(double f, double t, double zz):
	   from(f),
           to(t),
           z(zz)
	   {}
       };
   public:
     typedef _interval* interval;

   private:

     gdt::gdtlist< interval > the_set;


     /* 
	It return NULL if no interval has been hitted.
	The ray is shot at x position starting at z altitude. It shoot up if "up==true", 
        down if "up==false".
     */

     interval ray_shooting_generic( double x, double z, bool up);

   public:
     GDT_interval_set() {};

     ~GDT_interval_set();


   public:

     /* accessors */
     double altitude(interval i) const
       { return i->z; }

     double left(interval i) const
       { return i->from; }

     double right(interval i) const
       { return i->to; }

     /* modifiers */

     interval new_interval( double f, double t, double z );

     void del_interval( interval i ); 

     interval ray_shooting_up( double x, double z)
       { return ray_shooting_generic( x, z, true ); }

     interval ray_shooting_down( double x, double z)
       { return ray_shooting_generic( x, z, false ); }

     void print(std::ostream& o) const;
   };


std::ostream& operator<< (std::ostream& o, GDT_interval_set::interval i);
std::ostream& operator<< (std::ostream& o, const GDT_interval_set& s);

#endif

