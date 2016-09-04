//Stock Ticker retrieval program
//Andre Needham

//Uses the ESP8266 WiFi board to request a stock quote and displays the stock's price and day's change.

//Before you upload and run the sketch, change the "ssid" and "password" variables below to match your WiFi access point.

//After you run the sketch, give it about 15 seconds to start up and get the WiFi connection going.
//Use buttons S2 and S3 to move up/down the list of stocks.  Use button S4 to select that stock and get the quote.
//If you select the ">" entry, you can enter a custom stock ticker.  Use buttons S2/S3 to change the first letter of the ticker.
//Use S4 to select that letter, then use S2/S3 to selet the next letter.  After the last letter, press S1 to get the quote.
//Be patient, even afer you see 4 dots on the LCD screen, it may be 10 seconds before the quote shows up.

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

String host     = "www.google.com";
const int httpPort   = 80;
String uri		 = "/finance/info?q=";
String stocks[11] = {"AAPL","AMZN","BAC","FB","GE","GOOG","INTC","MSFT","NFLX","TSLA",">"};
int selectedStock = 0;

bool leftPressed = false;
bool rightPressed = false;
bool upPressed = false;
bool downPressed = false;

bool inCustomEntry = false;
String customTicker;
char customChar;
int customCharLoc = 9;

void setup() {

  digitalWrite(4, HIGH); //turn off SD light, it's flickering for some reason
  digitalWrite(13, LOW); //turn off LED1

  // initialize buttons
  pinMode(LEFT_BUTTON, INPUT);
  pinMode(RIGHT_BUTTON, INPUT);
  pinMode(UP_BUTTON, INPUT);
//  pinMode(DOWN_BUTTON, INPUT);  //same pin as UP_BUTTON, no need to set it twice
  
  lcd.setMCPType(LTI_TYPE_MCP23008);  
  lcd.begin(16, 2);
  lcd.setBacklight(HIGH); 
  lcd.setCursor(0,0);

  delay(50);

  Serial.begin(115200);

  //initial ESP8266 setup
  Serial1.begin(115200); 

  lcd.print("startup.");

  sendData("AT+RST\r\n", 5000, DEBUG); // reset ESP8266 
  lcd.print(".");

  sendData("AT+CWMODE=1\r\n", 5000, DEBUG); // configure ESP8266 mode  
  lcd.print(".");
  
  // connect to internet via wireless network   
  String cmd="AT+CWJAP=\"";   
  cmd += ssid; 
  cmd += "\",\""; 
  cmd += password; 
  cmd += "\"";
  sendData(cmd + "\r\n", 10000, DEBUG);  
  lcd.print(".");

  sendData("AT+CIPMUX=0\r\n", 8000, DEBUG); // Configure server connection type  
    
  lcd.clear();
  lcd.setCursor(0,0);  
  lcd.print("Ticker: " + stocks[selectedStock]);
}


