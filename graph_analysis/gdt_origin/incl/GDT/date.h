#ifndef _DATE_H_
#define _DATE_H_

#include<string>
#include<iostream>
#include<time.h>

using namespace std;


class date {

	private:
		
		int day;
		int month;
		int year;
		
		void swap(date& d);

	public:

		date();
		date(int g, int m, int a);
		
		int get_day() const;
		int get_month() const;
		int get_year() const;
		
		void set_day(int g);
		void set_month(int m);
		void set_year(int a);

		bool valid();
		/**
		Method compare. 
		Return true if date d precedes the current date
		*/
		bool compare(date d);   
		void nextday();
		int  diff(date d);
		void printdate(char c);
		void printdate();
				
};



std::ostream& operator<<(std::ostream& stream, const date& d);

bool operator==(const date& d1, const date&d2);

bool operator!=(const date& d1, const date&d2);

#endif
