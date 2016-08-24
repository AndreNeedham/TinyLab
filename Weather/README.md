Weather retrieval program

Andre Needham

Uses the ESP8266 WiFi board to request local weather data and displays it on the LCD.

Before you upload and run the sketch, change the "ssid" and "password" variables below to match your WiFi access point.
After you run the sketch, give it about 15 seconds to start up and get the WiFi connection going.

Now you need to enter a 5-digit ZIP code for the location you'd like to get weather data for.
Use buttons S2 and S3 to change the digit up/down.  Use button S4 to select that digit and move to the next one.
Once you press S4 on the 5th digit, the ESP8266 will send out the web request and wait to receive the data.
Be patient, even afer you see 4 dots on the LCD screen, it may be 10 seconds before the weather data shows up.
Once you see the temperature, you can press S2 or S3 to cycle through the weather data (temperature, conditions, humidity, wind speed and direction).
