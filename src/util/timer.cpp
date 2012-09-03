#include "timer.h"

typedef boost::chrono::duration<double> dsec;

timer::timer(bool ar) : m_autoReset(ar) {
	resetTime();
}

boost::chrono::nanoseconds timer::getTime() {
	boost::chrono::nanoseconds ms = boost::chrono::steady_clock::now() - m_start;
	if(m_autoReset) resetTime();
	return ms;
}

double timer::read() {
	boost::chrono::nanoseconds ns = getTime();
	dsec ds = ns;
	return ds.count();
}

void timer::resetTime() {
	m_start = boost::chrono::steady_clock::now();
}