/*
 * circularbuffer.inl
 *
 * Copyright (c) 2004 Network for Educational Technology ETH
 *
 * Written by Hannes Friederich, Andreas Fenkart.
 * Based on work of Shawn Pai-Hsiang Hsiao
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 * maccoreaudio.h
 *
 */


#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H


//#define PTRACE_CIRC(level, str) PTRACE(level, str) 
#define PTRACE_CIRC(level, str) 

/**
 * Simple circular buffer for one producer and consumer with a head and a 
 * tail that chase each other. The capacity is 1 byte bigger than necessary 
 * due to a sentinel, to tell apart full from empty by the following equations:
 *
 * full  := head_next == tail
 * empty := head      == tail
 *
 * Inspired by CircularBuffer from beaudio.
 * We need a lock when updating the tail index, due to the overwrite mechanism
 * in case of buffer overflow. The head index needs no locking because 
 * overwrite does not make sense in case of buffer underrun. 
 *
 * Keep in mind that this buffer does no know about frames or packets, it's up
 * to you to make sure you get the right number of bytes. In doubt use size() 
 * to compute how many frames/packets it is save to drain/fill.
 *
 */

class CircularBuffer
{
 public:

   explicit CircularBuffer(PINDEX len)
   :   capacity(len + 1),  /* plus sentinel */ head(0), tail(0)
   {
      buffer = (char *)malloc(capacity*sizeof(char));
      if(!buffer) {
         PTRACE(1, "Could not allocate circular buffer");
         init = false;
      } else {
         init = true;
      }

      /*
       * Mutex to block write thread when buffer is full
       */
      int rval;
      rval = pthread_mutex_init(&mutex, NULL);
      if (rval) {
        PTRACE(1, __func__ << " can not init mutex");
        init = false;
        //return FALSE;
      }
      rval = pthread_cond_init(&cond, NULL);
      if (rval) {
        PTRACE(1, __func__ << " can not init cond");
        init = false;
        //return FALSE;
      }
   }

   ~CircularBuffer()
   {
      if(buffer){
         std::free(buffer);
         buffer = NULL;
      }

      pthread_mutex_destroy(&mutex);
      pthread_cond_destroy(&cond);
   }
 

   BOOL Full()
   {
      /* head + 1 == tail */
      return head_next() == tail;
   }

   BOOL Empty()
   {
      return head == tail;
   }

   PINDEX size(){
      /* sentinel is outside of occupied area */
      return (head < tail ) ? head + capacity - tail : head - tail; 
   }

   PINDEX free(){
      return (capacity - size() -1 /*sentinel */);
   }

   /** 
    * Fill inserts data into the circular buffer. 
    * Remind that lock and overwrite are mutually exclusive. If you set lock,
    * overwrite will be ignored 
    */
   PINDEX Fill(const char* inbuf, PINDEX len, Boolean lock = true, 
                     Boolean overwrite = false);


   /** See also Fill */
   PINDEX Drain(char* outbuf, PINDEX len, Boolean lock = true);

 private:
   PINDEX head_next()
   {
      //return (head + 1 == capacity) ? 0 : (head + 1);
      return (head + 1 % capacity);
   }

   void increment_index(PINDEX &index, PINDEX inc)
   {
      index += inc;
      index %= capacity;
   }

   void increment_head(PINDEX inc)
   {
      increment_index(head, inc);
   }     
    
   void increment_tail(PINDEX inc)
   {
      increment_index(tail, inc);
   }



 private:
   char* buffer;
   const PINDEX capacity;
   PINDEX head, tail;
   Boolean init;

   pthread_mutex_t mutex;
   pthread_cond_t cond;
};


int CircularBuffer::Fill(const char *inbuf, PINDEX len, 
         Boolean lock, Boolean overwrite)
{
   int done = 0, todo = 0;
   
   PTRACE_CIRC(1, "Fill " << len << " bytes, contains " << size());

   if(inbuf== NULL){
      PTRACE(1, __func__ << "Input buffer is empty");
      return 0;
   }

   while(done != len && !Full()) {
      if(head >= tail) {
         // head unwrapped, fill from head till end of buffer
         if(tail == 0) /* buffer[capacity] == sentinel */
            todo = MIN(capacity -1 /*sentinel*/ - head, len - done);
         else
            todo = MIN(capacity - head, len - done);
      } else {
         // fill from head till tail 
         todo = MIN(tail -1 /*sentinel*/ - head, len - done);
      }
      memcpy(buffer + head, inbuf + done, todo);
      done += todo;
      increment_head(todo);
      PTRACE_CIRC(1, "copied " << done << " from input buffer"
         << " available " << size());
   }

   // What to do if buffer is full and more bytes
   // need to be copied ?  
   if(Full() && done != len && (overwrite || lock)) {
      PTRACE_CIRC(1, __func__ << "Circular buffer is full, while Fill " 
            << len); 
      if(lock) {
         pthread_mutex_lock(&mutex);
         PTRACE_CIRC(1, "Fill going to sleep");
         pthread_cond_wait(&cond, &mutex);
         // pthread_cond_timedwait
         PTRACE_CIRC(1, "Fill woke up");
         pthread_mutex_unlock(&mutex);
      } else if(overwrite){
         pthread_mutex_lock(&mutex);
         if(Full()){
            tail += len - done; // also shifts sentinel
            tail %= capacity; // wrap around
         }
         pthread_mutex_unlock(&mutex);
      }
      // try again
      done += Fill(inbuf + done, len - done, lock, overwrite);
   }

   // wake up read thread if necessary
   if(!Empty())
      pthread_cond_signal(&cond);
   
   
   PTRACE_CIRC(1, __func__ << " " << len << " bytes, stored " << done 
         << " available " << size());
   return done;
}


PINDEX CircularBuffer::Drain(char *outbuf, PINDEX len, Boolean lock) {
   PINDEX done = 0, todo = 0;
   
   PTRACE_CIRC(6, __func__ << " " << len << " bytes, available " << size() );

   if(outbuf == NULL){
      PTRACE(1, __func__ << " Out buffer is NULL"); 
      return PINDEX(0);
   }

   /* protect agains buffer corruption when write thread
    * is overwriting */
   pthread_mutex_lock(&mutex);
   PTRACE(6, "aquired lock");

   while(done != len && !Empty()) {
      if(head >= tail) {
         // head unwrapped 
         todo = MIN(head - tail, len - done);
      } else {
         // head wrapped 
         todo = MIN(capacity - tail, len - done);
      }        
      memcpy(outbuf + done, buffer + tail, todo);
      done += todo;
      increment_tail(todo);
      PTRACE_CIRC(1, __func__ << " for " << len " bytes, copied " << done 
            << " to output buffer,  available in buffer " << size());
   }


   if(done != len && (lock)) /* && Empty() */ {
      PTRACE(3, "Buffer underrun for Drain " << len << " bytes");
   }

   // what to do if not as many bytes are available then
   // requested ?
   if(done != len && (lock)) /* && Empty() */ {
      if (lock){
         pthread_cond_wait(&cond, &mutex);
         PTRACE_CIRC(2, "Read thread woke up, calling Drain");
         pthread_mutex_unlock(&mutex); // race with write thread...
         done += Drain(outbuf + done, len - done, lock);
         PTRACE_CIRC(2, "Returned from recursive");
      }
   }

   pthread_mutex_unlock(&mutex);

   if(!Full())
      pthread_cond_signal(&cond);

   PTRACE_CIRC(2, "End Drain " << len << " bytes, fetched " << done
         << " buffer fill " << size() );
   return done;

}
#endif
