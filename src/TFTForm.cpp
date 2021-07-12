#include "TFTForm.h"
#include "ArduinoJson.h"
#include "TFTLayout.h"
#include "SPIFFS.h"

const char keys[3][42] = {
  {'a','b','c','d','e','f',
   'g','h','i','j','k','l',
   'm','n','o','p','q','r',
   's','t','u','v','w','x',
   'y','z',130,131,132,133,
   ',','.','-','_','/','@',
   30,26,' ',' ',31,28},
  {'A','B','C','D','E','F',
   'G','H','I','J','K','L',
   'M','N','O','P','Q','R',
   'S','T','U','V','W','X',
   'Y','Z',127,128,129,133,
   ',','.','-','_','/','@',
   30,27,' ',' ',31,28},
  {'0','1','2','3','4','5',
   '6','7','8','9',';',':',
   '!','?','#',34,39,92,
   '(','[','{','}',']',')',
   '+','*','=','$','%','@',
   '|',134,'^','&','<','>',
   29,26,' ',' ',31,28}
};

const uint16_t color_palette[64] = {
  0x0000,0x4820,0xa800,0xf800,0x02c0,0x52a0,0xaaa0,0xfa80,
  0x0560,0x5541,0xb520,0xfd40,0x0fc0,0x5fe0,0xa7e0,0xffe1,
  0x000a,0x500a,0xa80a,0xf80a,0x02ea,0x52aa,0xaaac,0xfa8b,
  0x058a,0x554c,0xad4b,0xfd2c,0x07ec,0x57eb,0xb7e9,0xffcc,
  0x0016,0x5015,0xb014,0xf815,0x02d6,0x52b5,0xb295,0xfa94,
  0x0574,0x4d75,0xad54,0xfd73,0x07d5,0x5ff6,0xaff4,0xfff5,
  0x001f,0x501f,0xa81f,0xf81f,0x02be,0x5abf,0xaabf,0xfabf,
  0x053f,0x557f,0xad7f,0xfd9f,0x07df,0x5fdf,0xafff,0xffff
};

const char * tft_inputtypes[] = {"text","password","number","check","select","color","float"};

uint16_t convertColor(uint8_t r, uint8_t g, uint8_t b) {
  r = r/8;
  g = g/4;
  b = b/8;
  uint16_t tftColor = r*2048+g*32+b;
  return tftColor;
}

//encode extra character from unicode for display
String TFTForm::encodeUnicode(String text, bool intern){
  String res = "";
  uint8_t i = 0;
  char c;
  while (i<text.length()) {
    c=text[i];
    if (c==195) { //UTF8 characters German Umlaute
      i++;
      if (intern) {
        switch (text[i]) {
          case 164: c=132; break; //ä
          case 182: c=148; break; //ö
          case 188: c=129; break; //ü
          case 159: c=224; break; //ß
          case 132: c=142; break; //Ä
          case 150: c=153; break; //Ö
          case 156: c=154; break; //Ü
          default: c=0;
        }
      } else {
        switch (text[i]) {
          case 164: c=130; break; //ä
          case 182: c=131; break; //ö
          case 188: c=132; break; //ü
          case 159: c=133; break; //ß
          case 132: c=127; break; //Ä
          case 150: c=128; break; //Ö
          case 156: c=129; break; //Ü
          default: c=0;
        }
      }
    } else if (c == 194) { //UTF8 char for Degrees symbol
      i++;
      if (_font == NULL){
        if (text[i] == 176) c=247; else c=0;
      } else {
        if (text[i] == 176) c=134; else c=0;
      }
    } else if (c > 128) { //normal characters unchanged
      c=0;
    }
    if (c>0) res.concat(c);
    i++;
  }
  return res;
}

//convert keyboard entered text into UTF8
String TFTForm::fromKeyboard(String text) {
  String res = "";
  char c;
  for (uint8_t i=0; i<text.length(); i++) {
    c = text[i];
    if (c > 126){ //it is a special character
      if (_font == NULL) {
        if (c==247) {
          res.concat("°");
        } else {
          switch (c) {
            case 132: res.concat("ä"); break;
            case 148: res.concat("ö"); break;
            case 129: res.concat("ü"); break;
            case 224: res.concat("ß"); break;
            case 142: res.concat("Ä"); break;
            case 153: res.concat("Ö"); break;
            case 154: res.concat("Ü"); break;
          }
        }
      } else {
        if (c==134) {
          res.concat("°");
        } else {
          switch (c) {
            case 130: res.concat("ä"); break;
            case 131: res.concat("ö"); break;
            case 132: res.concat("ü"); break;
            case 133: res.concat("ß"); break;
            case 127: res.concat("Ä"); break;
            case 128: res.concat("Ö"); break;
            case 129: res.concat("Ü"); break;
          }
        }
      }
    } else { //normales Zeichen
      res.concat(c);
    }
  }
  return res;
}



