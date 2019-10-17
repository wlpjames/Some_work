
//compile with :: g++ -o bufferTests bufferTests.cpp
#include <thread>         // std::thread
#include "circular_buffer.h"

//forward declarations
int test_writeChunk_and_readTotal(int dataLen, int chunkSize, int buffLen);
int test_writeChunk_and_readChunk(int repetitions, int chunkSize, int bufferLen);
int test_async_read_and_write(int repetitions, int chunkSize, int bufferLen);
int test_writeChunk_and_readTotal_async(int readReps, int chunkSize, int bufferLen);

using namespace std;

int main (void) 
{
	cout << "Starting tests. \n\n";

	int bufferLen = 1024;
	circ_buffer <float> buffer(bufferLen);

	//try reading a non full buffer:
	float* attempt;
	if (buffer.read_total(attempt)) {
		 cout << "failed to block the read total\n";
	}
	else {
		cout << "succesfully blocked read_total on empty buffer\n";
	}

	//group of tests on writeing chunks and reading the total // the test just checks that the values
	int dataLen;
	int chunkSize;
	
	dataLen = 1024;
	bufferLen = 1024;
	chunkSize = 64;
	test_writeChunk_and_readTotal(dataLen, chunkSize, bufferLen);

	dataLen = 32;
	bufferLen = 16;
	chunkSize = 2;
	test_writeChunk_and_readTotal(dataLen, chunkSize, bufferLen);

	dataLen = 512;
	bufferLen = 16;
	chunkSize = 2;
	test_writeChunk_and_readTotal(dataLen, chunkSize, bufferLen);
	
	dataLen = 2048;
	bufferLen = 16;
	chunkSize = 5;
	test_writeChunk_and_readTotal(dataLen, chunkSize, bufferLen);

	dataLen = 1048576;
	bufferLen = 128;
	chunkSize = 32;
	test_writeChunk_and_readTotal(dataLen, chunkSize, bufferLen);

	dataLen = 134217728;
	bufferLen = 2048;
	chunkSize = 32;
	test_writeChunk_and_readTotal(dataLen, chunkSize, bufferLen);

	//block tests of reading and writing chunks
	int repetitions;

	repetitions = 5;
	chunkSize = 32;
	bufferLen = 64;
	test_writeChunk_and_readChunk(repetitions, chunkSize, bufferLen); 

	repetitions = 500;
	chunkSize = 32;
	bufferLen = 2048;
	test_writeChunk_and_readChunk(repetitions, chunkSize, bufferLen); 

	repetitions = 1048576;
	chunkSize = 16;
	bufferLen = 2048;
	test_writeChunk_and_readChunk(repetitions, chunkSize, bufferLen);


	//odd sizes
	repetitions = 13;
	chunkSize = 42;
	bufferLen = 153;
	test_writeChunk_and_readChunk(repetitions, chunkSize, bufferLen);

	//odd sizes
	repetitions = 5846;
	chunkSize = 111;
	bufferLen = 1845;
	test_writeChunk_and_readChunk(repetitions, chunkSize, bufferLen);

	//odd sizes
	repetitions = 5846;
	chunkSize = 111;
	bufferLen = 113;
	test_writeChunk_and_readChunk(repetitions, chunkSize, bufferLen);

	//testing async
	repetitions = 4;
	chunkSize = 16;
	bufferLen = 64;
	test_async_read_and_write(repetitions, chunkSize, bufferLen);

	repetitions = 40;
	chunkSize = 4;
	bufferLen = 160;
	test_async_read_and_write(repetitions, chunkSize, bufferLen);


	repetitions = 60;
	chunkSize = 128;
	bufferLen = 512;
	test_async_read_and_write(repetitions, chunkSize, bufferLen);


	repetitions = 400;
	chunkSize = 1024;
	bufferLen = 4096;
	test_async_read_and_write(repetitions, chunkSize, bufferLen);

	repetitions = 400;
	chunkSize = 1024;
	bufferLen = 4096;
	test_async_read_and_write(repetitions, chunkSize, bufferLen);

	//check it never stalls
	repetitions = 40000;
	chunkSize = 1024;
	bufferLen = 4096;
	test_async_read_and_write(repetitions, chunkSize, bufferLen);
	

	repetitions = 300;
	chunkSize = 233;
	bufferLen = 517;
	test_async_read_and_write(repetitions, chunkSize, bufferLen);

	repetitions = 30000;
	chunkSize = 184;
	bufferLen = 237;
	test_async_read_and_write(repetitions, chunkSize, bufferLen);

	repetitions = 300;
	chunkSize = 110;
	bufferLen = 111;
	test_async_read_and_write(repetitions, chunkSize, bufferLen);

	repetitions = 300;
	chunkSize = 1;
	bufferLen = 111;
	test_async_read_and_write(repetitions, chunkSize, bufferLen);

	repetitions = 300;
	chunkSize = 111;
	bufferLen = 111;
	test_async_read_and_write(repetitions, chunkSize, bufferLen);
	
	//-------------------------------------------------------------

	//ASYNC total read and write chunk

	///-------------------------------------------------------------
	
	bufferLen = 2048;
	chunkSize = 256;
	int readReps = 5;
	test_writeChunk_and_readTotal_async(readReps, chunkSize, bufferLen);
	
	bufferLen = 64;
	chunkSize = 16;
	readReps = 50;
	test_writeChunk_and_readTotal_async(readReps, chunkSize, bufferLen);

	bufferLen = 1846;
	chunkSize = 333;
	readReps = 200;
	test_writeChunk_and_readTotal_async(readReps, chunkSize, bufferLen);
	
	bufferLen = 32;
	chunkSize = 16;
	readReps = 500000;
	test_writeChunk_and_readTotal_async(readReps, chunkSize, bufferLen);
	
	bufferLen = 4096;
	chunkSize = 256;
	readReps = 5000;
	test_writeChunk_and_readTotal_async(readReps, chunkSize, bufferLen);
	
	bufferLen = 4096;
	chunkSize = 256;
	readReps = 200000;
	test_writeChunk_and_readTotal_async(readReps, chunkSize, bufferLen);

	bufferLen = 4096;
	chunkSize = 256;
	readReps = 20000000;
	test_writeChunk_and_readTotal_async(readReps, chunkSize, bufferLen);
	
}

