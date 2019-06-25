/*******************************************************************************
+
+  gdtpoint.h
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

// Pier Francesco Cortese - Jan 2004

/*! \file */

#ifndef __gdtgeometry__
#define __gdtgeometry__


#include<iostream>
#include<math.h>
#include<GDT/gdtlist.h>

#ifdef WIN32
#define M_PI 3.14159265358979323846
#endif

namespace gdt {

#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)<(b) ? (a) : (b))

#define SIGN(a) ((a>=0) ? 1 : -1)

class gdtpoint {  // 2-dimensional point

	private:

	double x_coord;
	double y_coord;

    public:

    // Constructors

	gdtpoint();

	gdtpoint(double x, double y);

	~gdtpoint();

    //Access Operations

    double xcoord() const;

    double ycoord() const;

    /**
	Returns the distance from (0,0).
    */
		double
	  distance() const;

	
	/**
	@param p a gdtpoint
	Returns the distance from gdtpoint p.
	*/

	  	double
	  distance(gdtpoint p) const;


	/**
	Translates the current point adding dx and dy to its coordinates.
	*/

	  	gdtpoint
	translate (double dx, double dy) ;


	/**
	Returns the x-distance from gdtpoint q.
	@param q a gdtpoint
	*/
		double
	xdist(gdtpoint q) const;


	/**
	Returns the y-distance from gdtpoint q.
	@param q a gdtpoint
	*/

        	double
	ydist(gdtpoint q) const;


	/**
	Rotates the current point with respect to point p of angle a. <br>
	@param q a gdtpoint, the center of rotation	
	@param a the angle of rotation, expressed in radians
	*/

		gdtpoint
	rotate(gdtpoint q, double a);

}; // end of class gdtpoint


// gdtpoint non-member functions

std::ostream& operator<< (std::ostream& os, const gdtpoint& p);

std::istream& operator>> (std::istream& is, gdtpoint& p);

bool operator!=(const gdt::gdtpoint& p1, const gdt::gdtpoint& p2);

bool operator==(const gdt::gdtpoint& p1, const gdt::gdtpoint& p2);

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
class gdtsegment {    // 2-dimensional segment

	private:
    		gdtpoint point1;
		gdtpoint point2;

	public:

		gdtsegment();

		~gdtsegment();

		gdtsegment(gdtpoint p1, gdtpoint p2);

		gdtpoint start() const;

		gdtpoint end() const;

		bool is_horizontal();
		
		bool is_almost_horizontal(double round=0.001);

            	bool is_vertical();
		
		bool is_almost_vertical(double round=0.001);

		double length();

/** 
Tests if the point p belongs to the current segment.
*/
		bool contains(gdtpoint p);


/**
Tests if there is an instersection point betwee this segment and segment b. <br>
If an intersection is found, the intersection point is assigned to p, and the method returns true.
Otherwise, the method returns false.
*/
		bool intersection(gdtsegment b, gdtpoint& p);


/**
Returns the angle of rotation, expressed in radians, of the current segment with respect to (0,0).
*/
		double direction();


/**
Returns the angle of rotation, expressed in radians, of the current segment with respect to (0,0).
*/
		double angle();

/**
Returns the angle of rotation, expressed in radians, of the current segment with respect to segment t.
*/
		double
		angle(gdtsegment t);

		
/*Returns the distance between this segment and point p
*/		
		double distance(gdtpoint p);

}; // end of class gdtsegment



class gdtline {

	private:

        // line is ax+by+c=0
	double a;
	double b;
	double c;

	double angle;

	public:

        gdtline(gdtpoint p1, gdtpoint p2);

	~gdtline();

	double direction() const;

}; // end of class gdtline

std::ostream& operator<< (std::ostream& os, const gdtsegment& s);


class gdtpolygon {

	private:
		gdtlist<gdtpoint> polygon_vertices;

	public:

		gdtpolygon();

		gdtpolygon(gdtlist<gdtpoint> V);

		~gdtpolygon();

		gdtlist<gdtpoint>
		vertices() const;

		gdtlist<gdtsegment> edges();
		
/**
Returns the list of the instersection points between the current polygon and segment s.
*/
		gdtlist<gdtpoint> intersection(gdtsegment s) ;

		double perimeter();

		double area();

};  // end of class gdtpolygon



class gdtcircle {

	private:
    		gdtpoint center;
		double   radius;

	public:
		gdtcircle(gdtpoint c, double r);

		~gdtcircle();

		gdtlist<gdtpoint> intersection(gdtsegment s);
}; // end of class gdtcircle


};  // end of namespace gdt


#endif
