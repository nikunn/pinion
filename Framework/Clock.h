#ifndef _CLOCK_H
#define _CLOCK_H

#include <iostream>
#include <ctime>
#include <chrono>

// Useful Macro to benchmark a scope
//=============================================================================\'
#define BENCH_SCOPE(name)                                                      \
  static Benchmark bench(name);                                                \
  StopWatch sw(&bench);                                                        \
//=============================================================================\'


// Some useful typedef
typedef std::chrono::seconds Seconds;
typedef std::chrono::milliseconds MilliSeconds;
typedef std::chrono::microseconds MicroSeconds;


//=================================== Clock ====================================
// Clock class
template<typename CLOCK>
class ClockType
{
public:

  //================================ Typedefs ================================
  typedef std::chrono::time_point<CLOCK> TimePoint;

public:

  //============================ public functions ============================
  // Get the current time
  static TimePoint Now() { return CLOCK::now(); }
};

// Define the default Clock for us
typedef ClockType<std::chrono::high_resolution_clock> Clock;

// Define the timepoint class based on this clock
typedef Clock::TimePoint TimePoint;


//================================= TimeSpan ===================================

// Define a time type
typedef float TimeType;

// Use the chrono duration
typedef std::chrono::duration<TimeType> Duration;

// Timespan is a wrapper of chrono duration
class TimeSpan : public Duration
{
public: 

  //============================ public functions ============================
  template<typename T>
  TimeSpan(T&& val) : Duration(std::forward<T>(val)) {};

  // Function to convert a timespan in a chosen unit (sec ms us...)
  template <typename UNIT>  // SALE
  TimeType as() { return std::chrono::duration_cast<UNIT>(Duration(this->count())).count(); }
};


//================================= Benchmark ==================================

class Benchmark
{
public:

  //============================ public functions ============================
  Benchmark(const std::string& name);
  ~Benchmark();

  // Add a point to the average
  void add(const TimeSpan&);

  // Get the average elapsed time
  TimeSpan avg() const { return _div > 0 ? _sum / _div : TimeSpan(0); }

  // Get the number of point in the average
  long div() const { return _div; }

  // Get the benchmark name
  const std::string& getName() const { return _name; }

private:

  //============================ private members =============================
  std::string _name;
  TimeSpan _sum;
  long _div;
};


//================================= StopWatch ==================================

class StopWatch
{
public:

  //============================ public functions ============================
  StopWatch(Benchmark*);
  ~StopWatch();

private:

  //============================ private members =============================
  Benchmark* _benchmark;
  TimePoint _start;
  TimePoint _stop;
};


#endif
