#ifndef EXOCET_TIMER
#define EXOCET_TIMER

#include "types.h"
#include <chrono>

class Timer {
private:
    using clock_t = std::chrono::steady_clock;
    using second_t = std::chrono::duration<double, std::ratio<1>>;    
    std::chrono::time_point<clock_t> m_beg;
public:
    Timer() : m_beg(clock_t::now()) {}
    void reset() {
        m_beg = clock_t::now();
    }
    double elapsed() const {
        return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
    }
};

#endif
