/*
  ==============================================================================

    The main oscillator Unit of a modular synthisis program, 

	Is contanded within a tree structure, owned by its parent module, generaly a channel object,
	owns child modules that can be used to modify this wave in FM and AM synthisis.

	TODO::redo write gets and sets into one function that can accept massages objects :: see messages.h

  ==============================================================================
*/

#pragma once
#include <complex>
#include <stdlib.h>
#include <time.h>
#include "inputs.h"

const float PI = 4.f * atan(1.0f);
const std::complex <float> I(0.0f, 1.0f);


class waveGen : public input 
{
public:
	
	waveGen();

	int prepare(float Freq, float velocity);

	
    int nextFrame(float* signal, int sigLen);

	//Gets and sets for UI
	void setVol(float x) { vol = x; }
	float getVol() { return vol; };

	int freqOffset(float x);

	void setVolOffset(float x) {
		//transforms range of -1 -- 1 to 0 -- 1
		volOffset = x+1;
	}

	void setModRatioOffset(int x) {
		currRatioIndex = x;
		modRatioOffset = modRatios[x];
	}
	float getModRatioOffset() { return (float)currRatioIndex; }

	void setType(int x) { type = x; }
	int getType() { return type; }

	int setBufferSize(int x)
	{
		bufferSize = x;
		return 0;
	}

	int setSampleRate(int x)
	{
		sampleRate = x;
		getOmega();
		
		for (int i = 0; i < modifiersNum; i++) {
			modInputs[i].chan->setSampleRate(x);
		}
		
		return 0;
	}

	int setFreq(float val)
	{
		freq = val;
		getOmega();
		return 0;
	}
	float getFreq() { return freq; }

	int setOctaveTune(float val)
	{
		octaveTuneOffset = (int) val;
		return 0;
	}

	float getOctaveTune() { return (float) octaveTuneOffset; }

	int setStepTune(int val)
	{
		stepTuneOffset = (int) val;
		return 0;
	}

	int getStepTune() { return stepTuneOffset; }

	int setFineTune(float val)
	{
		fineTuneOffset = (float) val;
		return 0;
	}

	int getFineTune() { return (int) fineTuneOffset; }

	input* addModifier(modifierType Type);
	input* addLFO(modifierType type);

private:

	int sampleRate, bufferSize, type, octaveTuneOffset = 0, stepTuneOffset = 0;
	float freq, offset = 1, vol, volOffset = 1, fineTuneOffset = 0;
	float offsettedFreq, offsettedVol, modRatioOffset = 1;

	float* FmInput;
	float* AmInput;

	//for sine and square
	std::complex <float> omega, last; // how to do this too?

	//for saw and tri
	float saw_last = -1;

	const float modRatios[10] = { 1.0f /8.0f, 1.0f / 4.0f ,1.0f / 2.0f, 1.0f, 3.0f / 2.0f, 4.0f / 3.0f, 5.0f / 4.0f, 6.0f / 5.0f, 2.0f / 1.0f, 4.0f / 1.0f };
	int currRatioIndex = 0;

	int getOmega();

	void sine(float* signal, int sigLen);

	float sign(float x);

	void square(float* signal, int sigLen);

	
	void tri_wave(float* signal, int siglen);
	
	
	void saw(float* signal, int siglen);

	void noise(float* signal, int sigLen);

	void calculateOffsets();

	
	void getModifierInputs(int sigLen);
	void executeModSample(int index);
	void addToModInput(input::modifierType, float* signal, int sigLen);
	void allocateModArrs(int sigLen);


};
