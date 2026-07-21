#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const uint8_t SCREEN_WIDTH = 128; //スクリーンの大きさ（横）
const uint8_t SCREEN_HEIGHT = 64; //スクリーンの大きさ（縦）
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int VRX_1 = A0;
const int VRY_1 = A1;
const int SW_1 = 2;

const int VRX_2 = A2;
const int VRY_2 = A3;
const int SW_2 = 3;

bool firstCheck_1 = false;
bool firstCheck_2 = false;
bool onceFirst = true;

bool gameStartCheck_1 = false;
bool gameStartCheck_2 = false;

uint8_t phase = 0;
enum PHASE
{
  MAP,
  GAME
};

uint8_t player1Pos = 0;
uint8_t player2Pos = 0;

uint8_t clearPos = 32;

uint8_t mapPhase = 0;
enum MAP_PHASE
{
  PLAYER1_DICE,
  PLAYER1_MOVE,
  PLAYER2_DICE,
  PLAYER2_MOVE
};

bool diceFirst = true;
bool didDice = false;
uint8_t dice = 0;
unsigned long diceEnd = 0;
const uint8_t DICE_END_NONE = 0;
const uint16_t DICE_END_TIME = 1500;
uint8_t moveCount = 0;
unsigned long moveWait = 0;
const uint8_t MOVE_WAIT_NONE = 0;
const uint16_t MOVE_TIME = 750;

unsigned long sendTime = 0;

int diceMove = 0;
int diceCount = 0;
int diceNum_1 = 1;
int diceNum_2 = 1;

const uint8_t GAME_NUM = 1;

bool gameResult = false;

uint8_t phase_instantGame = 0;
enum INSTANT_GAME
{
  IG_READY,
  IG_WAIT,
  IG_SHOW,
  IG_RESULT,
  IG_END
};

class InstantGame{

private:
  int direction;

  unsigned long startTime;
  unsigned long p1Time;
  unsigned long p2Time;

  bool p1Win;
  bool p2Win;

public:

  InstantGame(){
    p1Win = false;
    p2Win = false;
  }

  void readyScreen(){
  display.clearDisplay();

  display.setCursor(0, 0);
  display.println("Ready...");   // 前の画面を消す

  display.display();  //メモリの内容をOLEDに送信

  delay(1000);
  phase_instantGame = IG_WAIT;
  }

  void waitRandom(){
  int wait_time;
  wait_time = random(2000, 5001); //2～5秒の間でランダムに
  delay(wait_time);
  phase_instantGame = IG_SHOW;
  }

  void showDirection(){
    direction = random(4);

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(40, 20);

    switch(direction){
      case 0:
        display.println("UP");
        break;

      case 1:
        display.println("RIGHT");
        break;
      
      case 2:
        display.println("DOWN");
        break;
      
      case 3:
        display.println("LEFT");
        break;
    }

  display.display();
  startTime = millis();
  checkDirection();
  }

  void checkDirection(){
    p1Win = false;
    p2Win = false;

    while(millis() - startTime < 5000){
      int p1 = getDirection(VRX_1, VRY_1);
      int p2 = getDirection(VRX_2, VRY_2);

      if(p1 == direction){
        p1Time = millis() - startTime;
        p1Win = true;
        phase_instantGame = IG_RESULT;
        diceNum_1 = 2;
      }

      if(p2 == direction){
        p2Time = millis() - startTime;
        p2Win = true;
        phase_instantGame = IG_RESULT;
        diceNum_2 = 2;
      }
    }
    phase_instantGame = IG_RESULT;
  } 

  //0:上, 1:右, 2:下, 3:左, 4:例外
  int getDirection(int x, int y)
  {
    int joyX = analogRead(x);
    int joyY = analogRead(y);
    int xBase = joyX - 500;
    int yBase = joyY - 500;
    if(abs(xBase) > abs(yBase))
    {
      if(xBase >= 100)
      {
        return 2;
      }
      else if(xBase <= -100)
      {
        return 0;
      }
    }
    else
    {
      if(yBase >= 100)
      {
        return 1;
      }
      else if(yBase <= -100)
      {
        return 3;
      }
    }
    return 4;
  }
  
  void resultScreen(){
    display.clearDisplay();
    display.setCursor(20,20);

    if(p1Win){
      display.println("P1 WIN");

      display.setCursor(20,40);
      display.print(p1Time);
      display.println(" ms");
    }

    else if(p2Win){
      display.println("P2 WIN");

      display.setCursor(20,40);
      display.print(p2Time);
      display.println(" ms");
    }

    else{
      display.println("TIME UP");
    }

    display.display();
    delay(2000);
    phase_instantGame = IG_END;
  }

};

