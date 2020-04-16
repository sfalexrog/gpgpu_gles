#pragma once

#include <iostream>
#include <chrono>
#include <string>

namespace Debug
{

class Trace
{
private:
    std::chrono::system_clock::time_point start;
    std::string name;
public:
    Trace(const std::string& name) :  start(std::chrono::system_clock::now()), name(name){}
    ~Trace()
    {
        auto duration = std::chrono::system_clock::now() - start;
        std::cout << "Trace " << name << " took " << duration.count() / (1000.0 * 1000.0) << " milliseconds" << std::endl;
    }
};

}

#ifdef DO_TRACE
  #define TRACE_FN Debug::Trace trace_object(__PRETTY_FUNCTION__)
#else
  #define TRACE_FN
#endif
