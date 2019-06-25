/*! \file */

#ifndef _STOPWATCH_H
#define _STOPWATCH_H

//#include "define.h"

/** The following class implements a stopwatch. You may have more than
    one stopwatch running without any interference and without any
    additional computational work.  The clock interrupt has a
    frequency of 100 Hz (this is stated by the macro HZ, in
    /usr/include/asm/params.h).  The value of the clock is stored in
    the tms_utime field of the struct tms (in the file
    /usr/include/sys/times.h), and is of type clock_t (a long int
    defined in /usr/include/bits/types.h).  Since a long int has 32
    bits (/usr/include/values.h) it follows that the clock should have
    the same value after 2^32/100 secs, that is 715828 minutes (497
    days).  Since clock_t is a long int (and not an "unsigned long
    int") we don't have to worry about when the wrapping occurs.
    Suppose, for example that you start your stopwatch at MAXLONG and
    stop it at MINLONG+10, then (MINLONG+10) - MAXLONG = 11 (correct!
    11 ticks passed). 

    EXAMPLE OF USE:

    stopwatch* stop = new stopwatch();
    stop->start();
    ...
    ...    HEAVY OPERATIONS HERE
    ...
    cout.form("computational time = %f secs\n",stop->get()); 
    delete(stop);

*/

typedef enum {
  USER_TIME,
  SYSTEM_TIME,
  ABSOLUTE_TIME
} time_type;



class stopwatch {

 private:

  // ---------------
  // private members
  // ---------------
  
  time_type  _type;    // what do you want to measure? absolute time
                       // (SYSTEM_TIME) or the time you are using
                       // (USER_TIME)?
  long long int   _previous_clock_value;    // previous value of user time.
  long long int   _total_time_value;        // total user time.
  bool       _time_pause;              // stopwatch in pause

  // ---------------
  // private methods
  // ---------------

  /** Take the time of the host clock now. The output depends on the
   * type of the stopwatch you are considering: USER_TIME or
   * SYSTEM_TIME */

  long long int _take_the_clock();
 
  // --------------
  // public methods
  // --------------

 public:

  /** Create a new stopwatch and starts it. If no time_type is given the 
   * stopwatch will count the user time, otherwise if SYSTEM_TIME is specified,
   * the stopwatch will count absolute time. When created the stopwatch is up
   * and runnig. */

  stopwatch(time_type = USER_TIME);

  /** Start the stopwatch. If the stopwatch is already running, then
   * calling this function has no effect. If the stopwatch is in pause,
   * then calling this function will make it count time again. */

  void  start();

  /** Reset the stopwatch. Leaves the stopwatch in pause with zero as
   * a total time. */ 

  void reset();

  /** Return the time elapsed till now. The value returned is in seconds.
   * The stopwatch will continue counting */

  double get();

  /** Put the stopwatch in pause: the time is no more counted.  A
   * second calling of this function will be ignored.  The value of
   * the time (in seconds) elapsed till now is returned. To restart the
   * stopwatch use start(). */

  double pause(); 
  
};

#endif // _STOPWATCH_H

