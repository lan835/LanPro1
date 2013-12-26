/*******************************************************************************

  Jasper (JPEG2000) library intefrace for Borland CBuilder

  Programmer: Dima V. Fedorov Levit <dimin@dimin.net> <http://www.dimin.net/>

  History:
    21/07/2003 14:47:53 - First Creation

  Ver 1

*******************************************************************************/
#pragma package(smart_init)

#include "jasper.h"
#include <vcl.h>

#include <jasper/jasper.h>

#pragma hdrstop

//******************************************************************************
// Miscellaneous functions
//******************************************************************************

void borjas_setPaletteToGrey(Graphics::TBitmap *pBitmap)
{

  LOGPALETTE *pal;
  HPALETTE hpal;
  int i;
  int PaletteSize; //Size of the palette buffer

  pBitmap->PixelFormat = pf8bit;

  pal = NULL;
  try
  {
    PaletteSize = sizeof(TLogPalette) + (sizeof(TPaletteEntry) * 255);
    pal = (LOGPALETTE *) malloc(PaletteSize);

    pal->palVersion = 0x300;
    pal->palNumEntries = 256;

    // Set the colors to their Gray indexes
    for (i = 0 ; i <= 255; i++)
    {
      pal->palPalEntry[i].peRed   = (char) i;
      pal->palPalEntry[i].peGreen = (char) i;
      pal->palPalEntry[i].peBlue  = (char) i;
      pal->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
    }

    hpal = CreatePalette(pal);
    if (hpal != 0) pBitmap->Palette = hpal;

    free(pal);
  }
  catch ( ... )
  {

  }
}

//******************************************************************************
// JasPer interface 
//******************************************************************************