TFTForm::TFTForm(Adafruit_ILI9341 * tft, const GFXfont * font){
  _tft = tft;
  _font = font;
}
//load form descriptions
void TFTForm::setDescription(String parameter){
  _count = 0;
  addDescription(parameter);
}
//Add extra descriptions
void TFTForm::addDescription(String parameter){
  DeserializationError error;
  const int capacity = JSON_ARRAY_SIZE(TFT_MAXVALUES)
  + TFT_MAXVALUES*JSON_OBJECT_SIZE(50);
  Serial.printf("Capacity %i\n",capacity );
  //StaticJsonDocument<capacity> doc;

  DynamicJsonDocument doc(capacity);

  char tmp[40];
  error = deserializeJson(doc,parameter);
  if (error ) {
    Serial.println(parameter);
    Serial.print("JSON Description: ");
    Serial.println(error.c_str());
  } else {
    JsonArray array = doc.as<JsonArray>();
    uint8_t j = 0;
    for (JsonObject obj : array) {
      if (_count<TFT_MAXVALUES) {
        _description[_count].optionCnt = 0;
        _description[_count].readonly = false;
        if (obj.containsKey("readonly")) _description[_count].readonly = obj["readonly"];
        if (obj.containsKey("name")) strlcpy(_description[_count].name,obj["name"],15);
        if (obj.containsKey("label")) strlcpy(_description[_count].label,obj["label"],40);
        if (obj.containsKey("type")) {
          if (obj["type"].is<char *>()) {
            uint8_t t = 0;
            strlcpy(tmp,obj["type"],30);
            Serial.println(tmp);
            while ((t<TFT_INPUTTYPES) && (strcmp(tmp,tft_inputtypes[t])!=0)) t++;
            if (t>TFT_INPUTTYPES) t = 0;
            _description[_count].type = t;
          } else {
            _description[_count].type = obj["type"];
          }
        } else {
          _description[_count].type = TFT_INPUTTEXT;
        }
        _description[_count].max = (obj.containsKey("max"))?obj["max"] :100;
        _description[_count].min = (obj.containsKey("min"))?obj["min"] : 0;
        if (obj.containsKey("default")) {
          strlcpy(tmp,obj["default"],30);
          _values[_count] = String(tmp);
        } else {
          _values[_count]="0";
        }
        if (obj.containsKey("options")) {
          JsonArray opt = obj["options"].as<JsonArray>();
          j = 0;
          for (JsonObject o : opt) {
            if (j<TFT_MAXOPTIONS) {
              _description[_count].options[j] = o["v"].as<String>();
              _description[_count].labels[j] = o["l"].as<String>();
            }
            j++;
          }
          _description[_count].optionCnt = opt.size();
        }
      }
      _count++;
    }
    Serial.println("Description done");

  }
};

//read configuration from file
boolean TFTForm::readConfig(const char * filename){
  String line,name,value;
  uint8_t pos;
  int16_t index;
  if (!SPIFFS.exists(filename)) {
    //if configfile does not exist write default values
    writeConfig(filename);
  }
  File f = SPIFFS.open(filename,"r");
  if (f) {
    Serial.println("Read configuration");
    uint32_t size = f.size();
    while (f.position() < size) {
      line = f.readStringUntil(10);
      pos = line.indexOf('=');
      name = line.substring(0,pos);
      value = line.substring(pos+1);
      index = findName(name.c_str());
      if (!(index < 0)) {
        _values[index] = value;
        if (_description[index].type == TFT_INPUTPASSWORD) {
          Serial.printf("%s=*************\n",_description[index].name);
        } else {
          Serial.println(line);
        }
      }
    }
    f.close();
    return true;
  } else {
    Serial.println("Cannot read configuration");
    return false;
  }
}
//read configuration from default file
boolean TFTForm::readConfig(){
  return readConfig(TFT_CONFFILE);
}
//write configuration to file
boolean TFTForm::writeConfig(const char * filename){
  File f = SPIFFS.open(filename,"w");
  if (f) {
    for (uint8_t i = 0; i<_count; i++){
      f.printf("%s=%s\n",_description[i].name,_values[i].c_str());
    }
    return true;
  } else {
    Serial.println("Cannot write configuration");
    return false;
  }

}
//write configuration to default file
boolean TFTForm::writeConfig(){
  return writeConfig(TFT_CONFFILE);
}
//delete configuration file
boolean TFTForm::deleteConfig(const char * filename){
  return remove(filename);
}
//delete default configutation file
boolean TFTForm::deleteConfig(){
  return deleteConfig(TFT_CONFFILE);
}
void TFTForm::showForm(bool delbtn){
  _mode = MODE_FORM;
  _delbtn = delbtn;
  showFormIntern();
}

