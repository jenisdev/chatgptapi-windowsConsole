#include "stdafx.h"
#include "ChatGgtAPI.hpp"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	CHATGPT_API ChatGPT_OBJ("your_key");
	ChatGPT_OBJ.SetModel("gpt-3.5-turbo");
	LISTCONVERSATIONS hist_conver;
	LISTSESSIONS list_session;

	for (;;)
	{
		std::string prompt;
		std::cout << "Enter question:";

		std::getline(std::cin, prompt);

		if (std::size_t pos = prompt.find("%session") != string::npos) {
			int session_num;
			stringstream s(prompt);
			string pref_session;
			s >> pref_session;

			if ((s >> std::ws).eof()) {
				cout << "Session " << list_session.size() + 1 << ":" << endl;
				for (int i = 0; i < hist_conver.size(); ++i) {
					cout << "Question: " << hist_conver[i].question << endl;
					cout << "Answer: " << hist_conver[i].answer << endl;
				}
			}
			else 
			{
				s >> session_num;
				if (session_num > list_session.size()) {
					cout << "The number is not valid. Here are current session's conversations." << endl;
					cout << "Session " << list_session.size() + 1 << ":" << endl;
					for (int i = 0; i < hist_conver.size(); ++i) {
						cout << "Question: " << hist_conver[i].question << endl;
						cout << "Answer: " << hist_conver[i].answer << endl;
					}
				}
				else
				{
					cout << "Session " << session_num << ":" << endl;
					for (int i = 0; i < list_session[session_num - 1].size(); ++i) {
						cout << "Question: " << list_session[session_num - 1][i].question << endl;
						cout << "Answer: " << list_session[session_num - 1][i].answer << endl;
					}
				}
			}
			continue;
		}

		if (prompt == "%all") {
			for (int i = 0; i < list_session.size(); ++i) {
				for (int j = 0; j < list_session[i].size(); ++j)
					cout << list_session[i][j].question << " " << list_session[i][j].answer << endl;
				cout << "=======================================================" << endl;
			}
			continue;
		}

		if (prompt == "%") {
			// Restore the session
			LISTCONVERSATIONS item_conv(hist_conver);
			list_session.insert(list_session.end(), item_conv);

			hist_conver.clear();			
			continue;
		}
			
		if (prompt.empty()) {
			if (hist_conver.size() > 0) {
				LISTCONVERSATIONS item_conv(hist_conver);

				list_session.insert(list_session.end(), item_conv);
			}
			
			break;
		}

		auto off = ChatGPT_OBJ.Text(prompt.c_str());
		if (!off.has_value())
			continue;
		auto& r = off.value();

		// Retore the conversations in Queue
		ONE_CONVERSATION conv;
		conv.question = prompt;
		conv.answer = r.t;
		hist_conver.insert(hist_conver.end(), conv);
		
		std::cout << r.t << std::endl;
	}
}
