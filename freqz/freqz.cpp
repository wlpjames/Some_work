#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <complex>
#include <iostream>
#include <fstream>
using namespace std;

void freqz( float* b, float* a, int order, int samplerate, int num_samples, float* output, bool log_2=false)
{

    int niq = samplerate / 2;


    float inc =  log_2 ? float(log2(niq) / num_samples) : float(niq / num_samples);   
    
    float freq;
    
    float e  = 2.71828;

    for (int i = 0; i < num_samples; i++) {

        std::complex<float> w(M_PI * (freq / niq));
        freq = log_2 ? pow(2, i * inc) : (i * inc);
        
        //z = e ** ( 1j * w )
        std::complex<float> z = pow(e, ( w * (std::complex<float>)1j ));

        std::complex<float> SUM_B = std::complex<float>(b[0]);
        std::complex<float> SUM_A = std::complex<float>(a[0]);

        //asuming equal order
        for (int k = 1; k < order+1; k++) {
            SUM_B += b[k] * ( pow(z, k) );
            SUM_A +=  a[k] * ( pow( z, k ));
        }

        output[i] = abs( SUM_B / SUM_A );
              
    }
    return;
}



int main(int argc, char *argv[]) {

    if (!argc > 1) {
        cout << "args needed :: args = order, b, a, samplerate, numSamples, outputfile";
        return 1;
    }
    //problem here 
    string s(argv[1]);
    int order = std::stoi(s);

    if (argc != (order+1)*2 + 6) {
        cout << "args = order, b, a, samplerate, numSamples, outputfile";
        return 1;
    }

    float b[order+1]; 
    float a[order+1];
    for (int i = 0; i < order+1; i++) {
        
        s.assign(argv[i+2]);
        b[i] = std::stof(s);

        s.assign(argv[i+order+3]);
        a[i] = std::stof(s);

    }

    s.assign(argv[((order+1)*2) +2]);
    int samplerate = std::stoi(s);

    s.assign(argv[((order+1)*2) +3]);
    int numSamples = std::stoi(s);

    s.assign(argv[((order+1)*2) +5]);
    
    bool log_2 = stoi(s);
    
    cout << "\n\n" << log_2 << "\n\n";

    float output[numSamples];

    freqz(b, a, order, samplerate, numSamples, output, log_2); 

    std::ofstream file;
    file.open(argv[argc-2]);

    if (file.is_open()) {

        for (int i = 0; i < numSamples; i++) {
            file << output[i] << "\n";
        }
    }


    return 0;
}
