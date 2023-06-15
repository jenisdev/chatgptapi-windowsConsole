#include "stdafx.h"
#include "ChatGgtAPI.hpp"

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA w = {};
	WSAStartup(MAKEWORD(2, 2), &w);
	CHATGPT_API c("Your KEY");
	c.SetModel("gpt-3.5-turbo");
	for (;;)
	{
		std::string input;
		std::cout << "Enter question:";
		std::getline(std::cin, input);
		if (input.empty())
			break;
		auto off = c.Text(input.c_str());
		if (!off.has_value())
			continue;
		auto& r = off.value();
		std::cout << r.t << std::endl;
	}
}
