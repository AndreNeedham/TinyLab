// Rotary encoder test using 500hz interrupt to poll the encoder
// Andre Needham

// libraries
// library can be found at https://github.com/lincomatic/LiquidTWI2
#include <LiquidTWI2.h>
// library can be found at https://github.com/mathertel/RotaryEncoder
#include <RotaryEncoder.h>
// library can be found at https://github.com/PaulStoffregen/TimerOne
#include <TimerOne.h>

// instances
LiquidTWI2 lcd(0x20);
RotaryEncoder encoder(7, 6);

int8_t rotary_pos = 0;
volatile int8_t new_pos = 0;

void setup()
{
  digitalWrite(4, HIGH); //turn off SD light, I see it flickering/on sometimes

	// LCD setup
	lcd.setMCPType(LTI_TYPE_MCP23008);
	lcd.begin(16, 2);
	lcd.setBacklight(HIGH);
  lcd.setCursor(0, 0);
  lcd.print("Encoder position");

  // Timer1 setup
  Timer1.initialize(2000);
  Timer1.attachInterrupt(readEncoder); //readEncoder to run every .002 seconds, or 500 times per second
}

// check the encoder position and store the result
void readEncoder()
{
  encoder.tick();
  new_pos = encoder.getPosition();
}

void loop()
{  
 	// Rotary Encoder Test
	if (rotary_pos != new_pos)
	{
    rotary_pos = new_pos;
    lcd.setCursor(0, 1);
    lcd.print(rotary_pos);
    lcd.print("                ");
	}
}

