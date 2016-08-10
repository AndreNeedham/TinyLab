//"Breakout"-style game for TinyLab
//Andre Needham
//Press button S1 to launch ball.  Steer paddle with the potentiometer.  Try to break all blocks.

//NOTE: TinyLab's buzzer is on A1.  It doesn't sound very good, so I hooked an 8 ohm, .2 watt speaker between pin 5 and ground.
//It's loud enough, sounds good, and only seems to use 9ma or so, way under the maximum pin output current.
//Change the 5 below to A1 if you really want to hear the buzzer.
#define SPKR_PIN 5
#define LEFT_BUTTON 9

#include <LiquidTWI2.h> 
LiquidTWI2 lcd(0x20);

static bool leftPressed = false;

byte graphics[64]; //8 characters, 8 bytes of data each.

void drawLine(int x1, int y1, int x2, int y2, int color);
void setPixel(int x, int y, int color);
int note[] = {196, 220, 247, 262};

class MovingObject
{  
  public:  
  MovingObject(){}
  MovingObject(int _width, int _locationX, int _locationY, int _direction)
  {
    width = _width;
    locationX = _locationX;
    locationY = _locationY;
    direction = _direction;
  }

  int width = 5;
  int locationX;
  int locationY;
  int direction;
  
  int LocationX() { return locationX;}
  int LocationY() {return locationY;}
  int Width() {return width;}
  
  void Draw()
  {
    drawLine(locationX, locationY, locationX + width - 1, locationY, 1);
    drawLine(locationX, locationY + 1, locationX + width - 1, locationY + 1, 1);
  }
  
  //move object and draw delta
  void Move()
  {
    locationX += direction;
    if(locationX < (1 - width))
    {
      setPixel(0, locationY, 0);
      setPixel(0, locationY + 1, 0);
      locationX = 15;
      Draw();
    }
    else if(locationX > 15)
    {
      setPixel(15, locationY, 0);
      setPixel(15, locationY + 1, 0);
      locationX = 1 - width;
      Draw();
    }
    else
    {    
      if (direction == 1)
      {
        setPixel(locationX - 1, locationY, 0);
        setPixel(locationX - 1, locationY + 1, 0);
        setPixel(locationX + width - 1, locationY, 1);
        setPixel(locationX + width - 1, locationY + 1, 1);
      }
      else
      {
        setPixel(locationX + width, locationY, 0);
        setPixel(locationX + width, locationY + 1, 0);
        setPixel(locationX, locationY, 1);
        setPixel(locationX, locationY + 1, 1);
      }
    }
  }
  
  // Test if a given x,y is inside the object.  Return true if so or false if not.
  bool HitTest(int testX, int testY)
  {
    bool retval = false;
    if((testY == locationY) || (testY == locationY + 1))
    {
      if ((testX >= locationX) && (testX < locationX + width))
      {
        retval = true;
      }
    }
    return retval;
  }
};

int moveCounter = 0;
int score = 0;
int paddle = 0;
int ballX = 8;
int ballY = 6;
int ballXmove = 1;
int ballYmove = 1;

int move_base = 1280;
int move_base_fast = 960;
int moveThreshold = move_base; 

int blocksRemaining = 48;
int ballsRemaining = 5;
int blocks[16][3];

bool ballLaunched = false;
bool reversedInBlocks = false;

void setup()
{
  digitalWrite(13, LOW); //turn off LED1.  It's on sometimes on startup
  digitalWrite(4, HIGH); //turn off SD light, it's flickering for some reason sometimes
  pinMode(SPKR_PIN, OUTPUT);

  randomSeed(analogRead(0));
  
  pinMode(LEFT_BUTTON, INPUT);
  //apparently A0 is defaulted to analog input, nothing to set.
  
  lcd.setMCPType(LTI_TYPE_MCP23008);  
  lcd.begin(16, 2);
  lcd.setBacklight(HIGH); 

  // define 8 custom characters using our (currently blank) bit data
  for (int i = 0; i < 8; ++i)
  {
	lcd.createChar(i, &graphics[i << 3]);
  }
  
  //set the first 4 characters of the 2 rows to our 8 custom characters
  lcd.setCursor(0, 0);
  for(int i = 0; i < 4; i++) lcd.write((char)i);
  lcd.setCursor(0, 1);
  for(int i = 4; i < 8; i++) lcd.write((char)i);
  
  //draw initial paddle
  setPixel(paddle, 15, 1);
  setPixel(paddle + 1, 15, 1);
  
  //add a right wall to lower confusion where bounce will happen
  drawLine(16, 0, 16, 14, 1);
  
  resetGame();
}