//Display the form
void TFTForm::showFormIntern(){
  _tft->fillScreen(TFT_BACKGROUND);
  for (uint8_t i = 0; i< _count;i++){
    showEntry(i);
  }
  _editMode = EDT_NONE;
  if (_delbtn) {
    showRoundedBox(5,300,70,20,"Speichern",tft_styles[TFTSTYLEBUTTON],true);
    showRoundedBox(85,300,70,20,"Löschen",tft_styles[TFTSTYLEBUTTON],true);
    showRoundedBox(165,300,70,20,"Abbruch",tft_styles[TFTSTYLEBUTTON],true);
  }else{
    showRoundedBox(20,300,90,20,"Speichern",tft_styles[TFTSTYLEBUTTON],true);
    showRoundedBox(130,300,90,20,"Abbruch",tft_styles[TFTSTYLEBUTTON],true);
  }
  _formActive = true;
}
//Display a confiramtion dialog
void TFTForm::showDialog(String message){
  _mode = MODE_DIALOG;
  _tft->fillScreen(TFT_BACKGROUND);
  _editMode = EDT_NONE;
  showMessage(10,10,220,280,message);
  showRoundedBox(20,300,90,20,"JA",tft_styles[TFTSTYLEBUTTON],true);
  showRoundedBox(130,300,90,20,"NEIN",tft_styles[TFTSTYLEBUTTON],true);
  _formActive = true;
}

//Display a list of objects to edit
void TFTForm::showList(uint8_t count,String label,bool addbtn, bool delbtn){
  _listCount = count;
  _addbtn = addbtn;
  _delbtn = delbtn;
  _listLabel = label;
  _mode = MODE_LIST;
  _editMode = EDT_NONE;
  _listOffset = 0;
  showSelectionList();
  _formActive = true;
}

void TFTForm::showSelectionList() {
  _tft->fillScreen(TFT_BACKGROUND);
  _tft->setTextWrap(false);
  showBox(0,0,240,20,_listLabel,false,tft_styles[TFTSTYLELISTLABEL],true,false);
  if (_onListEntry) {
    String label;
    uint8_t i = 0;
    do {
      label = _onListEntry(i + _listOffset);
      if (label != "") showBox(0,i*20+20,240,20,label,false,tft_styles[TFTSTYLEOPTIONS],true,false);
      if ((i<13) && (label != "")) _tft->drawLine(0,i*20+39,240,i*20+39,ILI9341_BLACK);
      i++;
    } while ((label != "") && (i < 14));
  }
  _tft->fillRect(0,300,240,20,0xc638);
  if (_addbtn) {
    _tft->fillTriangle(15,304,45,304,30,316,ILI9341_BLUE);
    _tft->fillTriangle(195,316,225,316,210,304,ILI9341_BLUE);
    showRoundedBox(62,300,56,20,"Fertig",tft_styles[TFTSTYLEBUTTON],true);
    showRoundedBox(122,300,56,20,"Neu",tft_styles[TFTSTYLEBUTTON],true);
  } else {
    _tft->fillTriangle(25,304,55,304,40,316,ILI9341_BLUE);
    _tft->fillTriangle(185,316,215,316,200,304,ILI9341_BLUE);
    showRoundedBox(90,300,60,20,"Fertig",tft_styles[TFTSTYLEBUTTON],true);
  }
}

void TFTForm::handleClick(int16_t x, int16_t y) {
  //get the line
  if (y < 0) y=0;
  if (y > 320) y=320;
  if (x < 0) x=0;
  if (x > 240) x=240;
  switch (_editMode) {
    case EDT_NONE: formClick(x,y); break;
    case EDT_KEYBOARD: kbdClick(x,y); break;
    case EDT_COLOR: colorClick(x,y); break;
    case EDT_NUMBER: numberClick(x,y); break;
    case EDT_SELECTION: selectionClick(x,y); break;
  }
}

