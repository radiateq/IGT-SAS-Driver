#ifndef  FIFO_local
#define FIFO_local

//private FIFO

namespace FIFO
{typedef struct TFIFOdata
 {unsigned char *FIFO;
  int  size;
  unsigned char wcnt;
  unsigned char rcnt;
  //FIFOAck se uporablja kadar preberemo vrednost iz FIFO
  //takrat se FIFOAck postavi na true, ter ko je sprejet ACK
  // se stevec poveca ter FIFOAck postavi na false
  bool ack;
  //FIFOWarp se postavi na true, kadar FIFOWrite gre cez size
  bool warp;
  //FIFOOwerwrite je enako true kadar je FIFOWrite enako true in Wcnt postane enak Rcnt
  bool overwrite;
  //Variable is set to true if any data is overwritten
  //When reading from FIFO if exc70h is true ack is set to true and 0x70 is returned
  //After ACK has been recieved exc70h is set to false
  bool exc70h;
  //If this variable is false exc70h is never set to true
  //it is set to false by specifying negative value to the create function
  bool exc70hEnable;
 };

 int AllocateSize(void *hFIFO, unsigned int size);
}

#endif
