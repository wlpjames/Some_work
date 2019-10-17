/*
 A simple rolling Buffer that is designed for use in audio aplications that either syncronize reading a writing e.g a delay or phaser effect, or that ensure that there is only one thread reading and one writing at any time. e.g filling a buffer of audio data to be used for visualisation such as a spectogram or oscilloscope in a GUI.
 
 One must remember if using this that the write point is allowed to overtake the read point in this design
    and may cause unexpected behavior and nont continuos reads of data if read and write are not
    synchronised and if the read_chunk or read_value functions are being used.
*/

#include <iostream>
#include <cstring>
#include <mutex>

template <typename type>
class circ_buffer {
public:
    
    int read_ind = 0;
    int write_ind = 0;
    int num_values_added = 0; //keeps track of whether the buffer is full.
    int amount_to_read = 0;
    std::mutex buffer_mutex;
    type* buffer;
    int bufferlen;
    bool check_for_overlap;


    circ_buffer(int BufferLen, bool check_overlap = false)
    {
        bufferlen = BufferLen;
        buffer = new type[bufferlen];

        check_for_overlap = check_overlap;

        return;
    }
    
    ~circ_buffer()
    {
        delete[] buffer;
    }


    /*  READ TOTAL
    -A function that will give all the values of the buffer from oldest to newest,
    -returns 1 if fails or if the buffer is not yet full
    -returns 0 is success;
    */
    int read_total(type* ptr, bool mark_as_empty = false)
    {
    	
        //if buffer is not full, return as many as have been written
        if (num_values_added < bufferlen) {
            //this shouldnt be used
            //std::cout << "Num_values added : " << num_values_added << "\n";
            
            return 0;
        }
        buffer_mutex.lock();
        //copy the second section to the beggining
        memcpy(ptr, buffer + write_ind, (bufferlen - write_ind) * sizeof(type));
        
        //copy from beggining to the end of the buffer
        memcpy(ptr + (bufferlen - write_ind), (buffer), (write_ind)*sizeof(type));
        //buffer_mutex.unlock();

        if (mark_as_empty) {
        	num_values_added = 0;
        }
        buffer_mutex.unlock();
        return 1;
    }

    /*
        Will write a given chunk to the buffer
     
        If the chunk is larger than the buffer length the only the latter section will be stored.
    */

    int write_chunk(type* ptr, int len) {

    	//std::cout << "Write called\n";
    	if (len > bufferlen) {
    		std::cout << "chunk is greater than buffer len";
    		return 0;
    	}

    	if (check_for_overlap && (amount_to_read + len > bufferlen)) {
    		//std::cout << "not enought space to write || amount_to_read : " << amount_to_read << " len : " << len << "\n";
    		return 0;
    	}
    	buffer_mutex.lock();
    	if (len > bufferlen) {
    		//only write the last {bufferlen} values
    		ptr += (len-bufferlen);
    		len = bufferlen;
    	}

    	
    	//if write chunk + len < bufferlen simply copy
    	if ( write_ind+len <= bufferlen ) {
    		//buffer_mutex.lock();
    		memcpy(buffer + write_ind, ptr, len*sizeof(type));
    		//buffer_mutex.unlock();
            write_ind += len;
    	
    	}
    	//else will wrap around the end of the buffer
    	else {

    		//first copy onto the end of the buffer
    		int writeAmount = (bufferlen-write_ind);
    		//buffer_mutex.lock();
    		memcpy(buffer + write_ind, ptr, writeAmount*sizeof(type));
    		//buffer_mutex.unlock();

            //write the next onto the beggining of the buffer
            //buffer_mutex.lock();
            memcpy(buffer, ptr+writeAmount, (len-writeAmount)*sizeof(type));
            //buffer_mutex.unlock();
            write_ind = len-writeAmount;

            //add to num values added for
            

    	}
    	num_values_added += len;
    	amount_to_read += len;
    	//std::cout << "amount to read changed in WRITE thread || is : " << amount_to_read << "\n";
    	//catch this possability
    	if (write_ind == bufferlen) {
    		write_ind = 0;
    	}

    	buffer_mutex.unlock();

    	return 1;

    }

    /*
        Will read a given chunk from the buffer
     
        If the requested is larger than the buffer length the function will return 0.
    */

    int read_chunk(type* ptr, int len) 
    {
    	
    	if (check_for_overlap && (amount_to_read < len)) {
    		//std::cout << "Read Regected || len : " << len << ", amount_to_read : " << amount_to_read << "\n";
    		return 0;
    	}
    	buffer_mutex.lock();
    	//if no overlap, simply copy the data
    	if (read_ind+len <= bufferlen) {

    		//buffer_mutex.lock();
    		memcpy(ptr, buffer + read_ind, len*sizeof(type));
    		//buffer_mutex.unlock();
            read_ind += len;
    	
    	}
    	
    	//else the copy will be done in two parts as it overlaps the buffer array
    	else {

    		int read_amount = bufferlen-read_ind;
    		//buffer_mutex.lock();
    		memcpy(ptr, buffer + read_ind, sizeof(type) * read_amount );
    		memcpy(ptr+read_amount, buffer, sizeof(type) * (len-read_amount));
    		//buffer_mutex.unlock();
    		read_ind = len-read_amount;

    	}
    	amount_to_read -= len;
    	//std::cout << "amount to read changed in read thread || is : " << amount_to_read << "\n";
    	//catch this possability
    	if (read_ind == bufferlen) {
    		read_ind = 0;
    	}

    	buffer_mutex.unlock();
    	return 1;
    }
};