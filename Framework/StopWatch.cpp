#include <unistd.h>

#include "Framework/StopWatch.h"
#include "Framework/Logger.h"

//================================= Benchmark ==================================

// Constructor
StopWatch::Benchmark::Benchmark(const std::string& name) : _name(name), _sum(0), _div(0)
{
}

// Destructor, dump logs
StopWatch::Benchmark::~Benchmark()
{
  // Dump the log
  INFO_PF("Benchmark %s %u %u", getName().c_str(), avg().as<MicroSeconds>(), div());
}

// Add a point to the average
void StopWatch::Benchmark::add(const TimeSpan& ts)
{
  _sum += ts;
  _div += 1;
}


//================================= StopWatch ==================================

// Constructor, start the stopwatch
StopWatch::StopWatch(Benchmark* benchmark) : _benchmark(benchmark)
{
  // Get the current time to start the stopwatch here
  _start = Clock::now();
}

// Destructor, stop the stopwatch and add point to benchmark
StopWatch::~StopWatch()
{
  // Get the current time to stop the stopwatch there
  _stop = Clock::now();

  // Add the elapsed time in the benchmark
  _benchmark->add(_stop - _start);
}