void loop()
{
  //only move objects occasionally or input detection is nonexistent.
  moveCounter++;
  
  if(moveCounter == moveThreshold)
  {
    noTone(SPKR_PIN);
    moveCounter = 0;
    if(ballLaunched)
    {
      //erase old ball (but be careful if ball is falling through the blocks as it must do in certain situations)
      if((ballY > 2) && (ballY < 6))
      {
        if(blocks[ballX][ballY - 3] == 0) setPixel(ballX, ballY, 0);
      }
      else setPixel(ballX, ballY, 0);
      
      //calculate new ball location
      ballX = ballX + ballXmove;
      ballY = ballY + ballYmove;
      
      //check for wall bounce
      if((ballX == 0) && (ballXmove == -1))
      {
        ballXmove = 1;
        tone(SPKR_PIN, 523);
      }
      if((ballX == 15) && (ballXmove == 1))
      {
        ballXmove = -1;
        tone(SPKR_PIN, 523);
      }
      
      //check for roof bounce
      if((ballY == 0) && (ballYmove == -1))
      {
        tone(SPKR_PIN, 784);
        ballYmove = 1;
        //speed up
        moveThreshold = move_base_fast;
      }
      
      //check for block hit
      if((ballY > 2) && (ballY < 6))
      {
        if(!reversedInBlocks)
        {
          if(blocks[ballX][ballY - 3] == 1) //we hit
          {
            blocks[ballX][ballY - 3] = 0; //record hit
            blocksRemaining--;

            if(blocksRemaining == 0)
            {
              blocksRemaining = 48;
              drawBlocks();
            }

            //and bounce
            //we have to let ball go down "through" any further blocks in its path or it digs in, so we detect and mark that situation
            if((ballY == 4) || ((ballY == 3) && (ballYmove == -1)) || ((ballY == 5) && (ballYmove == 1))) reversedInBlocks = true;
            ballYmove = -ballYmove;
          
            score += (6 - ballY); // line 3 = 3, line 4 = 2, line 5 = 1
            displayScore(score);

            tone(SPKR_PIN, note[5 - ballY]);
          }
        }
      }
      else reversedInBlocks = false;
      
      //check for paddle bounce
      if(ballY == 14)
      {
        if ((ballX + ballXmove) == paddle - 1) //bounce left (unless we're against left wall)
        {
          if(ballX > 0) ballXmove = -1;
          else ballXmove = 1;
          ballYmove = -1;
          tone(SPKR_PIN, 784);
        }
        else if ((ballX + ballXmove) == paddle) //bounce up
        {
          ballXmove = 0;
          ballYmove = -1;
          tone(SPKR_PIN, 784);
        }
        else if ((ballX + ballXmove) == paddle + 1) //bounce right (unless we're against right wall)
        {
          if(ballX < 15) ballXmove = 1;
          else ballXmove = -1;
          ballYmove = -1;
          tone(SPKR_PIN, 784);
        }
        //otherwise we missed, let it move down one
      }
      else if (ballY == 15) //must have missed, lose ball
      {
        ballLaunched = false;
        moveThreshold = move_base; //slow back down
        ballsRemaining--;
        displayBalls(ballsRemaining);
        
        if (ballsRemaining == 0)
        {
          delay(500);
          lcd.setCursor(5, 1);
          noTone(SPKR_PIN);
          lcd.print("Game Over");
          delay(2000);
          resetGame();
        }
      }
       
      //draw ball in new location if we're still going
      if(ballLaunched) setPixel(ballX, ballY, 1);            
    }
  }
  
  // check user inputs and move
  if(digitalRead(LEFT_BUTTON) == LOW)
  {
    if(!leftPressed)
    {
      leftPressed = true;
      
      if(ballsRemaining > 0)
      {
        moveCounter = -move_base; //delay a bit before launching ball
        ballLaunched = true;
        ballX = random(2, 14);
        ballXmove = random(0, 2);
        if (ballXmove == 0) ballXmove = -1;
        ballY = 6;
        setPixel(ballX, ballY, 1);
      }
    }
  }
  else //no button, reset button held markers
  {
    leftPressed = false;
  }
  
  //todo we read this so often that if the player moves the paddle a lot, or it's on the edge of two values, the game slows.  Fix?  
  int newPaddle = analogRead(A0); //0 to 1023
  if (newPaddle < 300) newPaddle = 300;
  if (newPaddle > 699) newPaddle = 699;
  newPaddle -= 300;
  newPaddle = newPaddle / 20; //divide by 20 for 0 to 19
  newPaddle -= 2; //so -2 to 17
  if(newPaddle < 0) newPaddle = 0;
  if(newPaddle > 15) newPaddle = 15;
  if(newPaddle != paddle)
  {
    switch(newPaddle - paddle)
    {
      case -2:
        setPixel(paddle + 1, 15, 0);
        setPixel(paddle, 15, 0);
        paddle = newPaddle;
        setPixel(paddle, 15, 1);
        setPixel(paddle - 1, 15, 1);
      break;
      
      case -1:
        setPixel(paddle + 1, 15, 0);
        paddle = newPaddle;
        setPixel(paddle - 1, 15, 1);
      break;
      
      case 1:
        setPixel(paddle - 1, 15, 0);
        paddle = newPaddle;
        setPixel(paddle + 1, 15, 1);
      break;
      
      case 2:
        setPixel(paddle - 1, 15, 0);
        setPixel(paddle, 15, 0);
        paddle = newPaddle;
        setPixel(paddle, 15, 1);
        setPixel(paddle + 1, 15, 1);
      break;
      
      default:
        setPixel(paddle - 1, 15, 0);
        setPixel(paddle, 15, 0);
        setPixel(paddle + 1, 15, 0);
        paddle = newPaddle;
        setPixel(paddle - 1, 15, 1);
        setPixel(paddle, 15, 1);
        setPixel(paddle + 1, 15, 1);
      break;
    }
  }
}

