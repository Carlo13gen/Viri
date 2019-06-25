#include<iostream>
#include<GDT/date.h>

date::date() {
   
	#ifndef WIN32
	struct tm * timeinfo;	
	time_t rawtime;

  	time ( &rawtime );
  	timeinfo = localtime ( &rawtime );

  	day = timeinfo->tm_mday;
  	month = (timeinfo->tm_mon+1);
  	year = (timeinfo->tm_year+1900);

	#else
	struct tm  timeinfo;
	__time64_t longtime;
	_time64(&longtime);
	_localtime64_s(&timeinfo,&longtime);
	day = timeinfo.tm_mday;
  	month = (timeinfo.tm_mon+1);
  	year = (timeinfo.tm_year+1900);
	#endif
}


date::date(int g, int m, int a) {
	day=g;
	month=m;
	year=a;
	};




int date::get_day()const {return day;};
int date::get_month() const  {return month;};
int date::get_year() const  {return year;};

void date::set_day(int g) {day=g;};
void date::set_month(int m) {month=m;};
void date::set_year(int a) {year=a;};


// Stabilisce se una date è valida
bool date::valid() {
	
	int giorni[12]={31,28,31,30,31,30,31,31,30,31,30,31};
	
	//bisestile
	if ((year%4==0 && year%100!=0) || year%400==0) giorni[1]=29;

	if (day<=0 || month<=0 ) return false;
	if(month>12) return false;
	if (day>(giorni[month-1])) return false;
	return true;
};


//riporta vero se d è minore o uguale alla date su cui è invocato il metodo
bool date::compare(date d) {
		
	int g1=d.day;
	int m1=d.month;
	int a1=d.year;
	
	int g2=this->day;
	int m2=this->month;
	int a2=this->year;

	if (g1==g2 && m1==m2 && a1==a2) return false;
	if (a1>a2) return false;
	if (a1==a2 && m1>m2) return false;
	if (a1==a2 && m1==m2 && g1>g2) return false;
	
	return true;
	};



void date::nextday(){
	
	if (!this->valid()) return;
	int giorni[12]={31,28,31,30,31,30,31,31,30,31,30,31};
	//bisestile
	if ((year%4==0 && year%100!=0) || year%400==0) giorni[1]=29;
	if (day<giorni[month-1]) 
		{
		day++;
		return;
		};
	day=1;
	if (month==12) {
		month=1;
		year++;
		}
	else
		month++;
};


void date::swap(date& d) {
	int temp_day=this->day;
	int temp_month=this->month;
	int temp_year=this->year;

	this->day=d.day;
	this->month=d.month;
	this->year=d.year;
	d.day=temp_day;
	d.month=temp_month;
	d.year=temp_year;

};


// giorni di differenza tra due date 

int date::diff(date d1) {
	date d2(day,month,year);
	
	if (d1==d2) return 0;
	if (!d2.compare(d1)) d2.swap(d1); // d1 è minore

	int diff=0;
	while (d1!=d2) {
		d1.nextday();
		diff++;
	}
	return diff;
};


void date::printdate(char c) {
	std::cout << day << c << month << c << year << std::endl;
};

void date::printdate() {
	this->printdate('/');
};

// fine dei metodi della classe date




std::ostream& operator<<(std::ostream& stream, const date& d) {
	stream << d.get_day()<<"/"<< d.get_month()<<"/"<< d.get_year();
	return stream;


};

bool operator==(const date& d1, const date& d2) {
	return (d1.get_day()==d2.get_day() && d1.get_month()==d2.get_month() && d1.get_year()==d2.get_year());
};

bool operator!=(const date& d1, const date&d2) {
	return (d1.get_day()!=d2.get_day() || d1.get_month()!=d2.get_month() || d1.get_year()!=d2.get_year());
}; 
