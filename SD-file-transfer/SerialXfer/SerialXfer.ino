//SerialXfer app
//Transfer files from a PC over USB to the Tinylab's SD card, or from the SD card to the PC
//2016 Andre Needham

#include <LiquidTWI2.h> 
LiquidTWI2 lcd(0x20);

//Get SdFat library from https://github.com/greiman/SdFat
#include "SdFat.h"
SdFat sd;

long fileSize;
String filename;

void setup()
{
  lcd.setMCPType(LTI_TYPE_MCP23008);  
  lcd.begin(16, 2);
  lcd.setBacklight(HIGH); 

  sd.begin(4); //4 is the CS pin for the SD card

  Serial.begin(115200);
}

void loop()
{
  String fileSizeString;
  if(Serial.available())
  {
    filename = serialReadLn();

    if(filename == "DIR") // get directory listing and transfer it to the PC
    {
      getFileListFromSD();
    }
    else if(filename == "GET") // get file from SD card and transfer it to the PC
    {
      delay(20);      
      filename = serialReadLn();
      lcd.clear();
      lcd.print("Get ");
      lcd.print(filename);
      getFileFromSD();
      lcd.clear();
      lcd.print("done");
    }
    else if(filename == "DEL") // delete a file from SD card
    {
      delay(20);      
      filename = serialReadLn();
      lcd.clear();
      lcd.print("Del ");
      lcd.print(filename);
      deleteFileFromSD();
      lcd.clear();
      lcd.print("done");    
    }
    else // copy file from PC onto SD card
    {
      lcd.setCursor(0,0);
      lcd.clear();
      lcd.print(filename);
      delay(20);
      fileSizeString = serialReadLn();
      lcd.setCursor(0,1);
      fileSize = atol(fileSizeString.c_str());
      lcd.print(fileSize);
      saveFileToSD();
      lcd.clear();
      lcd.print("done");
    }
  }
}

// Read a line of text from the Serial input (from PC via USB cable).  Text is a file name, command, or other single-line info.
String serialReadLn()
{
  String readString;
  while (Serial.available())
  {
    delay(3);  //delay to allow buffer to fill 
    if (Serial.available() >0)
    {
      char c = Serial.read();  //get one byte from serial buffer
      if (c == 10)  return readString;
      if(c != 13) readString += c; //build the string readString
    } 
  }
}

void saveFileToSD()
{
  char filenameChars[14];
    filename.toCharArray(filenameChars, 14);
    if(sd.exists(filenameChars)) sd.remove(filenameChars); //note file will be overwritten if it already exists
    File fp = sd.open(filename, FILE_WRITE);

    char c;
    for(int index = 0; index < fileSize; index++)
    {
      //wait for byte
      while(!Serial.available());
      c = Serial.read();
      fp.write(c);
    }
    fp.close();
}

void getFileFromSD()
{
  char filenameChars[14];
  filename.toCharArray(filenameChars, 14);
  if(sd.exists(filenameChars))
  {
    File fp = sd.open(filenameChars, FILE_READ);
    
    //first return the file size so the other end knows how many bytes to expect.
    long fileSize = fp.fileSize();
    Serial.println(fileSize, DEC);

    //now transfer the file bytes
    char c;
    while(fp.available() > 0)
    {
      c = fp.read();
      Serial.write(c);
    }
    fp.close();
  }
}

void deleteFileFromSD()
{
  char filenameChars[14];
  filename.toCharArray(filenameChars, 14);
  if(sd.exists(filenameChars))
  {
    File fp = sd.open(filenameChars, FILE_WRITE);    
    fp.remove();
    fp.close();
  }
}

// Get list of files in the root folder only.
void getFileListFromSD()
{
  char curFileName[14];
  File dir = sd.open( "/" );
  dir.seek(0);

  while(true)
  {
    File entry = dir.openNextFile();
    if(!entry)
    {
      entry.close();
      break;
    }

    if(!entry.isDirectory())
    {
      if(entry.getName(curFileName, 14))
      {
        Serial.println(curFileName);
      }
    }
    entry.close();
  }
  dir.close();
  Serial.println("<end>");
}

