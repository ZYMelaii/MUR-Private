#include "./svrpipe.h"
#include <iostream>
#include <string>
#include <windows.h>

void ReportError() {
	LPTSTR lpMessage;
	DWORD dwErrCode = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, dwErrCode, 0, (LPTSTR)&lpMessage, 0, NULL);
	fprintf(stderr, "[ERROR] GetLastError(): %lu -> %s\n", dwErrCode, lpMessage);
	LocalFree(lpMessage);
}

std::string& trim(std::string &str) {
	const char* whitespaces = "\t\n\r\f\v ";
	str.erase(str.find_last_not_of(whitespaces) + 1);
	str.erase(0, str.find_first_not_of(whitespaces));
	return str;
}

int main(int argc, char const *argv[]) {
	PipeClient client;
	bool succeed = client.connect(argc > 1 ? argv[1] : "SvrLogger");
	while (true) {
		printf("$ ");
		std::string req;
		std::string content;
		std::cin >> req;
		std::getline(std::cin, content);
		if (!client.request(req.c_str(), trim(content))) {
			ReportError();
			break;
		}
	}
	return 0;
}