#ifndef _CLOCK_H
#define _CLOCK_H

#include <iostream>
#include <ctime>
#include <chrono>

// Some useful typedef
using Seconds = std::chrono::seconds;
using MilliSeconds = std::chrono::milliseconds;
using MicroSeconds = std::chrono::microseconds;
using NanoSeconds = std::chrono::nanoseconds;


//=================================== Clock ====================================
// Clock class
template <typename CLOCK, typename TYPE = long>
class ClockGen
{
public:

  //================================ Typedefs ================================
  using Duration = typename CLOCK::duration;
  using TimePoint = typename CLOCK::time_point;

public:

  //============================ public functions ============================
  // Get the current time
  static TimePoint now() { return CLOCK::now(); }
  static double resolution() { return (double) CLOCK::period::num / CLOCK::period::den; }

  //================================= TimeSpan ===================================

  // Timespan is a wrapper of chrono duration
  class TimeSpan : public Duration
  {
  public:

    //============================ public functions ============================
    template<typename T>
    TimeSpan(T&& val) : Duration(std::forward<T>(val)) {};

    // Function to convert a timespan in a chosen unit (sec ms us...)
    template <typename UNIT>  // PLOP
    TYPE as() { return std::chrono::duration_cast<UNIT>(Duration(this->count())).count(); }
  };
};


#endif
