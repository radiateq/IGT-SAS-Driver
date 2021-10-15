#ifndef  FIFO_H
#define FIFO_H

//#include <iostream.h>
#include <iostream>

//public FIFO
 
namespace FIFO
{//Creates FIFO register
 void* Create (int size);
 int   Destroy(void *hFIFO);
 
 int Write(void *hFIFO, unsigned char data);
 int Read (void *hFIFO);

 //Function resets ack
 void ResetACK(void *hFIFO);

 //Function returns ack state
 bool GetACK(void *hFIFO);

 //Function returnes true if FIFO contains data and false otherwise
 bool HasData(void *hFIFO);

 //Function removes first code found in FIFO
 int DeleteFromFIFO(void *hFIFO, unsigned char data);

 void Print(void *hFIFO); 

 //Sorts exceptions according to the priorityList
 void SortExceptions(void *hFIFO, unsigned char *priorityList);

}

#endif
