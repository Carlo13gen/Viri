/*******************************************************************************
+
+  gdtcolor.h
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

#ifndef __gdtcolor__
#define __gdtcolor__


typedef int color;


#define DEF_COLOR -16
#define invisible  -1
#define white      0
#define black      1
#define red        2
#define green      3
#define blue       4
#define yellow     5
#define violet     6
#define orange     7
#define cyan       8
#define brown      9
#define pink      10
#define green2    11
#define blue2     12
#define grey1     13
#define grey2     14
#define grey3     15
#define ivory     16




/*
enum {
  DEF_COLOR = -16,
  invisible = -1,
  white  =  0,
  black  =  1,
  red    =  2,
  green  =  3,
  blue   =  4,
  yellow =  5,
  violet =  6,
  orange =  7,
  cyan   =  8,
  brown  =  9,
  pink   = 10,
  green2 = 11,
  blue2  = 12,
  grey1  = 13,
  grey2  = 14,
  grey3  = 15,
  ivory  = 16
};

*/

/*
class gdtcolor {

	private:

	int R;
	int G;
	int B;


	int color_index;

	public:

	gdtcolor(int color_name)	{
    	color_index=color_name;
	};

	gdtcolor() {
	}

	ostream& operator<<(ostream& os) {
    return ( os << color_index );
	};

	~gdtcolor(){};
};



}   */
#endif