//Get results as a JSON string
String TFTForm::getResults(){
  char buffer[2000];
  StaticJsonDocument<1000> doc;
  for (uint8_t i = 0; i<_count; i++) {
    switch (_description[i].type) {
      case TFT_INPUTPASSWORD :
      case TFT_INPUTSELECT :
      case TFT_INPUTTEXT : doc[_description[i].name] = _values[i]; break;
      case TFT_INPUTCOLOR :
      case TFT_INPUTCHECKBOX :
      case TFT_INPUTNUMBER : doc[_description[i].name] = _values[i].toInt(); break;
      case TFT_INPUTFLOAT : doc[_description[i].name] = _values[i].toFloat(); break;

    }
  }
  uint16_t n = serializeJson(doc,buffer);
  buffer[n] = 0;
  return String(buffer);
}

//set values from a JSON string
void TFTForm::setValues(String json){
  DeserializationError error;
  char buffer[200];
  char name[20];
  int ival;
  float fval;
  StaticJsonDocument<1000> doc;
  error = deserializeJson(doc,json);
  if (error ) {
    Serial.println(json);
    Serial.print("JSON Values: ");
    Serial.println(error.c_str());
  } else {
    for (uint8_t i = 0; i<_count; i++) {
      strlcpy(name,_description[i].name,20);
      if (doc.containsKey(name)){
        switch (_description[i].type) {
          case TFT_INPUTPASSWORD :
          case TFT_INPUTSELECT :
          case TFT_INPUTTEXT : strlcpy(buffer,doc[name],200);
            _values[i] = String(buffer);
            break;
          case TFT_INPUTCOLOR :
          case TFT_INPUTCHECKBOX :
          case TFT_INPUTNUMBER : ival=doc[name]; _values[i] = String(ival); break;
          case TFT_INPUTFLOAT : fval = doc[name]; _values[i] = String(fval); break;

        }
      }
    }
  }
}

//register onSave callback
void TFTForm::registerOnSave(void (*callback)(String results)){
  _onSave = callback;
}
//register onCancel callback
void TFTForm::registerOnCancel(void (*callback)()){
  _onCancel = callback;
}

//register onCancel callback
void TFTForm::registerOnDelete(void (*callback)(uint8_t index)){
  _onDelete = callback;
}

//register onListEntry callback
void TFTForm::registerOnListEntry(String (*callback)(uint8_t index)){
  _onListEntry = callback;
}
//register onEntryEdit callback
void TFTForm::registerOnEntryEdit(String (*callback)(uint8_t index, bool add)){
  _onEntryEdit = callback;
}
//register onEntryDone callback
void TFTForm::registerOnEntryDone(void (*callback)(uint8_t index, String data)){
  _onEntryDone = callback;
}


//set labels and options for selection
void TFTForm::setOptions(const char name[], uint8_t optionsCount, String options[]){
  int8_t index = findName(name);
  if (index < 0) return;
  for (uint8_t i = 0;i<optionsCount;i++) {
    addOption(index, options[i]);
  }
}
//find entry by name
int8_t TFTForm::findName(const char name[]) {
  int8_t result = _count-1;
  while ((result >= 0) && (strcmp(name,_description[result].name)!= 0)) result--;
  return result;
}

//add an option to the optionlist
void TFTForm::addOption(const char name[], String option) {
  int8_t index = findName(name);
  if (index < 0) return;
  addOption(index,option);
}

//add an option to the optionlist
void TFTForm::addOption(uint8_t index, String option) {
  if (index > _count) return;
  TFT_DESCRIPTION * d = &_description[index];
  if (d->optionCnt < (TFT_MAXOPTIONS)) {
    Serial.print("Add Option ");
    Serial.println(option);
    int8_t p = option.indexOf("=");
    if (p<0) {
      d->options[d->optionCnt] = option;
      d->labels[d->optionCnt] = option;
    } else {
      d->options[d->optionCnt] = option.substring(0,p);
      d->labels[d->optionCnt] = option.substring(p+1);
    }
    d->optionCnt ++;
  }

}
//clear option list
void TFTForm::clearOptions(const char name[]) {
  int8_t index = findName(name);
  if (index > 0) _description[index].optionCnt = 0;
}