InstantGame _InstantGame;


void setup()
{
    diceCount = 0;
    diceMove = 0;

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
      Serial.println("OLEDが見つかりません");
      while (1);
    }

    pinMode(SW_1, INPUT_PULLUP);
    pinMode(SW_2, INPUT_PULLUP);
    // for(int i = 0; i < 8; i++)
    // {
    //     pinMode(ANODEPIN[i], OUTPUT);
    //     pinMode(CATHODEPIN[i], OUTPUT);

    //     //ここで全部消灯させる。
    //     digitalWrite(ANODEPIN[i], LOW);
    //     digitalWrite(CATHODEPIN[i], HIGH);
    // }
    Serial.begin(115200);
}



void loop()
{
    bool isPressed_1 = (digitalRead(SW_1) == LOW);
    bool isPressed_2 = (digitalRead(SW_2) == LOW);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    uint8_t playerActivePos = (mapPhase == PLAYER1_DICE || mapPhase == PLAYER1_MOVE) ? player1Pos : player2Pos;
    uint8_t playerNotActivePos = (mapPhase == PLAYER1_DICE || mapPhase == PLAYER1_MOVE) ? player2Pos : player1Pos;
    // Serial.print(playerActivePos);
    // Serial.print(',');
    // Serial.print(playerNotActivePos);
    // Serial.print(',');
    // Serial.println(phase);
    if (millis() - sendTime >= 20)
    {
        sendTime = millis();

        Serial.print(playerActivePos);
        Serial.print(',');
        Serial.print(playerNotActivePos);
        Serial.print(',');
        Serial.println(phase);
    }

    if(!firstCheck_1 && isPressed_1)
    {
      firstCheck_1 = true;
    }
    if(!firstCheck_2 && isPressed_2)
    {
      firstCheck_2 = true;
    }
    if(!firstCheck_1 || !firstCheck_2)
    {
      display.clearDisplay();
      String message_1 = (firstCheck_1) ? "Player1: OK" : "Player1: Waiting";
      String message_2 = (firstCheck_2) ? "Player2: OK" : "Player2: Waiting";
      DrawCenterText(message_1, (SCREEN_HEIGHT / 2) - 10, 1);
      DrawCenterText(message_2, (SCREEN_HEIGHT / 2) + 10, 1);
      display.display();
      return;
    }
    
    if(onceFirst)
    {
      randomSeed(millis());
      onceFirst = false;
    }

    if(player1Pos >= clearPos)
    {
      display.clearDisplay();
      DrawCenterText("Player1", (SCREEN_HEIGHT / 2) - 15, 1);
      DrawCenterText("WIN!!", (SCREEN_HEIGHT / 2) + 10, 2);
      display.display();
      return;
    }
    if(player2Pos >= clearPos)
    {
      display.clearDisplay();
      DrawCenterText("Player2", (SCREEN_HEIGHT / 2) - 15, 1);
      DrawCenterText("WIN!!", (SCREEN_HEIGHT / 2) + 10, 2);
      display.display();
      return;
    }

    switch(phase)
    {
      case MAP:
        switch(mapPhase)
        {
          case PLAYER1_DICE:
            DrawCenterText("Player1", (SCREEN_HEIGHT / 2) - 20, 1);

            if(diceFirst)
            {
              dice = random(1, 7);
              diceFirst = false;
            }

            if((digitalRead(SW_1) == LOW))
            {
              didDice = true;
            }

            if(!didDice)
            {
              uint8_t displayDice = random(1, 7);  //演出用のやつ

              DrawCenterText(String(displayDice), (SCREEN_HEIGHT / 2), 2);
            }
            else
            {
              DrawCenterText(String(dice), (SCREEN_HEIGHT / 2), 2);

              if(diceEnd == DICE_END_NONE)
              {
                diceEnd = millis() + DICE_END_TIME;
              }

              if(millis() >= diceEnd)
              {
                diceMove += dice;
                diceEnd = DICE_END_NONE;
                diceFirst = true;
                didDice = false;
                diceCount++;
                if(diceCount >= diceNum_1)
                {
                  mapPhase = PLAYER1_MOVE;
                  diceNum_1 = 1;
                }
              }
            }
            display.display();
            return;
          case PLAYER1_MOVE:
            DrawCenterText("Player1", (SCREEN_HEIGHT / 2) - 20, 1);
            DrawCenterText(String(diceMove), (SCREEN_HEIGHT / 2), 2);
            if(moveWait == MOVE_WAIT_NONE)
            {
              moveWait = millis() + MOVE_TIME;
              moveCount = 0;
            }
            if(millis() >= moveWait)
            {
              if(moveCount < diceMove)
              {
                moveWait = millis() + MOVE_TIME;
                moveCount++;
                if(player1Pos < clearPos)
                {
                  player1Pos++;
                }
              }
              else
              {
                mapPhase = PLAYER2_DICE;
                diceCount = 0;
                diceMove = 0;
                moveWait = MOVE_WAIT_NONE;
              }
            }
            display.display();
            return;
          case PLAYER2_DICE:
            DrawCenterText("Player2", (SCREEN_HEIGHT / 2) - 20, 1);

            if(diceFirst)
            {
              dice = random(1, 7);
              diceFirst = false;
            }

            if((digitalRead(SW_2) == LOW))
            {
              didDice = true;
            }

            if(!didDice)
            {
              uint8_t displayDice = random(1, 7);  //演出用のやつ

              DrawCenterText(String(displayDice), (SCREEN_HEIGHT / 2), 2);
            }
            else
            {
              DrawCenterText(String(dice), (SCREEN_HEIGHT / 2), 2);

              if(diceEnd == DICE_END_NONE)
              {
                diceEnd = millis() + DICE_END_TIME;
              }
              if(millis() >= diceEnd)
              {
                diceMove += dice;
                diceEnd = DICE_END_NONE;
                diceFirst = true;
                didDice = false;
                diceCount++;
                if(diceCount >= diceNum_2)
                {
                  mapPhase = PLAYER2_MOVE;
                  diceNum_2 = 1;
                }
              }
            }
            display.display();
            return;
          case PLAYER2_MOVE:
            DrawCenterText("Player2", (SCREEN_HEIGHT / 2) - 20, 1);
            DrawCenterText(String(diceMove), (SCREEN_HEIGHT / 2), 2);
            if(moveWait == MOVE_WAIT_NONE)
            {
              moveWait = millis() + MOVE_TIME;
              moveCount = 0;
            }
            if(millis() >= moveWait)
            {
              if(moveCount < diceMove)
              {
                moveWait = millis() + MOVE_TIME;
                moveCount++;
                if(player2Pos < clearPos)
                {
                  player2Pos++;
                }
              }
              else
              {
                mapPhase = PLAYER1_DICE;
                diceCount = 0;
                diceMove = 0;
                moveWait = MOVE_WAIT_NONE;

                phase = GAME;
                gameStartCheck_1 = false;
                gameStartCheck_2 = false;
                phase_instantGame = IG_READY;
              }
            }
            display.display();
            return;
          }
      case GAME:
        if(!gameStartCheck_1 && isPressed_1)
        {
          gameStartCheck_1 = true;
        }
        if(!gameStartCheck_2 && isPressed_2)
        {
          gameStartCheck_2 = true;
        }
        if(!gameStartCheck_1 || !gameStartCheck_2)
        {
          display.clearDisplay();
          String message_1 = (gameStartCheck_1) ? "Player1: OK" : "Player1: Waiting";
          String message_2 = (gameStartCheck_2) ? "Player2: OK" : "Player2: Waiting";

          DrawCenterText("MiniGame", (SCREEN_HEIGHT / 2) - 10, 1);
          DrawCenterText(message_1, (SCREEN_HEIGHT / 2), 1);
          DrawCenterText(message_2, (SCREEN_HEIGHT / 2) + 10, 1);
          display.display();
          return;
        }

        switch(phase_instantGame)
        {
          case IG_READY:
            _InstantGame.readyScreen();
            break;
          case IG_WAIT:
            _InstantGame.waitRandom();
            break;
          case IG_SHOW:
            _InstantGame.showDirection();
            break;
          case IG_RESULT:
            _InstantGame.resultScreen();
            break;
          default:
            break;
        }
        if(phase_instantGame == IG_END)
        {
          phase = MAP;
          mapPhase = PLAYER1_DICE;
        }
        display.display();
        return;
    }
}

void DrawCenterText(String text, int y, int textSize)
{
  int16_t x1, y1;
  uint16_t w, h;

  display.setTextSize(textSize);

  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  int x = (SCREEN_WIDTH - w) / 2;

  display.setCursor(x, y);
  display.print(text);
}