/*==============================================================================

	IRR_filter.h
	Created: 16 Aug 2019 4:00:26pm
	Author:  billy

    An infinite impulse response filter for use in audio applications that makes use of the liquid dsp library
    for the generation of filter coeficiants
  
    TODO ::
        Stress test,
        Implement the generation of live frequency responce for animation comonents.
        test with alternatives to liquid to assess audio quality.
  
  ==============================================================================
*/

#pragma once
#include <map>
#include <string>
#include <cstdlib>
#include <string>
#include <fstream>
#include <mutex>

#include "../JuceLibraryCode/JuceHeader.h"
#include "messages.h"
#include "liquid.h"

using namespace std;

class IIR_filter
{
public:

	//Atributes

	int samplerate = 0;

	float* A = nullptr;
	float* B = nullptr;

	int M, order, arraysLen;
	float freq, center_freq;
    bool is_on = false;
    string type;
    
    //circ_buffer<float> cleanBuffer;
    float last_overlap[100];
    float clean_last[100];
    
    //values set from UI
    float level = 1; // from -4 << x << 4
    float width = 200; // only for bandpass/stop
    
    float pass_band_ripple = 0.1; // 0 < x < 2
    

	//init
	IIR_filter(const char* Type, int Order, float init_freq = 1000, float Center_freq = 1200)
    {
        //settings for the filter
        type = Type;
        order = Order;
        
        //M = order+1;
        M = 3;
        freq = init_freq;
        center_freq = Center_freq;
        
        calc_arraysLen();
        //calc_coefs();
        
        //zero arrays for overlap
        for (int i = 0; i < M; i++) {
            last_overlap[i] = 0.0;
            clean_last[i] = 0.0;
        }
        
        
    }
    ~IIR_filter()
    {
        free(A);
        free(B);
    }

	//methods
	int set_params(message* params)
    {
        const char* key = params->get_key <const char*>();
        if (strcmp(key, "cutoff") == 0) {
            freq = (params->get_value <float>() > 0) ? params->get_value <float>() : freq; //protection as 0 causes errors ->
            center_freq = freq + width;
            calc_coefs();
        }
        else if (strcmp(key, "samplerate") == 0) {
            samplerate = params->get_value <int>();
            calc_coefs();
        }
        else if (strcmp(key, "slope") == 0) {
            
            /*
            if (type == "highpass") {
                order = int(params->get_value <float>());
            }
            */
            pass_band_ripple = params->get_value <float>();
            //resize arrays
            calc_arraysLen();
            calc_coefs();
        }
        else if (strcmp(key, "actave")==0){
            is_on = params->get_value <bool>();
        }
        
        else if (strcmp(key, "level")==0) {
            float l = params->get_value<float>();
            level = l > -3 && l < 3 ? l : level;
        }
        
        else if (strcmp(key, "width")==0) {
            float w = params->get_value<float>();
            width = w > 0 ? w : width;
            center_freq = freq + width;
            calc_coefs();
        }
        
        return 0;
    }
    
