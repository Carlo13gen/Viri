/*******************************************************************************
+
+  gdt_geometry.h
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

#include<GDT/gdtgeometry.h>
#include<GDT/gdt_error.h>
#include<stdlib.h>

namespace gdt {


gdtpoint::gdtpoint() 
	{        
	x_coord=0;
	y_coord=0;
	};


gdtpoint::gdtpoint(double x, double y) {
    	x_coord=x;
	y_coord=y;
	};


gdtpoint::~gdtpoint(){};


	double
gdtpoint::xcoord() const
	{return x_coord;};


	double
gdtpoint::ycoord() const
	{return y_coord; };




	double
gdtpoint::distance() const
	 {
	 return sqrt(pow(x_coord,2)+pow(y_coord,2));
	 };



	double
gdtpoint::distance(gdtpoint p) const {
      	return sqrt(pow((x_coord-p.x_coord),2)+pow((y_coord-p.y_coord),2));
	  };


	gdtpoint
gdtpoint::translate (double dx, double dy) {
       gdtpoint p(this->xcoord()+dx, this->ycoord()+dy);
	   return p;
	};


	double
gdtpoint::xdist(gdtpoint q) const
	{
	double x1=xcoord();
	double x2=q.xcoord();
        if (x2>=x1)
		return (x2-x1);
	else
		return (x1-x2);
	};



        double
gdtpoint::ydist(gdtpoint q) const
	{
	double y1=ycoord();
	double y2=q.ycoord();
	if (y2>=y1)
		return (y2-y1);
	else
		return (y1-y2);
	};


	gdtpoint
gdtpoint::rotate(gdtpoint q, double a) 
	{
 	double x=xcoord()-q.xcoord();
	double y=ycoord()-q.ycoord();
		
	double x1=x*cos(a)-y*sin(a);
	double y1=x*sin(a)+y*cos(a);

	gdtpoint r((x1+q.xcoord()),(y1+q.ycoord()));
        return r;
	}

/////////////////////////////////////////////////////////////////////////////
//Segment

gdtsegment::gdtsegment(){};

gdtsegment::~gdtsegment(){};

gdtsegment::gdtsegment(gdtpoint p1, gdtpoint p2) 
	{
        point1=p1;
	point2=p2;
	};

	gdtpoint
gdtsegment::start() const
	{return point1;};


	gdtpoint
gdtsegment::end() const
	{return point2;};


	bool
gdtsegment::is_horizontal() 
	{
       	return ((start()).ycoord()==(end()).ycoord());
	};

	bool
gdtsegment::is_almost_horizontal (double round)
	{
	double y1 = (start()).ycoord();	
	double y2 = (end()).ycoord();
	double diff = y1-y2;
	if (diff < 0) diff=-diff;
	return (diff < round);
	}

	bool
gdtsegment::is_vertical()
	{
        return ((start()).xcoord()==(end()).xcoord());
	};


	bool
gdtsegment::is_almost_vertical (double round)
	{
	double x1 = (start()).xcoord();	
	double x2 = (end()).xcoord();
	double diff = x1-x2;
	if (diff < 0) diff=-diff;
	return (diff < round);
	}


	double
gdtsegment::length()
	{
	gdtpoint p1=start();
	gdtpoint p2=end();
	return p1.distance(p2);
	};


	bool
gdtsegment::contains(gdtpoint p) 
	{
	double x1=(this->start()).xcoord();
        double y1=(this->start()).ycoord();
	double x2=(this->end()).xcoord();
	double y2=(this->end()).ycoord();
	double x3=(p.xcoord());
	double y3=(p.ycoord());

        if (y1==y2)  //horizontal
		return (y3==y1 && x3<=MAX(x1,x2) && x3>=MIN(x1,x2));
	if (x1==x2)  //vertical
		return (x3==x1 && y3<=MAX(y1,y2) && y3>=MIN(y1,y2));

	// cout << "x1=" << x1 <<"\tx2="<<x2<<"\tx3="<<x3<<"\ty1="<<y1<<"\ty2="<<y2<<"\ty3="<<y3<<endl;

	bool condition1=(((y3-y1)/(y2-y1))==((x3-x1)/(x2-x1)));
	bool condition2= (x3<=MAX(x1,x2) && x3>=MIN(x1,x2));
	bool condition3= (y3<=MAX(y1,y2) && y3>=MIN(y1,y2));

	return (condition1 && condition2 && condition3);

	};


	bool
gdtsegment::intersection(gdtsegment b, gdtpoint& p) {

	double x1=(this->start()).xcoord();
	double y1=(this->start()).ycoord();
	double x2=(this->end()).xcoord();
	double y2=(this->end()).ycoord();

        double x3=(b.start()).xcoord();
	double y3=(b.start()).ycoord();
	double x4=(b.end()).xcoord();
	double y4=(b.end()).ycoord();	

	if ( (x1==x3 && y1==y3) || (x1==x4 && y1==y4) ) 
		{
		gdtpoint extremal_intersection(x1,y1);
		p=extremal_intersection;
		return true;
		} 
	if ( (x2==x3 && y2==y3) || (x2==x4 && y2==y4) ) 
		{
		gdtpoint extremal_intersection(x2,y2);
		p=extremal_intersection;
		return true;
		} 
	double denominator=x1*(y3-y4)+x2*(y4-y3)+(x3-x4)*(y2-y1);
	if (denominator==0)  //parallel segments
		{
        	//std::cout << "Parallel segments" << std::endl;
		return false;
		}

	double x_numerator=x1*(x3*(y2-y4)+x4*(y3-y2))-x2*(x3*(y1-y4)+x4*(y3-y1));
	double y_numerator=x1*y2*(y3-y4)+x2*y1*(y4-y3)+(y2-y1)*(x3*y4-x4*y3);

	double x=x_numerator/denominator;
	double y=y_numerator/denominator;
	
	bool condition1=false;
	bool condition2=false;
        bool condition3=false;
	bool condition4=false;
		
	double precision=0.001;
		
	if ((x<=MAX(x1,x2) && x>=MIN(x1,x2)) || (fabs(x-MAX(x1,x2)))<precision || (fabs(x-MIN(x1,x2)))<precision ) condition1=true;
     	
	if ((x<=MAX(x3,x4) && x>=MIN(x3,x4)) || (fabs(x-MAX(x3,x4)))<precision || (fabs(x-MIN(x3,x4)))<precision ) condition2=true;
		
	if ((y<=MAX(y1,y2) && y>=MIN(y1,y2)) || (fabs(y-MAX(y1,y2)))<precision || (fabs(y-MIN(y1,y2)))<precision ) condition3=true;
		
	if ((y<=MAX(y3,y4) && y>=MIN(y3,y4)) || (fabs(y-MAX(y3,y4)))<precision || (fabs(y-MIN(y3,y4)))<precision ) condition4=true;
		
	if (condition1 && condition2 && condition3 && condition4)
		{
        	gdtpoint intersection(x,y);
		p=intersection;
		return true;
		//cout << "Intersection found!" << endl;
		};
	return false;
	};




	double
gdtsegment::direction()
	{
    	double x1=start().xcoord();
	double y1=start().ycoord();
	double x2=end().xcoord();
	double y2=end().ycoord();
	//cout << "y2=" << y2 <<"\ty1=" << y1 << endl;

        if(x1==x2 && y2>y1) return (M_PI/2);

        if(x1==x2 && y2<y1) return (-M_PI/2);

	if (y2>=y1 && x2>x1)  // 1°
		return(atan((y2-y1)/(x2-x1)));

	if (y2>=y1 && x2<x1)   // 2°
		return(atan((y2-y1)/(x2-x1))+M_PI);

	if (y2<y1 && x2<x1)    // 3°
		return(atan((y2-y1)/(x2-x1))-M_PI);

	return (atan((y2-y1)/(x2-x1))); // 4°
	};


	double
gdtsegment::angle() 
	{return this->direction(); };



	double
gdtsegment::angle(gdtsegment t) 
	{
    	return (t.direction() - this->direction());
	}


	double
gdtsegment::distance(gdtpoint p)
	{
        double distance;
        double x1=(this->start()).xcoord();
        double y1=(this->start()).ycoord();
	double x2=(this->end()).xcoord();
	double y2=(this->end()).ycoord();
        double x3=p.xcoord();
        double y3=p.ycoord();

       	if (this->contains(p)) return 0;

	if (is_horizontal())
		{
            	if (x3<=MAX(x1,x2) && x3>=MIN(x1,x2))
			distance=fabs(y3-y1);
		else
			distance=MIN(p.distance(start()),p.distance(end()));
		return distance;
		};

        if (is_vertical())
		{
            	if (y3<=MAX(y1,y2) && x3>=MIN(y1,y2))
			distance=fabs(x3-x1);
		else
			distance=MIN(p.distance(start()),p.distance(end()));
		return distance;
		};

	double m=tan(this->direction());
	double m2=-(1/m);


	double denominator=m2*(x1-x2)-y1+y2;
	if (denominator==0) // the segment is a point
		return (p.distance(this->start()));

	double x_intersection=(m2*x3*(x1-x2)+x1*(y2-y3)+x2*(y3-y1))/denominator;
	double y_intersection=(m2*(x1*y2-x2*y1+x3*(y1-y2))-y3*(y1-y2))/denominator;

	gdtpoint intersection(x_intersection,y_intersection);
	if (this->contains(intersection))
		{
         	double a=-((y2-y1)/(x2-x1));
		double b=1;
		double c=(((y2-y1)/(x2-x1))*x1)-y1;
		distance=fabs((a*x3+y3+c)/sqrt(pow(a,2)+pow(b,2)));
		}
	else
		{
		distance=MIN(p.distance(this->start()),p.distance(this->end()));
            	//cout << "L'intersezione NON è contenuta nel segmento" << endl;
		}
	return distance;
	};


/////////////////////////////////////////////////////////////////////////////
//GDTLINE

gdtline::gdtline(gdtpoint p1, gdtpoint p2)
	{
	double x1=p1.xcoord();
        double y1=p1.ycoord();
	double x2=p2.xcoord();
	double y2=p2.ycoord();

        a=(y1-y2);
	b=(x2-x1);
	c=(y2-y1)*x1-y1*(x2-x1);

	gdtsegment s(p1,p2);
	angle=s.direction();
	};


gdtline::~gdtline(){};

	double
gdtline::direction() const 
	{return angle;};

/////////////////////////////////////////////////////////////////////////////
//GDTPOLYGON

gdtpolygon::gdtpolygon(){};

gdtpolygon::gdtpolygon(gdtlist<gdtpoint> V)
	{
	polygon_vertices=V;
	};

gdtpolygon::~gdtpolygon()
	{
       	polygon_vertices.clear();
	};

	gdtlist<gdtpoint>
gdtpolygon::vertices() const
	{
       	return polygon_vertices;
	}


	gdtlist<gdtsegment>
gdtpolygon::edges()
	{
       	gdtlist<gdtsegment> solution;
       	if (polygon_vertices.length()<=1) return solution;

	list_item l1=polygon_vertices.first();
	list_item l2=polygon_vertices.succ(l1);
       	list_item fli=l1;
	while (l2)
		{
        	gdtsegment s(polygon_vertices.inf(l1),polygon_vertices.inf(l2));
        	//cout << "Added segment" << s << endl;
		solution.append(s);
		l1=l2;
		l2=polygon_vertices.succ(l2);
		}
	gdtsegment s1(polygon_vertices.inf(l1),polygon_vertices.inf(fli));
	solution.append(s1);
	//cout << "Added segment" << s1 << endl;
	return solution;
	};

			

	gdtlist<gdtpoint>
gdtpolygon::intersection(gdtsegment s) 
	{
	//std::cout << "\nFunction polygon.intersection" << std::endl;
       	gdtlist<gdtpoint> solution;
	gdtlist<gdtsegment> polygon_edges=edges();
       	gdtsegment temp_s;

        forall(temp_s,polygon_edges)
            	{
		//std::cout << "Testing segment "<< temp_s << " and " << s << std::endl;
		gdtpoint temp_point;
		gdtpoint last_added_point;

		if (s.intersection(temp_s,temp_point))
			{
			//std::cout << "Added point " << temp_point << std::endl;
			solution.append(temp_point);
			last_added_point=temp_point;	
			}
		}
            	//std::cout << "End of polygon.intersection: solution length=" << solution.length() << std::endl;
		return solution;
		};


	double
gdtpolygon::perimeter()
	{
	gdtlist<gdtsegment> e=edges();
	double per=0;
	gdtsegment s;
	forall(s,e)
		per+=s.length();
	return per;
	};


	double
gdtpolygon::area()
	{
	gdtlist<gdtpoint> vert=vertices();
	gdtlist<gdtpoint> vert2;
	double A=0;
	if (vert.length()<=1) return A;
	double xmin=0;
	double ymin=0;
	gdtpoint p,q;
	forall(p,vert)
		{
		if (p.xcoord()<xmin) xmin=p.xcoord();
		if (p.ycoord()<ymin) ymin=p.ycoord();
		}			
	forall(p,vert) 
		{
		p=p.translate(-xmin,-ymin);
		vert2.append(p);
		}
	forall(q,vert2)
		if (q.xcoord()<0 || q.ycoord()<0)
			{
			std::cerr << "Coordinate negative!\t";		
			std::cerr << "xmin=" << xmin << "\tymin=" << ymin ;
			std::cerr << std::endl;
			}
		
	list_item l1=vert2.first();
	list_item l2=vert2.succ(l1);
	list_item fli=l1;
	gdtpoint p1,p2;
	while (l2)
		{
		p1=vert2.inf(l1);
		p2=vert2.inf(l2);
		A+=(p1.ycoord()+p2.ycoord())*(p2.xcoord()-p1.xcoord());
		l1=l2;
		l2=vert2.succ(l2);
		}
	p1=vert2.inf(l1);
	p2=vert2.inf(fli);
	A+=(p1.ycoord()+p2.ycoord())*(p2.xcoord()-p1.xcoord());
	A=A/2;
		
	if (A>0) return A;
	else vert2.reverse();
	A=0;
	l1=vert2.first();
	l2=vert2.succ(l1);
	fli=l1;
	while (l2)
		{
		p1=vert2.inf(l1);
		p2=vert2.inf(l2);
		A+=(p1.ycoord()+p2.ycoord())*(p2.xcoord()-p1.xcoord());
		l1=l2;
		l2=vert2.succ(l2);
		}
	p1=vert2.inf(l1);
	p2=vert2.inf(fli);
	A+=(p1.ycoord()+p2.ycoord())*(p2.xcoord()-p1.xcoord());
	A=A/2;
	if (A<0) gdt_error("Area negativa");
	return A;
	};

/////////////////////////////////////////////////////////////////////////////
//GDTCIRCLE

gdtcircle::gdtcircle(gdtpoint c, double r)
	{        	
	center=c;
	radius=r;
	};

gdtcircle::~gdtcircle(){};


	gdtlist<gdtpoint>
gdtcircle::intersection(gdtsegment s)
	{
	gdtlist<gdtpoint> solutions;

	// circle is x^2 + y^2 + ax + by +c=0
	// line is dx+ey+f=0

        double x1=(s.start()).xcoord();
        double y1=(s.start()).ycoord();
	double x2=(s.end()).xcoord();
	double y2=(s.end()).ycoord();

	double alpha=center.xcoord();
	double beta=center.ycoord();

        double a=-2*alpha;
	double b=-2*beta;
	double c= pow(alpha,2) + pow(beta,2) - pow(radius,2);

	double d=(y1-y2);
	double e=(x2-x1);
	double f=(y2-y1)*x1-y1*(x2-x1);

        double a2=pow(a,2);
        double b2=pow(b,2);
	//double c2=pow(c,2);
	double d2=pow(d,2);
        double e2=pow(e,2);
	double f2=pow(f,2);

	double delta=(a2*e2+2*a*d*(2*f-b*e)+b2*d2+4*b*e*f-4*(c*(d2+e2)+f2));

	if (delta<0) return solutions; //no intersections

	/*if (delta==0)
		{
		cout << "Delta=0" << endl;
        	cout << "a="<<a<<"\tb="<<b<<"\tc="<<c<<"\td="<<d<<"\te="<<e<<"\tf="<<f<< endl;
		}
         */

	double denominator=2*(d2+e2);
	//cout << "denominator=" << denominator << endl;

        double inter_x1=-(sqrt(delta)*fabs(e)+a*e2-d*(b*e-2*f))/denominator;
	double inter_y1=(d*sqrt(delta)*SIGN(e)+a*d*e-b*d2-2*e*f)/denominator;
	
	if(inter_x1==-0) inter_x1=0;

	double inter_x2=(sqrt(delta)*fabs(e)-a*e2+d*((b*e-2*f)))/denominator;
	double inter_y2=-(d*sqrt(delta)*SIGN(e)-a*d*e+b*d2+2*e*f)/denominator;

	if(inter_x2==-0) inter_x2=0;

        if (inter_x1>=MIN(x1,x2) && inter_x1<=MAX(x1,x2) && inter_y1>=MIN(y1,y2) && inter_y1<=MAX(y1,y2))
		{
            	gdtpoint intersection1(inter_x1,inter_y1);
		solutions.append(intersection1);
		//cout << "Added: " << intersection1 << endl;
		};

	if (inter_x2>=MIN(x1,x2) && inter_x2<=MAX(x1,x2) && inter_y2>=MIN(y1,y2) && inter_y2<=MAX(y1,y2))
		{
            	gdtpoint intersection2(inter_x2,inter_y2);
		solutions.append(intersection2);
            	//cout << "Added: " << intersection2 << endl;
		};

	return solutions;
	};

