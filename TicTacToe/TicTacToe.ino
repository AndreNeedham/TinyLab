//Tic-Tac-Toe game for TinyLab
//Andre Needham

//Play Tic-Tac-Toe against the computer.  use the S1 and S2 buttons to move the cursor (dot) to the square you want to play in, then press S3.  Then the computer moves.  Alternate turns until you either lose or draw.  The computer plays well enough to never lose.

#include <LiquidTWI2.h> 

#define LEFT_BUTTON 9
#define RIGHT_BUTTON 8
#define SELECT_BUTTON A5

LiquidTWI2 lcd(0x20);

byte graphics[64]; //8 characters, 8 bytes of data each.

void drawLine(int x1, int y1, int x2, int y2, int color);
void setPixel(int x, int y, int color);

int squareState[3][3]; // 0 = off, 1 = player, 2 = computer

int moves = 0;
int markerX = 1;
int markerY = 1;
bool buttonPressed = false;

void setup() {  
  randomSeed(analogRead(0));

  digitalWrite(13, LOW); //turn off LED1
  digitalWrite(4, HIGH); //turn off SD light, it's flickering for some reason

  pinMode(LEFT_BUTTON, INPUT);
  pinMode(RIGHT_BUTTON, INPUT);
  pinMode(SELECT_BUTTON, INPUT);

  lcd.setMCPType(LTI_TYPE_MCP23008);  
  lcd.begin(16, 2);
  lcd.setBacklight(HIGH); 

  ResetGame();  
}

void loop() {
  bool userSelected = false;
  int button;
  int nextX, nextY;

  button = GetUserInput();
  switch(button)
  {
    case 0: //nothing
    break;

    case 1: //marker left
       ClearMarker(markerX, markerY);
       GetPrevFreeSquare(nextX, nextY);
       markerX = nextX;
       markerY = nextY;
       SetMarker(markerX, markerY);
       WaitForNoUserInput();
    break;

    case 2: //marker right
       ClearMarker(markerX, markerY);
       GetNextFreeSquare(nextX, nextY);
       markerX = nextX;
       markerY = nextY;
       SetMarker(markerX, markerY);
       WaitForNoUserInput();
    break;

    case 3: //select
       userSelected = true;
       WaitForNoUserInput();
    break;
  }
  
  if(userSelected)
  {
    ClearMarker(markerX, markerY);

    squareState[markerX][markerY] = 1;
    DrawX(markerX, markerY);
    moves++;
      
    delay(200);
      
    if(CheckForWin())
    {
      FlashWinner(1);
      ResetGame();
    }
    else if (moves == 5) //tie
    {
      FlashWinner(3);
      ResetGame();
    }
    else
    {
      DoComputerMove();
      GetNextFreeSquare(markerX, markerY);
      SetMarker(markerX, markerY);
      if(CheckForWin())
      {
        FlashWinner(2);
        ResetGame();
      }
    }
  }  
}

