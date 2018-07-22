// vim: set noet ts=4 sts=4 sw=4:
#pragma once

#include <vector>
#include <ostream>
#include <iomanip>
#include <cstdio>

namespace profiler {

class single_profiler;

class profiler {
	std::vector<const single_profiler *> profs;
	int indent;
	profiler() { }
	const std::string format_time(double v) const {
		char buf[1024];
		if (v < 1e-6)
			sprintf(buf, "%8.2lfns", 1e9 * v);
		else if (v < 1e-3)
			sprintf(buf, "%8.2lfus", 1e6 * v);
		else if (v < 1)
			sprintf(buf, "%8.2lfms", 1e3 * v);
		else
			sprintf(buf, "%8.2lfs ", v);
		return std::string(buf);
	}
public:
	static profiler &getInstance() {
		static profiler instance;
		return instance;
	}
	void reg(const single_profiler *prof) {
		profs.push_back(prof);
	}
	int enter() { return indent++; }
	void leave() { indent--; }
	std::ostream &print(std::ostream &o) const;
	profiler(const profiler &) = delete;
	profiler(profiler &&) = delete;
	profiler &operator=(const profiler &) = delete;
};

inline const profiler &getInstance() {
	return profiler::getInstance();
}

class single_profiler {
	timespec startmark;
	int _calls;
	double tottime;
	int depth;
	int indent;
	void start() {
		clock_gettime(CLOCK_MONOTONIC_RAW, &startmark);
	}
	double stop() {
		timespec stopmark;
		clock_gettime(CLOCK_MONOTONIC_RAW, &stopmark);
		int nsec = stopmark.tv_nsec - startmark.tv_nsec;
		int sec = stopmark.tv_sec - startmark.tv_sec;
		return sec + 1e-9 * nsec;
	}
	const std::string _filename;
	const std::string _pretty_function;
	const int _line_number;
public:
	single_profiler(const char *fn, const int line, const char *func)
		: _calls(0), tottime(0), depth(0), _filename(fn)
		, _pretty_function(func), _line_number(line)
	{
		profiler::getInstance().reg(this);
	}
	void enter() {
		_calls++;
		if (depth > 0)
			tottime += stop(); // suspend
		else
			indent = profiler::getInstance().enter();
		depth++;
		start();
	}
	void leave() {
		tottime += stop();
		depth--;
		if (depth > 0)
			start(); // resume
		else
			profiler::getInstance().leave();
	}
	const std::string &filename() const { return _filename; }
	int line_number() const { return _line_number; }
	const std::string function() const { return std::string(indent, ' ') + _pretty_function; }
	int calls() const { return _calls; }
	double total_time() const { return tottime; }
};

template<typename T>
void profile_gate(const char *fn, const int line, const char *func, bool enter, T *) {
	static single_profiler prof(fn, line, func);
	if (enter)
		prof.enter();
	else
		prof.leave();
}

inline std::ostream &profiler::print(std::ostream &o) const {
	int fnwidth = 120;
	o << "\n" << std::left << std::setw(fnwidth) << "function";
	o << "line	calls	   tottime	avgtime\n\n";
	for (const single_profiler *p : profs) {
		o << std::left << std::setfill('.') << std::setw(fnwidth) << p->function() << std::setfill(' ')
			<< " " << std::setw(8) << p->line_number()
			<< " " << std::setw(8) << p->calls() << " "
			<< format_time(p->total_time()) << " " << format_time(p->total_time() / p->calls()) << "\n";
	}
	return o;
}

inline std::ostream &operator<<(std::ostream &o, const profiler &prof) {
	return prof.print(o);
}

}

#define PROFILE_ME_AS(name) \
	struct __prof_data { bool early_exit; \
		__prof_data(const char *fn, const int line, const char *func) { \
			early_exit = false; \
			profiler::profile_gate(fn, line, func, true, this); \
		} \
		void stop() { \
			early_exit = true; \
			profiler::profile_gate(nullptr, -1, nullptr, false, this); \
		} \
		~__prof_data() { \
			if (!early_exit) \
				stop(); \
		} \
	} __helper_var(__FILE__, __LINE__, name)
#define PROFILE_ME PROFILE_ME_AS(__PRETTY_FUNCTION__)
#define PROFILE_END \
	__helper_var.stop()
