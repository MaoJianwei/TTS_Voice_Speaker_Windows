
#include "speakVoice.h"

int main() {

	speakVoice voice;


	//voice.maoAsyncSpeak(L"联系塔台1203");

	for (int i = 0; i < 10; i++) {
		voice.maoAsyncSpeak(L"雷达引导离场");
	}
	while (true)
		int a = 0;

	return 0;
}