#include "MaoVoice.h"


MaoVoice::MaoVoice()
{
	canREC = false;
	canPLAY = false;
	rec_finish = false;
	needRelease = false;

	REC = (HANDLE)_beginthreadex(NULL, 0, Rec, this, 0, NULL);
	PLAY = (HANDLE)_beginthreadex(NULL, 0, Play, this, 0, NULL);

	InitializeCriticalSection(&outBufCritical);
	InitializeCriticalSection(&playBufCritical);
}


MaoVoice::~MaoVoice()
{
	needRelease = true;

	DWORD exitCode = 0;
	while (true)
	{
		if (0 != GetExitCodeThread(REC, &exitCode))
		{
			if (STILL_ACTIVE != exitCode)
				break;
			else
				Sleep(1);
		}
		else
		{
			DWORD e = GetLastError();// TODO - for debug
		}
	}
	while (true)
	{
		if (0 != GetExitCodeThread(PLAY, &exitCode))
		{
			if (STILL_ACTIVE != exitCode)
				break;
			else
				Sleep(1);
		}
		else
		{
			DWORD e = GetLastError();// TODO - for debug
		}
	}

	DeleteCriticalSection(&outBufCritical);
	DeleteCriticalSection(&playBufCritical);
}


unsigned int WINAPI MaoVoice::Rec(PVOID para)
{
	MaoVoice * me = (MaoVoice*)para;

	HWAVEIN hWaveIn;
	WAVEFORMATEX wavform;
	wavform.wFormatTag = WAVE_FORMAT_PCM;
	wavform.nChannels = Channels;
	wavform.nSamplesPerSec = Sample_Rate;
	wavform.nAvgBytesPerSec = Sample_Rate * Quantize_Bits * Channels / 8;
	wavform.nBlockAlign = Channels * Quantize_Bits / 8;
	wavform.wBitsPerSample = Quantize_Bits;
	wavform.cbSize = 0;


	waveInOpen(&hWaveIn, WAVE_MAPPER, &wavform, (DWORD_PTR)MaoVoice::waveInProc, (DWORD_PTR)me, CALLBACK_FUNCTION);

	WAVEHDR wh;

	while (true)
	{
		if (true == me->needRelease)
		{
			break;
		}
		else if (true == me->canREC)
		{
			wh.lpData = new char[BUFFER_SIZE];
			wh.dwBufferLength = BUFFER_SIZE;
			wh.dwBytesRecorded = 0;
			wh.dwUser = NULL;
			wh.dwFlags = 0;
			wh.dwLoops = 1;
			wh.lpNext = NULL;
			wh.reserved = 0;

			waveInPrepareHeader(hWaveIn, &wh, sizeof(WAVEHDR));
			waveInAddBuffer(hWaveIn, &wh, sizeof(WAVEHDR));

			me->bufRecTemp = new char[BUFFER_SIZE];

			/* Â¼Òô */

			me->rec_finish = false;
			waveInStart(hWaveIn);
			while (true != me->rec_finish)
				Sleep(1);

			EnterCriticalSection(&me->outBufCritical);
			me->outBufQueue.push(me->bufRecTemp);
			LeaveCriticalSection(&me->outBufCritical);
			if (true == me->canPLAY)
			{
				EnterCriticalSection(&me->playBufCritical);
				me->playBufQueue.push(me->bufRecTemp);
				LeaveCriticalSection(&me->playBufCritical);
			}
			me->bufRecTemp = NULL;

			waveInUnprepareHeader(hWaveIn, &wh, sizeof(WAVEHDR));
			delete[] wh.lpData;
		}
		else
			Sleep(1);
	}
	waveInStop(hWaveIn);
	waveInReset(hWaveIn);
	waveInClose(hWaveIn);

	return 0;
}

unsigned int WINAPI MaoVoice::Play(PVOID para)
{
	MaoVoice * me = (MaoVoice*)para;

	HWAVEOUT hWaveOut;
	WAVEFORMATEX wavform;
	wavform.wFormatTag = WAVE_FORMAT_PCM;
	wavform.nChannels = Channels;
	wavform.nSamplesPerSec = Sample_Rate;
	wavform.nAvgBytesPerSec = Sample_Rate * Quantize_Bits * Channels / 8;
	wavform.nBlockAlign = Channels * Quantize_Bits / 8;
	wavform.wBitsPerSample = Quantize_Bits;
	wavform.cbSize = 0;


	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wavform, (DWORD_PTR)MaoVoice::waveOutProc, 0, CALLBACK_FUNCTION);

	char * buf = NULL;
	WAVEHDR wavhdr;
	while (true)
	{
		if (true == me->needRelease)
		{
			break;
		}
		else if (true == me->canPLAY)
		{
			EnterCriticalSection(&me->playBufCritical);
			if (false == me->playBufQueue.empty())
			{
				buf = me->playBufQueue.front();
				me->playBufQueue.pop();
				LeaveCriticalSection(&me->playBufCritical);
			}
			else
			{
				LeaveCriticalSection(&me->playBufCritical);
				Sleep(1);
				continue;
			}

			wavhdr.dwBufferLength = BUFFER_SIZE;
			wavhdr.lpData = buf;
			wavhdr.dwFlags = 0;
			wavhdr.dwLoops = 0;
			waveOutPrepareHeader(hWaveOut, &wavhdr, sizeof(WAVEHDR));

			/* ·ÅÒô */

			waveOutWrite(hWaveOut, &wavhdr, sizeof(WAVEHDR));
		}
		else
			Sleep(1);
	}

	waveOutReset(hWaveOut);
	waveOutClose(hWaveOut);

	return 0;
}


void CALLBACK MaoVoice::waveInProc(HWAVEIN hwi,
	UINT uMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2)
{
	MaoVoice * me = (MaoVoice *)dwInstance;

	if (WIM_DATA == uMsg)
	{
		memcpy(me->bufRecTemp, LPWAVEHDR(dwParam1)->lpData,BUFFER_SIZE);
		me->rec_finish = true;
	}
}

void CALLBACK MaoVoice::waveOutProc(HWAVEOUT hwo,
	UINT uMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2)
{
	if (WOM_DONE == uMsg)
		delete[] LPWAVEHDR(dwParam1)->lpData;
}



void MaoVoice::startREC()
{
	canREC = true;
}
void MaoVoice::startPLAY()
{
	canPLAY = true;
}

void MaoVoice::stopREC()
{
	canREC = false;
}
void MaoVoice::stopPLAY()
{
	canPLAY = false;
}

void MaoVoice::clearBuffer()
{	
	EnterCriticalSection(&outBufCritical);
	while (false == outBufQueue.empty())
	{
		delete outBufQueue.front();
		outBufQueue.pop();
	}
	LeaveCriticalSection(&outBufCritical);


	EnterCriticalSection(&playBufCritical);
	while (false == playBufQueue.empty())
	{
		delete playBufQueue.front();
		playBufQueue.pop();
	}
	LeaveCriticalSection(&playBufCritical);
}

bool MaoVoice::getVoice(void *& data)
{
	EnterCriticalSection(&outBufCritical);
	if (false == outBufQueue.empty())
	{
		data = outBufQueue.front();
		outBufQueue.pop();
		LeaveCriticalSection(&outBufCritical);
		return true;
	}
	else
	{
		LeaveCriticalSection(&outBufCritical);
		return false;
	}	
}