int ReadJPEG2000Image(char *fileName, Graphics::TBitmap *Bmp, void *Params)
{
  long y;
  jas_image_t *jp2_image;
  jas_matrix_t *pixels[4];
  jas_stream_t *jp2_stream;
  register unsigned long i, x;
  int components[4];
  unsigned long number_components;
  unsigned int channel_scale[4], maximum_component_depth=8;

  int_fast32_t height;
  int_fast32_t width;
  uchar *px;
  uchar pixel[4];
  uint dim;

  // Initialize JPEG 2000 API.
  if (jas_init()) return -1;

  if( !(jp2_stream = jas_stream_fopen( fileName, "rb" )) ) return -1;
  jp2_stream->openmode_ = JAS_STREAM_READ | JAS_STREAM_BINARY;

  jp2_image = jas_image_decode(jp2_stream, 0, 0);

  if (jp2_image == (jas_image_t *) NULL) {
    (void) jas_stream_close(jp2_stream);
    return -1; //UnableToDecodeImageFile
  }

  switch (jas_clrspc_fam(jas_image_clrspc(jp2_image)))
  {
    case JAS_CLRSPC_FAM_RGB:
    {
      components[0]=jas_image_getcmptbytype(jp2_image,
        JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_R));
      components[1]=jas_image_getcmptbytype(jp2_image,
        JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_G));
      components[2]=jas_image_getcmptbytype(jp2_image,
        JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_B));
      if ((components[0] < 0) || (components[1] < 0) || (components[2] < 0))
        {
          (void) jas_stream_close(jp2_stream);
          jas_image_destroy(jp2_image);
          return -1; //MissingImageChannel
        }
      number_components=3;
      components[3]=jas_image_getcmptbytype(jp2_image,
        JAS_IMAGE_CT_COLOR(JAS_IMAGE_CT_OPACITY));
      if (components[3] > 0)
        {
          //image->matte=True; // dima - what a hell is this???
          number_components++;
        }
      break;
    }
    case JAS_CLRSPC_FAM_GRAY:
    {
      components[0]=jas_image_getcmptbytype(jp2_image,
        JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_GRAY_Y));
      if (components[0] < 0)
        {
          (void) jas_stream_close(jp2_stream);
          jas_image_destroy(jp2_image);
          return -1; //MissingImageChannel
        }
      number_components=1;
      break;
    }
    case JAS_CLRSPC_FAM_YCBCR:
    default:
    {
      (void) jas_stream_close(jp2_stream);
      jas_image_destroy(jp2_image);
      return -1; //ColorspaceModelIsNotSupported
    }
  }

  //-------------------------------------------------------------------
  // Now create the QImage 
  //-------------------------------------------------------------------

  height = jas_image_height(jp2_image); 
  width  = jas_image_width(jp2_image); 
  
  // test for components geometry
  for (i=0; i < (ulong) number_components; i++)
  {
    if ((jas_image_cmptwidth(jp2_image,components[i]) != width) ||
        (jas_image_cmptheight(jp2_image,components[i]) != height) ||
        (jas_image_cmpttlx(jp2_image,components[i]) != 0) ||
        (jas_image_cmpttly(jp2_image,components[i]) != 0) ||
        (jas_image_cmpthstep(jp2_image,components[i]) != 1) ||
        (jas_image_cmptvstep(jp2_image,components[i]) != 1) ||
        (jas_image_cmptsgnd(jp2_image,components[i]) != FALSE))
      {
        (void) jas_stream_close(jp2_stream);
        jas_image_destroy(jp2_image);
        return -1; // IrregularChannelGeometryNotSupported
      }
  }

  // now get the max component depth
  for (i=0; i < (ulong) number_components; i++)
  {
    maximum_component_depth = max((unsigned int) jas_image_cmptprec(jp2_image,components[i]),
      maximum_component_depth);
    pixels[i] = jas_matrix_create(1, (unsigned int) width);
    if (pixels[i] == (jas_matrix_t *) NULL)
      {
        jas_image_destroy(jp2_image);
        return -1; //MemoryAllocationFailed
      }
  }

  if (number_components == 1)
  {
    Bmp->PixelFormat = pf8bit;

    borjas_setPaletteToGrey(Bmp);
  }
  else
  {
    Bmp->PixelFormat = pf24bit;
  }

  Bmp->Width = width;
  Bmp->Height = height;

  for (i=0; i < (ulong) number_components; i++)
  {
    channel_scale[i]=1;
    if (jas_image_cmptprec(jp2_image,components[i]) < 16)
      channel_scale[i]=
        (1 << (16-jas_image_cmptprec(jp2_image,components[i])))+1;
  }

  //-------------------------------------------------------------------
  // Now copy image data
  //-------------------------------------------------------------------
  
  if (number_components == 1)
  for (y=0; y < (long) height; y++)
  {
    for (i=0; i < number_components; i++)
      jas_image_readcmpt(jp2_image, components[i], 0, y, width, 1, pixels[i]);

    px = (uchar *) Bmp->ScanLine[y];

    for (x=0; x < (ulong) width; x++)
      px[x] = (unsigned char) jas_matrix_getv(pixels[0],x)*channel_scale[0];
  } // for x fir 1 component

  if (number_components != 1)
  for (y=0; y < (long) height; y++)
  {
    for (i=0; i < number_components; i++)
      jas_image_readcmpt(jp2_image, components[i], 0, y, width, 1, pixels[i]);

    px = (uchar *) Bmp->ScanLine[y];

    for (x=0; x < (ulong) width; x++)
    {
      px[x*3+2] = (jas_matrix_getv(pixels[0],x)*channel_scale[0]);
      px[x*3+1] = (jas_matrix_getv(pixels[1],x)*channel_scale[1]);
      px[x*3+0] = (jas_matrix_getv(pixels[2],x)*channel_scale[2]);
    } // for x
  }

  // clean up all mess in memory
  (void) jas_stream_close(jp2_stream);
  jas_image_destroy(jp2_image);
  for (i=0; i < (ulong) number_components; i++) jas_matrix_destroy(pixels[i]);

  return 0;
}