void DoComputerMove()
{
  bool done = false;
  
  if(moves == 1)
  {
    //if first player move isn't the center, take the center
    if(squareState[1][1] == 0)
    {
      squareState[1][1] = 2;
      DrawO(1, 1);
    }
    else // take any corner.  A side could equal a loss later.
    {
      int row = random(0, 2) << 1;
      int col = random(0, 2) << 1;
      
      squareState[row][col] = 2;
      DrawO(row, col);
    }
  }
  
  if(moves == 2)
  {
    done = false;
    // if player has 2 in a row, block it
    done = BlockPlayerWin();
    
    // look for any doubling traps.
    if(!done)
    {
      // Corner-opposite corner.  Take any side.  Corner would lead to loss.
      if (((squareState[0][0] == 1) && (squareState[2][2] == 1)) ||
          ((squareState[0][2] == 1) && (squareState[2][0] == 1)))
      {
        int row = random(0,3);
        int col = 1;
        if(row == 1) col = random(0,2) << 1;
        squareState[row][col] = 2;
        DrawO(row, col);
        done = true;
      }
      
      // Side-corner. Take inside corner.
      //top-center plus corner
      if(!done && (squareState[0][1] == 1))
      {
        if(squareState[2][0] == 1)
        {
          squareState[0][0] = 2;
          DrawO(0, 0);
          done = true;
        }
        else if (squareState[2][2] == 1)
        {
          squareState[0][2] = 2;
          DrawO(0, 2);
          done = true;
        }
      }
      
      //bottom -center plus corner
      if(!done && (squareState[2][1] == 1))
      {
        if(squareState[0][0] == 1)
        {
          squareState[2][0] = 2;
          DrawO(2, 0);
          done = true;
        }
        else if (squareState[0][2] == 1)
        {
          squareState[2][2] = 2;
          DrawO(2, 2);
          done = true;
        }
      }
      
      //center-left plus corner
      if(!done && (squareState[1][0] == 1))
      {
        if(squareState[0][2] == 1)
        {
          squareState[0][0] = 2;
          DrawO(0, 0);
          done = true;
        }
        else if (squareState[2][2] == 1)
        {
          squareState[2][0] = 2;
          DrawO(2, 0);
          done = true;
        }
      }
      
      //center-right plus corner
      if(!done && (squareState[1][2] == 1))
      {
        if(squareState[0][0] == 1)
        {
          squareState[0][2] = 2;
          DrawO(0, 2);
          done = true;
        }
        else if (squareState[2][0] == 1)
        {
          squareState[2][2] = 2;
          DrawO(2, 2);
          done = true;
        }
      }
      
      // Center-corner (if computer has opposite corner).  Take either remaining corner.
      if(!done)
      {
        if(squareState[1][1] == 1)
        {
          if(((squareState[0][0] == 2) && (squareState[2][2] == 1)) || ((squareState[0][0] == 1) && (squareState[2][2] == 2)))
          {
            if(random(0,2) == 1)
            {
              squareState[0][2] = 2;
              DrawO(0, 2);
            } 
            else
            {
              squareState[2][0] = 2;
              DrawO(2, 0);
            }
            done = true;
          }
          else if(((squareState[0][2] == 2) && (squareState[2][0] == 1)) || ((squareState[0][2] == 1) && (squareState[2][0] == 2)))
          {
            if(random(0,2) == 1)
            {
              squareState[0][0] = 2;
              DrawO(0, 0);
            }
            else
            {
              squareState[2][2] = 2;
              DrawO(2, 2);
            }
            done = true;
          }
        }
      }
    
      // Otherwise take any random spot
      if (!done)
      {
        int row = 1;
        int col = 1;
        do
        {
          row = random(0, 3);
          col = random(0, 3);
        }
        while(squareState[row][col] > 0);
      
        squareState[row][col] = 2;
        DrawO(row, col);
      }
    }
  }
  
  if(moves >= 3)
  {
    done = false;
    //if we have 2 in a row and can get 3 to win, do it
    done = FindComputerWin();
    
    //if player has 2 in a row, block it
    if(!done) done = BlockPlayerWin();
    
    // take any random spot
    if(!done)
    {
      int row = 1;
      int col = 1;
      do
      {
        row = random(0, 3);
        col = random(0, 3);
      }
      while(squareState[row][col] > 0);
      
      squareState[row][col] = 2;
      DrawO(row, col);
    }
  }
}

