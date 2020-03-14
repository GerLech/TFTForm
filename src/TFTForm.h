#ifndef TFTForm_h
#define TFTForm_h

#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include "Arduino.h"

//maximum number of parameters
#define TFT_MAXVALUES 15

//maximum number of options per parameters
#define TFT_MAXOPTIONS 50

//name for the config file
#define TFT_CONFFILE "/TFTConf.conf"

#define TFT_INPUTTEXT 0
#define TFT_INPUTPASSWORD 1
#define TFT_INPUTNUMBER 2
#define TFT_INPUTCHECKBOX 3
#define TFT_INPUTSELECT 4
#define TFT_INPUTCOLOR 5
#define TFT_INPUTFLOAT 6
//count of types
#define TFT_INPUTTYPES 7


#define TFT_BACKGROUND ILI9341_LIGHTGREY
#define TFT_FONTCOLOR ILI9341_BLACK

#define EDT_ALIGNCENTER 0
#define EDT_ALIGNLEFT 1
#define EDT_ALIGNRIGHT 2

#define EDT_NONE 0
#define EDT_KEYBOARD 1
#define EDT_NUMBER 2
#define EDT_COLOR 3
#define EDT_SELECTION 4

#define MODE_FORM 0
#define MODE_DIALOG 1
#define MODE_LIST 2
#define MODE_LISTFORM 3

typedef
struct {
  uint16_t fill;
  uint16_t border;
  uint16_t color;
  uint8_t alignment;
  const GFXfont *font;
} TFTSTYLE;

//data structure to hold the parameter Description
typedef //Struktur eines Datenpakets
struct  {
  char name[15];
  char label[30];
  uint8_t type;
  int min;
  int max;
  uint8_t optionCnt;
  bool readonly;
  String options[TFT_MAXOPTIONS];
  String labels[TFT_MAXOPTIONS];
} TFT_DESCRIPTION;

class TFTForm {
protected:
  void showEntry(uint8_t index);
  void showCheckbox(uint8_t line, uint16_t position, bool checked, TFTSTYLE style);
  void showColorBox(uint8_t line, uint16_t position, uint16_t color);
  void showRoundedBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, String text,TFTSTYLE style, bool unicode);
  void showBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, String text, bool withoutBox, TFTSTYLE style, bool unicode, bool password = false);
  void showMessage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, String text);
  void showEditKbd();
  void showEditColor();
  void showEditNumber();
  void showEditSelection();
  void formClick(int16_t x, int16_t y);
  void kbdClick(int16_t x, int16_t y);
  void colorClick(int16_t x, int16_t y);
  void numberClick(int16_t x, int16_t y);
  void selectionClick(int16_t x, int16_t y);
  void startEditKbd(uint8_t line);
  void startEditColor(uint8_t line);
  void startEditNumber(uint8_t line);
  void startEditSelection(uint8_t line);
  void updateEditKbd();
  void updateEditColor();
  void updateEditNumber();
  void endEdit(String result);
  void alignText(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t alignment, String text, bool intFont);
  String fromKeyboard(String text);
  void showSelectionList();
  void showFormIntern();
  String encodeUnicode(String text, bool intern);
public:
  TFTForm(Adafruit_ILI9341 * tft, const GFXfont * font);
  //load form descriptions
  void setDescription(String parameter);
  //Add extra descriptions
  void addDescription(String parameter);
  //read configuration from file
  boolean readConfig(const char *  filename);
  //read configuration from default file
  boolean readConfig();
  //write configuration to file
  boolean writeConfig(const char *  filename);
  //write configuration to default file
  boolean writeConfig();
  //delete configuration file
  boolean deleteConfig(const char *  filename);
  //delete default configutation file
  boolean deleteConfig();
  //Display the form
  void showForm(bool delbtn = false);
  //Display a confirmation dialog
  void showDialog(String message);
  //Display a list of objects to edit
  void showList(uint8_t count,String label,bool addbtn = false, bool delbtn = false);
  //Set a new Listcount
  void setListCount(uint16_t count);
  //Handle a click event from Touch-Screen
  void handleClick(int16_t x, int16_t y);
  //Get results as a JSON string
  String getResults();
  //register onSave callback
  void registerOnSave(void (*callback)(String results));
  //register onCancel callback
  void registerOnCancel(void (*callback)());
  //register onDelete callback
  void registerOnDelete(void (*callback)(uint8_t index));
  //register onListEntry callback
  void registerOnListEntry(String (*callback)(uint8_t index));
  //register onEntryEdit callback
  void registerOnEntryEdit(String (*callback)(uint8_t index, bool add));
  //register onEntryDone callback
  void registerOnEntryDone(void (*callback)(uint8_t index, String data));
  //set labels and options for selection
  void setOptions(const char name[],  uint8_t optionsCount, String options[]);
  //add an option to the optionlist
  void addOption(const char name[], String option);
  void addOption(uint8_t index, String option);
  //clear option list
  void clearOptions(const char name[]);
  //set values from a JSON string
  void setValues(String json);
  //find entry by name
  int8_t findName(const char name[]);
  bool isActive();
  //return option label for value
  String getOption(uint8_t index, String val);
private:
  String _values[TFT_MAXVALUES];
  char _buf[1000];
  bool _formActive; //true if form is shown and want touch events
  uint8_t _mode = MODE_FORM;
  uint8_t _count;
  uint8_t _editMode = EDT_NONE;
  uint8_t _editLine;
  uint8_t _kbdlevel;
  uint8_t _selectionOffset = 0;
  uint8_t _listOffset = 0;
  uint8_t _listCount;
  uint8_t _listEdt;
  String _listLabel;
  bool _delbtn;
  bool _addbtn;
  String _editTmp;
  String _pwdTmp;
  TFT_DESCRIPTION _description[TFT_MAXVALUES];
  Adafruit_ILI9341 * _tft;
  const GFXfont * _font;
  void(*_onSave)(String result) = NULL;
  void(*_onCancel)() = NULL;
  void(*_onDelete)(uint8_t index) = NULL;
  String (*_onListEntry)(uint8_t index) = NULL;
  String (*_onEntryEdit)(uint8_t index, bool add) = NULL;
  void (*_onEntryDone)(uint8_t index, String data) = NULL;
};
#endif
