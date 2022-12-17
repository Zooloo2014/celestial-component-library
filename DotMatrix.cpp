//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <stdlib.h>
#include "DotMatrix.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TDotMatrix *)
{
        new TDotMatrix(NULL);
}
//---------------------------------------------------------------------------
__fastcall TDotMatrix::~TDotMatrix(void)
{
  delete Buffer;
}
//---------------------------------------------------------------------------
__fastcall TDotMatrix::TDotMatrix(TComponent* Owner)
        : TGraphicControl(Owner)
{
  Visible = false;

  Buffer = new TPicture();
  Buffer->Bitmap = new Graphics::TBitmap();

  FScrollOffset = 0;
  FSmallScroll = 0;
  FMatrixWidth = 6;
  FMatrixHeight = 8;

  Width = 50;
  Height = 60;

  FColor = clRed;
  FElementColor = clGray;
  FBackgroundColor = clBlack;
  FDisplayWidth = 1;
  FCharIndex = 1;
  FElementWidth = 4;
  FElementHeight = 4;
  FHorizontalGap = 1;
  FVerticalGap = 1;
  FSmoothScrolling = False;


  // Non-standard characters

  Lookup[30][0] = BIN2VAL (0,0,0,0,0,0,0,0); // Down arrow
  Lookup[30][1] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[30][2] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[30][3] = BIN2VAL (1,1,1,1,1,0,0,0);
  Lookup[30][4] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[30][5] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[30][6] = BIN2VAL (0,0,0,0,0,0,0,0);

  Lookup[31][0] = BIN2VAL (0,0,0,0,0,0,0,0); // Up arrow
  Lookup[31][1] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[31][2] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[31][3] = BIN2VAL (1,1,1,1,1,0,0,0);
  Lookup[31][4] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[31][5] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[31][6] = BIN2VAL (0,0,0,0,0,0,0,0);


  // Standard ASCII characters

  Lookup[32][0] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[32][1] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[32][2] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[32][3] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[32][4] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[32][5] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[32][6] = BIN2VAL (0,0,0,0,0,0,0,0);

  Lookup[33][0] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[33][1] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[33][2] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[33][3] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[33][4] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[33][5] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[33][6] = BIN2VAL (0,0,1,0,0,0,0,0);

  Lookup[34][0] = BIN2VAL (0,1,0,1,0,0,0,0);
  Lookup[34][1] = BIN2VAL (0,1,0,1,0,0,0,0);
  Lookup[34][2] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[34][3] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[34][4] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[34][5] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[34][6] = BIN2VAL (0,0,0,0,0,0,0,0);

  Lookup[35][0] = BIN2VAL (0,1,0,1,0,0,0,0);
  Lookup[35][1] = BIN2VAL (1,1,1,1,1,0,0,0);
  Lookup[35][2] = BIN2VAL (0,1,0,1,0,0,0,0);
  Lookup[35][3] = BIN2VAL (0,1,0,1,0,0,0,0);
  Lookup[35][4] = BIN2VAL (0,1,0,1,0,0,0,0);
  Lookup[35][5] = BIN2VAL (1,1,1,1,1,0,0,0);
  Lookup[35][6] = BIN2VAL (0,1,0,1,0,0,0,0);

  Lookup[36][0] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[36][1] = BIN2VAL (0,1,1,1,1,0,0,0);
  Lookup[36][2] = BIN2VAL (1,0,1,0,0,0,0,0);
  Lookup[36][3] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[36][4] = BIN2VAL (0,0,1,0,1,0,0,0);
  Lookup[36][5] = BIN2VAL (1,1,1,1,0,0,0,0);
  Lookup[36][6] = BIN2VAL (0,0,1,0,0,0,0,0);

  Lookup[37][0] = BIN2VAL (1,1,0,0,0,0,0,0);
  Lookup[37][1] = BIN2VAL (1,1,0,0,1,0,0,0);
  Lookup[37][2] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[37][3] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[37][4] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[37][5] = BIN2VAL (1,0,0,1,1,0,0,0);
  Lookup[37][6] = BIN2VAL (0,0,0,1,1,0,0,0);

  Lookup[38][0] = BIN2VAL (0,1,1,0,0,0,0,0);
  Lookup[38][1] = BIN2VAL (1,0,0,1,0,0,0,0);
  Lookup[38][2] = BIN2VAL (1,0,1,0,0,0,0,0);
  Lookup[38][3] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[38][4] = BIN2VAL (0,1,1,0,1,0,0,0);
  Lookup[38][5] = BIN2VAL (1,0,0,1,0,0,0,0);
  Lookup[38][6] = BIN2VAL (0,1,1,0,1,0,0,0);

  Lookup[39][0] = BIN2VAL (0,1,1,0,0,0,0,0);
  Lookup[39][1] = BIN2VAL (0,1,1,0,0,0,0,0);
  Lookup[39][2] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[39][3] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[39][4] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[39][5] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[39][6] = BIN2VAL (0,0,0,0,0,0,0,0);

  Lookup[40][0] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[40][1] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[40][2] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[40][3] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[40][4] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[40][5] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[40][6] = BIN2VAL (0,0,0,1,0,0,0,0);

  Lookup[41][0] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[41][1] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[41][2] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[41][3] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[41][4] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[41][5] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[41][6] = BIN2VAL (0,1,0,0,0,0,0,0);

  Lookup[42][0] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[42][1] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[42][2] = BIN2VAL (1,0,1,0,1,0,0,0);
  Lookup[42][3] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[42][4] = BIN2VAL (1,0,1,0,1,0,0,0);
  Lookup[42][5] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[42][6] = BIN2VAL (0,0,0,0,0,0,0,0);

  Lookup[43][0] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[43][1] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[43][2] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[43][3] = BIN2VAL (1,1,1,1,1,0,0,0);
  Lookup[43][4] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[43][5] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[43][6] = BIN2VAL (0,0,0,0,0,0,0,0);

  Lookup[44][0] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[44][1] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[44][2] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[44][3] = BIN2VAL (0,1,1,0,0,0,0,0);
  Lookup[44][4] = BIN2VAL (0,1,1,0,0,0,0,0);
  Lookup[44][5] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[44][6] = BIN2VAL (0,1,0,0,0,0,0,0);

  Lookup[45][0] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[45][1] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[45][2] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[45][3] = BIN2VAL (1,1,1,1,1,0,0,0);
  Lookup[45][4] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[45][5] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[45][6] = BIN2VAL (0,0,0,0,0,0,0,0);

  Lookup[46][0] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[46][1] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[46][2] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[46][3] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[46][4] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[46][5] = BIN2VAL (0,1,1,0,0,0,0,0);
  Lookup[46][6] = BIN2VAL (0,1,1,0,0,0,0,0);

  Lookup[47][0] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[47][1] = BIN2VAL (0,0,0,0,1,0,0,0);
  Lookup[47][2] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[47][3] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[47][4] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[47][5] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[47][6] = BIN2VAL (0,0,0,0,0,0,0,0);

  Lookup[48][0] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[48][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[48][2] = BIN2VAL (1,0,0,1,1,0,0,0);
  Lookup[48][3] = BIN2VAL (1,0,1,0,1,0,0,0);
  Lookup[48][4] = BIN2VAL (1,1,0,0,1,0,0,0);
  Lookup[48][5] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[48][6] = BIN2VAL (0,1,1,1,0,0,0,0);

  Lookup[49][0] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[49][1] = BIN2VAL (0,1,1,0,0,0,0,0);
  Lookup[49][2] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[49][3] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[49][4] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[49][5] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[49][6] = BIN2VAL (0,1,1,1,0,0,0,0);

  Lookup[50][0] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[50][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[50][2] = BIN2VAL (0,0,0,0,1,0,0,0);
  Lookup[50][3] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[50][4] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[50][5] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[50][6] = BIN2VAL (1,1,1,1,1,0,0,0);

  Lookup[51][0] = BIN2VAL (1,1,1,1,1,0,0,0);
  Lookup[51][1] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[51][2] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[51][3] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[51][4] = BIN2VAL (0,0,0,0,1,0,0,0);
  Lookup[51][5] = BIN2VAL (0,0,0,0,1,0,0,0);
  Lookup[51][6] = BIN2VAL (1,1,1,1,0,0,0,0);

  Lookup[52][0] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[52][1] = BIN2VAL (0,0,1,1,0,0,0,0);
  Lookup[52][2] = BIN2VAL (0,1,0,1,0,0,0,0);
  Lookup[52][3] = BIN2VAL (1,0,0,1,0,0,0,0);
  Lookup[52][4] = BIN2VAL (1,1,1,1,1,0,0,0);
  Lookup[52][5] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[52][6] = BIN2VAL (0,0,0,1,0,0,0,0);

  Lookup[53][0] = BIN2VAL (1,1,1,1,1,0,0,0);
  Lookup[53][1] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[53][2] = BIN2VAL (1,1,1,1,0,0,0,0);
  Lookup[53][3] = BIN2VAL (0,0,0,0,1,0,0,0);
  Lookup[53][4] = BIN2VAL (0,0,0,0,1,0,0,0);
  Lookup[53][5] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[53][6] = BIN2VAL (0,1,1,1,0,0,0,0);

  Lookup[54][0] = BIN2VAL (0,0,1,1,0,0,0,0);
  Lookup[54][1] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[54][2] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[54][3] = BIN2VAL (1,1,1,1,0,0,0,0);
  Lookup[54][4] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[54][5] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[54][6] = BIN2VAL (0,1,1,1,0,0,0,0);

  Lookup[55][0] = BIN2VAL (1,1,1,1,1,0,0,0);
  Lookup[55][1] = BIN2VAL (0,0,0,0,1,0,0,0);
  Lookup[55][2] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[55][3] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[55][4] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[55][5] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[55][6] = BIN2VAL (0,0,1,0,0,0,0,0);

  Lookup[56][0] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[56][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[56][2] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[56][3] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[56][4] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[56][5] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[56][6] = BIN2VAL (0,1,1,1,0,0,0,0);

  Lookup[57][0] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[57][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[57][2] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[57][3] = BIN2VAL (0,1,1,1,1,0,0,0);
  Lookup[57][4] = BIN2VAL (0,0,0,0,1,0,0,0);
  Lookup[57][5] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[57][6] = BIN2VAL (0,1,1,0,0,0,0,0);

  Lookup[65][0] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[65][1] = BIN2VAL (0,1,0,1,0,0,0,0);
  Lookup[65][2] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[65][3] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[65][4] = BIN2VAL (1,1,1,1,1,0,0,0);
  Lookup[65][5] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[65][6] = BIN2VAL (1,0,0,0,1,0,0,0);

  Lookup[66][0] = BIN2VAL (1,1,1,1,0,0,0,0);
  Lookup[66][1] = BIN2VAL (0,1,0,0,1,0,0,0);
  Lookup[66][2] = BIN2VAL (0,1,0,0,1,0,0,0);
  Lookup[66][3] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[66][4] = BIN2VAL (0,1,0,0,1,0,0,0);
  Lookup[66][5] = BIN2VAL (0,1,0,0,1,0,0,0);
  Lookup[66][6] = BIN2VAL (1,1,1,1,0,0,0,0);

  Lookup[67][0] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[67][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[67][2] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[67][3] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[67][4] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[67][5] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[67][6] = BIN2VAL (0,1,1,1,0,0,0,0);

  Lookup[68][0] = BIN2VAL (1,1,1,1,0,0,0,0);
  Lookup[68][1] = BIN2VAL (0,1,0,0,1,0,0,0);
  Lookup[68][2] = BIN2VAL (0,1,0,0,1,0,0,0);
  Lookup[68][3] = BIN2VAL (0,1,0,0,1,0,0,0);
  Lookup[68][4] = BIN2VAL (0,1,0,0,1,0,0,0);
  Lookup[68][5] = BIN2VAL (0,1,0,0,1,0,0,0);
  Lookup[68][6] = BIN2VAL (1,1,1,1,0,0,0,0);

  Lookup[69][0] = BIN2VAL (1,1,1,1,1,0,0,0);
  Lookup[69][1] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[69][2] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[69][3] = BIN2VAL (1,1,1,1,0,0,0,0);
  Lookup[69][4] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[69][5] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[69][6] = BIN2VAL (1,1,1,1,1,0,0,0);

  Lookup[70][0] = BIN2VAL (1,1,1,1,1,0,0,0);
  Lookup[70][1] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[70][2] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[70][3] = BIN2VAL (1,1,1,1,0,0,0,0);
  Lookup[70][4] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[70][5] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[70][6] = BIN2VAL (1,0,0,0,0,0,0,0);

  Lookup[71][0] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[71][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[71][2] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[71][3] = BIN2VAL (1,0,0,1,1,0,0,0);
  Lookup[71][4] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[71][5] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[71][6] = BIN2VAL (0,1,1,1,0,0,0,0);

  Lookup[72][0] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[72][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[72][2] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[72][3] = BIN2VAL (1,1,1,1,1,0,0,0);
  Lookup[72][4] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[72][5] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[72][6] = BIN2VAL (1,0,0,0,1,0,0,0);

  Lookup[73][0] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[73][1] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[73][2] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[73][3] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[73][4] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[73][5] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[73][6] = BIN2VAL (0,1,1,1,0,0,0,0);

  Lookup[74][0] = BIN2VAL (0,0,1,1,1,0,0,0);
  Lookup[74][1] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[74][2] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[74][3] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[74][4] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[74][5] = BIN2VAL (1,0,0,1,0,0,0,0);
  Lookup[74][6] = BIN2VAL (0,1,1,0,0,0,0,0);

  Lookup[75][0] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[75][1] = BIN2VAL (1,0,0,1,0,0,0,0);
  Lookup[75][2] = BIN2VAL (1,0,1,0,0,0,0,0);
  Lookup[75][3] = BIN2VAL (1,1,0,0,0,0,0,0);
  Lookup[75][4] = BIN2VAL (1,0,1,0,0,0,0,0);
  Lookup[75][5] = BIN2VAL (1,0,0,1,0,0,0,0);
  Lookup[75][6] = BIN2VAL (1,0,0,0,1,0,0,0);

  Lookup[76][0] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[76][1] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[76][2] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[76][3] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[76][4] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[76][5] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[76][6] = BIN2VAL (1,1,1,1,1,0,0,0);

  Lookup[77][0] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[77][1] = BIN2VAL (1,1,0,1,1,0,0,0);
  Lookup[77][2] = BIN2VAL (1,0,1,0,1,0,0,0);
  Lookup[77][3] = BIN2VAL (1,0,1,0,1,0,0,0);
  Lookup[77][4] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[77][5] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[77][6] = BIN2VAL (1,0,0,0,1,0,0,0);

  Lookup[78][0] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[78][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[78][2] = BIN2VAL (1,1,0,0,1,0,0,0);
  Lookup[78][3] = BIN2VAL (1,0,1,0,1,0,0,0);
  Lookup[78][4] = BIN2VAL (1,0,0,1,1,0,0,0);
  Lookup[78][5] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[78][6] = BIN2VAL (1,0,0,0,1,0,0,0);

  Lookup[79][0] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[79][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[79][2] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[79][3] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[79][4] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[79][5] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[79][6] = BIN2VAL (0,1,1,1,0,0,0,0);

  Lookup[80][0] = BIN2VAL (1,1,1,1,0,0,0,0);
  Lookup[80][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[80][2] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[80][3] = BIN2VAL (1,1,1,1,0,0,0,0);
  Lookup[80][4] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[80][5] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[80][6] = BIN2VAL (1,0,0,0,0,0,0,0);

  Lookup[81][0] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[81][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[81][2] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[81][3] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[81][4] = BIN2VAL (1,0,1,0,1,0,0,0);
  Lookup[81][5] = BIN2VAL (1,0,0,1,0,0,0,0);
  Lookup[81][6] = BIN2VAL (0,1,1,0,1,0,0,0);

  Lookup[82][0] = BIN2VAL (1,1,1,1,0,0,0,0);
  Lookup[82][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[82][2] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[82][3] = BIN2VAL (1,1,1,1,0,0,0,0);
  Lookup[82][4] = BIN2VAL (1,0,1,0,0,0,0,0);
  Lookup[82][5] = BIN2VAL (1,0,0,1,0,0,0,0);
  Lookup[82][6] = BIN2VAL (1,0,0,0,1,0,0,0);

  Lookup[83][0] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[83][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[83][2] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[83][3] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[83][4] = BIN2VAL (0,0,0,0,1,0,0,0);
  Lookup[83][5] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[83][6] = BIN2VAL (0,1,1,1,0,0,0,0);

  Lookup[84][0] = BIN2VAL (1,1,1,1,1,0,0,0);
  Lookup[84][1] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[84][2] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[84][3] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[84][4] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[84][5] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[84][6] = BIN2VAL (0,0,1,0,0,0,0,0);

  Lookup[85][0] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[85][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[85][2] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[85][3] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[85][4] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[85][5] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[85][6] = BIN2VAL (0,1,1,1,0,0,0,0);

  Lookup[86][0] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[86][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[86][2] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[86][3] = BIN2VAL (0,1,0,1,0,0,0,0);
  Lookup[86][4] = BIN2VAL (0,1,0,1,0,0,0,0);
  Lookup[86][5] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[86][6] = BIN2VAL (0,0,1,0,0,0,0,0);

  Lookup[87][0] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[87][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[87][2] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[87][3] = BIN2VAL (1,0,1,0,1,0,0,0);
  Lookup[87][4] = BIN2VAL (1,0,1,0,1,0,0,0);
  Lookup[87][5] = BIN2VAL (1,1,0,1,1,0,0,0);
  Lookup[87][6] = BIN2VAL (1,0,0,0,1,0,0,0);

  Lookup[88][0] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[88][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[88][2] = BIN2VAL (0,1,0,1,0,0,0,0);
  Lookup[88][3] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[88][4] = BIN2VAL (0,1,0,1,0,0,0,0);
  Lookup[88][5] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[88][6] = BIN2VAL (1,0,0,0,1,0,0,0);

  Lookup[89][0] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[89][1] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[89][2] = BIN2VAL (0,1,0,1,0,0,0,0);
  Lookup[89][3] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[89][4] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[89][5] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[89][6] = BIN2VAL (0,0,1,0,0,0,0,0);

  Lookup[90][0] = BIN2VAL (1,1,1,1,1,0,0,0);
  Lookup[90][1] = BIN2VAL (0,0,0,0,1,0,0,0);
  Lookup[90][2] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[90][3] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[90][4] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[90][5] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[90][6] = BIN2VAL (1,1,1,1,1,0,0,0);

  Lookup[91][0] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[91][1] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[91][2] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[91][3] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[91][4] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[91][5] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[91][6] = BIN2VAL (0,1,1,1,0,0,0,0);

  Lookup[92][0] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[92][1] = BIN2VAL (1,0,0,0,0,0,0,0);
  Lookup[92][2] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[92][3] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[92][4] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[92][5] = BIN2VAL (0,0,0,0,1,0,0,0);
  Lookup[92][6] = BIN2VAL (0,0,0,0,0,0,0,0);

  Lookup[93][0] = BIN2VAL (0,1,1,1,0,0,0,0);
  Lookup[93][1] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[93][2] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[93][3] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[93][4] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[93][5] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[93][6] = BIN2VAL (0,1,1,1,0,0,0,0);

  Lookup[94][0] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[94][1] = BIN2VAL (0,1,0,1,0,0,0,0);
  Lookup[94][2] = BIN2VAL (1,0,0,0,1,0,0,0);
  Lookup[94][3] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[94][4] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[94][5] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[94][6] = BIN2VAL (0,0,0,0,0,0,0,0);

  Lookup[95][0] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[95][1] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[95][2] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[95][3] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[95][4] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[95][5] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[95][6] = BIN2VAL (1,1,1,1,1,0,0,0);

  Lookup[96][0] = BIN2VAL (0,0,1,1,0,0,0,0);
  Lookup[96][1] = BIN2VAL (0,0,1,1,0,0,0,0);
  Lookup[96][2] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[96][3] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[96][4] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[96][5] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[96][6] = BIN2VAL (0,0,0,0,0,0,0,0);

  Lookup[123][0] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[123][1] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[123][2] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[123][3] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[123][4] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[123][5] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[123][6] = BIN2VAL (0,0,0,1,0,0,0,0);

  Lookup[124][0] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[124][1] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[124][2] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[124][3] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[124][4] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[124][5] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[124][6] = BIN2VAL (0,0,1,0,0,0,0,0);

  Lookup[125][0] = BIN2VAL (0,1,0,0,0,0,0,0);
  Lookup[125][1] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[125][2] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[125][3] = BIN2VAL (0,0,0,1,0,0,0,0);
  Lookup[125][4] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[125][5] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[125][6] = BIN2VAL (0,1,0,0,0,0,0,0);

  Lookup[126][0] = BIN2VAL (0,0,1,0,1,0,0,0);
  Lookup[126][1] = BIN2VAL (0,1,0,1,0,0,0,0);
  Lookup[126][2] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[126][3] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[126][4] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[126][5] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[126][6] = BIN2VAL (0,0,0,0,0,0,0,0);

  Lookup[127][0] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[127][1] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[127][2] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[127][3] = BIN2VAL (0,0,0,0,0,0,0,0);
  Lookup[127][4] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[127][5] = BIN2VAL (0,0,1,0,0,0,0,0);
  Lookup[127][6] = BIN2VAL (0,0,1,0,0,0,0,0);

  Visible = true;
}
//---------------------------------------------------------------------------
//namespace Dotmatrix
//{
//        void __fastcall PACKAGE Register()
//        {
//                 TComponentClass classes[1] = {__classid(TDotMatrix)};
//                 RegisterComponents("Celestial", classes, 0);
//        }
//}
//---------------------------------------------------------------------------
TColor __fastcall TDotMatrix::GetColor() {
  return FColor;
}
//---------------------------------------------------------------------------
void __fastcall TDotMatrix::SetColor(TColor AColor) {
  FColor = AColor;
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
TColor __fastcall TDotMatrix::GetElementColor() {
  return FElementColor;
}
//---------------------------------------------------------------------------
void __fastcall TDotMatrix::SetElementColor(TColor AColor) {
  FElementColor = AColor;
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
TColor __fastcall TDotMatrix::GetBackgroundColor() {
  return FBackgroundColor;
}
//---------------------------------------------------------------------------
void __fastcall TDotMatrix::SetBackgroundColor(TColor AColor) {
  FBackgroundColor = AColor;
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
AnsiString __fastcall TDotMatrix::GetText() {
  return FText;
}
//---------------------------------------------------------------------------
void __fastcall TDotMatrix::SetText(AnsiString AValue) {
  FText = AValue.UpperCase();
  ScrollReset();
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
int __fastcall TDotMatrix::GetDisplayWidth() {
  return FDisplayWidth;
}
//---------------------------------------------------------------------------
void __fastcall TDotMatrix::SetDisplayWidth(int AValue) {
  FDisplayWidth = AValue;
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
int __fastcall TDotMatrix::GetElementWidth() {
  return FElementWidth;
}
//---------------------------------------------------------------------------
void __fastcall TDotMatrix::SetElementWidth(int AValue) {
  FElementWidth = AValue;
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
int __fastcall TDotMatrix::GetElementHeight() {
  return FElementHeight;
}
//---------------------------------------------------------------------------
void __fastcall TDotMatrix::SetElementHeight(int AValue) {
  FElementHeight = AValue;
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
int __fastcall TDotMatrix::GetHorizontalGap() {
  return FHorizontalGap;
}
//---------------------------------------------------------------------------
void __fastcall TDotMatrix::SetHorizontalGap(int AValue) {
  FHorizontalGap = AValue;
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
int __fastcall TDotMatrix::GetVerticalGap() {
  return FVerticalGap;
}
//---------------------------------------------------------------------------
void __fastcall TDotMatrix::SetVerticalGap(int AValue) {
  FVerticalGap = AValue;
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
bool __fastcall TDotMatrix::GetStretch() {
  return FStretch;
}
//---------------------------------------------------------------------------
void __fastcall TDotMatrix::SetStretch(bool AValue) {
  FStretch = AValue;
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
bool __fastcall TDotMatrix::GetSmoothScrolling() {
  return FSmoothScrolling;
}
//---------------------------------------------------------------------------
void __fastcall TDotMatrix::SetSmoothScrolling(bool AValue) {
  FSmoothScrolling = AValue;
  ScrollReset();
  if (Visible) Paint();
}
//---------------------------------------------------------------------------
void __fastcall TDotMatrix::Paint(void) {
  static TRect Area;
  static TRect Blank;

  if (Visible) {
    Buffer->Bitmap->Height = Height;
    Buffer->Bitmap->Width = Width;

    int pWidth = FElementWidth;
    int pHeight = FElementHeight;
    int pHGap = FHorizontalGap;
    int pVGap = FVerticalGap;

    int ElemWide = pWidth + pHGap;
    int ElemHigh = pHeight + pVGap;
    int DigitWide = ElemWide * FMatrixWidth;


    if (FStretch) {
      double w = DigitWide * (FDisplayWidth * 1.0);
      double Factor = Width / w;
      pWidth = (int)pWidth * Factor;
      pHGap = (int)pHGap * Factor;

      double h = ElemHigh * FMatrixHeight;
      Factor = Height / h;
      pHeight = (int)pHeight * Factor;
      pVGap = (int)pVGap * Factor;

      ElemWide = pWidth + pHGap;
      ElemHigh = pHeight + pVGap;
      DigitWide = ElemWide * FMatrixWidth;
    }


    Blank.Top = 0;
    Blank.Left = 0;
    Blank.Right = Width;
    Blank.Bottom = Height;
    Buffer->Bitmap->Canvas->Brush->Color = FBackgroundColor;
    Buffer->Bitmap->Canvas->FillRect(Blank);

    int DigStartX = 1;
    int DigStartY = 1;

    int sl = FText.Length();
    int si = FCharIndex + FScrollOffset;


    for (int z = 0; z < FDisplayWidth; z++) {
      for (int y = 0; y < FMatrixHeight; y++) {
        unsigned char n = 128;

        if (y == (FMatrixHeight - 1)) // Last Element Row of display is blank...
          n = 255;

        if (z == 0)   // First character is partially-shown, if "smooth-scrolling"...
          if (FSmoothScrolling)
            n >>= FSmallScroll;


        // Draw the dot matrix character, or part of it
        //    if we're "smooth-scrolling"...

        for (int x = 0; x < FMatrixWidth; x++) {
          if (FSmoothScrolling)
            Area.Left = DigStartX + ((x * ElemWide) - (FSmallScroll * ElemWide));
          else
            Area.Left = DigStartX + (x * ElemWide);

          Area.Right = Area.Left + pWidth;
          Area.Top = DigStartY + (y * ElemHigh);
          Area.Bottom = Area.Top + pHeight;

          unsigned char k;
          if (si <= sl)   // si and sl are 1-based...
            k = FText[si];
          else
            k = ' ';

          unsigned char l = Lookup[k][y];
          if ((l & n) == n)
            Buffer->Bitmap->Canvas->Brush->Color = FColor;
          else
            Buffer->Bitmap->Canvas->Brush->Color = FElementColor;

          Buffer->Bitmap->Canvas->FillRect(Area);

          n >>= 1;
          if (n < 8)
            n = 255;
        }


        // Fill the fraction of a digit at end of display
        //    with blank Element Columns, if "smooth-scrolling"...

        int q = Area.Left;
        for (int x = 0; x < (FSmallScroll + 1); x++) {
          Area.Left = q + (x * ElemWide);
          Area.Right = Area.Left + pWidth;
          Area.Top = DigStartY + (y * ElemHigh);
          Area.Bottom = Area.Top + pHeight;
          Buffer->Bitmap->Canvas->Brush->Color = FElementColor;
          Buffer->Bitmap->Canvas->FillRect(Area);
        }
      }

      DigStartX += DigitWide;
      si++;
    }

    Canvas->Draw(0, 0, Buffer->Bitmap);
  }
}
//---------------------------------------------------------------------------
void __fastcall TDotMatrix::Scroll(void) {
  if (FSmoothScrolling) {
    FSmallScroll++;
    if (FSmallScroll > FMatrixWidth) {
      FSmallScroll = 1;
      FCharIndex++;
    }
  } else
    FScrollOffset++;

  Paint();
}
//---------------------------------------------------------------------------
void __fastcall TDotMatrix::ScrollReset(void) {
  FScrollOffset = 0;
  FSmallScroll = 0;
  FCharIndex = 1;
  Paint();
}
//---------------------------------------------------------------------------
void __fastcall TDotMatrix::AppendChar(unsigned char c) {
  char *Buffer;

  int i = FText.Length();

  if (c > 31)
    FText += c;
  else {
    if ((Buffer = (char*)malloc(i)) != NULL) {
      strcpy(Buffer, FText.c_str());

      Buffer[i] = c;
      Buffer[i + 1] = '\0';
      FText = Buffer;

      free(Buffer);
    }
  }
}
//---------------------------------------------------------------------------
int __fastcall TDotMatrix::GetStartingIndex(void) {
  return FCharIndex;
}
//---------------------------------------------------------------------------

