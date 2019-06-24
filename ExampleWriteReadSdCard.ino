// ========================================================================================
//      Meine Welt in meinem Kopf
// ========================================================================================
// Projekt:       Example write read sd card
// Author:        Johannes P. Langner
// Controller:    WEMOS D1 Mini
// Modul:         SD Card
// Description:   Mehrer Daten als json format schreiben und wieder auslesen.
// ========================================================================================

#include <SPI.h>
#include <SD.h>    
#include <ArduinoJson.h>

// ========================================================================================
// SD Card
Sd2Card mCard;

// change this to match your SD shield or module;
// WeMos Micro SD Shield V1.0.0: D8
// LOLIN Micro SD Shield V1.2.0: D4 (Default)
const int mChipSelect = D4;
String mFilename = String("mydata");

File mReadFile;

const int mCountData = 12;

int mCount = 0;

// ========================================================================================
void setup() {

  Serial.begin(9600);

  Serial.print("Size of int:"); Serial.println(sizeof(int), DEC);
  Serial.print("Size of int16: "); Serial.println(sizeof(int16_t), DEC);
  Serial.print("Size of int32: "); Serial.println(sizeof(int32_t), DEC);
  Serial.print("Size of float: "); Serial.println(sizeof(float), DEC);

  SdCardCheckAndRemove();
  SdCardCreateFile();
  Serial.println("----------------------------");
  SdCardWriteJsonToFile();
}

// ========================================================================================
// measure data object
class MeasureData {

  public:
    int Day;
    int Month;
    int Year;
    int Hour;
    int Minute;
    float Temperature;
    float Humidity;
    float Pressure;

    // print all values 
    void Print() {
      Serial.print("Day "); Serial.print(Day, DEC); Serial.print(" ");
      Serial.print("Month "); Serial.print(Month, DEC); Serial.print(" ");
      Serial.print("Year "); Serial.print(Year, DEC); Serial.print(" ");

      Serial.print("Hour "); Serial.print(Hour, DEC); Serial.print(" ");
      Serial.print("Minute "); Serial.print(Minute, DEC); Serial.print(" ");

      Serial.print("Temperature "); Serial.print(Temperature, DEC); Serial.print(" ");
      Serial.print("Humidity "); Serial.print(Humidity, DEC); Serial.print(" ");
      Serial.print("Pressure "); Serial.print(Pressure, DEC); Serial.println(" ");
    }

    void ReadFromJson(JsonObject measure) {
      Day = measure["Day"];
      Month = measure["Month"];
      Year = measure["Year"];
      Hour = measure["Hour"];
      Minute = measure["Minute"];
      Temperature = measure["Temperature"];
      Humidity = measure["Humidity"];
      Pressure = measure["Pressure"];
    }
};

// ========================================================================================
void loop() {

  mReadFile = SD.open(mFilename);
  
  if(mReadFile) {
  
    ReadCsvContent(mReadFile.readString());
    Serial.println("----------------------------");
  }
  else
  {
    Serial.println("SD Card: can not read");
    delay(10000);
    return;
  }

  Serial.println();
  delay(1000);

  Serial.print("Loop: "); Serial.println(mCount, DEC);
  mCount++;
}

// ========================================================================================
// check the selected sd card start and remove exist file
void SdCardCheckAndRemove() {
  
  if(!SD.begin(mChipSelect)) {
    Serial.println("error - SD can not start"); 
    while(1) ;
  }

  if(SD.exists(mFilename)){
    SD.remove(mFilename);
    Serial.print("delete weather file...");
  }
}

// ========================================================================================
// create a file to save the json format information
void SdCardCreateFile(){

    if(SD.exists(mFilename)) {
      return;
    }

    Serial.print("Create file...");
    mReadFile = SD.open(mFilename, FILE_WRITE);
    mReadFile.close(); 
    Serial.println("...created ...");
}

// ========================================================================================
// write the json object to file with example data
 void SdCardWriteJsonToFile() {  

  Serial.println("...write json...");
  const size_t capacity = JSON_ARRAY_SIZE(mCountData) * JSON_OBJECT_SIZE(8);           
  StaticJsonDocument<capacity> jsonBuffer;         // set the content size
  JsonArray data = jsonBuffer.to<JsonArray>();      // setup array to write
  
  for (int i = 0; i < mCountData; i++) {
  
    JsonObject measure = data.createNestedObject(); // created to add a new object

    measure["Day"] = 1 * i;
    measure["Month"] = 4 * i;
    measure["Year"] = 2019  + i;
    measure["Hour"] = i;
    measure["Minute"] = 0;
    measure["Temperature"] = 21.4f  * i;
    measure["Humidity"] = 50.5f * i;
    measure["Pressure"] = 12345.6f * i;

    Serial.print("Add data: "); Serial.println(i, DEC);
  }
  
  mReadFile = SD.open(mFilename, FILE_WRITE);
  serializeJson(jsonBuffer, mReadFile);             // write json document to file
  mReadFile.close(); 
}

// ========================================================================================
// parse the json object and print to serial.
void ReadCsvContent(String fileContent) {
  
  //Serial.println(fileContent);
  Serial.println("----------------------------");
  
  const size_t capacity = JSON_ARRAY_SIZE(mCountData) * JSON_OBJECT_SIZE(8);           
  DynamicJsonDocument doc(capacity);
  deserializeJson(doc, fileContent);

  MeasureData d;
  
  for (int index = 0; index < mCountData; index++) {

    Serial.print(index, DEC); Serial.println(". Measure");

    JsonObject measure = doc[index];
    MeasureData d;
    d.ReadFromJson(measure);
    d.Print();
    Serial.println();
  }
}
