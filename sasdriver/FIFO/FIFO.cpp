#include "FIFO_local.h"
#include "FIFO.h"

using namespace std;

void FIFO::Print(void *hFIFO)
{if (NULL == hFIFO)
  return;
 cout << "PRINT-------------------------" << endl;
 cout << "wcnt      :" << (int)((TFIFOdata*)hFIFO)->wcnt << endl 
      << "rcnt      :" << (int)((TFIFOdata*)hFIFO)->rcnt << endl
      << "ack       :" << ((TFIFOdata*)hFIFO)->ack       << endl;
 cout << "warp      :" << ((TFIFOdata*)hFIFO)->warp      << endl;
 cout << "owerwrite :" << ((TFIFOdata*)hFIFO)->overwrite << endl;
 cout << "exc70h    :" << ((TFIFOdata*)hFIFO)->exc70h    << endl;
 for(int cnt = 0; cnt < (((TFIFOdata*)hFIFO)->size +1); cnt++)
  //cout << "FIFO pos :" << cnt << ":" << (int)((TFIFOdata*)hFIFO)->FIFO[cnt] << endl;
  printf("FIFO pos :%i:%x\n", cnt, (int)((TFIFOdata*)hFIFO)->FIFO[cnt]);
 cout << "PRINT-------------------------" << endl;
}

//Creates FIFO register. If size is negative, exception 0x70 is never returned
//If size is 0 it is increased to 1
void* FIFO::Create(int size)
{TFIFOdata *FIFO = new TFIFOdata();
 if (0 == size)
  size = 1;
 if (0 > size)
 {((TFIFOdata*)FIFO)->exc70hEnable = false;
  size = -size;
 }
 else
  ((TFIFOdata*)FIFO)->exc70hEnable = true;
 if (0 < AllocateSize(FIFO, size))
  return NULL;
 return FIFO;
}

int FIFO::Destroy(void *hFIFO)
{
free (((TFIFOdata*)hFIFO)->FIFO);
 delete ((TFIFOdata*)hFIFO);
 return 0;
}

/*Function accepts FIFO General Poll Size It returns -1 on faluer and 0 on success
 
  On successfull memory allocation it fills FIFO with 0-s and sets some system 
  variables concerning FIFO functionality
*/
int FIFO::AllocateSize(void *hFIFO, unsigned int size)
{if (NULL == hFIFO)
  return -1;
 ((TFIFOdata*)hFIFO)->FIFO = (unsigned char*) malloc(size);
 if (NULL == ((TFIFOdata*)hFIFO)->FIFO)
  return -1;
 ((TFIFOdata*)hFIFO)->size      = size - 1;
 ((TFIFOdata*)hFIFO)->wcnt      = 0;
 ((TFIFOdata*)hFIFO)->rcnt      = 0;
 ((TFIFOdata*)hFIFO)->ack       = false;
 ((TFIFOdata*)hFIFO)->warp      = false;
 ((TFIFOdata*)hFIFO)->overwrite = false;
 ((TFIFOdata*)hFIFO)->exc70h    = false;
 memset(((TFIFOdata*)hFIFO)->FIFO, 0, size);
 return 0;
}

bool FIFO::GetACK(void *hFIFO)
{return ((TFIFOdata*)hFIFO)->ack;
}

//Read char TGeneralPoll::FIFORead() for explanation
void FIFO::ResetACK(void *hFIFO)
{if (NULL == hFIFO)
  return;
 ((TFIFOdata*)hFIFO)->ack = false;
}
              
//Function returnes true if FIFO contains data and false otherwise   
bool FIFO::HasData(void *hFIFO)
{return (bool)!((((TFIFOdata*)hFIFO)->wcnt == ((TFIFOdata*)hFIFO)->rcnt)&&!((TFIFOdata*)hFIFO)->overwrite);
}

