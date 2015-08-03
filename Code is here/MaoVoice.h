
#pragma once

#include <process.h>
#include <windows.h>
#include <mmsystem.h>  
#pragma comment(lib, "winmm.lib") 

#include <Queue>
using namespace std;


#define Sample_Rate 44100
#define Quantize_Bits 16
#define Channels 2

#define BUFFER_SIZE int( Sample_Rate * Quantize_Bits * Channels / 8 * 0.02) //20ms



class MaoVoice
{
public:
	MaoVoice();
	~MaoVoice();

public:
	void startREC();
	void startPLAY();

	void stopREC();
	void stopPLAY();

	void clearBuffer();
	bool getVoice(void*& data); //will pop

private:
	static unsigned int WINAPI Rec(PVOID para);
	static unsigned int WINAPI Play(PVOID para);

	static void CALLBACK waveOutProc(HWAVEOUT hwo,
		UINT uMsg,
		DWORD_PTR dwInstance,
		DWORD_PTR dwParam1,
		DWORD_PTR dwParam2);

	static void CALLBACK waveInProc(HWAVEIN hwi,
		UINT uMsg,
		DWORD_PTR dwInstance,
		DWORD_PTR dwParam1,
		DWORD_PTR dwParam2);

public:
	CRITICAL_SECTION outBufCritical;
	CRITICAL_SECTION playBufCritical;

private:

	char * bufRecTemp = NULL;
	bool rec_finish;
	queue<char*> outBufQueue;
	queue<char*> playBufQueue;


	bool canREC;
	bool canPLAY;
	bool needRelease;

	HANDLE REC;
	HANDLE PLAY;
};