#include "./pipelogger.h"

PipeLogger::PipeLogger(const char *pipe) {
	client_.connect(pipe);
}

PipeLogger::~PipeLogger() {
	client_.disconnect();
}

bool PipeLogger::submit() {
	bool succeed = client_.request("logger", oss_.str());
	oss_.clear();
	return succeed;
}

bool PipeLogger::submit(const char *req, const std::string &content) {
	return client_.request(req, content);
}