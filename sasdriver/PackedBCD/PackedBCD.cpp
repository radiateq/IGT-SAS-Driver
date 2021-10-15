#include "PackedBCD.h"
#include "PackedBCD_local.h"

//#include <iostream.h>
#include <iostream>

unsigned long long PBCD::raise(unsigned long long number, int exponent)
{unsigned long long result = number;
 if (0 == exponent)
  return 1;
 if (exponent < 0)
  exponent = - exponent;
 if (1 == exponent--)
  return number;
 for (;exponent;exponent--)
  result *= number;
 return result;  
}

//Function converts BCDnumber in to decimalNumber
//Function accepts char array, digits number (0 means no data and is therefore invalid
//Function stores deciaml Number in variable whose address is passed to decimalNumber pointer
//On success returns 0 on failure -1                             
int PBCD::PBCD2Int(unsigned char *PBCDnumber, int len, unsigned long long *decimalNumber)
{unsigned char tmpChar;
 int tmpInt = 0;
 if (1 == (len%2))
  len++;
 *decimalNumber = 0;
	do
	{tmpChar = PBCDnumber[tmpInt] >> 4;
 	*decimalNumber += tmpChar * PBCD::raise(10, --len);
		tmpChar = PBCDnumber[tmpInt++] << 4;
  tmpChar >>= 4;
		*decimalNumber += tmpChar * PBCD::raise(10, --len);
	} while (len);
 return 0;
}

//Fumction converts decimalNumber in to BCDnumber, where is len length of characters used in PBCDnumber
//BCDsize is used to set trailing zeors it is in bytes
// Example(decimalNumber = 1, BCDsize = 2, result is 00000000 00000001) 
//On success returns 0 on failure -1  
int PBCD::Int2PBCD(unsigned long long decimalNumber, unsigned char *PBCDnumber, int *len, int BCDsize)
{unsigned char tmpChar;
 unsigned long long tmpInt = decimalNumber;
 *len = 0;

 //Count digits 
 do
 {decimalNumber /= 10;
  (*len)++;
 } while (decimalNumber);
 decimalNumber = tmpInt;
 //Set len to first larger even number
 if ((*len) % 2) 
  (*len)++;
 tmpInt = (*len) / 2 - 1;

 //Requested length is smaller than calculated
 //0 is default value
 if ((BCDsize)&&(tmpInt > --BCDsize))
  return -1;

 //Set counter so BCD has trailing zeros
 if (tmpInt < BCDsize)
  tmpInt = BCDsize; 

 *len = tmpInt + 1; 

 //Pack decimal
 do  
 {
  PBCDnumber[tmpInt] = decimalNumber % 10;
  decimalNumber /= 10;
  tmpChar = (decimalNumber % 10) << 4;
  PBCDnumber[tmpInt--] |= tmpChar;    
  decimalNumber /= 10;
 } 
 while (-1 != tmpInt); 

 return 0;
}

