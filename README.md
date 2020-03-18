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
Es wird ein Dialogformular mit der im Parameter message angegebenen Nachricht angezeigt. Der Dialog hat zwei Knöpfe mit „Ja“ und „Nein“. Wird der Knopf „Ja“ an-geklickt, so wird die Callback-Funktion onSave(String data) aufgerufen und beim Knopf „Nein“ die Callback-Funktion onCancel(). Bei “Ja” enthält der Pa-rameter data den konstanten String „YES“.

## void showList(uint8_t count,String label,bool addbtn = false, bool delbtn = false)
Diese Funktion zeigt eine Liste von Strings an, die nicht in der Klasse TFTForm verwaltet wird. Es können daher bis zu 255 Elemente angezeigt werden. In der ers-ten Zeile wird der im Parameter label angegebene String als Überschrift ange-zeigt. In der letzten Zeile werden zwei blaue Dreiecke angezeigt, mit denen geblät-tert werden kann. Außerdem in der Mitte ein Knopf „Fertig“ mit dem die Bearbei-tung beendet werden kann. Ist der Parameter addbtn wahr, wird in der letzten Zeile auch ein Knopf „Neu“ angezeigt. Ist der Parameter delbtn wahr, wird im Bearbeitungsformular ein Knopf „Löschen“ angezeigt.
Die Anzeige und Änderung von Daten werden über Callback-Funktionen gesteuert. Diese Funktionen haben immer den Index des gewünschten Elements als Parame-ter. Wird die Liste neu angezeigt, so wird für jede Zeile die Callback-Funktion String onListEntry(uint8_t index) aufgerufen. Der zurückgegebene String wird dann angezeigt. Wird auf ein Element in der Liste geklickt, so wird die Callback-Funktion String onEntryEdit(uint8_t index, bool add) aufgerufen. Diese Callback-Funktion muss einen String mit den Daten des entspre-chenden Eintrags im JSON Format liefern. Es wird das, mit setDescription er-stellte Formular, angezeigt und mit den Daten aus dem JSON String gefüllt. Der Pa-rameter add ist in diesem Fall falsch. Dasselbe passiert auch, wenn der Knopf „Neu“ geklickt wurde, mit dem Unterschied, dass dann der Parameter add wahr ist.
Wird im Bearbeitungsformular der Knopf „Speichern“ geklickt, so wird die Callback-Funktion onEntryDone(uint8_t index, String data) aufgerufen und das Formular geschlossen. Der Parameter data enthält alle geänderten Daten im JSON-Format. Wird der Knopf „Abbruch“ geklickt, wird nur das Formular geschlos-sen. Beim Klicken auf den Knopf „Löschen“ wird die Callback-Funktion onDelete(uint8_t index) aufgerufen und das Formular geschlossen.
Nach dem Schließen des Bearbeitungsformulars wird die Liste wieder aktualisiert. Um die Liste selbst zu schließen, muss der Knopf „Fertig“ geklickt werden. Es wird die Callback-Funktion onCancel() aufgerufen.

## void setListCount(uint16_t count)
Mit dieser Funktion wird die Anzahl der Elemente in der Liste verändert. Diese An-zahl bestimmt wie viele Elemente angezeigt werden sollen.

## void handleClick(int16_t x, int16_t y)
Mit dieser Funktion werden Klickereignisse vom Touchscreen an die TFTForm Klasse weitergegeben. Die Parameter x und y geben die Position des Berührungs-punktes an.
String getResults()
Diese Funktion gibt alle Daten, des gerade angezeigten Formulars in einem JSON-String zurück. Als Schlüsselworte, werden die Namen der Eingabeelemente ver-wendet.

## void registerOnSave(void (\*callback)(String results))
Die Callback-Funktion onSave(String results) wird registriert. Sie wird dann aufgerufen, wenn ein Formular mit dem Knopf „Speichern“ oder ein Ja/Nein Dialog mit dem Knopf „Ja“ beendet wurde. Der Parameter results enthält die Daten des Formulars als JSON String beziehungsweise „YES“ beim Dialog.

## void registerOnCancel(void (\*callback)())
Die Callback-Funktion onCancel() wird registriert. Sie wird dann aufgerufen, wenn ein Formular mit dem Knopf „Abbruch“, eine Liste mit dem Knopf „Fertig“ oder ein Ja/Nein Dialog mit dem Knopf „Nein“ geschlossen wird.

