/*
  ==============================================================================

    osc.cpp
    Created: 15 Jan 2019 6:32:45pm
    Author:  billy

	TODO::take advantage of std:vector in places to avaiod repeated mallocs in dynamically sized arrays.
		::write and use enums for types.

  ==============================================================================
*/

#include "osc.h"

waveGen::waveGen()
{

	type = 0;
	freq = 440;
	vol = 1;
	sampleRate = 0;
	bufferSize = 0;

	//for sine and square

	last = I;

	//for saw and tri
	saw_last = 0;

	//random for noise
	srand(time(NULL));
}

int waveGen::prepare(float Freq, float velocity)
{
	//function to process initial data input from the midi Handler.
	freq = Freq;

	//resets values for new note
	last = I;
	getOmega();

	for (int i = 0; i < modifiersNum; i++) {
		if (modInputs[i].type == FM || modInputs[i].type == AM) {
			modInputs[i].chan->prepare(freq, velocity);
		}
	}

	return 0;
}

int waveGen::nextFrame(float* signal, int sigLen)
{
	allocateModArrs(sigLen);
	getModifierInputs(sigLen);
	//a call for audio data from the parent module.
	if (type == 0) {
		sine(signal, sigLen);
		free(FmInput);
		free(AmInput);
		return 0;
	}
	else if (type == 1) {
		square(signal, sigLen);
		free(FmInput);
		free(AmInput);
		return 0;
	}
	else if (type == 2) {
		saw(signal, sigLen);
		free(FmInput);
		free(AmInput);
		return 0;
	}
	else if (type == 3) {
		tri_wave(signal, sigLen);
		free(FmInput);
		free(AmInput);
		return 0;
	}
	else if (type == 4) {
		noise(signal, sigLen);
	}
	
	//type not recognised
	return 1;
}

int waveGen::freqOffset(float x)
{
	//max of one octave down; linier modification.
	if (x > -1 && x < 1) {
		//tranfoms input from range -1 -- 1, to .5 -- 1 
		offset = 1 + x;

		//updates precalculated values 
		getOmega();
	}
	return 0;
}

int waveGen::getOmega()
{
	//a preCalculation of values used in the sine wave calculation,
	//used when the frequency is updated.
	calculateOffsets();
	omega = exp(I * ((float)2 * PI * offsettedFreq / ((float)sampleRate)));
	return 0;
}

void waveGen::sine(float* signal, int sigLen)
{
	//the indroduction of modifiers here creates large bottleneck in performence 
	getOmega();
	for (int i = 0; i < sigLen; i++) {
		executeModSample(i);
		signal[i] = (real(last)) * offsettedVol;
		last *= omega;
	}
	return;
}

float waveGen::sign(float x)
{
	//return 1 if value above 0, -1 if value below 0// found on stackexchange
	return (float)(x > 0) - (x < 0);
}

void waveGen::square(float* signal, int sigLen)
{
	getOmega();
	for (int i = 0; i < sigLen; i++) {
		executeModSample(i);
		signal[i] = sign((real(last))) * (offsettedVol / 3 /* divide by 3 to offset super loud square*/);
		last *= omega;
	}
	return;
}

void waveGen::tri_wave(float* signal, int siglen)
{

	for (int i = 0; i < siglen; i++) {
		executeModSample(i);
		float angle = 2.0f / ((float)sampleRate / offsettedFreq);
		signal[i] = (saw_last + angle) > 1.0f ? -1.0f : (saw_last + angle);
		saw_last = signal[i];
		signal[i] = (2 * abs(signal[i]) - 1) * offsettedVol;

	}

	return;
}

void waveGen::saw(float* signal, int siglen)
{
	for (int i = 0; i < siglen; i++) {
		executeModSample(i);
		float angle = 2.0f / ((float)sampleRate / offsettedFreq);
		signal[i] = (saw_last + angle) > 1.0f ? -1.0f : (saw_last + angle);
		saw_last = signal[i];
		signal[i] *= offsettedVol;
	}
	return;
}

void waveGen::noise(float* signal, int sigLen)
{
	// might work// as yet untested.
	for (int i = 0; i < sigLen; i++) {
		signal[i] = ((((float)rand()) / ((float)RAND_MAX)) * 2 - 2) * offsettedVol;
	}
}

void waveGen::calculateOffsets()
{
	//calculates offsets in volume and frequecy.
	const float TWELTHROOTOFTWO = 1.059463094359f;
	//transforming offsets in cents, to an expected frequency
	offsettedFreq = (freq * pow(2.0f, octaveTuneOffset)) * pow(TWELTHROOTOFTWO, stepTuneOffset + (fineTuneOffset / 100.0f)) * offset * modRatioOffset;

	offsettedVol = (vol * volOffset) / 5;
}


void waveGen::getModifierInputs(int sigLen)
{
	//calls on child modules used as modifiers 

	//get modifier inputs

	for (int i = 0; i < modifiersNum; i++) {
		
		float*  chanSignal = (float*) calloc(sizeof(float), sigLen);
		modInputs[i].chan->nextFrame(chanSignal, sigLen);

		addToModInput(modInputs[i].type, chanSignal, sigLen);

		free(chanSignal);
	}
}

void waveGen::addToModInput(input::modifierType type, float* signal, int sigLen)
{
	if (type == FM) {
		for (int i = 0; i < sigLen; i++) {
			FmInput[i] += signal[i];
		}
	}
	else if (type == AM) {
		for (int i = 0; i < sigLen; i++) {
			AmInput[i] += signal[i];
		}
	}
}

void waveGen::executeModSample(int i)
{
	freqOffset(FmInput[i]);
	setVolOffset(AmInput[i]);
	calculateOffsets();
	getOmega();
}

void waveGen::allocateModArrs(int sigLen)
{
	FmInput = (float*) calloc(sizeof(float), sigLen);
	AmInput = (float*) calloc(sizeof(float), sigLen);
}

