
#include <GDT/gdtgeometry.h>

using namespace gdt;
using namespace std;

int main(int argc, char** argv) {

	cout << "Test strutture geometriche" << endl;
	gdtpoint p0(0,0);
	gdtpoint p1(1,1);
	gdtpoint p2(4,5);
	gdtpoint p3(4,5);
	gdtpoint p4(2,3);
	gdtpoint p5(1,2);
	gdtpoint p6(4,3);
	gdtpoint p7(0,2);
	gdtpoint p8(8,3);

	double angle=M_PI/2;

	cout << p1 << "\t" << p2 << "\t" << p3 << "\t" << p4 << endl;
	cout << "Distance p1-p2=" << p1.distance(p2) << endl;
	cout << "Distance p2-p1=" << p2.distance(p1) << endl;
	if (p2==p3) cout << "Operator == OK" << endl;
	if (p2!=p4) cout << "Operator != OK" << endl;
	cout << "Rotate : " << p4 << " with respect to " << p1 << "\t angle=" << angle << endl;
	cout << p4.rotate(p1,angle) << endl;

	gdtsegment s1(p5,p6);
	gdtsegment s2(p7,p8);

 	cout << "Test intersection between "<< s1 << " and " << s2 << endl;
	gdtpoint intersection_point;

	if(s1.intersection(s2,intersection_point))
		cout << intersection_point << endl;
	else
		cout << "No intersection" << endl;


	gdtlist<gdtpoint> vert;
	gdtpoint pp1(2,2);
	gdtpoint pp2(3,5);
	gdtpoint pp3(5,5);
	gdtpoint pp4(7,3);
	gdtpoint pp5(5,1);
	gdtpoint pp6(3,1);

	vert.append(pp1);
	vert.append(pp2);
	vert.append(pp3);
	vert.append(pp4);
	vert.append(pp5);
	vert.append(pp6);

	cout << "Creation of Polygon" << endl;
	gdtpolygon pol(vert);

	gdtlist<gdtsegment> edges=pol.edges();
	cout << "OK" << endl;
	gdtsegment temp1;

	forall(temp1,edges)
		{
	        cout << temp1 << "\t";
		}
	cout << endl;

	gdtlist<gdtpoint> intersection_points=pol.intersection(s2);
	gdtpoint temp;
	forall(temp,intersection_points)
	        {
	        cout << temp << "\t";
		}
	cout << endl;

	p1=gdtpoint(3,6);
	p2=gdtpoint(5,10);
	p3=gdtpoint(4,3);
	p4=gdtpoint(4,8);
	gdtsegment s(p1,p2);
	if (s.contains(p3))
		cout << s << " contains " << p3 << endl;
	else
		cout << s << " does not contain " << p3 << endl;

	if (s.contains(p4))
		cout << s << " contains " << p4 << endl;
	else
		cout << s << " does not contains " << p4 << endl;

	//cout << "Insert a point:" << endl;
	//cin >> p1;
	//cout << "Read point: "<< p1 << endl;
	return 0;
}
