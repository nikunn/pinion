#ifndef _STOPWATCH_H
#define _STOPWATCH_H

#include <iostream>
#include <ctime>
#include <chrono>

#include "Clock.h"

// Useful Macro to benchmark a scope
//=============================================================================\'
#define BENCH_SCOPE(name)                                                      \
  static Benchmark bench(name);                                                \
  StopWatch sw(&bench);                                                        \
//=============================================================================\'


//================================= StopWatch ==================================

class StopWatch
{
  //=============================== Clock ================================

  using Clock = ClockGen<std::chrono::high_resolution_clock>;
  using TimePoint = Clock::TimePoint;
  using TimeSpan = Clock::TimeSpan;


  //=============================== Benchmark ================================

  class Benchmark
  {
  public:

    //========================== public functions ==========================
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

    //========================== private members ===========================
    std::string _name;
    TimeSpan _sum;
    long _div;
  };

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