int test_writeChunk_and_readTotal(int dataLen, int chunkSize, int bufferLen) 
{

	//formating
	cout << "\n----------------------------------------------\n";
	cout << "Testing datalen : " << dataLen << ", chunkSize : " << chunkSize << ", BuffLen : " << bufferLen << "\n"; 
	//creating data to feed to the buffer
	circ_buffer <float> buffer(bufferLen);
	float* data = (float*) malloc(sizeof(float)*dataLen);

	if (!data) {
		return 1;
	}

	for (int i = 0; i < dataLen; i++) {
		data[i] = (float) i;
	}

	//write this data as chunks of 64 values to the buffer
	cout << "Writing " << (dataLen/chunkSize) << " chunks\n";
	for (int i = 0; i < dataLen/chunkSize; i++) {
		buffer.write_chunk(data + (i*chunkSize), chunkSize);
	}

	//test that data has equal values to the buffer
	float* total_read = (float*) malloc(sizeof(float)*bufferLen);


	if (!buffer.read_total(total_read)) {
		cout << "Failed to read the total buffer after assigning values\n";
		free(data);
		free(total_read);
		return 0;
	}
	
	int fail_count = 0;
	for (int i = 1; i < bufferLen; i++) 
	{
		if (!total_read[i] == total_read[i-1] + 1) {
			cout << "Failure on line : " << i << ", value : " << total_read[i] << "\n"; 
			fail_count++;
		} 
	}
	free(data);
	free(total_read);
	if (fail_count > 0) {
		cout << "There were " << fail_count <<" failures in the reading";
		cout << "----------------------------------------------\n";
		return 0;
	}
	else {
		cout << "read_total performed correctly on " << dataLen/chunkSize << " chunks.\n";
		cout << "----------------------------------------------\n";
		return 1;
	}

}

int read_total_async(circ_buffer<float>* buffer, int reps, float* output, bool* stop_signal, bool* Success)
{

	for (int i = 0; i < reps; i++) {

		while (!buffer->read_total(output, true)) {
			continue;

		}
		//check that it is contiguos
		for (int k = 1; k < buffer->bufferlen; k++) {
			if ((output[k] != output[k-1]+1) && output[k]!=0 && output[k] != 0) {
				cout << "memory is not read correctly || i = " << k << ", values : " << output[k-1] << ", " << output[k] << ", read = " << i << ".\n";
				*Success = false;

				for(int j = 0 ; j < buffer->bufferlen; j++) {
					cout << output[j] << ", \n";
				}

				return 0;

			}
		}

		
	}

	*stop_signal = true;
	return 1;
}

int w_async(circ_buffer<float>* buffer, bool* stop_signal, int chunkSize)
{

	float curr_num = 0; 
	while(!*stop_signal) {
		
		float chunk[chunkSize];
		for (int i = 0; i < chunkSize; i++) {
			chunk[i] = curr_num;
			curr_num += 1.0;
			//if (curr_num == UINT_MAX) {
			//	cout << "currnum : " << curr_num;
			//}
		}
		//write chunk to buffer
		buffer->write_chunk(chunk, chunkSize);

	} 
	return 1;
}