void TFTForm::formClick(int16_t x, int16_t y) {
  uint8_t line = y/20;
  if ((_mode == MODE_FORM) || (_mode == MODE_LISTFORM)){
    if (line < _count) {
      if (!_description[line].readonly) {
        switch (_description[line].type) {
          case TFT_INPUTPASSWORD :
          case TFT_INPUTTEXT : startEditKbd(line);
            break;
          case TFT_INPUTCOLOR : startEditColor(line);
            break;
          case TFT_INPUTNUMBER :
          case TFT_INPUTFLOAT  : startEditNumber(line);
            break;
          case TFT_INPUTSELECT  : startEditSelection(line);
            break;
          case TFT_INPUTCHECKBOX : _values[line]=(_values[line]=="0")?"1":"0";
             _tft->fillRect(0,line*20,240,20,TFT_BACKGROUND);
             showEntry(line);
             break;
        }
      }
    }
  }
  if (_mode == MODE_LIST) {
    if ((line > 0)&&(line < 15)) {
      if (_onEntryEdit) {
        _listEdt = line-1+_listOffset;
        if (_listEdt < _listCount) {
          String val = _onEntryEdit(_listEdt,false);
          if (val == "~~close~~") {
            if (_onCancel) _onCancel();
            _formActive = false;
          } else {
            if ((val=="") || (val[0]= '{') ) {
              if (val != "") setValues(val);
              _mode = MODE_LISTFORM;
              showFormIntern();
            }
          }
        }
      }

    }

  }
  if (line == 15) {
    switch (_mode) {
      case MODE_FORM: if (_delbtn) {
          if ((x<80) && (_onSave)) _onSave(getResults());
          if ((x>79) && (x<160) && (_onDelete)) _onDelete(0);
          if ((x>159) && (_onCancel)) _onCancel();
        } else {
          if ((x<120) && (_onSave)) _onSave(getResults());
          if ((x>119) && (_onCancel)) _onCancel();
        }
        _formActive = false;
        break;
      case MODE_DIALOG: if ((x<120) && (_onSave)) _onSave("YES");
        if ((x > 119) && (_onCancel)) _onCancel();
        _formActive = false;
        break;
      case MODE_LIST: if (_addbtn) {
          if ((x<60) && (_listCount > _listOffset+15)) {
            _listOffset += 15;
            showSelectionList();
          }
          if ((x>59) && (x<120)) {
            if (_onCancel) _onCancel();
            _formActive = false;
          }
          if ((x>119) && (x<180) && (_onEntryEdit) ) {
            String val = (_onEntryEdit(_listEdt,true));
            if ((val=="") || (val[0]= '{')) {
              if (val != "") setValues(val);
              _listEdt = _listCount-1;
              _mode = MODE_LISTFORM;
              showFormIntern();
            }
          }
          if ((x>180) && (_listOffset > 0)) {
            _listOffset -= 15;
            showSelectionList();
          }
        } else {
          if ((x<80) && (_listCount > _listOffset+15)) {
            _listOffset += 15;
            showSelectionList();
          }
          if ((x>79) && (x<160)) {
            if (_onCancel) _onCancel();
            _formActive = false;
          }
          if ((x>160) && (_listOffset > 0)) {
            _listOffset -= 15;
            showSelectionList();
          }
        }
        break;
      case MODE_LISTFORM: if (_delbtn) {
          if ((x<80) && (_onEntryDone)) _onEntryDone(_listEdt,getResults());
          if ((x>79) && (x<160) && (_onDelete)) {
            _onDelete(_listEdt);
            showSelectionList();
          }
        } else {
          if ((x<120) && (_onEntryDone)) {
            _onEntryDone(_listEdt,getResults());
            showSelectionList();
          }
          if ((x>119) && (_onCancel)) _onCancel();
        }
        _mode = MODE_LIST;
        showSelectionList();
        break;
    }
  }
}

String TFTForm::getValue(const char name[]){
  uint8_t index;
  index = findName(name);
  if (index < 0) {
    return "";
  } else {
    return _values[index];
  }
}

void TFTForm::closeForm() {
  _formActive = false;
}
void TFTForm::kbdClick(int16_t x, int16_t y){
  uint8_t r,c,index;
  char ch;
  if (y>40) { //below input area
    //wir ermitteln die Zeile und Spalte der Taste
    r = (y-40) / 40;
    c = (x) / 40;
    //index into keys array
    index = r*6+c;
    Serial.printf("x = %i, y = %i, index = %i\n",x,y,index);
    //Wert der berührten Taste
    ch = keys[_kbdlevel][index];
    if (ch > 31) {
      // above 31 are valid characters to add to input text
      if (_font == NULL) {
        switch (ch) {
          case 130: ch=132; break; //ä
          case 131: ch=148; break; //ö
          case 132: ch=129; break; //ü
          case 133: ch=224; break; //ß
          case 127: ch=142; break; //Ä
          case 128: ch=153; break; //Ö
          case 129: ch=154; break; //Ü
          case 134: ch=247; break; //°
        }
      }
      _editTmp += ch; //und geben den neuen Text aus
      updateEditKbd();
    } else {
      //we have a control key
      switch (ch) {
        case 26: _kbdlevel = 1; //switch to capitals
          showEditKbd();
          break;
        case 27: _kbdlevel = 0; //switch to normal
          showEditKbd();
          break;
        case 29: _kbdlevel = 0; //switch to normal
          showEditKbd();
          break;
        case 30: _kbdlevel = 2; //switch to signs and numbers
          showEditKbd();
          break;
        case 31: _editTmp.remove(_editTmp.length()-1); //delete rightmost character
          updateEditKbd();
          break;
        case 28: endEdit(fromKeyboard(_editTmp)); //switch keyboard off
          break;
      }
    }
  }
}

