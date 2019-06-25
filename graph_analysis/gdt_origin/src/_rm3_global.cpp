
/*******************************************************************************
+
+  _rm3_global.c
+
+  This file is part of GDToolkit. It can be
+  used free of charge in academic research and teaching.
+  Any direct or indirect commercial use of this software is illegal
+  without a written authorization of
+  the Dipartimento di Informatica e Automazione
+  Universita' di Roma Tre, Roma, ITALIA
+
+ 
+
*******************************************************************************/


#include "expiration_date.cpp"


//-----------------------------------------------------------------------------
// global: global definitions for the rm3 files
//
// W.Didimo e A.Leonforte (1996)
//-----------------------------------------------------------------------------

#include <stdlib.h>
//#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <GDT/rm3_global.h>
#include <GDT/date.h>

const int INFINITE = INT_MAX;
const int BOUND_ON_NODES = 20;

//inline ostream& operator <<(ostream& out,const gdt::gdtlist<int>& x)
//     {out << *((unsigned int *)((void *)&x)) ;return(out);}
#ifdef __GNUC__
inline  std::istream& operator >>(std::istream& in,gdt::gdtlist<int>& x)
     {in >> *((unsigned int *)((void *)&x)) ;return(in); }
#endif



	int
random
	(int a, int b)
	{
	static bool first_call = true;
	if (first_call)
		{
		// -------------------------------
		// Initialize the random generator
		// -------------------------------
		//
                // ### Begin import from GDT fork of W. Didimo on June, 18 2004
		//int seed = ((int)time(NULL)%100) + 1;
		int seed = ((int)time(NULL)) + 1;
                // ### End import
		srand (seed);	
		first_call = false;
		} 
	int number;
	number = (int)(a + ((b-a+1) * (double)rand()/(double)(RAND_MAX+1.0)));
	return number;
	}
	
	
	
	int
fact 
	(int n)
	{
	int f;
	if (n == 0) f = 0;
	else
		{
		f = 1;
		for (int i=2; i<=n; i++) f = f*i;
		}
	return f;
	}



//	array<int_perm>
	gdt::gdtarray<gdt::gdtlist<int> >
find_all_permutations
	(int n)
	{
	//map<int,int_perm> permutation_number;
	int number_of_permutations = fact(n);
	//	array<int_perm> permutation_number (number_of_permutations);
       	gdt::gdtarray<gdt::gdtlist<int> > permutation_number (number_of_permutations);
	
	gdt::gdtarray<int> p(n); 			// p[i] = element in position i-1;
	int i = 0;
	int j,k,h,temp;
	bool end_permutations = false;
	
	// make the first permutation
	//
	for (j = 0; j <= n-1; j++) 
		{
		p[j] = j;
		permutation_number[i].append(j);
		}
		
	//cout << "\nfirst permutation ok!" << flush; 
		
	// make all other permutations
	//
	while (!end_permutations)
		{
		// ----------------------- 1 -------------------------
		// saerch the highest index j such that p[j] < p[j+1].
		// If it not exists then end algorithm.
		// ---------------------------------------------------
		//
		end_permutations = true;
		for (j = n-2; j >= 0; j--)
			if (p[j] < p[j+1])
				{
				end_permutations = false; 
				break;
				}
		
		if (!end_permutations)
			{
			// --------------------------------- 2 ---------------------------------
			// search the index k such that p[k] = min{p[h] : h > j and p[h] > p[j]} 
			// ---------------------------------------------------------------------
			//
			k = j+1;
			for (h = j+2; h <= n-1; h++)
				if ((p[h] > p[j]) && (p[h] < p[k])) k = h;
				
			// --------------------------------- 3 ---------------------------------
			// swap p[j] and p[k] and reverse the order of the element p[h] : h > j 
			// ---------------------------------------------------------------------
			// 
			temp = p[k];
			p[k] = p[j];
			p[j] = temp;
			
			for (h = 1; h <= (n-1-j)/2; h++)
				{
				temp   = p[n-h];
				p[n-h] = p[j+h]; 
				p[j+h] = temp;
				}
			
			// ------------------- 4 -------------------
			// copy the array p in permutation_number[i]
			// -----------------------------------------
			//
			i++;
			for (j = 0; j < n; j++)
				permutation_number[i].append(p[j]);
			//cout << "\npermutation " << i << " ok!" << flush;
			}
		}
	//cout << "\nend permutations\n" << flush;
	return permutation_number;
	};


//inline int gdt::Hash(void* p) {return int(long(p));};
//inline int gdt::Hash(int p) {return p;};


	  std::string
replace_all(std::string s,std::string s1, std::string s2) {
		//std::cout << "Chiamata di replace all" << std::endl;
		//std::cout << "S=" << s << "\ts1=" << s1 << "\ts2=" << s2 << std::endl;
		std::string result;
		std::string temp;
        int i=0;
		int len=s1.length();
		bool stop=false;
		if (s.find(s1,0)<0)
			{
			result=s;
			return result;
			}
        while(!stop)
			{
            int j=s.find(s1,i);
			if (j<0) stop=true;
			else
				{
            	temp.assign(s,i,j-i);
				result=result+temp;
            	result=result+s2;
				i=j+len;
				}
             }
        temp.assign(s,i,s.length());
		result=result+temp;
		//std::cout << "replace_all ritorna: " << result << std::endl;
		return result;
	};



		int
gdt::gdtceil(double f) {
	int n=(int) f;
	//cout << "n=" << n << endl;
	if ((double)n == f) return n;
	else return (n+1);
	};
	
	
	std::string
gdt_itoa(
	int n
	)
	{
	std::string result="";
	if(n==0)
		result="0";
	while (n>0)
		{
		int m=n%10;
		if (m==0) result="0"+result;
		if (m==1) result="1"+result;
		if (m==2) result="2"+result;
		if (m==3) result="3"+result;
		if (m==4) result="4"+result;
		if (m==5) result="5"+result;
		if (m==6) result="6"+result;
		if (m==7) result="7"+result;
		if (m==8) result="8"+result;
		if (m==9) result="9"+result;
		n=n/10;
		}
	return result;	
}



	//Time limited version of the library

	bool
	check_date()
	{
	date today;
	#ifdef TIME_LIMITED	
	date expiration(EXPIRATION_DAY,EXPIRATION_MONTH,EXPIRATION_YEAR);
	//cout << "TODAY:" << today << endl;
	//cout << "Expiration date: " << expiration << endl;		
	if (today.compare(expiration)) return false;
	else return true;
	#else
	return true;
	#endif
	
	
	}

