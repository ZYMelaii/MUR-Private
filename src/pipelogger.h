#ifndef PIPELOGGER_H
#define PIPELOGGER_H

#include <iostream>
#include <sstream>
#include "./svrpipe.h"

class PipeLogger {
public:
	PipeLogger(const char *pipe);
	~PipeLogger();
	bool submit();
	bool submit(const char *req, const std::string &content = {});
	template <typename T>
	PipeLogger& operator<<(const T &object) {
		oss_ << object;
		return *this;
	}
private:
	PipeClient client_;
	std::stringstream oss_;
};

#endif /*PIPELOGGER_H*/