void TFTForm::colorClick(int16_t x, int16_t y){
  if ((x>40) && (x<200) && (y > 80) && (y<240)) {
    uint16_t ix=((y-80)/20) * 8 + (x-40)/20;
    _editTmp = String(color_palette[ix]);
    updateEditColor();
  }
  if ((x>70) && (x<170) && (y > 280) && (y<310)) endEdit(_editTmp);
}

void TFTForm::numberClick(int16_t x, int16_t y){
  bool isFloat = (_description[_editLine].type == TFT_INPUTFLOAT);
  uint8_t r,c;
  int8_t num;
  boolean update = true;
  if ((y>80) && (y <240) && (x > 40) && (x < 200)) { //num pad area
    r = (y-80) / 40;
    c = (x-40) / 40;
    if ((c < 3) && (r < 3)) {
      num = (2-r) * 3 + 1 + c;
      _editTmp += char(48+num);
    } else if ((r == 3) && (c<3)) {
      if (c == 0) {
        if (isFloat && (_editTmp.indexOf('.') < 0)) _editTmp += ".";
      } else if (c == 1){
        _editTmp += "0";
      } else {
        if (_editTmp.length()==0) _editTmp="-";
      }
    } else {
      if (r<2) {
        _editTmp.remove(_editTmp.length()-1);
      } else {
        endEdit(_editTmp);
        update = false;
      }
    }
    if (update) updateEditNumber();
  }
}

void TFTForm::selectionClick(int16_t x, int16_t y){
  uint8_t cnt = _description[_editLine].optionCnt;
  uint8_t sl = y/20;
  if (sl == 15) {
    if ((x<120) && (cnt > _selectionOffset+15)) {
      _selectionOffset += 15;
      showEditSelection();
    }
    if ((x>119) && (_selectionOffset > 0)) {
      _selectionOffset -= 15;
      showEditSelection();
    }
  } else {
    if (sl< cnt) {
      endEdit(_description[_editLine].options[sl+_selectionOffset]);
    }
  }
}

void TFTForm::showEntry(uint8_t index){
  _tft->setTextWrap(false);
  Serial.println(_description[index].label);
  showBox(0,index * 20,115,20,_description[index].label,true,tft_styles[TFTSTYLELABEL],true,false);
  switch (_description[index].type) {
    case TFT_INPUTTEXT :
      showBox(125,index*20,110,20,_values[index].c_str(),false,tft_styles[TFTSTYLEINBOX],true,false);
      break;
    case TFT_INPUTPASSWORD :
      showBox(125,index*20,110,20,_values[index].c_str(),false,tft_styles[TFTSTYLEINBOX],true,true);
      break;
    case TFT_INPUTFLOAT :
    case TFT_INPUTNUMBER :
      showBox(125,index*20,110,20,_values[index].c_str(),false,tft_styles[TFTSTYLEINBOX],true,false);
      break;
    case TFT_INPUTCHECKBOX : showCheckbox(index,125,(_values[index].toInt()!=0),tft_styles[TFTSTYLEINBOX]);
      break;
    case TFT_INPUTSELECT :
      showBox(125,index*20,110,20,getOption(index,_values[index]).c_str(),false,tft_styles[TFTSTYLEINBOX],true,false);
      break;
    case TFT_INPUTCOLOR : showColorBox(index,125,_values[index].toInt());
      break;
  }
}


void TFTForm::showColorBox(uint8_t line, uint16_t position, uint16_t color) {
  Serial.print("Farbe = ");
  Serial.println(color);
  _tft->fillRect(position, line*20+1,60,18,color);
  _tft->drawRect(position, line*20+1,60,18,ILI9341_BLACK);
}