/*This function accepts data of type char
  and writes it to FIFO array
  Function returns -1 on internal program error and 0 otherwise
  buffer can not overflow due to SAS specification
  that oldest data is overwritten
  FIFO does not accept binary value of 00000000 since it presents no data to the FIFO functions

  Once any data is overwritten exception 0x70 is issued according to the protocol
*/
int FIFO::Write(void *hFIFO, unsigned char data)
{if (NULL == hFIFO)
  return -1;
 if (0 != data)
 {//Writes data to current position and increments the counter
  ((TFIFOdata*)hFIFO)->FIFO[((TFIFOdata*)hFIFO)->wcnt++] = data;
  //Counter is checked for range
  //((TFIFOdata*)hFIFO)->FIFOWarp is used to indicate that ((TFIFOdata*)hFIFO)->FIFOWcnt exceeded ((TFIFOdata*)hFIFO)->FIFOSize
  //((TFIFOdata*)hFIFO)->FIFOWarp works in conjuction with ((TFIFOdata*)hFIFO)->FIFOWarp, explained later in ((TFIFOdata*)hFIFO) function
  if (((TFIFOdata*)hFIFO)->wcnt > ((TFIFOdata*)hFIFO)->size)
  {((TFIFOdata*)hFIFO)->wcnt = 0;
   ((TFIFOdata*)hFIFO)->warp = true;
  }
   
  //Warp true and Read and Write counters are same
  //indicated that data from ((TFIFOdata*)hFIFO) point on are overwritten
  //Actions necessary are explained later in ((TFIFOdata*)hFIFO) function
  if ((((TFIFOdata*)hFIFO)->rcnt == ((TFIFOdata*)hFIFO)->wcnt)&&(((TFIFOdata*)hFIFO)->warp))
  {((TFIFOdata*)hFIFO)->warp      = false;
   ((TFIFOdata*)hFIFO)->overwrite = true;  
  }
 
  //Since ((TFIFOdata*)hFIFO)->wcnt is actually pointing on next position to be written
  //current position has to be calculated
  unsigned char tmpch = ((TFIFOdata*)hFIFO)->wcnt==0 ? ((TFIFOdata*)hFIFO)->size : ((TFIFOdata*)hFIFO)->wcnt - 1;
  //Did Write counter reach Read counter (((TFIFOdata*)hFIFO)->FIFOOwerwrite == true) or
  //are they only at same position (((TFIFOdata*)hFIFO)->FIFOOwerwrite == false)
  if ((((TFIFOdata*)hFIFO)->overwrite)&&(((TFIFOdata*)hFIFO)->rcnt == tmpch))        
  {//Write counter has reached Read counter from behind
   //That indicates that counter has to be pushed in to the next position
   if (((TFIFOdata*)hFIFO)->rcnt < ((TFIFOdata*)hFIFO)->size)
    ((TFIFOdata*)hFIFO)->rcnt ++;
   else
    ((TFIFOdata*)hFIFO)->rcnt = 0;
   //If sent data was not ACK-ed, now when it was overwritten ACK variable needs to be reset too
   FIFO::ResetACK(hFIFO);
   //Write had pushed read variable so exception 0x70 has to be reisued
   ((TFIFOdata*)hFIFO)->exc70h = true&((TFIFOdata*)hFIFO)->exc70hEnable;
  }
 }
 return 0;
}

/*((TFIFOdata*)hFIFO) function is called twice for each read operation
  First time it is called only data is returned or in case there is no data 0 is returned
  Second time it is called function clears returned data and increments Read counter ((TFIFOdata*)hFIFO)->FIFORcnt 
  Variable determinig mode (Read or ACK) is ((TFIFOdata*)hFIFO)->FIFOAck where true means data has been returned but
  state of FIFO register have not changed yet
  FIFO does not accept a binary value of 00000000 since it presents that no data to the FIFO register
 
  Once any data is overwritten exception 0x70 is issued according to the protocol
*/
int FIFO::Read(void *hFIFO)
{int erasedData;
 if (NULL == hFIFO)
  return 0;
 if(((TFIFOdata*)hFIFO)->ack)
 {((TFIFOdata*)hFIFO)->ack = false;
  if (((TFIFOdata*)hFIFO)->exc70h)
   ((TFIFOdata*)hFIFO)->exc70h = false;
  else
  {
   erasedData = ((TFIFOdata*)hFIFO)->FIFO[((TFIFOdata*)hFIFO)->rcnt];
   ((TFIFOdata*)hFIFO)->FIFO[((TFIFOdata*)hFIFO)->rcnt] = 0;  
   //Checks read counter ((TFIFOdata*)hFIFO)->FIFORcnt for range
   if (((TFIFOdata*)hFIFO)->rcnt < ((TFIFOdata*)hFIFO)->size)
    ((TFIFOdata*)hFIFO)->rcnt ++;
   else
    ((TFIFOdata*)hFIFO)->rcnt = 0;  
   //Calculates previous Write counter ((TFIFOdata*)hFIFO)->FIFOWcnt position
   unsigned char tmpch = ((TFIFOdata*)hFIFO)->wcnt==0 ? ((TFIFOdata*)hFIFO)->size : ((TFIFOdata*)hFIFO)->wcnt - 1;
   //Did read counter reach write counter
   if (((TFIFOdata*)hFIFO)->rcnt == tmpch)
    //If it did buffer is empty and there is no need to push read counter in to next position
    ((TFIFOdata*)hFIFO)->overwrite = false; 
  }
  //Everything 0.K.!
  return erasedData;
 }
 else
 {//If there is data to be returned ((TFIFOdata*)hFIFO)->ack is set to true so that next time
  //function is called FIFO state is calculated
  if (((TFIFOdata*)hFIFO)->exc70h)
  {//FIFO is full return exception 0x70
   ((TFIFOdata*)hFIFO)->ack = true;
   return 0x70;
  }
  else if (((TFIFOdata*)hFIFO)->FIFO[((TFIFOdata*)hFIFO)->rcnt])
  {((TFIFOdata*)hFIFO)->ack = true;
   return ((TFIFOdata*)hFIFO)->FIFO[((TFIFOdata*)hFIFO)->rcnt];
  }
  else
   return 0;
 }
}



