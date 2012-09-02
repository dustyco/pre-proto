#pragma once
#include <map>
#include <list>
#include <vector>
#include <stdint.h>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/circular_buffer.hpp>

namespace Profiling {
	typedef boost::chrono::nanoseconds nsec;
	class Profile;
	class Section;

	struct Tracepoint {
		unsigned long id;
		std::string name;
		nsec avgCallFreqProcess, avgCallFreqThread;
		unsigned long calls;
	};

	class Span {
	public:
		friend class Profile;
		friend class Section;
	public:
		nsec getThreadAverage();
		nsec getProcessAverage();
		std::string getName();
	protected:
		Span(std::string name, unsigned long id, Section* parent);

		void update(nsec t, nsec tt);

		unsigned long m_id;
		std::string m_name;
		Section* m_parent;
		Profile* m_root;
		boost::circular_buffer<nsec> m_times, m_tTimes;
		nsec m_tAvg, m_pAvg;
	};

	class Section : private Span {
		friend class Profile;
		friend class Span;
	public:
		size_t numSections();
		size_t numSpans();

		// If you're going to keep these, keep a reference to the Profile's shared
		// pointer too, otherwise they risk being deleted
		Section* getSection(size_t idx);
		Span* getSpan(size_t idx);
	private:
		// This trusts the initializer to set m_parent
		Section(std::string name, unsigned long id);

		void addChild(Section* s);
		void addChild(Span* s);

		std::map<unsigned long, Section*> m_sections;
		std::map<unsigned long, Span*> m_spans;
		std::vector<unsigned long> m_sectionIndices;
		std::vector<unsigned long> m_spanIndices;
	};

	class Profile {
	public:
		Profile();
		~Profile();
		
		void updateTracepoint(unsigned long id, nsec tTSL, nsec timeSinceLast);
		void updateSection(unsigned long id, nsec dThread, nsec dProcess);
		void updateSpan(unsigned long id, nsec dThread, nsec dProcess);

		void addTracepoint(unsigned long id, std::string name, std::string fname);
		void addSection(unsigned long id, unsigned long parent, std::string name);
		void addSpan(unsigned long id, unsigned long parent, std::string name);

		Tracepoint getTracepoint(std::string name);

		void lock();
		void unlock();
	private:
		std::map<unsigned long, Section*> m_sections;
		std::map<unsigned long, Span*> m_spans;

		std::map<unsigned long, boost::circular_buffer<nsec> > m_tracepointBufs, m_tTracepointBufs;
		std::map<unsigned long, nsec> m_tracepointAvgs, m_tTracepointAvgs;
		std::map<unsigned long, unsigned long> m_tracepointCalls;
		std::map<unsigned long, std::string> m_tracepointNames;
		std::map<std::string, unsigned long> m_tracepointIDs;
		std::map<unsigned long, std::string> m_tracepointFunctions;

		boost::recursive_mutex m_lock;
	};
};
