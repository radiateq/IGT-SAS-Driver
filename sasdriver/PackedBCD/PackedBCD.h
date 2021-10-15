#ifndef PACKEDBCD_H
#define PACKEDBCD_H

#include <stdlib.h>

namespace PBCD
{int PBCD2Int(unsigned char *PBCDnumber, int len, unsigned long long *decimalNumber); 
 int Int2PBCD(unsigned long long  decimalNumber, unsigned char *PBCDnumber, int *len, int BCDsize = 0);
 unsigned long long raise(unsigned long long number, int exponent);
}

#endif