void TFTForm::showEditKbd() {
  for (uint8_t i = 1; i<8; i++) {
    for (uint8_t j = 0; j<6; j++) {
      if ((i==7) && ((j==2) || (j == 3))) {
        if (j==2) showRoundedBox(j*40+1,i*40+1,78,38," ",tft_styles[TFTSTYLEKBDKEY],false);
      } else {
        showRoundedBox(j*40+1,i*40+1,38,38,String(keys[_kbdlevel][(i-1)*6+j]),tft_styles[TFTSTYLEKBDKEY],false);
      }
    }
  }
}

void TFTForm::showEditColor() {
  for (uint8_t y =0; y<8; y++) {
    for (uint8_t x = 0; x<8; x++){
      _tft->fillRect(x*20+40,y*20+80,20,20,color_palette[y*8+x]);
      _tft->drawRect(x*20+40,y*20+80,20,20,ILI9341_WHITE);
    }
  }
  showRoundedBox(70,280,100,30,"Fertig",tft_styles[TFTSTYLEBUTTON],true);
}

void TFTForm::showEditNumber() {
  for (uint8_t i = 0; i<3; i++) {
    for (uint8_t j = 0; j<3; j++) {
      showRoundedBox(40+i*40+1,80+j*40+1,38,38,String(char((2-j)*3+i+49)),tft_styles[TFTSTYLEKBDKEY],false);
    }
  }
  showRoundedBox(41,201,38,38,".",tft_styles[TFTSTYLEKBDKEY],false);
  showRoundedBox(81,201,38,38,"0",tft_styles[TFTSTYLEKBDKEY],false);
  showRoundedBox(121,201,38,38,"-",tft_styles[TFTSTYLEKBDKEY],false);
  showRoundedBox(161,81,38,78,String(char(31)),tft_styles[TFTSTYLEKBDKEY],false);
  showRoundedBox(161,161,38,78,String(char(28)),tft_styles[TFTSTYLEKBDKEY],false);
}

void TFTForm::showEditSelection() {
  uint8_t sl;
  uint8_t cnt = _description[_editLine].optionCnt;
  _tft->fillScreen(ILI9341_BLACK);
  for (uint8_t i = 0; i < 15; i++) {
    if ((i+_selectionOffset) < cnt) {
      sl = (_values[_editLine]==_description[_editLine].options[i+_selectionOffset])?TFTSTYLESELECTED:TFTSTYLEOPTIONS;
      showBox(0,i*20,240,20,_description[_editLine].labels[i+_selectionOffset],false,tft_styles[sl],true,false);
    }
  }
  _tft->fillRect(0,300,240,20,0xc638);
  _tft->fillTriangle(45,304,75,304,60,316,ILI9341_BLUE);
  _tft->fillTriangle(165,316,195,316,180,304,ILI9341_BLUE);
}

void TFTForm::startEditKbd(uint8_t line){
  _kbdlevel = 0;
  _editTmp = encodeUnicode(_values[line],true);
  _editLine = line;
  _editMode = EDT_KEYBOARD;
  _tft->fillScreen(ILI9341_BLACK);
  updateEditKbd();
  showEditKbd();
}

void TFTForm::startEditColor(uint8_t line){
  _editTmp = _values[line];
  _editLine = line;
  _editMode = EDT_COLOR;
  _tft->fillScreen(ILI9341_BLACK);
  updateEditColor();
  showEditColor();
}

void TFTForm::startEditNumber(uint8_t line){
  _editTmp = _values[line];
  _editLine = line;
  _editMode = EDT_NUMBER;
  _tft->fillScreen(ILI9341_BLACK);
  updateEditNumber();
  showEditNumber();
}

void TFTForm::startEditSelection(uint8_t line){
  _editTmp = _values[line];
  _editLine = line;
  _editMode = EDT_SELECTION;
  _selectionOffset = 0;
  _tft->fillScreen(ILI9341_BLACK);
  if (_description[line].optionCnt > 0){
    showEditSelection();
  } else {
    endEdit("");
  }
}

void TFTForm::updateEditKbd() {
  showBox(0,0,240,40,_editTmp,false,tft_styles[TFTSTYLEKBDINP],false,false);
}

void TFTForm::updateEditColor() {
  _tft->fillRect(70,40,100,30,_editTmp.toInt());
  _tft->drawRect(70,40,100,30,ILI9341_WHITE);
}

void TFTForm::updateEditNumber() {
  showBox(40,20,160,40,_editTmp,false,tft_styles[TFTSTYLEKBDINP],false,false);
}

