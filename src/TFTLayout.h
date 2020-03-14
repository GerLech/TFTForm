#include "TFTKeyboardfont.h"
#include "TFTLabelFont.h"

#define TFTSTYLELABEL 0
#define TFTSTYLEINBOX 1
#define TFTSTYLEBUTTON 2
#define TFTSTYLEKBDKEY 3
#define TFTSTYLEKBDINP 4
#define TFTSTYLEOPTIONS 5
#define TFTSTYLESELECTED 6
#define TFTSTYLELISTLABEL 7


const TFTSTYLE tft_styles[] = {
  { //TFTSTYLELABEL
    .fill = ILI9341_WHITE,
    .border = ILI9341_BLACK,
    .color = ILI9341_BLACK,
    .alignment = EDT_ALIGNRIGHT,
    .font = NULL
  },
  { //TFTSTYLEINBOX
    .fill = ILI9341_WHITE,
    .border = ILI9341_BLACK,
    .color = ILI9341_BLACK,
    .alignment = EDT_ALIGNLEFT,
    .font = NULL
  },
  { //TFTSTYLEBUTTON
    .fill = ILI9341_DARKGREY,
    .border = ILI9341_BLACK,
    .color = ILI9341_WHITE,
    .alignment = EDT_ALIGNCENTER,
    .font = NULL
  },
  { //TFTSTYLEKBDKEY
    .fill = ILI9341_WHITE,
    .border = ILI9341_BLUE,
    .color = ILI9341_BLACK,
    .alignment = EDT_ALIGNCENTER,
    .font = &TFTKeyboardfont
  },
  { //TFTSTYLEKBDINP
    .fill = ILI9341_WHITE,
    .border = ILI9341_BLUE,
    .color = ILI9341_BLACK,
    .alignment = EDT_ALIGNLEFT,
    .font = NULL
  },
  { //TFTSTYLEOPTIONS
    .fill = ILI9341_WHITE,
    .border = 0xdefb,
    .color = ILI9341_DARKGREY,
    .alignment = EDT_ALIGNLEFT,
    .font = NULL
  },
  { //TFTSTYLESELECTED
    .fill = 0xceff,
    .border = 0x2bb,
    .color = ILI9341_BLACK,
    .alignment = EDT_ALIGNLEFT,
    .font = NULL
  },
  { //TFTSTYLELISTLABEL
    .fill = ILI9341_NAVY,
    .border = ILI9341_NAVY,
    .color = ILI9341_LIGHTGREY,
    .alignment = EDT_ALIGNCENTER,
    .font = &TFTLabelFont
  }
};