bool FindComputerWin()
{
  bool done = false;
  
  //down diag
  if((squareState[0][0] == 2) && (squareState[1][1] == 2) && (squareState[2][2] == 0))
  {
    squareState[2][2] = 2;
    DrawO(2, 2);
    done = true;
  }
  else if((squareState[2][2] == 2) && (squareState[1][1] == 2) && (squareState[0][0] == 0))
  {
    squareState[0][0] = 2;
    DrawO(0, 0);
    done = true;
  }
  
  //up diag
  if(!done)
  {
    if((squareState[0][2] == 2) && (squareState[1][1] == 2) && (squareState[2][0] == 0))
    {
      squareState[2][0] = 2;
      DrawO(2, 0);
      done = true;
    }
    else if((squareState[2][0] == 2) && (squareState[1][1] == 2) && (squareState[0][2] == 0))
    {
      squareState[0][2] = 2;
      DrawO(0, 2);
      done = true;
    }
  }
  if (done) return done;
  
  //rows
  for(int row = 0; row < 3; row++)
  {
    if((squareState[row][0] == 2) && (squareState[row][1] == 2) && (squareState[row][2] == 0))
    {
      squareState[row][2] = 2;
      DrawO(row, 2);
      done = true;
      break;
    }
    else if((squareState[row][0] == 2) && (squareState[row][1] == 0) && (squareState[row][2] == 2))
    {
      squareState[row][1] = 2;
      DrawO(row, 1);
      done = true;
      break;
    }
    else if((squareState[row][0] == 0) && (squareState[row][1] == 2) && (squareState[row][2] == 2))
    {
      squareState[row][0] = 2;
      DrawO(row, 0);
      done = true;
      break;
    }
  }
  if (done) return done;

  //cols
  for(int col = 0; col < 3; col++)
  {
    if((squareState[0][col] == 2) && (squareState[1][col] == 2) && (squareState[2][col] == 0))
    {
      squareState[2][col] = 2;
      DrawO(2, col);
      done = true;
      break;
    }
    else if((squareState[0][col] == 2) && (squareState[1][col] == 0) && (squareState[2][col] == 2))
    {
      squareState[1][col] = 2;
      DrawO(1, col);
      done = true;
      break;
    }
    else if((squareState[0][col] == 0) && (squareState[1][col] == 2) && (squareState[2][col] == 2))
    {
      squareState[0][col] = 2;
      DrawO(0, col);
      done = true;
      break;
    }
  }
  return done;
}

bool BlockPlayerWin()
{
  bool done = false;
  
  //down diag
  if((squareState[0][0] == 1) && (squareState[1][1] == 1) && (squareState[2][2] == 0))
  {
    squareState[2][2] = 2;
    DrawO(2, 2);
    done = true;
  }
  else if((squareState[2][2] == 1) && (squareState[1][1] == 1) && (squareState[0][0] == 0))
  {
    squareState[0][0] = 2;
    DrawO(0, 0);
    done = true;
  }
  
  //up diag
  if(!done)
  {
    if((squareState[0][2] == 1) && (squareState[1][1] == 1) && (squareState[2][0] == 0))
    {
      squareState[2][0] = 2;
      DrawO(2, 0);
      done = true;
    }
    else if((squareState[2][0] == 1) && (squareState[1][1] == 1) && (squareState[0][2] == 0))
    {
      squareState[0][2] = 2;
      DrawO(0, 2);
      done = true;
    }
  }
  if (done) return done;
  
  //rows
  for(int row = 0; row < 3; row++)
  {
    if((squareState[row][0] == 1) && (squareState[row][1] == 1) && (squareState[row][2] == 0))
    {
      squareState[row][2] = 2;
      DrawO(row, 2);
      done = true;
      break;
    }
    else if((squareState[row][0] == 1) && (squareState[row][1] == 0) && (squareState[row][2] == 1))
    {
      squareState[row][1] = 2;
      DrawO(row, 1);
      done = true;
      break;
    }
    else if((squareState[row][0] == 0) && (squareState[row][1] == 1) && (squareState[row][2] == 1))
    {
      squareState[row][0] = 2;
      DrawO(row, 0);
      done = true;
      break;
    }
  }
  if (done) return done;

  //cols
  for(int col = 0; col < 3; col++)
  {
    if((squareState[0][col] == 1) && (squareState[1][col] == 1) && (squareState[2][col] == 0))
    {
      squareState[2][col] = 2;
      DrawO(2, col);
      done = true;
      break;
    }
    else if((squareState[0][col] == 1) && (squareState[1][col] == 0) && (squareState[2][col] == 1))
    {
      squareState[1][col] = 2;
      DrawO(1, col);
      done = true;
      break;
    }
    else if((squareState[0][col] == 0) && (squareState[1][col] == 1) && (squareState[2][col] == 1))
    {
      squareState[0][col] = 2;
      DrawO(0, col);
      done = true;
      break;
    }
  }
  return done;
}


