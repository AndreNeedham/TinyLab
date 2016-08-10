//"Simon"-style memory repeat game for TinyLab
//Andre Needham

//NOTE: TinyLab's buzzer is on A1.  It doesn't sound very good, so I hooked an 8 ohm, .2 watt speaker between pin 5 and ground.
//It's loud enough, sounds good, and only seems to use 9ma or so, way under the maximum pin output current.
//Change the 5 below to A1 if you really want to hear the buzzer.
#define SPKR_PIN 5

#include <LiquidTWI2.h>
LiquidTWI2 lcd(0x20);

#define in1 9
#define in2 8
#define in3 A5
#define in4 A5
#define out1 13
#define out2 12
#define out3 11
#define out4 10

int notes[] = {131, 165, 196, 262};
int pressed1, pressed2, pressed3, pressed4;
int sequence_num;
bool show;
int sequence[64];

// the setup routine runs once when you press reset:
void setup() {
  
  randomSeed(analogRead(0));  //bugbug seems to always be the same.
  pinMode(SPKR_PIN, OUTPUT);
  pinMode(in1, INPUT);
  pinMode(in2, INPUT);
  pinMode(in3, INPUT);
  //pinMode(in4, INPUT);  //in3 and in4 are both on A5, no need to set pinMode twice.
  
  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
  pinMode(out3, OUTPUT);
  pinMode(out4, OUTPUT);
  
  sequence_num = 1;
  show = true;

  lcd.setMCPType(LTI_TYPE_MCP23008);  
  lcd.begin(16, 2);
  lcd.setBacklight(HIGH);
  lcd.print("Simon");  
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(out1, LOW);
  digitalWrite(out2, LOW);
  digitalWrite(out3, LOW);
  digitalWrite(out4, LOW);
  
  delay(800);

  int which = random(1,5);
  sequence[sequence_num] = which;
  
  for(int index = 1; index <= sequence_num; index++)
  {
    tone(SPKR_PIN, notes[sequence[index] - 1]);
    digitalWrite(-sequence[index] + 14, HIGH);  //magic number 14 - (1 through 4) will map to LED on pin 13, 12, 11, or 10.
    delay(500);
    noTone(SPKR_PIN);
    digitalWrite(-sequence[index] + 14, LOW);
    delay(500);   
  }
  delay(250);
  
  bool failed = false;
  int repeat_num = 1;
  
  while(!failed && repeat_num <= sequence_num)
  {
    int input = waitForInput();
    if(input > 0)
    {
      tone(SPKR_PIN, notes[input - 1]);
      digitalWrite(-input + 14, HIGH);
      delay(500);
      noTone(SPKR_PIN);
    }

    bool ok = waitForInputRelease();
    if(input > 0) digitalWrite(-input + 14, LOW);
    delay(200);

    if(!ok) failed = true;
    if(input != sequence[repeat_num]) failed = true;
    repeat_num++;
  }
  
  if(failed)
  {
    flash4Times();
    sequence_num = 1;
  }
  else
  {
    sequence_num++;
    delay(500);
  }
}

int waitForInput()
{
  for(int index = 0; index < 60; index++)
  {
    pressed1 = digitalRead(in1);
    pressed2 = digitalRead(in2);
    pressed3 = (analogRead(in3) > 180) && (analogRead(in3) < 220);
    pressed4 = (analogRead(in4) > 480) && (analogRead(in4) < 540);
    
    if(pressed1 == LOW) return 1;
    if(pressed2 == LOW) return 2;
    if(pressed3) return 3;
    if(pressed4) return 4;
    
    delay(50);
  }
  return 0;
}

bool waitForInputRelease()
{
   for(int index = 0; index < 50; index++)
  {
    pressed1 = digitalRead(in1);
    pressed2 = digitalRead(in2);
    pressed3 = (analogRead(in3) > 180) && (analogRead(in3) < 220);
    pressed4 = (analogRead(in4) > 480) && (analogRead(in4) < 540);
    
    if((pressed1 == HIGH) && (pressed2 == HIGH) && (!pressed3) && (!pressed4))
    {
      return true;
    }
    
    delay(50);
  }
  return false;
}

void flash4Times()
{
  for(int index = 0; index < 4; index++)
  {
    tone(SPKR_PIN, 58);
    digitalWrite(out1, HIGH);
    digitalWrite(out2, HIGH);
    digitalWrite(out3, HIGH);
    digitalWrite(out4, HIGH);
    delay(500);
    noTone(SPKR_PIN);
  
    digitalWrite(out1, LOW);
    digitalWrite(out2, LOW);
    digitalWrite(out3, LOW);
    digitalWrite(out4, LOW);
    delay(500);
  }
}
