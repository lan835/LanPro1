/*******************************************************************************

  Jasper (JPEG2000) library intefrace for Borland CBuilder

  Programmer: Dima V. Fedorov Levit <dimin@dimin.net> <http://www.dimin.net/>

  History:
    21/07/2003 14:47:53 - First Creation

  Ver 1

******************************************************************************/

#ifndef JASPER_H
#define JASPER_H

#include <vcl.h>

#define MaxTextExtent 2053

#ifndef max
#define max(a, b)  (((a) > (b)) ? (a) : (b))
#endif

int ReadJPEG2000Image(char *fileName, Graphics::TBitmap *Bmp, void *Params);
int WriteJPEG2000Image(Graphics::TBitmap *Bmp, char *fileName, void *Params);

#endif