void TFTForm::endEdit(String result){
  if (_editMode != EDT_NONE) {
    _values[_editLine] = result;
    _editMode = EDT_NONE;
    showFormIntern();
  }
}

//center text a height of 0 means center horizontal only
void TFTForm::alignText(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t alignment, String text, bool intFont){
  uint16_t w1,h1,xx;
  int16_t x1,y1,hx;
  _tft->getTextBounds(text,x,y,&x1,&y1,&w1,&h1);
  hx = y-y1;
  xx = 0;
  switch (alignment) {
    case EDT_ALIGNCENTER : xx = x+(w-w1)/2; break;
    case EDT_ALIGNLEFT : xx = x+4; break;
    case EDT_ALIGNRIGHT : xx = x + w - w1-4;
  }
  if (intFont) {
    if (h > 0) {
      _tft->setCursor(xx,y+(h-8)/2);
    } else {
      _tft->setCursor(xx,y);
    }
  } else {
    if (h > 0) {
      _tft->setCursor(xx,y+(h-hx)/2+hx);
    } else {
      _tft->setCursor(xx,y);
    }
  }
  _tft->print(text);
}

//show a rounded box with centered text with given style
void TFTForm::showRoundedBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, String text,TFTSTYLE style, bool unicode){
  if (style.font) _tft->setFont(style.font); else _tft->setFont(_font);
  _tft->setTextColor(style.color);
  _tft->fillRoundRect(x,y,w,h,6,style.fill);
  _tft->drawRoundRect(x,y,w,h,6,style.border);
  _tft->drawRoundRect(x+1,y+1,w-2,h-2,5,style.border);
  if (unicode) text = encodeUnicode(text, (style.font==NULL));
  alignText(x,y,w,h,style.alignment,text,(style.font==NULL));
}

void TFTForm::showBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, String text, bool withoutBox, TFTSTYLE style,bool unicode, bool password){
  if (style.font) _tft->setFont(style.font); else _tft->setFont(_font);
  _tft->setTextColor(style.color);
  if (!withoutBox) {
    _tft->fillRect(x,y,w,h,style.fill);
    _tft->drawRect(x,y,w,h,style.border);
  }
  if (unicode) text = encodeUnicode(text, (style.font==NULL));
  if (password) {
    alignText(x,y,w,h,style.alignment,"********",(style.font==NULL));
  } else {
    alignText(x,y,w,h,style.alignment,text,(style.font==NULL));
  }
}

void TFTForm::showMessage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, String text){
  int16_t pos;
  _tft->setFont(_font);
  _tft->setTextColor(ILI9341_BLACK);
  _tft->setTextSize(1);
  String txt = encodeUnicode(text,(_font == NULL));
  uint8_t lines = (txt.length()+24) / 20;
  int16_t ys=h/2-(lines*20)/2;
  while (txt.length() > 20) {
    pos = txt.indexOf(" ",16);
    if ((pos<0)||(pos>26)) pos=24;
    alignText(0,y+ys,240,20,EDT_ALIGNCENTER,txt.substring(0,pos),(_font==NULL));
    ys+=20;
    txt = txt.substring(pos+1);
  }
  alignText(0,y+ys,240,20,EDT_ALIGNCENTER,txt,(_font==NULL));
}

void TFTForm::showCheckbox(uint8_t line, uint16_t position, bool checked, TFTSTYLE style) {
  _tft->fillRect(position, line*20+1,18,18,style.fill);
  _tft->drawRect(position, line*20+1,18,18,style.border);
  _tft->drawRect(position+1, line*20+2,16,16,style.border);
  if (checked) {
    _tft->drawLine(position+1, line*20+2,position+17, line*20+18,style.border);
    _tft->drawLine(position+1, line*20+3,position+16, line*20+18,style.border);
    _tft->drawLine(position+2, line*20+2,position+17, line*20+17,style.border);
    _tft->drawLine(position+1, line*20+18,position+17, line*20+2,style.border);
    _tft->drawLine(position+2, line*20+18,position+17, line*20+3,style.border);
    _tft->drawLine(position+1, line*20+17,position+16, line*20+2,style.border);
  }
}

bool TFTForm::isActive() {
  return _formActive;
}

String TFTForm::getOption(uint8_t index, String val) {
  uint8_t i = 0;
  TFT_DESCRIPTION * d = &_description[index];
  while ((i<d->optionCnt) && (d->options[i] != val)) i++;
  if (i<d->optionCnt) {
    return d->labels[i];
  } else {
    return "unknown";
  }
}

void TFTForm::setListCount(uint16_t count){
  _listCount = count;
}
