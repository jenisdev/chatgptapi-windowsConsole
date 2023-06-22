#include "stdafx.h"
#include "ChatGgtAPI.hpp"

using namespace std;

std::string escape_json(const std::string &s) {
	std::ostringstream o;
	for (auto c = s.cbegin(); c != s.cend(); c++) {
		switch (*c) {
		case '\t': o << "\\t"; break;
		case '"': o << "\\\""; break;
		case '\\': o << "\\\\"; break;
		case '\b': o << "\\b"; break;
		case '\f': o << "\\f"; break;
		case '\n': o << "\\n"; break;
		case '\r': o << "\\r"; break;
		default: o << *c;
		}
	}
	return o.str();
}

int _tmain(int argc, _TCHAR* argv[])
{
	CHATGPT_API ChatGPT_OBJ("your-key");
	ChatGPT_OBJ.SetModel("gpt-3.5-turbo");
	LISTCONVERSATIONS hist_conver;
	LISTSESSIONS list_session;
	bool isNewSession = false;
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
			// Restore the session
			if (hist_conver.size() > 0) {
				LISTCONVERSATIONS item_conv(hist_conver);
				list_session.insert(list_session.end(), item_conv);
				hist_conver.clear();
			}

			if ((s >> std::ws).eof()) {
				cout << "Session " << list_session.size() + 1 << ":" << endl;
				session_num = (int)list_session.size();

				for (int i = 0; i < list_session[session_num - 1].size(); ++i) {
					cout << "Question: " << list_session[session_num - 1][i].question << endl;
					cout << "Answer: " << list_session[session_num - 1][i].answer << endl;
				}
			}
			else 
			{
				s >> session_num;
				if (session_num > list_session.size() ) {
					cout << "The number is not valid. Here are last session's conversations." << endl;
					session_num = (int)list_session.size();
				}

				cout << "Session " << session_num << ":" << endl;
				for (int i = 0; i < list_session[session_num - 1].size(); ++i) {
					cout << "Question: " << list_session[session_num - 1][i].question << endl;
					cout << "Answer: " << list_session[session_num - 1][i].answer << endl;
				}
				
			}
			continue;
		}

		if (prompt == "%all") {
			// Restore the session
			if (hist_conver.size() > 0) {
				LISTCONVERSATIONS item_conv(hist_conver);
				list_session.insert(list_session.end(), item_conv);
				hist_conver.clear();
			}

			int session_cnt = 0;
			for (int i = 0; i < list_session.size(); ++i) {
				cout << "Session " << ++session_cnt << endl;
				for (int j = 0; j < list_session[i].size(); ++j){
					cout << "Question: " << list_session[i][j].question << endl;
					cout << "Answer: " << list_session[i][j].answer << endl;
				}
				cout << "------------------------------------------" << endl;
			}
			continue;
		}

		if (prompt == "%") {
			// Restore the session
			if (hist_conver.size() > 0) {
				LISTCONVERSATIONS item_conv(hist_conver);
				list_session.insert(list_session.end(), item_conv);
				hist_conver.clear();
			}

			continue;
		}
			
		if (prompt.empty()) {
			break;
		}
		
		string request_part = "";
		if (!isNewSession) {
			for (int i = 0; i < hist_conver.size(); ++i) {
				string question = hist_conver[i].question;
				string answer = hist_conver[i].answer;

				request_part += R"({"role":"user", "content":")" + escape_json(question) + R"("},)";
				request_part += R"({"role":"assistant", "content":")" + escape_json(answer) + R"("},)";
			}

			
			request_part += R"({"role":"user", "content":")" + escape_json(prompt) + R"("})";
		}
		else
		{
			
			request_part = R"("role": "user", "content":")" + escape_json(prompt) + R"("})";
		}

		auto off = ChatGPT_OBJ.Text(request_part.c_str());
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
