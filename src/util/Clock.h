#pragma once


#include <boost/chrono.hpp>


// A variable speed clock that provides:
//   -Absolute time
//   -Duration since a specific epoch
// and can be waited on
//
class Clock : private boost::mutex {
public:
	typedef boost::chrono::steady_clock::time_point time;
	typedef boost::chrono::steady_clock::duration   duration;

public:
	Clock () : m_warped(false) {};
	void warp (float speed);
	void unwarp ();
	time getTime ();
	
	void setEpoch ();
	void setEpoch (time);
	duration getDurationSinceEpoch ();
	
//	void waitUntil (time);
//	void waitFor   (duration);
	
private:
	bool  m_warped;
	float m_speed;
	time  m_ref_real;
	time  m_ref_warped;
	time  m_epoch;
};