int test_writeChunk_and_readTotal_async(int readReps, int chunkSize, int bufferLen)
{

	//formating
	cout << "\n----------------------------------------------\n";
	cout << "Testing asyn read_total and writeChunk.\n" <<
			"ReadReps : " << readReps << ", chunkSize : "
			 << chunkSize << ", BuffLen : " << bufferLen << "\n"; 

	circ_buffer <float> buffer(bufferLen);

	//float* data = (float*) malloc( sizeof( float ) * dataLen);
	//float* read_total = (float*) malloc( sizeof( float ) * bufferLen);

	float read_total[bufferLen];

	bool stop_signal = false;
	bool Success = true;

	//set a thread that will read the buffer and anouther that will write the thread
	thread write (w_async, &buffer, &stop_signal, chunkSize);
	thread read (read_total_async, &buffer, readReps, &read_total[0], &stop_signal, &Success);

  	//wait for them to finish
  	write.join();
  	read.join();

  	cout << "\n----------------------------------------------\n";

  	if (Success) {
  		cout << "All data read correctly.\n";
  	} else {
  		cout << "Test failed, data read incorectly.\n";
  	}

	//free(data);
	//free(read_total);
}

int test_writeChunk_and_readChunk(int repetitions, int chunkSize, int bufferLen) 
{

	cout << "\n----------------------------------------------\n";
	cout << "Testing writeing and reading Chunks \n"; 

	circ_buffer <float> buffer(bufferLen);
	int dataLen = ( chunkSize*repetitions );
	float* data = (float*) malloc( sizeof( float ) * dataLen);
	float* output = (float*) malloc( sizeof( float ) * dataLen);

	if (!data || !output) {
		cout << "failure to malloc arrays\n";
		return 1;
	}

	for (int i = 0; i < dataLen; i++) {
		data[i] = (float) i;
	}	



	//Consectuivly write then read for a defined amount of repetitions
	for (int i = 0; i < repetitions; i++) {
		
		int index = (i*chunkSize);

		//write to the buffer then read the reasult
		buffer.write_chunk(data+index, chunkSize);

		//read a chunk from the buffer
		buffer.read_chunk(output+index, chunkSize);

	}

	//check for equivelence
	for (int i = 0; i < dataLen; i++) {
		
		if (data[i] != output[i]) {
			cout << "Failure on line : " << i << ", value : " << output[i] << "\n"; 
			return 0;
		}
	}

	cout << "read_chunks performed correctly on " 
			<< repetitions << " repetitions of " 
			<< chunkSize  << " chunks, and bufferLen of "
			<< bufferLen << ".\n";

	cout << "----------------------------------------------\n";

	free(data);
	free(output);

	return 1;

}

void read_thread(circ_buffer<float>* buffer, int repetitions, int chunkSize, int bufferLen, float* output)
{

	int index;
	for (int i = 0; i < repetitions; i++) {
		//read a given number of chunks into the output
		index = i * chunkSize;

		//will wait untill either not blocked by thread or write to advance
		while (!buffer->read_chunk(output+index, chunkSize)) {
			continue;
		}
		//cout << "Read at : " << i << "\n";
	}

	//now has finished
	return;
}

void write_thread(circ_buffer<float>* buffer, int repetitions, int chunkSize, int bufferLen, float* input)
{
	int index;
			//cout<<"in write thread";
	for (int i = 0; i < repetitions; i++) {
		index = i * chunkSize;

		//will wait untill either not blocked by thread or read to catch up
		while(!buffer->write_chunk(input+index, chunkSize)) {
			//cout<<"write failed\n";
			continue;
		}
		
		//cout << "Write at : " << i << "\n";
	}

	//has finished
	return;
}

int test_async_read_and_write(int repetitions, int chunkSize, int bufferLen)
{

	cout << "----------------------------------------------\n";
	cout << "Testing asyncronous reading and writeing of chunks\n";
	cout << "BufferLen : " << bufferLen << ", repetitions : " << repetitions << ", chunkSize : " << chunkSize << "\n";

	//the buffer
	circ_buffer <float> buffer(bufferLen, true);


	//input and output data
	int dataLen = ( chunkSize*repetitions );
	float* data = (float*) malloc( sizeof( float ) * dataLen);
	for (int i = 0; i < dataLen; i++) {
		data[i] = (float) i;
	}

	float* output = (float*) malloc( sizeof( float ) * dataLen);

	if (!data || !output) {
		cout << "failure to malloc arrays\n";
		return 1;
	}

	//set a thread that will read the buffer and anouther that will write the thread
	thread read (read_thread, &buffer, repetitions, chunkSize, bufferLen, output);
	thread write (write_thread, &buffer, repetitions, chunkSize, bufferLen, data);

  	//wait for them to finish
  	write.join();
  	read.join();

  	cout << "exited threads\n";

  	//check the input and output are equal
	for (int i = 0; i < dataLen; i++) {
		
		if (data[i] != output[i]) {
			cout << "Failure on line : " << i << ", value : " << output[i] << "\n"; 
			return 0;
		}
	}

	cout << "Test Success!!\n";
	cout << "----------------------------------------------\n\n";

	free(data);
	free(output);

	return 1;

}

