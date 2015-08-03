
#include "MaoVoice.h"


 
int main()
{
	MaoVoice * voice = new MaoVoice;

	system("pause");

	voice->startREC();

	system("pause");

	voice->startPLAY();

	system("pause");

	voice->stopPLAY();

	system("pause");

	voice->stopREC();

	system("pause");

	voice->startREC();

	system("pause");

	voice->startPLAY();

	system("pause");

	delete voice;

	return 0;
}