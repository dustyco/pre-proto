#pragma once


#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/mutex.hpp>


// A variable speed clock that provides:
//   -Absolute time
//   -Duration since a specific epoch
// and can be waited on
//
class Clock : private boost::mutex {
public:
	typedef boost::posix_time::ptime         time;
	typedef boost::posix_time::time_duration duration;

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
