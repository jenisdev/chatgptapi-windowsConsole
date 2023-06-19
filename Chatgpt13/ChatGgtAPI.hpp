#include <iostream>
#include <sstream>
#include <Windows.h>
#include <WinInet.h>
#include <vector>
#include <iterator>
#include <string>
#include <sstream>
#include <map>
#include <memory>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wininet.lib")
#include "jsonxx.h"
#include "rest.h"
#include "boost\optional.hpp"

#define _BUFFERSIZE 10000
#define REQ_BUFFSIZE 100010
#define AUTH_BUFF 200
#define MX_TOKENS 4096
#define UNSIGLONG unsigned long
#define STD_CHARVECTOR std::vector<char>
#define STDSTR std::string
#define JSONOBJECT jsonxx::Object
#define JSONARRAY jsonxx::Array
#define JSONSTRING jsonxx::String
#define LISTCONVERSATIONS std::vector<ONE_CONVERSATION>
#define LISTSESSIONS std::vector<LISTCONVERSATIONS>

struct ONE_CONVERSATION
{
	STDSTR question;
	STDSTR answer;
};

struct CHATGPT_RESULT
{
	JSONOBJECT o;
	STDSTR t;
	STD_CHARVECTOR data;
};

STD_CHARVECTOR Fetch(const char* TheLink)
{
	STD_CHARVECTOR resp;

	DWORD Size;
	UNSIGLONG bufferSize = _BUFFERSIZE;
	TCHAR ss[_BUFFERSIZE];
	DWORD TotalTransferred = 0;

	int ERR = 1;

	HINTERNET hInternet = 0, hRequest = 0;

	hInternet = InternetOpen(L"ChatGPT-API", INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
	if (hInternet)
	{
		hRequest = InternetOpenUrlA(hInternet, TheLink, 0, 0, INTERNET_FLAG_NO_CACHE_WRITE, 0);
		if (hRequest)
		{

			if (!HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH, ss, &bufferSize, 0))
				Size = (DWORD)-1;
			else
				Size = _wtoi(ss);

			for (;;)
			{
				DWORD nCNT;
				char Buff[REQ_BUFFSIZE] = { 0 };
				memset(Buff, 0, REQ_BUFFSIZE);
				BOOL  isREAD = InternetReadFile(hRequest, Buff, REQ_BUFFSIZE, &nCNT);
				if (isREAD == false)
				{
					ERR = 2;
					break;
				}
				if (nCNT == 0)
				{
					// End of file !
					ERR = 0;
					break;
				}
				TotalTransferred += nCNT;

				//Write to File !
				//char xx = Buff[n];
				size_t olds = resp.size();
				resp.resize(olds + nCNT);
				memcpy(resp.data() + olds, Buff, nCNT);

				int NewPos = 0;
				if (Size != -1)
					NewPos = (100 * TotalTransferred) / Size;
			}
			InternetCloseHandle(hRequest);
		}
		InternetCloseHandle(hInternet);
	}
	return resp;
}

class CHATGPT_API
{
	STDSTR APIKEY;
	STDSTR model;

	public:
	CHATGPT_API(const char* KEY)
	{
		APIKEY = KEY;
	}

	void SetModel(const char* MODEL)
	{
		model = MODEL;
	}

	std::wstring Bearer()
	{
		wchar_t auth[AUTH_BUFF] = {};
		swprintf_s(auth, AUTH_BUFF, L"Authorization: Bearer %S", APIKEY.c_str());
		return auth;
	}

	boost::optional<CHATGPT_RESULT> Text(const char* prompt, int Temperature = 0, int max_tokens = MX_TOKENS)
	{
		STD_CHARVECTOR data(_BUFFERSIZE);
		sprintf_s(data.data(), _BUFFERSIZE,
			R"({"model": "%s", "messages": [{"role": "system", "content": "You are a helpful assistant."}, {"role": "user", "content": "%s"}]})",
			model.c_str(), prompt);

		data.resize(strlen(data.data()));

		STDSTR str = data.data();

		RESTAPI::REST hREST;
		hREST.Connect(L"api.openai.com", true, 0, 0, 0, 0);
		std::initializer_list<std::wstring> hdrs = {
			Bearer(),
			L"Content-Type: application/json",
		};
		auto hInternetConnection = hREST.RequestWithBuffer(L"/v1/chat/completions", L"POST", hdrs, data.data(), data.size());

		STD_CHARVECTOR hRESPONSE;
		hREST.ReadToMemory(hInternetConnection, hRESPONSE);
		hRESPONSE.resize(hRESPONSE.size() + 1);

		try
		{
			JSONOBJECT jObject;
			jObject.parse(hRESPONSE.data());
			CHATGPT_RESULT r;
			r.o = jObject;
			
			if (jObject.has<JSONOBJECT>("error"))
				r.t = jObject.get<JSONOBJECT>("error").get<jsonxx::String>("message");
			else {
				auto& choices = jObject.get<JSONARRAY>("choices");
				auto& choice0 = choices.get<JSONOBJECT>(0);
				auto& message = choice0.get<JSONOBJECT>("message");
				r.t = message.get<JSONSTRING>("content");
			}

			return r;
		}
		catch (int Err)
		{
			std::cout << "An exception occurred. Exception Nr." << Err << std::endl;
		}
		return{};
	}
};