## void registerOnDelete(void (\*callback)(uint8_t index))
Die Callback-Funktion onDelete(uint8_t index) wird registriert. Sie wird dann aufgerufen, wenn ein Formular mit dem Knopf „Löschen“ beendet wird. Der Parameter index gibt den Index des Elements in der Liste an. Wurde das Formular nicht aus einer Liste angezeigt, ist der Parameter 0.

## void registerOnListEntry(String (\*callback)(uint8_t index))
Die Callback-Funktion String onListEntry(uint8_t index) wird regis-triert. Sie wird immer dann ausgerufen, wenn ein Element aus der Liste angezeigt wird. Der Parameter index gibt den Index des Elements in der Liste an. Die Funkti-on muss den Text, der angezeigt werden soll, zurückgeben.

## void registerOnEntryEdit(String (\*callback)(uint8_t index, bool add))
Die Callback-Funktion String onEntryEdit(uint8_t index, bool add) wird registriert. Sie wird immer dann aufgerufen, wenn ein Element in der Liste oder der Knopf „Neu“ geklickt wurde. Der Parameter index gibt, den Index des Elements in der Liste an. Der Parameter add ist wahr, wenn der Knopf „Neu“ geklickt wurde. Es wird das Formular, das mit der Funktion setDescription definiert wurde, angezeigt, und mit den Daten, die die Callback-Funktion liefert, gefüllt. Die Daten müssen im JSON Format sein und als Schlüsselworte die Namen der Eingabefelder des Formulars verwenden.

## void registerOnEntryDone(void (\*callback)(uint8_t index, String data))
Die Callback-Funktion onEntryDone(uint8_t index, String data) wird registriert. Sie wird dann aufgerufen, wenn ein Formular, das aus einer Liste aufgerufen wurde, mit dem Knopf „Speichern“ beendet wurde. Der Parameter index gibt den Index des Elements in der Liste an und der Parameter data ent-hält die Daten des Formulars im JSON Format. Als Schlüsselworte werden die Namen der Eingabefelder verwendet.

## void setOptions(const char name[],  uint8_t optionsCount, String options[])
Mit dieser Funktion können zu einem Auswahllisten Eingabeelement Optionen hin-zugefügt werden. Der Parameter name gibt den Namen des Eingabefeldes an. Der Parameter optionsCount gibt die Anzahl der Optionen an, die hinzugefügt wer-den sollen. Der Parameter options enthält eine Liste von Strings, die die einzel-nen Optionen definieren. Wenn ein String mit einer Option ein Gleichheitszeichen enthält, wird der Teil vor dem Gleichheitszeichen als Wert und der nach dem Gleichheitszeichen als Beschriftung interpretiert. Enthält der String kein Gleichheits-zeichen, wird er für Wert und Beschriftung verwendet.

## void addOption(const char name[], String option)
Diese Funktion fügt eine neue Option hinzu. Der Parameter name ist der Name des Eingabefeldes. Der Parameter option ist ein String mit der Option. Für das Format des Strings gilt dasselbe wie bei der Funktion setOptions.
void addOption(uint8_t index, String option)
Diese zweite Variante der Funktion addOption erhält mit dem Parameter index den Index des Eingabefeldes im Formular an Stelle des Namens. Die Funktion selbst ist identisch.

## void clearOptions(const char name[])
Diese Funktion löscht alle Optionen eines Auswahllisten Eingabeelements. Der Pa-rameter name enthält den Namen des Eingabeelements.

## void setValues(String json)
Diese Funktion füllt das aktuelle Formular mit den Daten aus dem Parameter json. Wie der Name sagt, handelt es sich dabei um einen String im JSON Format. Als Schlüsselworte werden die Namen der Eingabeelemente verwendet.

## int8_t findName(const char name[])
Diese Funktion returniert den Index des Eingabeelements mit dem Namen, der im Parameter name angegeben wurde. Wenn kein entsprechende Eingabeelement gefunden wurde, wird -1 zurückgegeben.

## bool isActive()
Mit dieser Funktion kann festgestellt werden, ob ein Formular, eine Liste oder ein Ja/Nein Dialog angezeigt wird. Der Rückgabewert dieser Funktion kann dazu genutzt werden, Touchscreen Ereignisse nur dann an TFTForm weiterzugeben, wenn eine Anzeige aktiv ist.

## String getOption(uint8_t index, String val)
Diese Funktion returniert vom Auswahllisten Eingabefeld mit dem, im Parameter index angegebenen Index die Beschriftung der Option, die dem im Parameter val angegebenen Wert, entspricht. Gibt es keine solche Option, so wird ein Leer-string zurückgegeben.
