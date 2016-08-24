//Weather retrieval program
//Andre Needham

//Uses the ESP8266 WiFi board to request local weather data and displays it on the LCD.

//Before you upload and run the sketch, change the "ssid" and "password" variables below to match your WiFi access point.

//After you run the sketch, give it about 15 seconds to start up and get the WiFi connection going.
//Use buttons S2 and S3 to change the digit up/down.  Use button S4 to select that digit and move to the next one.
//Once you press S4 on the 5th digit, the ESP8266 will send out the web request and wait to receive the data.
//Be patient, even afer you see 4 dots on the LCD screen, it may be 10 seconds before the weather data shows up.
//Once you see the temperature, you can press S2 or S3 to cycle through the weather data (conditions, humidity, wind speed and direction).

#include <LiquidTWI2.h> 

//name buttons
#define LEFT_BUTTON 9
#define UP_BUTTON 8
#define DOWN_BUTTON A5
#define RIGHT_BUTTON A5

LiquidTWI2 lcd(0x20);

//If for some reason things aren't working, you can do "Tools / Serial Monitor" in the Arduino IDE after the sketch starts.
//Any ouput from the ESP8266 will be sent to the serial monitor if you set DEBUG to true:
boolean DEBUG = false;

String ssid     = "your_ssid";	// put your WiFi's SSID (name) here
String password = "your_password";  // put your WiFi password here

//this also works, just can't search by ZIP code (save in case openweathermap breaks)
/*
String host     = "www.wunderground.com";
const int httpPort   = 80;
String uri		= "/auto/rss_full/WA/Seattle";  
String uriEnd	= ".xml";
*/
String host     = "api.openweathermap.org"; 
const int httpPort   = 80;
String uri		= "/data/2.5/weather?zip=";
String uriEnd	= "&units=Imperial&appid=1a702a15a2f46e405e61804cf67c0d30"; 

char ZIPBuffer[6];

//struct for finding an item in an HTTP GET result stream
struct findItem
{
  findItem(char _find[10], char _findEnd, char _resultEnd)
  {
    strcpy(find, _find);
    findEnd = _findEnd;
    resultEnd = _resultEnd;
  }
  
  char find[10];		//look for these characters before the result we want
  char findEnd;			//look for this char just before the result we want
  int findLoc = 0;		//number of characters we've currently matched in the result stream
  bool saving = false;		//mark true once we've found the pattern and need to grab the result
  char result[17];		//the actual result
  int resultLoc = 0;		//what char position in the result we've written
  char resultEnd;               //char to stop grabbing result at
};

//for wunderground: (works)
//findItem findTemp = findItem("rature: ", ' ', '&');

//for openweathermap: (works)
findItem findTemp = findItem("\"temp\":", ':', ',');
findItem findHumid = findItem("idity\":", ':', ',');
findItem findWindSpd = findItem("speed\":", ':', ',');
findItem findWindDir = findItem("\"deg\":", ':', ',');
findItem findDesc = findItem("main\":", ':', ',');
findItem findCity = findItem("name\":", ':', ',');

findItem* itemsToFind[] = {&findTemp, &findHumid, &findWindSpd, &findWindDir, &findDesc, &findCity}; 
int numItemsToFind = 6;

char findOK[] = "OK\r\n";
int findOKLoc = 0;

int curCharLoc = 0;
bool enteringChars = true;
bool displayingResults = false;
int displayItemNum = 0; //0 = temp, 1 = conditions, 2 = humidity, 3 = wind

bool leftPressed = false;
bool rightPressed = false;
bool upPressed = false;
bool downPressed = false;

bool err = false;

