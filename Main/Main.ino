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

unsigned long diceWait = 0;
const uint8_t DICE_WAIT_NONE = 0;
const uint16_t DICE_TIME = 1000;
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
      DrawCenterText("Player1", (SCREEN_HEIGHT / 2) - 15, 1);
      DrawCenterText("WIN!!", (SCREEN_HEIGHT / 2) + 10, 2);
      display.display();
    }
    if(player2Pos >= clearPos)
    {
      DrawCenterText("Player2", (SCREEN_HEIGHT / 2) - 15, 1);
      DrawCenterText("WIN!!", (SCREEN_HEIGHT / 2) + 10, 2);
      display.display();
    }

    switch(phase)
    {
      case MAP:
        switch(mapPhase)
        {
          case PLAYER1_DICE:
            DrawCenterText("Player1", (SCREEN_HEIGHT / 2) - 20, 1);
            if(diceWait == DICE_WAIT_NONE)
            {
              diceWait = millis() + DICE_TIME;
              dice = random(1, 7);  //1から6までの乱数
            }
            if(millis() < diceWait)
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
                diceWait = DICE_WAIT_NONE;
                diceCount++;
                if(diceCount >= diceNum_1)
                {
                  mapPhase = PLAYER1_MOVE;
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
            if(diceWait == DICE_WAIT_NONE)
            {
              diceWait = millis() + DICE_TIME;
              dice = random(1, 7);  //1から6までの乱数
            }
            if(millis() < diceWait)
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
                diceWait = DICE_WAIT_NONE;
                diceCount++;
                if(diceCount >= diceNum_2)
                {
                  mapPhase = PLAYER2_MOVE;
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
              }
            }
            display.display();
            return;
          }
      case GAME:
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