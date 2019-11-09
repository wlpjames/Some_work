Some short example of some work that I have done in C++.

## messages.h :: 
A template function to help comunicate between GUI components and DSP classes, an array of which could act as a pythonesque dict.

## rollingBuffer.h :: 
A simple rolling buffer to be used in tasks that ensure coordinated reading and writting of data. Examples of current use are in a tape-like delay system, or in the buffering of audio data that can be animated and used in a spectogram.

## oscillator ::
A header and cpp file that defines the main oscilator used in some of my audio work. This is designed to be used in a modular settings. deals with aliasing quite badly, this has been improved in later verions. These can be linked to achieve FM sysnthisis but some work will have to be done to lessen the aliasing that is caused be this.

## filter :: 
An example of a audio filter that I have created for use in a standalone audio plugin that is in development. 
