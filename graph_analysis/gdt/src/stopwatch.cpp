#include <stdarg.h>
#include <sys/times.h>
#include <sys/time.h>      // for gettimeofday, timeval, timezone
#include <sys/param.h>

//#include <sys/utime.h>
//#include <sys/timeb.h>

#include <limits.h>        // for MAXLONG, MINLONG
#include <GDT/stopwatch.h>


// ==================================================

long long int stopwatch::_take_the_clock() {
  tms my_tms;       // the tms structure to extract times.
  struct timeval  tv;
  struct timezone tz;
  long int result = 0;  // used only for absolute time. Initialized to avoid a compilation warning

  switch(_type) {
  case USER_TIME:
    times(&my_tms);   // take the time
    return my_tms.tms_utime;
    break;
  case SYSTEM_TIME:
    times(&my_tms);   // take the time
    return my_tms.tms_stime;
    break;
  case ABSOLUTE_TIME:
    gettimeofday (&tv, &tz);
    result = tv.tv_sec;          // number of secs from epoch
    result = result * 100;       // number of 100th of sec from epoch
    result += tv.tv_usec / 10000;
    return result;
    break;
  }
  return result; /* you will never arrive here.
		    I'm avoiding a compilation warning */
}

// ==================================================

stopwatch::stopwatch(time_type t) {
  _type = t;
  _time_pause           = false;    // not in pause now!

  start();
}

// ==================================================

void stopwatch::start() {

  if( _time_pause ) {   // restart it after a time_pause

    _time_pause = false;
    _previous_clock_value = _take_the_clock();

  } else {              // the stopwatch is running, you meant to
                        // reset it

    _total_time_value     = 0;        // reset the stopwatch
    _previous_clock_value = _take_the_clock();
  }
}

// ==================================================

void stopwatch::reset() {

    _time_pause = false;
    _total_time_value = 0;
}

// ==================================================

double stopwatch::get() {
  if( !_time_pause ) { // the stopwatch is running

    long int _now = _take_the_clock();

    // time lapsed between last "start_stopwatch" and now
    //
    long int diff = _now - _previous_clock_value;

    // countinue counting time
    //
    _previous_clock_value = _now;
    _total_time_value += diff;
  }
  return ((double)_total_time_value)/HZ;
}

// ==================================================

double stopwatch::pause()
{
  if( !_time_pause ) // only if running
  {
    _time_pause = true;
    long int diff = _take_the_clock() - _previous_clock_value;
    _total_time_value += diff;

    // _previous_clock_value has no meaning now !

  }
  return ((double)_total_time_value)/HZ;
}