int WriteJPEG2000Image(Graphics::TBitmap *Bmp, char *fileName, void *Params)
{
  char options[MaxTextExtent];
  double rate;
  int format;
  ulong y;
  jas_image_cmptparm_t component_info;
  jas_image_t *jp2_image;
  jas_matrix_t *pixels[4];
  jas_stream_t *jp2_stream;
  register unsigned long i, x;
  int status;
  unsigned long number_components;

  uchar *px;


  // Intialize JasPer JPEG 2000 API
  jas_init();

  if ( !(jp2_stream = jas_stream_fopen(fileName, "w+b")) ) return -1;

  if (Bmp->PixelFormat == pf8bit)
    number_components = 1;
  else 
    number_components = 3;

  jp2_image=jas_image_create0();
  if (jp2_image == (jas_image_t *) NULL) return -1; //UnableToCreateImage


  for (i=0; i < (ulong) number_components; i++)
  {
    (void) memset((void *) &component_info,0,sizeof(jas_image_cmptparm_t));
    component_info.tlx=0;
    component_info.tly=0;
    component_info.hstep=1;
    component_info.vstep=1;
    component_info.width=(unsigned int) Bmp->Width;
    component_info.height=(unsigned int) Bmp->Height;
    component_info.prec=(unsigned int) 8;//img.depth() <= 8 ? 8 : 16;
    component_info.sgnd=FALSE;
    if (jas_image_addcmpt(jp2_image,i,&component_info))
    {
      jas_image_destroy(jp2_image);
      return -1; //UnableToCreateImageComponent
    }
  }

  if (number_components == 1)
  {
    //  sRGB Grayscale
    jas_image_setclrspc(jp2_image, JAS_CLRSPC_SGRAY);
    jas_image_setcmpttype(jp2_image, 0, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_GRAY_Y));
  }
  else
  {
    //  sRGB
    jas_image_setclrspc(jp2_image, JAS_CLRSPC_SRGB);
    jas_image_setcmpttype(jp2_image, 0, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_R));
    jas_image_setcmpttype(jp2_image, 1, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_G));
    jas_image_setcmpttype(jp2_image, 2, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_B));
    if (number_components == 4 )
      jas_image_setcmpttype(jp2_image, 3, JAS_IMAGE_CT_COLOR(JAS_IMAGE_CT_OPACITY));
  }


  //  Convert to JPEG 2000 pixels.
  for (i=0; i < (ulong) number_components; i++)
  {
    pixels[i] = jas_matrix_create(1, (unsigned int) Bmp->Width);
    if (pixels[i] == (jas_matrix_t *) NULL)
      {
        for (x=0; x < i; x++) jas_matrix_destroy(pixels[x]);
        jas_image_destroy(jp2_image);
        return -1; //MemoryAllocationFailed
      }
  }
  
  
  // now copy the data itself
  if (number_components == 1)
  for (y=0; y < (ulong) Bmp->Height; y++)
  {
    px = (uchar *) Bmp->ScanLine[y];

    for (x=0; x < (ulong) Bmp->Width; x++)
        jas_matrix_setv(pixels[0], x, px[x] );

    for (i=0; i < (ulong) number_components; i++)
      jas_image_writecmpt(jp2_image, i, 0, y, Bmp->Width, 1, pixels[i]);
  }  // end of byte data copy

  // now copy the data itself
  if (number_components != 1)
  for (y=0; y < (ulong) Bmp->Height; y++)
  {
    px = (uchar *) Bmp->ScanLine[y];

    for (x=0; x < (ulong) Bmp->Width; x++)
    {
      jas_matrix_setv(pixels[0], x, px[x*3+2] );
      jas_matrix_setv(pixels[1], x, px[x*3+1] );
      jas_matrix_setv(pixels[2], x, px[x*3+0] );
    }

    for (i=0; i < (ulong) number_components; i++)
      jas_image_writecmpt(jp2_image, i, 0, y, Bmp->Width, 1, pixels[i]);
  }  // end of byte data copy

  format = jas_image_fmtfromname(fileName);
  
  rate=1.0;
  if (Bmp->Width*Bmp->Height > 2500)
    {
      double alpha, header_size, number_pixels, target_size;
      int depth=8;

      alpha=115-100;
      rate=100.0/(alpha*alpha);
      header_size=550.0;
      header_size+=(number_components-1)*142;
      if (Bmp->PixelFormat != pf8bit) depth = 24;
      number_pixels=(double) Bmp->Width*Bmp->Height*(depth/8)*number_components;
      target_size=(number_pixels*rate)+header_size;
      rate=target_size/number_pixels;
    }
  
  sprintf(options, "rate=%g", rate);
  status = jas_image_encode(jp2_image, jp2_stream, format, options);
  
  // clean up our mess
  (void) jas_stream_close(jp2_stream);       //write stream to file
  for (i=0; i < (ulong) number_components; i++) jas_matrix_destroy(pixels[i]);
  jas_image_destroy(jp2_image);

  if (status == -1) return -2; //UnableToEncodeImageFile

  return 0;
}









