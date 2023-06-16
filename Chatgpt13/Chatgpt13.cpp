#include "stdafx.h"
#include "ChatGgtAPI.hpp"

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA w = {};
	WSAStartup(MAKEWORD(2, 2), &w);

	CHATGPT_API ChatGPT_OBJ("your-key");
	ChatGPT_OBJ.SetModel("gpt-3.5-turbo");

	for (;;)
	{
		std::string prompt;
		std::cout << "Enter question:";

		std::getline(std::cin, prompt);
		if (prompt.empty())
			break;
		auto off = ChatGPT_OBJ.Text(prompt.c_str());
		if (!off.has_value())
			continue;
		auto& r = off.value();
		std::cout << r.t << std::endl;
	}
}
