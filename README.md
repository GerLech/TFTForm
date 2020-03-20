# 	TFTForm
This library works with a TFT-Color Display with Touch-Screen based on ILI9341. The display should have 240 x 320 pixels in portrait mode. It requires the Adafruit_ILI9341 and the ArduinoJson library. The library can be used to display editable forms and lists on the display. It offers different type of form fields like alphanumeric input, numeric intpu, check boxes, selection lists and color selection. The data exchange is handled wit JSO formatted strings.

## TFTForm(Adafruit_ILI9341 * tft, const GFXfont * font)
This is the constructor. We need a pointer on a Adafruit_ILI9341 instance and another pointer on a GFX font. The font pointer can be NULL. In this case the font used for headlines is the small internal fon with 5x7 pixels.

## void setDescription(String parameter)
This functionsetup the form. The parameter parameter discribes the form in a list of JSON objects. There are only 15 input fields allowed for one form. Every object in the list describes one input field. The objects have the following members
+ name  name of the field
+ label label of the field displayed lefthand should not have more than 20 characters
+ type  type of the input field. could be the name or the number. the following types are available

| Nr	| predefined const. | type name 	| description                |
| --- | ----------------- | ----------- | -------------------------- |
|  0	| TFT_INPUTTEXT     |	text        |	text input field           |
|  1	| TFT_INPUTPASSWORD |	password	  | password input field       |
|  2	| TFT_INPUTNUMBER	  | number	    | integer input field        |
|  3	| TFT_INPUTCHECKBOX |	check	      | check box                  |
|  4	| TFT_INPUTSELECT	  | select	    | selection list             |
|  5	| TFT_INPUTCOLOR	  | color	      | color selection            |
|  6	| TFT_INPUTFLOAT	  | float	      | floating point input field |

+ readonly if true the field will be displayed but cant be changed.

Some input fields have additional object members. Numeric input fields can set the minimum and maximum values with min and max.
The selection list has the member options which is a list of objects with the members v for the value and l for the label. Values and labels have to be strings. Options can be modified after construction of the form has finished. The maximum number of options is 50.

## void addDescription(String parameter)
This funktion allows to add more input fields as long as the maximum number of 15 does not exceede. The parameter has the same format as for setDescription.

## boolean readConfig(const char *  filename)
The function tries to open a file with the given filename. If the file exists in the SPIFFS, the current form will be filled with the values from the file. The format should be <name>=<value>. If the file does not exist, false will be returned

## boolean readConfig()
The same function as before, but the default filename /TFTConf.conf will be used.

## boolean writeConfig(const char *  filename)
The function saves the values of all fields in the format <name>=<value> into the SPIFFS. The parameter will be used as the filename. False will be returned, if an error occurs.
  
## boolean writeConfig()
The function saves the values of all fields in the format <name>=<value> into the SPIFFS. /TFTConf.conf will be used as the filename. False will be returned, if an error occurs.

## boolean deleteConfig(const char *  filename)
The file with the given filename will be removed from SPIFFS. False will be returned, if an error occurs.

## boolean deleteConfig()with 
The /TFTConf.conf will be removed from SPIFFS. False will be returned, if an error occurs.

## void showForm(bool delbtn = false)
The current form will be displayed and can be edited. In the bottommost line two buttons will be shown. One labeled "Speichern" for save and one labeled "Abbruch" for cancel. If the parameter delbtn is true, a third button labeled "Löschen" will be shown for delete. If "Speichern" will be clicked, the callback function onSave(String data) will be called. The parameter data contains the values from all form fields JSON formatted. If "Abbruch" will be clicked, the callback function onCancel() will be called. If the button "Löschen" will be clicked, the callback function onDelete(uint8_t index) will be called. The parameter index is allways 0. This parameter is required for lists.
If an input field will be clicked, the corresponding editor will be shown to allow changing the vallue of the field.

## void showDialog(String message)
A YES/NO dialog will be displayed together with a message given in the parameter string. If the YES button labeled with "Ja" will be clicked, the callback function onSave(String data) will be called with the constant value "YES" in the data parameter. If the NO button, labeld with "Nein" will be clicked, the callback function onCancel() will be called.

