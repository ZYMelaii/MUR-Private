#include "svrpipe.h"
#include <windows.h>
#include <assert.h>
#include <string.h>
#include <string>
#include <iostream>
#include <thread>

PipeServer::PipeServer() :
	should_exit_(false), pipe_(nullptr) {
}

PipeServer& PipeServer::init(const char *pipename) {
	char name[256];
	sprintf_s(name, 256, "\\\\.\\pipe\\%s", pipename);
	pipe_ = strdup(name);
	return *this;
}

PipeServer::~PipeServer() {
	if (pipe_) {
		free((void*)pipe_);
	}
}

void PipeServer::run() {
	while (!should_exit_) {
		HANDLE inst = CreateNamedPipe(
			pipe_,
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			4096,
			4096,
			0,
			NULL);
		assert(inst != nullptr);

		if (ConnectNamedPipe(inst, NULL)) {
			std::thread([this](HANDLE instance) {
				while (!should_exit_) {
					Request request = { 0 };
					DWORD size = 0;
					bool succeed = ReadFile(instance, &request, sizeof(RequestHeader), &size, nullptr);
					if ((!succeed || size == 0) && GetLastError() == ERROR_BROKEN_PIPE) {
						break;
					}
					if (size == 0) continue;
					assert(size == sizeof(RequestHeader));
					size = request.header.content_length;
					char buffer[4096] = { 0 };
					std::string content;
					while (succeed && size != 0) {
						DWORD read_bytes = 0;
						succeed &= static_cast<bool>(ReadFile(instance, buffer, 4096, &read_bytes, NULL));
						content.insert(content.size(), buffer, read_bytes);
						size -= read_bytes;
					}
					if (!succeed) continue;
					request.content = (void*)content.c_str();
					route(request);
				}
				FlushFileBuffers(instance);
   				DisconnectNamedPipe(instance);
   				CloseHandle(instance);
			}, inst).detach();
		} else {
			CloseHandle(inst);
		}
	}
}

void PipeServer::shutdown() const {
	should_exit_ = true;
}

PipeClient::~PipeClient() {
	disconnect();
}

bool PipeClient::connect(const char *pipename) {
	if (instance_ != nullptr) {
		disconnect();
	}
	char pipe[256];
	sprintf_s(pipe, 256, "\\\\.\\pipe\\%s", pipename);
	instance_ = CreateFile( 
		pipe,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	return instance_ != nullptr;
}

bool PipeClient::disconnect() {
	if (!instance_) {
		return true;
	}
	CloseHandle(instance_);
	instance_ = nullptr;
	return true;
}

bool PipeClient::request(const char *req, const void *content, size_t size) {
	if (!instance_) {
		return false;
	}
	RequestHeader header;
	strcpy_s(header.req, req);
	header.content_length = size;
	bool succeed = WriteFile(instance_, &header, sizeof(RequestHeader), nullptr, nullptr);
	succeed &= static_cast<bool>(WriteFile(instance_, content, size, nullptr, nullptr));
	return succeed;
}

bool PipeClient::request(const char *req, const std::string &content) {
	return request(req, content.c_str(), content.size());
}