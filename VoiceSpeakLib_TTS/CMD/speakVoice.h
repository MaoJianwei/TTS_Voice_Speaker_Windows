
#pragma once

#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override something,
//but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <sapi.h>
#include <process.h>


class speakVoice
{
public:
	speakVoice();
	~speakVoice();

	// asynchronous call
	// LPCWSTR, such as  L"string"  or  QString.utf16()
	void maoAsyncSpeak(LPCWSTR words);

	// blocking call
	bool maoSyncSpeak(LPCWSTR words);

private:
	ISpVoice * pVoice;
	bool initRunBusyGood;

	static unsigned int WINAPI threadVoice(void * words);
};

struct MaoVoiceStruct
{
	speakVoice * me;
	LPCWSTR words;
};
