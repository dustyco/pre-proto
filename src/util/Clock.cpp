

#include <iostream>
using namespace std;

#include <boost/thread/locks.hpp>
#include <boost/thread/condition_variable.hpp>
using namespace boost;

#include "Clock.h"
using namespace boost::chrono;

// The 1000000 stuff is a hack because I don't know how to multiply a
// duration by a float
void Clock::warp (float speed) {
	lock();
		time now = chrono::steady_clock::now();
//		m_ref_warped = (m_warped) ? (m_ref_warped + (now-m_ref_real)*m_speed) : (now);
		m_ref_warped = now;
		m_ref_real = now;
		m_speed = speed;
		m_warped = true;
	unlock();
}
void Clock::unwarp () {
	lock();
//		unique_lock<mutex> lock(*this);
		m_warped = false;
	unlock();
}
Clock::time Clock::getTime () {
	lock();
		time r;
		time now = chrono::steady_clock::now();
//		r = (m_warped) ? (m_ref_warped + (now-m_ref_real)*m_speed) : (now);
		r = now;
	unlock();
	return r;
}
void Clock::setEpoch () {
	m_epoch = getTime();
}
void Clock::setEpoch (time epoch) {
	m_epoch = epoch;
}
Clock::duration Clock::getDurationSinceEpoch () {
	return getTime() - m_epoch;
}

/*
// Doesn't work yet because of no interoperability between boost date_time and chrono time types
void Clock::waitUntil (ptime time) {
	waitFor(time - getTime());
}
void Clock::waitFor   (time_duration duration) {
	chrono::steady_clock::time_point timeout = chrono::steady_clock::now() + chrono::microseconds(duration.total_microseconds());
	condition_variable cv;
	mutex m;
	unique_lock<mutex> l(m);
	while (true) {
		if (cv.wait_until(l, timeout) == cv_status::timeout) {
			// Timed out
			break;
		}
	}
}
*/