void FIFO::SortExceptions(void *hFIFO, unsigned char *priorityList)
{unsigned char tmpData;
 //Contains data that was sent but was not ACK-ed
 //otherwise contains 0
 //This data is read from FIFO first and stored
 //Before any other data was written dataACK is sent to the fifo and ACK is set to true
 unsigned char dataACK = 0;
 bool dataFound = false;
 int  FIFOsize  = ((TFIFOdata*)hFIFO)->size;
 //This array will contain number of exceptions in FIFO
 //index of exception matches one in the priorityList
 unsigned char *FIFOSorted = (unsigned char*)malloc(priorityList[0] + 1);
#ifdef SAS_LINUX
	 bzero(FIFOSorted, priorityList[0] + 1);
#endif
#ifdef SAS_WINDOWS
	 memset(FIFOSorted, 0, priorityList[0] + 1);
#endif

 //This array will contain all exceptions not listed in priorityList
 //These are placed last in hFIFO
 unsigned char *FIFONotSorted = (unsigned char*)malloc(FIFOsize + 1);
#ifdef SAS_LINUX
	bzero(FIFONotSorted, FIFOsize + 1);
#endif
#ifdef SAS_WINDOWS
	memset(FIFONotSorted, 0, FIFOsize + 1);
#endif
 int cntFIFONotSorted = -1;

 //Check if there is data sent, that was not ACK-ed
 if (FIFO::GetACK(hFIFO))
 {FIFO::ResetACK(hFIFO);
  dataACK = FIFO::Read(hFIFO);
  //ACK data - clear it from FIFO
  FIFO::Read(hFIFO);
 }  

 //Count exceptions - empty FIFO
 while((tmpData = FIFO::Read(hFIFO))&&(tmpData))
 {FIFO::Read(hFIFO);//ACK
  //Search for index of read data
  for(int cnt = 1; cnt <= priorityList[0]; cnt++)
  {if (tmpData == priorityList[cnt])
   {dataFound = true;
    FIFOSorted[cnt]++; 
    break; 
   }   
  }
  //If exception is not to be sorted
  if (!dataFound)
   FIFONotSorted[++cntFIFONotSorted] = tmpData;
  dataFound = false;  
 }

 //Setup FIFO so that if any data that had to be sent (not ACK-ed) is sent
 if (dataACK)
 {FIFO::Write(hFIFO, dataACK);
  FIFO::Read(hFIFO);
 } 
 //Fill FIFO with data again
 //Fill FIFO with exceptions listed in prioritylist
 for (int cnt = 1; cnt <= priorityList[0]; cnt++)
  for (int cnt2 = 0; cnt2 < FIFOSorted[cnt]; cnt2++)
   FIFO::Write(hFIFO, priorityList[cnt]);
 //Fill FIFO with data not listed in priorityList
 for (int cnt = 0; cnt <= cntFIFONotSorted; cnt++)
  FIFO::Write(hFIFO, FIFONotSorted[cnt]);

 free(FIFOSorted);
 free(FIFONotSorted);
}

//Function removes all data that from FIFO that equals "data"
//Returns 1 if data is deleted 
//Returns 0 if no data is found
//Returns 2 if data to be deleted was not ACK-ed
//Returns -1 on error
int FIFO::DeleteFromFIFO(void *hFIFO, unsigned char data)
{unsigned char *tmpFIFO = (unsigned char *)malloc(((TFIFOdata*)hFIFO)->size); 
 int tmpFIFOcnt = 0;
 //Remembers state of hFIFO ACK variable
 bool ACK = false;
 int returnValue = 0;
 if (NULL == tmpFIFO)
  return -1;
 //Is there ACK waiting
 if (FIFO::GetACK(hFIFO))
 {ACK = true;
  FIFO::ResetACK(hFIFO);
 }
 do
 {//Read data
  tmpFIFO[tmpFIFOcnt] = FIFO::Read(hFIFO);
  //ACK
  FIFO::Read(hFIFO);
  if (data == tmpFIFO[tmpFIFOcnt])      
   if ((!tmpFIFOcnt)&&(ACK))
    //Data to be deleted was not ACK-ed - therefore will not be deleted
    returnValue = 2;
   else
    //Data deleted
   {tmpFIFO[tmpFIFOcnt--] = 0;
    if (2 != returnValue)
     returnValue = 1;
   }  
 } while(0 != tmpFIFO[tmpFIFOcnt++]);
 for (int cnt = 0; cnt < tmpFIFOcnt; cnt++) 
  FIFO::Write(hFIFO, tmpFIFO[cnt]); 
 //Set ACK to right state
 if (ACK)
  FIFO::Read(hFIFO);
 free (tmpFIFO);
 return returnValue;
}
