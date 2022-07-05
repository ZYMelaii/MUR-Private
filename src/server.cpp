#include "./svrpipe.h"
#include "./winhelper.h"
#include <chrono>
#include <thread>
#include <functional>
#include <string_view>
#include <iostream>

class SvrLogger : public PipeServer {
public:
	virtual void route(const Request &request) {
		auto match = [&request](auto key) {
			constexpr auto hash = std::hash<std::string_view>{};
			return hash(request.header.req) == hash(key);
		};
		if (match("logger")) {
			puts((const char*)request.content);
		} else if (match("clear")) {
			clear();
		} else if (match("shutdown")) {
			puts("[INFO] server will shutdown later.");
			shutdown();
		} else {
			printf("[WARN] route request `%s` failed.\n", request.header.req);
		}
	}
};

int main(int argc, char const *argv[]) {
	SvrLogger().init(argc > 1 ? argv[1] : "SvrLogger").run();
	return 0;
}