void resetGame()
{
  drawBlocks();
  
  int moveCounter = 0;
  score = 0;
  paddle = 0;
  ballX = 8;
  ballY = 6;
  ballXmove = 1;
  ballYmove = 1;

  moveThreshold = move_base;

  blocksRemaining = 48;
  ballsRemaining = 5;
  
  displayScore(score);
  displayBalls(ballsRemaining);
}

void drawBlocks()
{
  for(int y = 0; y < 3; y++)
  for(int x = 0; x < 16; x++)
  {
    blocks[x][y] = 1;
    setPixel(x, y + 3, 1);
  }
}

void displayScore(int score)
{
  char scoreString[10];
  sprintf(scoreString, "Score:%05d", score);
  lcd.setCursor(5, 0);
  lcd.print(scoreString);
}

void displayBalls(int balls)
{
  char ballsString[10];
  sprintf(ballsString, "Balls:%d  ", balls);
  lcd.setCursor(5, 1);
  lcd.print(ballsString);
}


//graphics routines:
void setPixel(int x, int y, int color)
{
  //sanity checks.  SetPixel would trash memory and crash if these were exceeded.
  if((x < 0) || (y < 0) || (x > 19) || (y > 15)) return;
  
  int col = x / 5; //will be 0 through 3
  int row = y >> 3; // div 8.  will be 0 or 1

  int col_inner = x % 5; //0 through 4
  int row_inner = y % 8; // 0 through 7
  col_inner = 4 - col_inner; //col 1 comes before col 2, but is a higher bit in the bitmap (lsb is rightmost).
  
  int charnum = row * 4 + col;
  
  if(color == 1) //set pixel
  {
    graphics[(charnum << 3) + row_inner] |= color << col_inner;
  }
  else // clear pixel if set    
  {
    int bitmask = B11111 - (1 << col_inner);    
    graphics[(charnum << 3) + row_inner] &= bitmask;
  }
  
  lcd.createChar(charnum, &graphics[charnum << 3]);
  //apparently don't need to re-lcd.write, character updates on lcd automatically if line below is used.
  lcd.setCursor(col, row);  
}

void drawLine(int x1, int y1, int x2, int y2, int color)
{
  float incr;
  int xabs = abs(x1 - x2);
  int yabs = abs(y1 - y2);
  if((xabs == 0) && (yabs == 0)) //single pixel, just set it and bail
  {
    setPixel(x1, y1, color);
    return;
  }
  //line between 0 and 19 needs 20 pixels for example, so add one.
  xabs++;
  yabs++;
  
  if (xabs < yabs) //mostly vertical, x increments < 1
  {
    float x = x1 + 0.5;  //add 0.5 to deal with up-rounding vs. down-rounding.
    int yinc = (y1 < y2) ? 1 : -1;

    incr = (float)(x2 - x1) / (float)yabs;
    for(int y = y1; y != y2; y+= yinc)
    {
      setPixel(x, y, color);
      x += incr;
    }
    setPixel(x, y2, color);
  }
  else //mostly horizontal, y increments < 1
  {
    float y = y1 + 0.5;  //add 0.5 to deal with up-rounding vs. down-rounding.
    int xinc = (x1 < x2) ? 1 : -1;

    incr = (float)(y2 - y1) / (float)xabs;
    for(int x = x1; x != x2; x+= xinc)
    {
      setPixel(x, y, color);
      y += incr;
    }
    setPixel(x2, y, color);
  }
}

