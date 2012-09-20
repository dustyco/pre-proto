#pragma once
#include <boost/chrono.hpp>

class timer {
public:
	timer(bool autoReset=true); // Resets when time is retrieved if autoReset=true
	boost::chrono::nanoseconds getTime();
	double read(); // Same as getTime but converts to double
	void resetTime();

private:
	boost::chrono::steady_clock::time_point m_start;
	bool m_autoReset;
};

void tsleep(double secs);
