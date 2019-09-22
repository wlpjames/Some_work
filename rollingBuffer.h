//
//  main.cpp
//  buffer
//
//  Created by William James on 20/09/2019.
//  Copyright © 2019 William James. All rights reserved.
//


#include <iostream>
#include <cstring>
#include <mutex>


/*
 A simple rolling Buffer that is designed for use in audio aplications that either syncronize reading a writing e.g a delay or phaser effect, or that ensure that there is only one thread reading and one writing at any time. e.g filling a buffer of audio data to be used for visualisation such as a spectogram or oscilloscope in a GUI.
 
 One must remember if using this that the write point is allowed to overtake the read point in this design
    and may cause unexpected behavior and nont continuos reads of data if read and write are not
    synchronised and if the read_chunk or read_value functions are being used.
*/
template <typename type>
class rollingBuffer {
public:
    
    type* buffer = NULL;
    int read_ind = 0;
    int write_ind = 0;
    int bufferlen;
    int num_values_added = 0; //keeps track of whether the buffer is full.
    std::mutex buffer_mutex;
    
    rollingBuffer(int BufferLen)
    {
        bufferlen = BufferLen;
        buffer = (type*) malloc(sizeof(type)*bufferlen);
        return;
    }
    
    ~rollingBuffer()
    {
        if(buffer) {
            free(buffer);
        }
    }
    
    /*
        -A function that will give all the values of the buffer from oldest to newest,
     
        -returns 1 if fails or if the buffer is not yet full
     
        -returns 0 is success;
    */
    int read_total(type* ptr)
    {
        //if buffer is not full, return as many as have been written
        if (num_values_added < bufferlen) {
            //this shouldnt be used
            return 1;
        }
        

        //copy the second section to the beggining
        memcpy(ptr, buffer + write_ind, (bufferlen - write_ind) * sizeof(type));
        std::cout << "\n";
        for (int i = 0; i < bufferlen; i++) {
            std::cout << buffer[i];
        }
        
        //copy from beggining to the end of the buffer
        memcpy(ptr + (bufferlen - write_ind), (buffer), (write_ind)*sizeof(type));

        return 0;
    }
    
    /*
        Will write a given chunk to the buffer
     
        If the chunk is larger than the buffer length the only the latter section will be stored.
    */
    void write_chunk(type* ptr, int len)
    {
        //this function wraps the recursave write, unsuring that the buffer mutex is locked while it works
        buffer_mutex.lock();
        w_c(ptr, len);
        buffer_mutex.unlock();
    }
    void w_c(type* ptr, int len)
    {
        //a recursave function that will write a given sized chunk of data to a buffer, is does
        //not handle chunk sizes that a many multiples of the buffersize efficiantly and will be
        //updated in time
        
        if (len+write_ind < bufferlen) {
            memcpy(buffer + write_ind, ptr, len*sizeof(type));
            write_ind += len;
            num_values_added += len;
        }
         
        else {
            //the value of bufferlen - write_index
            int b_minus_ind = bufferlen-write_ind;
            
            //copy the fist part of the values
            memcpy(buffer+write_ind, ptr, (b_minus_ind)*sizeof(type));
            
            //do the rest recursavly
            write_ind = 0;
            num_values_added += b_minus_ind;
            w_c(ptr + (b_minus_ind), len - b_minus_ind);
        }
    }

    /*
        Will read a given chunk from the buffer
     
        Will return non zero if the length to be read would surpass the write point or is an attempt is made to read
            a length greater that the bufferlength
    */
    int read_chunk(type* ptr, int len)
    {
        //this function wraps the read chunk to allow locking the recursave function
        buffer_mutex.lock();
        int r = r_c(ptr, len);
        buffer_mutex.unlock();
        return r;
    }
    int r_c(type* ptr, int len)
    {
        //a recursave function that writes a chunk of data to the buffer of a given size,
        //ensuring the the reading length is not beyond the writing point.
        //returns non zero if opperation is not permited.
        if ((read_ind+len)%bufferlen > write_ind) {
            return 1;
        }

        if (len+read_ind < bufferlen) {
            memcpy(ptr, buffer+read_ind, len*sizeof(type));
            read_ind += len;
        }
        else {
            //value of bufferlen - read_ing
            int to_read = bufferlen-read_ind;
            
            //memcopy what I can and the recursavle do the bit
            memcpy(ptr, buffer+read_ind, (to_read)*sizeof(type));
            read_ind = 0;
            r_c(ptr+to_read, len-to_read);
        }
        return 0;
    }
    
    /*
        fuctions to read and write individual values to the buffer
    */
    type read_val()
    {
        //ensure we are not reading old or uninitiated values
        if (read_ind >= write_ind) {
            return NULL;
        }
        buffer_mutex.lock();
        type val = buffer[read_ind++];
        buffer_mutex.unlock();
        
        if (read_ind >= bufferlen) {
            read_ind -= bufferlen;
        }
        return val;
    }
    
    void write_val(type val)
    {
        buffer_mutex.lock();
        buffer[write_ind++] = val;
        buffer_mutex.unlock();
        
        num_values_added++;
        
        if (write_ind >= bufferlen) {
            write_ind -= bufferlen;
        }
    }
    
    //A simple fuction to test is the buffer is full and the read_total() function can be called
    bool is_buffer_full()
    {
        return bufferlen < num_values_added;
    }
};
