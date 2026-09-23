#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <cstdint>

class Timer {
public:
    // Alias
    using Nanos = std::chrono::nanoseconds;
    using Micros = std::chrono::microseconds;
    using Millis = std::chrono::milliseconds;
    using Seconds = std::chrono::seconds;
    using Minutes = std::chrono::minutes;
    using Hours = std::chrono::hours;

    //Constructor to start timing as the Timer is called
    Timer() 
    : startTime(std::chrono::steady_clock::now()) {}

    // Restart the clock
    void restart() {
        startTime = std::chrono::steady_clock::now();
    }

    template <typename T>
    uint64_t glance() {
        // get the current time 
        auto currentTime = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<T>(currentTime - startTime).count(); // return the time elasped
    }

    template <typename T>
    uint64_t click() {
        // get the current time 
        auto currentTime = std::chrono::steady_clock::now();
        //calculate the elasped time
        uint64_t elaspedTime = std::chrono::duration_cast<T>(currentTime - startTime).count();
        //start the timer again
        startTime = currentTime;
        return elaspedTime; // return the time elasped
    }

private:
    // We start the timer as Timer is called
    std::chrono::time_point<std::chrono::steady_clock> startTime;

    
};


#endif  // TIMER_H
