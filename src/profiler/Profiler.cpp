#include "ProfilerObj.h"
#include <utility>
#include <stdexcept>

using namespace Profiling;
using namespace std;
using namespace boost::chrono;

static map<std::string, boost::thread::id> namedThreadIDs;
static map<boost::thread::id, boost::shared_ptr<Profile> > threadProfiles;
static boost::mutex threadGlobalInfoLock;

Profiler& Profiler::getInstance() {
	return *getInstancePtr();
}

Profiler* Profiler::getInstancePtr() {
	static boost::thread_specific_ptr<Profiler> pPtr;
	if(!pPtr.get()) {
		pPtr.reset(new Profiler());
	}
	return pPtr.get();
}

Profiler::Profiler() {
	m_cSectionID = 0; // Section 0 is always the root section
	m_profile.reset(new Profile());
	m_name = "";
}

Profiler::~Profiler() {
	if(m_name.length() > 0) {
		threadGlobalInfoLock.lock();
		namedThreadIDs.erase(m_name);
		threadProfiles.erase(boost::this_thread::get_id());
		threadGlobalInfoLock.unlock();
	}
}

void Profiler::beginSection(const char* name) {
	unsigned long sid = 0;
	unsigned long csection = (m_sectionTree.size() == 0) ? 0 : m_sectionTree.back();
	std::pair<unsigned long, std::string> k = std::make_pair(csection, string(name));
	if(m_sectionIDs.find(k) == m_sectionIDs.end()) sid = insertSection(name, csection); else
		sid = m_sectionIDs[k];
	m_sectionTree.push_back(sid);
	m_tSectionStarts.push_back(boost::chrono::thread_clock::now());
	m_sectionStarts.push_back(boost::chrono::process_real_cpu_clock::now());
}

void Profiler::endSection() {
	if(m_sectionTree.size() == 0) throw std::runtime_error("Already at top of section tree!");

	// Get durations
	nanoseconds tTime = boost::chrono::thread_clock::now() - m_tSectionStarts.back();
	nanoseconds time = boost::chrono::process_real_cpu_clock::now() - m_sectionStarts.back();
	unsigned long csid = m_sectionTree.back();
	m_tSectionStarts.pop_back();
	m_sectionStarts.pop_back();
	m_sectionTree.pop_back();

	// Update profile
	m_profile->updateSection(csid, tTime, time);
}

void Profiler::beginSpan(const char* name) {
	if(inSpan) throw std::runtime_error("Already in span");
	unsigned long csection = (m_sectionTree.size() == 0) ? 0 : m_sectionTree.back();
	std::pair<unsigned long, std::string> k = std::make_pair(csection, string(name));
	unsigned long sid = 0;
	if(m_spanIDs.find(k) == m_spanIDs.end()) sid = insertSpan(name, csection); else
		sid = m_spanIDs[k];
	m_tSpanStart = boost::chrono::thread_clock::now();
	m_spanStart = boost::chrono::process_real_cpu_clock::now();
	m_activeSpan = sid;
}

void Profiler::endSpan() {
	if(!inSpan) throw std::runtime_error("Not in span");
	nanoseconds tTime = boost::chrono::thread_clock::now() - m_tSpanStart;
	nanoseconds time = boost::chrono::process_real_cpu_clock::now() - m_spanStart;
	m_profile->updateSpan(m_activeSpan, tTime, time);
	inSpan = false;
}

void Profiler::tracepoint(const char* name, const char* fname) {
	unsigned long tpid = 0;
	if(m_tracepointIDs.find(string(name)) == m_tracepointIDs.end()) tpid = insertTracepoint(name, fname); else
		tpid = m_tracepointIDs[string(name)];
	boost::chrono::thread_clock::time_point tpNow = boost::chrono::thread_clock::now();
	boost::chrono::process_real_cpu_clock::time_point pNow = boost::chrono::process_real_cpu_clock::now();
	nanoseconds tsl = pNow - m_lastTracepoint[tpid];
	nanoseconds ttsl = tpNow - m_tLastTracepoint[tpid];
	m_lastTracepoint[tpid] = pNow;
	m_tLastTracepoint[tpid] = tpNow;
	m_profile->updateTracepoint(tpid, ttsl, tsl);
}

unsigned long Profiler::insertTracepoint(std::string name, std::string fname) {
	unsigned long tpid = m_cTracepointID++;
	m_tracepointIDs[string(name)] = tpid;
	m_tLastTracepoint[tpid] = boost::chrono::thread_clock::now();
	m_lastTracepoint[tpid] = boost::chrono::process_real_cpu_clock::now();
	m_profile->addTracepoint(tpid, name, fname);
}

unsigned long Profiler::insertSection(std::string name, unsigned long parent) {
	unsigned long csection = (m_sectionTree.size() == 0) ? 0 : m_sectionTree.back();
	unsigned long sid = m_cSectionID++;
	m_sectionIDs[std::make_pair(parent, name)] = sid;
	m_profile->addSection(sid, parent, name);
}

unsigned long Profiler::insertSpan(std::string name, unsigned long parent) {
	unsigned long csection = (m_sectionTree.size() == 0) ? 0 : m_sectionTree.back();
	unsigned long sid = m_cSpanID++;
	m_spanIDs[std::make_pair(parent, name)] = sid;
	m_profile->addSpan(sid, parent, name);
}

boost::shared_ptr<Profile> Profiler::getExternalProfile(std::string name) {
	threadGlobalInfoLock.lock();
	boost::shared_ptr<Profile> out;
	if(namedThreadIDs.find(name) != namedThreadIDs.end()) {
		out = threadProfiles[namedThreadIDs[name]];
	} else {
		out = boost::shared_ptr<Profile>();
	}
	threadGlobalInfoLock.unlock();
	return out;
}

Profile& Profiler::getProfile() {
	return *m_profile;
}

void Profiler::setName(const char* name) {
	m_name = string(name);
	threadGlobalInfoLock.lock();
	namedThreadIDs[m_name] = boost::this_thread::get_id();
	threadProfiles[boost::this_thread::get_id()] = m_profile;
	threadGlobalInfoLock.unlock();
}