/////////////////////////////////////////////////////////////////////////////
//INPUT-OUTPUT

std::ostream& operator<< (std::ostream& os, const gdtpoint& p)    {
        return (os << "(" << p.xcoord()<<","<< p.ycoord() << ")");
		};

bool operator!=(const gdt::gdtpoint& p1, const gdt::gdtpoint& p2) {
     return ( (p1.xcoord()!=p2.xcoord()) || (p1.ycoord()!=p2.ycoord()) );
	};


bool operator==(const gdt::gdtpoint& p1, const gdt::gdtpoint& p2) {
     return ( (p1.xcoord()==p2.xcoord()) && (p1.ycoord()==p2.ycoord()) );
	};

std::ostream& operator<< (std::ostream& os, const gdtsegment& s)    {
        return (os << "[" << s.start()<<"--"<< s.end() << "]");
		};

std::istream& operator>> (std::istream& is, gdtpoint& p){

    char c;
	double x,y;
	is >> c;
    if(c!='(')
		{
		std::cout << "Letto c=" << c << std::endl;
		gdt_error("errore lettura carattere (");
		exit(1);
		};
	is >> x;
	is >> c;
	if (c!=',') {
		gdt_error("errore lettura carattere ,");
		exit(1);
		};
	is >> y;
	is >> c;
	if (c!=')') {
		gdt_error("errore lettura carattere )");
		exit(1);
		};
	p=gdtpoint(x,y);

	return is;
};

}