// the setup routine runs once when you press reset:
void setup() {
  // initialize buttons
  pinMode(LEFT_BUTTON, INPUT);
  pinMode(RIGHT_BUTTON, INPUT);
  pinMode(UP_BUTTON, INPUT);
//  pinMode(DOWN_BUTTON, INPUT); same input pin as right button
  
  lcd.setMCPType(LTI_TYPE_MCP23008);  
  lcd.begin(16, 2);
  lcd.setBacklight(HIGH); 
  lcd.setCursor(0,0);

  if(DEBUG) Serial.begin(115200);

  //initial ESP8266 setup
  Serial1.begin(115200);	
  
  lcd.print("startup.");

  sendData("AT+RST\r\n", 7000, DEBUG); // reset ESP8266
  if(err) lcd.print("!");
  else lcd.print(".");

  sendData("AT+CWMODE=1\r\n", 7000, DEBUG); // configure ESP8266 mode
  if(err) lcd.print("!");
  else lcd.print(".");
  
  // connect to internet via wireless network   
  String cmd="AT+CWJAP=\"";   
  cmd += ssid; 
  cmd += "\",\""; 
  cmd += password; 
  cmd += "\"";
  sendData(cmd + "\r\n", 10000, DEBUG); 
  if(err){
    lcd.print("!");
  }
  else lcd.print(".");

  sendData("AT+CIPMUX=0\r\n", 8000, DEBUG); // Configure server connection type
  if(err) {
    lcd.print("!");
  }
  else lcd.print(".");
    
  // Open TCP connection to the host:
  sendData("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort + "\r\n", 15000, DEBUG);
  if(err) lcd.print("!");
  else
  {
    lcd.clear();
    lcd.setCursor(0,0);  
    lcd.print("Enter ZIP: 5");
    ZIPBuffer[0] = '5';
  }
}

// make HTTP GET for selected ZIP code, get weather data
void getWeather() //selected ZIP code is global var
{
  char outputString[8];
  
  lcd.setCursor(0, 1);
  lcd.print(".");

  // Construct our HTTP call
  String httpPacket = "GET " + uri + ZIPBuffer + uriEnd + " HTTP/1.1\r\nHost: " + host + "\r\n" /*+ userAgent +*/ "\r\n";
  int length2 = httpPacket.length();
  
  // Send our message length  
  sendData("AT+CIPSEND=" + String(length2) + "\r\n", 4000, DEBUG); 
  lcd.print(".");

  // Send our http request
  sendData(httpPacket, 5000, DEBUG);
  if(err) {
    lcd.print("!");
  }
  else lcd.print(".");

  //ignore returned http header
  receiveData(8000, DEBUG, false);
  lcd.print(".");

  //get our data
  receiveData(8000, DEBUG, true);

  //todo display temp, humidity, wind, pressure (have user press up/down to toggle through)
  lcd.clear();
  lcd.print(findCity.result);
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");

  lcd.print(findTemp.result);
  lcd.print("F");    
}


// the loop routine runs over and over again forever:
void loop() {
  //read up/dn buttons, change digit 0-9
  //read right button, move to next digit.  Once we move past 5, do HTTP query
  if(digitalRead(LEFT_BUTTON) == LOW)
  {
    if(!leftPressed)
    {
      leftPressed = true;
      //todo new city?
    }
  }
  else if((analogRead(RIGHT_BUTTON) > 480) && (analogRead(RIGHT_BUTTON) < 540))
  {
    if(!rightPressed)
    {
      rightPressed = true;
      if(enteringChars)
      {      
        //move to next for the ZIP code
        if(curCharLoc == 5) //done entering
        {
          ZIPBuffer[5] = '\0';
          enteringChars = false;
          getWeather();
          displayingResults = true;
        }
        else
        {
          curCharLoc++;
          lcd.setCursor(curCharLoc + 11, 0);
          lcd.print('5');
          ZIPBuffer[curCharLoc] = '5';
        }
      }
    }
  }
  else if (digitalRead(UP_BUTTON) == LOW)
  {
    if(!upPressed)
    {
      upPressed = true;
      
      if(enteringChars)
      {
        ZIPBuffer[curCharLoc]++;
        if(ZIPBuffer[curCharLoc] > '9') ZIPBuffer[curCharLoc] = '0';
        lcd.setCursor(curCharLoc + 11, 0);
        lcd.print(ZIPBuffer[curCharLoc]);
      }
      else if(displayingResults)
      {
        //switch among temp, conditions, pressure, wind
        displayItemNum--;
        if(displayItemNum < 0) displayItemNum = 3;
        displayItem(displayItemNum);
      }
    }
  }
  else if((analogRead(DOWN_BUTTON) > 180) && (analogRead(DOWN_BUTTON) < 220))
  {
    if(!downPressed)
    {
      downPressed = true;
      
      if(enteringChars)
      {
        ZIPBuffer[curCharLoc]--;
        if(ZIPBuffer[curCharLoc] < '0') ZIPBuffer[curCharLoc] = '9';
        lcd.setCursor(curCharLoc + 11, 0);
        lcd.print(ZIPBuffer[curCharLoc]);
      }
      else if(displayingResults)
      {
        //switch among temp, conditions, pressure, wind
        displayItemNum++;
        displayItemNum %= 4;
        displayItem(displayItemNum);
      }
    }
  }
  else //no button, reset button held markers
  {
    leftPressed = upPressed = downPressed = rightPressed = false;
  }  
}

// send AT command function  
String sendData(String command, const long timeout, boolean debug) 
{
  Serial1.print(command); // send the data to the esp8266 
     
  return receiveData(timeout, debug, false);
} 

// receive the http response.  Scan it for data we want and for "OK" signal that we're done
String receiveData(const long timeout, boolean debug, boolean findData) 
{ 
    err = true;
     
    long int time = millis(); 
     
    while((time+timeout) > millis()) 
    { 
      while(Serial1.available())           // The esp has data
      { 
        char c = Serial1.read(); // read the next character. 
        if(DEBUG) Serial.print(c);

        if(findData)
        {
          for(int index = 0; index < numItemsToFind; index++)
          {
            findItem* curItem = itemsToFind[index];
            if(curItem->saving)
            {
              if(c == curItem->resultEnd) //end of result value
              {
                curItem->saving = false;
                curItem->result[curItem->resultLoc] = 0; //null terminate
                //curItem->resultLoc = 0; going to use nonzero resultLoc to indicate we found our string
                //-ran into an issue of finding the string >1 time in output, only want to find 1st one (always I hope).
              }
              else
              {
                curItem->result[curItem->resultLoc++] = c;
              }
            }

            //need to character-by-character look for strings we want and save them
            if (c == curItem->find[curItem->findLoc])
            {
              curItem->findLoc++;
              if(c == curItem->findEnd)  //end of item name, value follows
              {
                if(curItem->resultLoc == 0) //don't save if we already found and saved one
                {
                  curItem->findLoc = 0;
                  //flag that we should save characters until we see end marker
                  curItem->saving = true;
                  curItem->resultLoc = 0;
                }
              }
            }
            else curItem->findLoc = 0;
          }
        }
                
        // detect "OK\r\n" so we can leave early
        if (c == findOK[findOKLoc])
        {
          findOKLoc++;
          if(findOKLoc == 4)
          {
            err = false;
            return "OK\r\n";
          }
        }
        else findOKLoc = 0;
      }   
    } 
   return "ERROR\r\n";
} 

void displayItem(int itemNum)
{ 
  lcd.clear();
  lcd.print(findCity.result);
  lcd.setCursor(0, 1);
  
  switch(itemNum)
  {
    case 0: // temp
      lcd.print("Temp: ");
      lcd.print(itemsToFind[0]->result);
      lcd.print("F");
    break;
    
    case 1: // conditions
      lcd.print(itemsToFind[4]->result);
    break;
    
    case 2: // humidity
      lcd.print("humidity: ");
      lcd.print(itemsToFind[1]->result);
      lcd.print("%");
    break;
    
    case 3: //wind dir/speed
      lcd.print("wind: ");
      lcd.print(windAngleToCompass(atoi(itemsToFind[3]->result)));
      lcd.print(" ");
      lcd.print(itemsToFind[2]->result);
      lcd.print("MPH");
    break;
  }
}

String windAngleToCompass(int angle)
{
  String compass[] = {"N","NNE","NE","ENE","E","ESE","SE","SSE","S","SSW","SW","WSW", "W","WNW","NW","NNW"};
  float fangle = angle + 11.25;
  fangle /= 22.5;
  return compass[(int)fangle];
}