bool CheckForWin()
{
  for(int player = 1; player < 3; player++)
  {
    if(squareState[0][0] == player)
    {
      if((squareState[1][1] == player) && (squareState[2][2] == player)) return true; // right-down diag
      if((squareState[1][0] == player) && (squareState[2][0] == player)) return true; //col 1
      if((squareState[0][1] == player) && (squareState[0][2] == player)) return true; // row 1
    }
    if((squareState[1][0] == player) && (squareState[1][1] == player) && (squareState[1][2] == player)) return true; //row 2
    if((squareState[2][0] == player) && (squareState[2][1] == player) && (squareState[2][2] == player)) return true; //row 3
    if((squareState[0][1] == player) && (squareState[1][1] == player) && (squareState[2][1] == player)) return true; //col 2
    if((squareState[0][2] == player) && (squareState[1][2] == player) && (squareState[2][2] == player)) return true; //col 3

    if((squareState[2][0] == player) && (squareState[1][1] == player) && (squareState[0][2] == player)) return true; //right-up diag
  }
  return false;
}

void FlashWinner(int playerOrComp)
{
  lcd.setCursor(5, 0);
  if(playerOrComp == 1)
  {
    lcd.print("You win!");
  }
  else if (playerOrComp == 2)
  {
    lcd.print("You lose.");
  }
  else  //tie
  {
    lcd.print("It's a tie");
  }
  delay(2000);
  lcd.setCursor(5, 0);
  lcd.print("          ");
}

void ResetGame()
{
  moves = 0;
  markerX = 1; markerY = 1;

  //erase the squareState array
  for(int row = 0; row < 3; row++)
      for(int col = 0; col < 3; col++)
        squareState[row][col] = 0;

  //erase graphics
  for(int index = 0; index < 64; index++) graphics[index] = 0;

  // define 8 custom characters using our (now blank) bit data
  for (int i = 0; i < 8; ++i)
  {
    lcd.createChar(i, &graphics[i << 3]);
  }
  
  //set the first 4 characters of the 2 rows to our 8 custom characters
  lcd.setCursor(0, 0);
  for(int i = 0; i < 4; i++) lcd.write((char)i);
  lcd.setCursor(0, 1);
  for(int i = 4; i < 8; i++) lcd.write((char)i);

  //draw grid
  drawLine(5, 0, 5, 15, 1);
  drawLine(11, 0, 11, 15, 1);
  drawLine(0, 5, 15, 5, 1);
  drawLine(0, 11, 15, 11, 1);

  //draw marker
  SetMarker(markerX, markerY);
}

int GetUserInput()
{
  if (digitalRead(LEFT_BUTTON) == LOW) return 1;
  if (digitalRead(RIGHT_BUTTON) == LOW) return 2;
  if ((analogRead(SELECT_BUTTON) > 100) && (analogRead(SELECT_BUTTON) < 300)) return 3;

  return 0;
}

void SetMarker(int x, int y)
{
   setPixel((x * 6) + 2, (y * 6) + 2, 1);
}

void ClearMarker(int x, int y)
{
   setPixel((x * 6) + 2, (y * 6) + 2, 0);
}

void GetPrevFreeSquare(int& nextX, int& nextY)
{
   int tempX = markerX;
   int tempY = markerY;
   
   do{
     tempX--;
     if(tempX == -1)
     {
       tempX = 2;
       tempY--;
       if(tempY == -1) tempY = 2;
     }
   }while (squareState[tempX][tempY] > 0);

   nextX = tempX;
   nextY = tempY;
}

void GetNextFreeSquare(int& nextX, int& nextY)
{
   int tempX = markerX;
   int tempY = markerY;
   
   do{
     tempX++;
     if(tempX == 3)
     {
       tempX = 0;
       tempY++;
       if(tempY == 3) tempY = 0;
     }
   }while (squareState[tempX][tempY] > 0);

   nextX = tempX;
   nextY = tempY;
}

void DrawX(int x, int y)
{
   int startX = x * 6 + 1;
   int startY = y * 6 + 1;
   setPixel(startX, startY, 1);
   setPixel(startX + 2, startY, 1);
   setPixel(startX, startY + 2, 1);
   setPixel(startX + 1, startY + 1, 1);
   setPixel(startX + 2, startY + 2, 1);
}

void DrawO(int x, int y)
{
   int startX = x * 6 + 1;
   int startY = y * 6 + 1;
   drawLine(startX, startY, startX + 2, startY, 1);
   drawLine(startX, startY + 2, startX + 2, startY + 2, 1);
   setPixel(startX, startY + 1, 1);
   setPixel(startX + 2, startY + 1, 1);
}

void WaitForNoUserInput()
{
  while(GetUserInput() > 0);
  delay(50);  //button debounce
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
