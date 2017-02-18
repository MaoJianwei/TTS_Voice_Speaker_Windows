#include "speakVoice.h"

speakVoice::speakVoice()
{
	pVoice = NULL;
	if (FAILED(::CoInitialize(NULL))){
		initRunBusyGood = false;
		return;
	}

	HRESULT hResult = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice);
	if (FAILED(hResult)) {
		initRunBusyGood = false;
		return;
	}

	initRunBusyGood = true;

	//maoSyncSpeak(L"成功构造语音系统");
	maoSyncSpeak(L"");


	//QString bbaa = "成功构造语音系统";
	//HANDLE h = (HANDLE)_beginthreadex(NULL, 0, threadVoice, (void*)bbaa.utf16(), 0, NULL);
	//CloseHandle(h);
}

speakVoice::~speakVoice()
{
	if (initRunBusyGood == false)
		return;

	if (pVoice == NULL)
		return;
	
	pVoice->Release();
	pVoice = NULL;
	::CoUninitialize();
}

void speakVoice::maoAsyncSpeak(LPCWSTR words)
{
	// asynchronous call

	if (initRunBusyGood == false)
		return;

	MaoVoiceStruct * data = new MaoVoiceStruct{ this, words };
	HANDLE h = (HANDLE)_beginthreadex(NULL, 0, threadVoice, (void*)data, 0, NULL);
	CloseHandle(h);
}



unsigned int WINAPI speakVoice::threadVoice(void * para)
{
	MaoVoiceStruct * data = (MaoVoiceStruct *)para;
	bool ret = (data->me)->maoSyncSpeak(data->words);

	delete data;
	return ret;
}

bool speakVoice::maoSyncSpeak(LPCWSTR words)
{
	// blocking call

	if (initRunBusyGood == false)
		return false;

	//initRunBusyGood = false;
	HRESULT hResult = pVoice->Speak(words, SPF_DEFAULT, NULL);

	if (FAILED(hResult)) {
		return false;
	}
	else {
		initRunBusyGood = true;
		return true;
	}
}