    int calc_arraysLen() {
        
        if (type == "bandpass" || type == "bandstop") {
            arraysLen = 2*order;
        }
        else {
            arraysLen = order;
        }
        
        //compute size of SOS filters
        int r = (arraysLen%2 == 0? 0 : 1);
        int L = (arraysLen-r)/2;
        arraysLen = 3*(L + r);
        
        //filter_mutex.lock();
        //filter coeficiants want memcpy
        if (A == nullptr) {
            A = (float*) malloc(sizeof(float) * arraysLen);
        } else {
            A = (float*) realloc(A, sizeof(float) * arraysLen);
        }
        
        if (B == nullptr) {
            B = (float*) malloc(sizeof(float) * arraysLen);
        } else {
            B = (float*) realloc(B, sizeof(float) * arraysLen);
        }
        //filter_mutex.unlock();
        if (A && B) {
            return 0;
        }
        return 1;
    }
    
    
    int calc_coefs()
    {
        //a function that can calulate the coeficiants given the parameters using cheby
        //this is a stopgap that should be fixed
        if (samplerate==0
            ) {
            return 1;
        }
        
        if (type == "lowpass") {

            float _CO = freq / (samplerate);
            auto t = LIQUID_IIRDES_HIGHPASS;
            
            liquid_iirdes(LIQUID_IIRDES_CHEBY1, //alg
                          t, //type
                          LIQUID_IIRDES_SOS, // outpout (ab)
                          order, //order
                          _CO, //cutoff
                          0,
                          pass_band_ripple,
                          pass_band_ripple,
                          B,
                          A
                          );
            
        }
        else if (type == "highpass") {

            float _CO = freq / (samplerate);
            auto t = LIQUID_IIRDES_HIGHPASS;
            
            liquid_iirdes(LIQUID_IIRDES_CHEBY1, //alg
                          t, //type
                          LIQUID_IIRDES_SOS, // outpout (ab)
                          order, //order
                          _CO, //cutoff
                          0,
                          pass_band_ripple,
                          pass_band_ripple,
                          B,
                          A
                          );
        }
        
        else if (type == "bandpass") {

            float _CO = freq / samplerate;
            float _cen_freq = center_freq / samplerate;
            
            //cout << "cutoff: " << freq << "\ncenter: " << center_freq << "\n";
            auto t = LIQUID_IIRDES_BANDPASS;
            liquid_iirdes(LIQUID_IIRDES_BUTTER, //alg
                          t, //type
                          LIQUID_IIRDES_SOS, // outpout (ab)
                          order, //order
                          _CO, //cutoff
                          _cen_freq, //normalised center freq
                          0.2f, //ripple
                          0.2f, //ripple
                          B,
                          A
                          );
        }
        
        cout << "\nB coefs\n";
        for (int i = 0; i < arraysLen; i++) {
            cout << B[i] << ": ";
        }
        
        cout << "\n A coefs\n";
        for (int i = 0; i < arraysLen; i++) {
            cout << A[i] << ": ";
        }
        return 1;
    }
    
    int process(float* signal, int siglen)
    {

        if (!is_on) {
            return 0;
        }
        
        //input prep
        float input [siglen+M];
        for (int i = 0; i < M; i++) {
            input[i] = clean_last[i];
        }
        memcpy(input+M, signal, siglen*sizeof(float));
        
        //the output prep
        float output[siglen+M];
        for (int i = 0; i < M; i++) {
            output[i] = last_overlap[i];
        }
        for(int i = M; i < siglen+M; i++){
            output[i] = 0.0f;
        }

        //for each SOS triple
        float a[M];
        float b[M];
        for (int i = 0; i < arraysLen/M; i++) {

            //load coeficiants group
            for (int j = 0; j < M; j++) {
                a[j] = A[(i*M)+j];
                b[j] = B[(i*M)+j];
            }
            
            //do the convolution
            for (int N = M; N < siglen+M; N++) {
                
                //the first coef is not effected by feedback
                output[N] = b[0] * input[N];
                
                //acumilate the rest of the coeficiants
                for (int K = 1; K < M; K++) {
                    
                    float feedback = a[K] * output[N - K];
                    
                    output[N] += (b[K] * input[N - K]) - feedback;
                }
            }
            
        }

        //reset the overlaps
        for (int i = 0; i < M; i++) {
            clean_last[i] = input[siglen+i];
            last_overlap[i] = output[siglen+i];
        }
        
        //copy to signal and l=times by level
        if (type == "bandpass") {
            for (int i =0; i < siglen; i++) {
                signal[i] = input[i] + (output[i] * level); // this needs to continuosly add to the
            }
        }
        else if (type == "highpass") {
            for (int i =0; i < siglen; i++) {
                signal[i] = output[i] * level;
            }
        }
        else if (type == "lowpass") {
            for (int i =0; i < siglen; i++) {
                signal[i] = input[i] - output[i];
            }
        }
        return 0;
    }
    
    int processSingleUse(float *signal, int sigLen) {
        //this is a function used to test the responce of a filter
        
        float output[sigLen];
        //for each SOS triple
        float a[M];
        float b[M];
        for (int i = 0; i < arraysLen/M; i++) {

            //load coeficiants group
            for (int j = 0; j < M; j++) {
                a[j] = A[(i*M)+j];
                b[j] = B[(i*M)+j];
            }
            
            //do the convolution
            for (int N = M; N < sigLen; N++) {
                
                //the first coef is not effected by feedback
                output[N] = b[0] * signal[N];
                
                //acumilate the rest of the coeficiants
                for (int K = 1; K < M; K++) {
                    
                    float feedback = a[K] * output[N - K];
                    
                    output[N] += (b[K] * signal[N - K]) - feedback;
                }
            }
            
        }
        
        return 0;
        
    }
    
};
