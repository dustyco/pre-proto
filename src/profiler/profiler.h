#include "ProfilerObj.h"

#ifdef NOPROFILER

#define LOCALPROF()
#define PROF_BEGIN_SECTION(name)
#define PROF_END_SECTION()
#define PROF_MARK_TRACEPOINT(name)
#define PROF_BEGIN_SPAN(name)
#define PROF_END_SPAN()

#else

#define LOCALPROF() Profiling::Profiler::getInstance()
#define PROF_BEGIN_SECTION(name) Profiling::Profiler::getInstance().beginSection(name);
#define PROF_END_SECTION() Profiling::Profiler::getInstance().endSection();
#define PROF_MARK_TRACEPOINT(name) Profiling::Profiler::getInstance().tracepoint(name, __FUNCTION__);
#define PROF_BEGIN_SPAN(name) Profiling::Profiler::getInstance().beginSpan(name);
#define PROF_END_SPAN() Profiling::Profiler::getInstance().endSpan();

#endif
