#pragma once
#include <boost/chrono.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <map>
#include <list>
#include <utility>
#include <string>

#include "Profile.h"

// Yes, it should be named Profiler.h
// No, it can't be because NTFS is an asshole

namespace Profiling {
	/* The core of the Paradox profiling system - a low-overhead way of tracking your code's
	 * performance that can easily be disabled in release builds. The fundamental components
	 * of the profiling system are Sections, Tracepoints, and Spans. Sections and Spans are
	 * very similar, with the only difference being that sections can contain other sections
	 * or spans while spans cannot contain other spans or sections, and that spans are faster
	 * to enter and leave. Both spans and sections measure the time elapsed between entering
	 * and leaving them. Tracepoints simply record the number of times and frequency with which
	 * they are triggered. Tracepoints are completely independent of the span/section system,
	 * and can be marked at any point in the code. If an attempt is made to end begin a span
	 * within an existing span or to end a span multiple times, a runtime_error exception will
	 * be raised. The same will happen if a span crosses a section boundary. Spans must be
	 * fully contained within a single section to work correctly.
	 *
	 * The Paradox Profiler is fully thread-safe, and each thread has its own Profiler instance
	 * and corresponding profiling tree. Threads can view the profiles of other threads through
	 * their own Profiler's getExternalProfile method, although keep in mind that each call to
	 * the Profile's read methods acts as a synchronization point between the thread being profiled
	 * and the thread reading the Profile. For debug overlays and the like, excessive synchronization
	 * can be avoided by locking the Profile before use and unlocking afterwards.*/
	typedef boost::chrono::nanoseconds nsec;
	class Profiler {
	public:
		static Profiler& getInstance();
		static Profiler* getInstancePtr();
		~Profiler();

		void beginSection(const char* name);
		void endSection();

		void beginSpan(const char* name);
		void endSpan();

		void tracepoint(const char* name, const char* fname);

		// Read methods
		boost::shared_ptr<Profile> getExternalProfile(std::string name);
		Profile& getProfile();

		// These configure the thread's local instance of the Profiler
		void setName(const char* name); // Name the thread's profile tree
	private:
		Profiler();

		unsigned long insertTracepoint(std::string name, std::string fname);
		unsigned long insertSection(std::string name, unsigned long parent);
		unsigned long insertSpan(std::string name, unsigned long parent);

		// State
		bool inSpan;
		std::map<std::pair<unsigned long, std::string>, unsigned long> m_sectionIDs, m_spanIDs;
		std::map<std::string, unsigned long> m_tracepointIDs;
		std::map<unsigned int, boost::chrono::thread_clock::time_point> m_tLastTracepoint;
		std::map<unsigned int, boost::chrono::process_real_cpu_clock::time_point> m_lastTracepoint;
		std::list<unsigned long> m_sectionTree;
		unsigned long m_activeSpan;
		unsigned long m_cSectionID, m_cSpanID, m_cTracepointID;

		// Span info (thread-local and relative to process)
		boost::chrono::thread_clock::time_point m_tSpanStart;
		boost::chrono::process_real_cpu_clock::time_point m_spanStart;

		// Section info
		std::list<boost::chrono::thread_clock::time_point> m_tSectionStarts;
		std::list<boost::chrono::process_real_cpu_clock::time_point> m_sectionStarts;

		// Profile data
		std::string m_name;
		boost::shared_ptr<Profile> m_profile;
	};
};
