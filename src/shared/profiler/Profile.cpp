#include "Profile.h"
#include "../logging/logging.h"

#define PROFILE_HISTORY_SIZE 32

using namespace Profiling;

Profile::Profile() {
	m_sections[0] = new Section("root", 0);
	m_sections[0]->m_parent = NULL;
	m_sections[0]->m_root = this;
}

Profile::~Profile() {
}

void Profile::updateTracepoint(unsigned long id, nsec tTSL, nsec timeSinceLast) {
	lock();
	boost::circular_buffer<nsec> &buf = m_tracepointBufs[id];
	boost::circular_buffer<nsec> &tbuf = m_tTracepointBufs[id];
	nsec end = buf.front();
	nsec tend = tbuf.front();
	buf.push_back(timeSinceLast);
	tbuf.push_back(tTSL);
	nsec& tavg = m_tTracepointAvgs[id];
	nsec& avg = m_tracepointAvgs[id];
	tavg = tavg - (tend / PROFILE_HISTORY_SIZE) + (tTSL / PROFILE_HISTORY_SIZE);
	avg = avg - (end / PROFILE_HISTORY_SIZE) + (timeSinceLast / PROFILE_HISTORY_SIZE);
	m_tracepointCalls[id]++;
	unlock();
}

void Profile::updateSection(unsigned long id, nsec dThread, nsec dProcess) {
	lock();
	m_sections[id]->update(dProcess, dThread);
	unlock();
}

void Profile::updateSpan(unsigned long id, nsec dThread, nsec dProcess) {
	lock();
	m_spans[id]->update(dProcess, dThread);
	unlock();
}

void Profile::addTracepoint(unsigned long id, std::string name, std::string fname) {
	lock();
	boost::circular_buffer<nsec> &buf = (m_tracepointBufs[id] =
			boost::circular_buffer<nsec>(PROFILE_HISTORY_SIZE));
	boost::circular_buffer<nsec> &tbuf = (m_tTracepointBufs[id] =
			boost::circular_buffer<nsec>(PROFILE_HISTORY_SIZE));
	for(int i=0;i<PROFILE_HISTORY_SIZE;i++) {
		buf.push_back(nsec(0));
		tbuf.push_back(nsec(0));
	}
	m_tracepointAvgs[id] = m_tTracepointAvgs[id] = nsec(0);
	m_tracepointNames[id] = name;
	m_tracepointFunctions[id] = fname;
	m_tracepointCalls[id] = 0;
	m_tracepointIDs[name] = id;
	unlock();
}

void Profile::addSection(unsigned long id, unsigned long parent, std::string name) {
	lock();
	Section* s = new Section(name, id);
	s->m_root = this;
	s->m_parent = m_sections[parent];
	m_sections[id] = s;
	unlock();
}

void Profile::addSpan(unsigned long id, unsigned long parent, std::string name) {
	lock();
	Section* s = m_sections[parent];
	Span* sp = new Span(name, id, s);
	m_spans[id] = sp;
	s->addChild(sp);
	unlock();
}

Tracepoint Profile::getTracepoint(std::string name) {
	Tracepoint t;
	lock();
	t.id = m_tracepointIDs[name];
	t.name = m_tracepointNames[t.id];
	t.avgCallFreqProcess = m_tracepointAvgs[t.id];
	t.avgCallFreqThread = m_tTracepointAvgs[t.id];
	t.calls = m_tracepointCalls[t.id];
	unlock();
	return t;
}

void Profile::lock() {
	m_lock.lock();
}

void Profile::unlock() {
	m_lock.unlock();
}

Span::Span(std::string name, unsigned long id, Section* parent) : m_name(name), m_id(id), m_parent(parent) {
	m_tAvg = m_pAvg = nsec(0);
	m_times.set_capacity(PROFILE_HISTORY_SIZE);
	m_tTimes.set_capacity(PROFILE_HISTORY_SIZE);
	for(int i=0;i<PROFILE_HISTORY_SIZE;i++) {
		m_times.push_back(nsec(0));
		m_tTimes.push_back(nsec(0));
	}
	m_root = m_parent->m_root;
}

void Span::update(nsec t, nsec tt) {
	nsec tBegin = m_tTimes.front();
	nsec begin = m_times.front();
	m_times.push_back(t);
	m_tTimes.push_back(tt);
	m_tAvg = m_tAvg - (tBegin / PROFILE_HISTORY_SIZE) + (tt / PROFILE_HISTORY_SIZE);
	m_pAvg = m_pAvg - (begin / PROFILE_HISTORY_SIZE) + (t / PROFILE_HISTORY_SIZE);
}

nsec Span::getThreadAverage() {
	m_root->lock();
	nsec out = m_tAvg;
	m_root->unlock();
	return out;
}

nsec Span::getProcessAverage() {
	m_root->lock();
	nsec out = m_pAvg;
	m_root->unlock();
	return out;
}

std::string Span::getName() {
	m_root->lock();
	std::string out = m_name;
	m_root->unlock();
	return out;
}

Section::Section(std::string name, unsigned long id) : Span(name, id, this) {
}

size_t Section::numSections() {
	m_root->lock();
	size_t n = m_sections.size();
	m_root->unlock();
	return n;
}

size_t Section::numSpans() {
	m_root->lock();
	size_t n = m_spans.size();
	m_root->unlock();
	return n;
}

Section* Section::getSection(size_t idx) {
	m_root->lock();
	Section* out = m_sections[m_sectionIndices[idx]];
	m_root->unlock();
	return out;
}

Span* Section::getSpan(size_t idx) {
	m_root->lock();
	Span* out = m_spans[m_spanIndices[idx]];
	m_root->unlock();
	return out;
}

void Section::addChild(Section* s) {
	m_root->lock();
	m_sections[s->m_id] = s;
	m_sectionIndices.push_back(s->m_id);
	m_root->unlock();
}

void Section::addChild(Span* s) {
	m_root->lock();
	m_spans[s->m_id] = s;
	m_spanIndices.push_back(s->m_id);
	m_root->unlock();
}