// make HTTP get for selected stock, return string of last trade and change
String getStockQuote() //selected stock is global var
{
  String outputString = "";
  
  lcd.setCursor(0, 1);
  lcd.print(".");

  // Open TCP connection to the host:
  sendData("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort + "\r\n", 15000, DEBUG);
  lcd.print(".");

  // Construct our HTTP call
  String httpPacket = "GET " + uri + ((selectedStock == 10) ? customTicker : stocks[selectedStock]) + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
  int length2 = httpPacket.length();
  
  // Send our message length  
  sendData("AT+CIPSEND=" + String(length2) + "\r\n", 5000, DEBUG);

  lcd.print(".");

  // Send our http request
  sendData(httpPacket, 5000, DEBUG);

  lcd.print(".");

  //ignore returned http header
  receiveData(10000, DEBUG);
  lcd.print(".");

  //get our data
  String response = receiveData(10000, DEBUG);

  int curPos = response.indexOf("\r\n\r\n");  // search for a blank line which defines the end of the http header
  if (curPos > -1)
  {    
    curPos = response.indexOf("\"l\" : \"", curPos); // find the last trade value
    curPos += 7;
    while (response[curPos] != '\"')
    {
      outputString += response[curPos];
      curPos++;
    }
    
    lcd.print(".");
    outputString += " ";
    
    curPos = response.indexOf("\"c\" : \"", curPos); // find the change value
    curPos += 7;
    while (response[curPos] != '\"')
    {
      outputString += response[curPos];
      curPos++;
    }    
  }

  sendData("AT+CIPCLOSE\r\n", 2000, DEBUG); // Close server connection

  return outputString;
}


void loop() {
  //read up/dn buttons, change stock ticker or change letter if in custom entry mode
  //read right button, query stock price or add letter if in custom entry mode
  //read left button, query stock price if in custom entry mode
  if(digitalRead(LEFT_BUTTON) == LOW)
  {
    if(!leftPressed)
    {
      leftPressed = true;

      if(inCustomEntry)
      {
        inCustomEntry = false;
        customTicker += customChar;
        String quote = getStockQuote();
        lcd.setCursor(0, 1);
        lcd.print(quote);
      }
    }
  }
  else if((analogRead(RIGHT_BUTTON) > 480) && (analogRead(RIGHT_BUTTON) < 540))
  {
    if(!rightPressed)
    {
      rightPressed = true;

      if((selectedStock == 10) && (!inCustomEntry)) //time to do custom entry
      {
        inCustomEntry = true;
        customCharLoc = 9;
        customTicker = "";
        customChar = 'A';
        lcd.setCursor(customCharLoc, 0);
        lcd.print(customChar);
      }
      else if (inCustomEntry) //add next char to custom entry
      {
        customTicker += customChar;
        customCharLoc++;
        if(customCharLoc == 13) //fourth char entered, do ticker query
        {
          inCustomEntry = false;
          String quote = getStockQuote();
          lcd.setCursor(0, 1);
          lcd.print(quote);
        }
        else //still selecting letters
        {
          customChar = 'A';
          lcd.setCursor(customCharLoc, 0);
          lcd.print(customChar);
        }
      }
      else //selected a ticker
      {
        String quote = getStockQuote();
        lcd.setCursor(0, 1);
        lcd.print(quote);
      }
    }
  }
  else if (digitalRead(UP_BUTTON) == LOW)
  {
    if(!upPressed)
    {
      upPressed = true;
      
      if(inCustomEntry)
      {
        customChar--;
        if(customChar < 'A') customChar = 'Z';
        lcd.setCursor(customCharLoc, 0);
        lcd.print(customChar);
      }
      else
      {
        lcd.clear();
        selectedStock--;
        if (selectedStock < 0) selectedStock = 10;
        lcd.setCursor(0, 0);
        lcd.print("Ticker: " + stocks[selectedStock]);
      }
    }
  }
  else if((analogRead(DOWN_BUTTON) > 180) && (analogRead(DOWN_BUTTON) < 220))
  {
    if(!downPressed)
    {
      downPressed = true;
      
      if(inCustomEntry)
      {
        customChar++;
        if(customChar > 'Z') customChar = 'A';
        lcd.setCursor(customCharLoc, 0);
        lcd.print(customChar);
      }
      else
      {
        lcd.clear();
        selectedStock++;
        if (selectedStock > 10) selectedStock = 0;
        lcd.setCursor(0, 0);
        lcd.print("Ticker: " + stocks[selectedStock]);
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
    Serial1.print(command); // send the read character to the esp8266 
     
    return receiveData(timeout, debug);
} 

// receive the http response
String receiveData(const long timeout, boolean debug) 
{ 
    String response = ""; 
     
    long int time = millis(); 
     
    while((time+timeout) > millis()) 
    { 
      while(Serial1.available()) 
      { 
        // The esp has data so display its output to the serial window  
        char c = Serial1.read(); // read the next character. 
        response += c; 
        if(debug) Serial.print(c);
      }   

      //we're done if we got back "OK"
      if(response.indexOf("OK\r\n") > -1) break;
    } 
     
    return response; 
} 