## void showList(uint8_t count,String label,bool addbtn = false, bool delbtn = false)
This function shows a list of strings. These stringsw are not managed inside the TFTForm class. Up to 255 entries can be handeled. The parameter count gets the number of entries to be displayed. The string in the parameter label will be displayed in the topmost line as a headline. In the bottommost line two tringles will be shown on the right and on the left. Clicking on this triangles allows to scroll the list if more than 14 lines are in the list. There is also a DONE button labeled "Fertig" in the bottommost line. Clicking this button calls the callback function onCancel(). If the parameter addbtn is true a second button labele "Neu" will be displayed. A new entry can be created with this button. 
Since the data for the list are not stored inside the class, callback functions will be called for data exchange. When a line should be displayed, the text to be displayed will be read by the callback function String onListEntry(uint8_t index) will be called. The parameter indexdefines which element in the list should be displayed.
If an entry in the list will be clicked, the callback function String onEntryEdit(uint8_t index, bool add) will be called. The edit form previously defined with the setDescription function, will be displayed and filled with the data returned by the callback function. The data are JSON formatted. If the parameter delbtn was true, the edit form gets also a delete button labeled with "Löschen".
If the edit form will be closed by the save button, labeled "Speichern", the callback function onEntryDone(uint8_t index, String data) will be called. The parameter index is the position in the list. The parameter data contains the values of all fields in the edit form. The data are JSON formatted. If the editi form will be closed by the cancel button labeled "Abbruch", no callback function will be called. Finally if there is a delete button labeled "Löschen" and this button will be clicked to close the edit form, the callback function onDeleteEntry(uint8_t index) will be called. The parameter index defines which entry has to be deleted from the list.

## void setListCount(uint16_t count)
Since data for the list comes from outside, it is necessary to keep the number of list entries current. This can be done with this function. Everytime when the number of entries changes, this function should be called with the new number.

## void handleClick(int16_t x, int16_t y)
The TFTForm class needs to know, when the touch screen will be clicked on a certzain position. Everytime such an event occurs, this function should be called with the x and y position, where the screen was touched.

## String getResults()
If a form will be displayed, this function returns the values of all fields in the form. The data are JSON formatted.

## void registerOnSave(void (\*callback)(String results))
This function register a callback function onSave(String results). This callback function will be called if a form will be closed with the save button or a YES/NO dialog will be closed by the yes button. The parameter data contains the values from all fields in the form. The string is JSON formatted.

## void registerOnCancel(void (\*callback)())
This function register a callback function onCancel(). This callback function will be called if a form will be closed by the cancel button or if a YES/NO dialog will be closed by the no button or a list will be closed by the done button.

## void registerOnDelete(void (\*callback)(uint8_t index))
This function register a callback function onDelete(uint8_t index). This function will be called if a form will be closed by the delete button. The parameter index gives the position, if the form was called inside a list.

## void registerOnListEntry(String (\*callback)(uint8_t index))
This function register a callback function String onListEntry(uint8_t index). This callback function will be called whenever a line in a list  should be displayed. The function should return the display string. The parameter index ist the position in the list.

## void registerOnEntryEdit(String (\*callback)(uint8_t index, bool add))
This function register a callback function String onEntryEdit(uint8_t index, bool add). This callback function will be called whenever an entry in a list will be clicked to be edited. The function should return the JSON formatted data to fill the form. The parameter index is the position inside the list.

## void registerOnEntryDone(void (\*callback)(uint8_t index, String data))
This function register a callback function onEntryDone(uint8_t index, String data). This callback function will be called whenever a form to edit a list entry will be closed by the save button. The paramter index is the position within the list and the parameter data contains the JSON formatted values from the form.

## void setOptions(const char name[],  uint8_t optionsCount, String options[])
With this function more options can be added to a selection list. The parameter name is the field name of the selection list. The parameter optionsCount is the number of options to be added. The last parameter options is a list of strings to hold the new options. if a string contains an equal sign, the string before the equal sign is used as value and the string after the equal sign is used as the label. If there is no equal sign, the string will be used for both, value and label.

## void addOption(const char name[], String option)
This function adds one new option to a selection list. The parameter name is the field name of the selection list. The parameter option is a string to hold the new option. if the string contains an equal sign, the string before the equal sign is used as value and the string after the equal sign is used as the label. If there is no equal sign, the string will be used for both, value and label. 

## void addOption(uint8_t index, String option)
In difference to the previous function, this function needs the position of the selection list field within the form.

## void clearOptions(const char name[])
This function clears all options from the selection list of the input field named name.

## void setValues(String json)
This function fills all input fields of the current form with values. The parameter json contains a JSON object. The name of the input fi8eld is used as the key.

## int8_t findName(const char name[])
This function returns the position of an input field named name within the current form. If no field was found, the function returns -1.

## bool isActive()
Theis function returns true, if a form, a list or a dialog will be displayed. It can be used to decide if a touchscreen event should be forwarded to the TFTForm instance or not.

## String getOption(uint8_t index, String val)
This function returns the label for the option within the selection list on position index in the form, which has the value val. If no option was found, the function returns -1
