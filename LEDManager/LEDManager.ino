int ANODEPIN[8] = {10, 11, 12, 14, 15, 16, 17, 18};
int CATHODEPIN[8] = {2, 3, 4, 5, 6, 7, 8, 9};

int playerActivePos = 0;
int playerNotActivePos = 0;

const int BRIGHTNESS_LEVEL = 16;
const int PLAYER_ACTIVE = 15;
const int PLAYER_NOT_ACTIVE = 10;
const int PLAYER_IN_GAME = 13;
const int MAP_BRIGHTNESS = 4;

int phase = 0;
enum PHASE
{
  MAP,
  GAME
};

bool mapData[8][8] =
{
  {0, 0, 0, 1, 1, 1, 0, 1},
  {0, 0, 0, 1, 0, 1, 0, 1},
  {0, 0, 0, 1, 0, 1, 0, 1},
  {1, 1, 1, 1, 0, 1, 0, 1},
  {1, 0, 0, 0, 0, 1, 0, 1},
  {1, 0, 0, 1, 1, 1, 0, 1},
  {1, 0, 0, 1, 0, 0, 0, 1},
  {1, 0, 0, 1, 1, 1, 1, 1}
};

struct Pos
{
  uint8_t y;
  uint8_t x;
};

//超絶ゴリ押し
Pos playerRoute[33] = 
{
  {7, 0}, {6, 0}, {5, 0}, {4, 0}, {3, 0}, {3, 1}, {3, 2}, {3, 3}, {2, 3}, {1, 3}, {0, 3}, {0, 4}, {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5},
  {5, 4}, {5, 3}, {6, 3}, {7, 3}, {7, 4}, {7, 5}, {7, 6}, {7, 7}, {6, 7}, {5, 7}, {4, 7}, {3, 7}, {2, 7}, {1, 7}, {0, 7}
};

char buffer[32];
uint8_t index = 0;

void setup()
{
  Serial.begin(115200);
	for(int ano = 0; ano < 8; ano++)
	{
		pinMode(ANODEPIN[ano], OUTPUT);
		digitalWrite(ANODEPIN[ano], LOW);
	}
	for(int cat = 0; cat < 8; cat++)
	{
		pinMode(CATHODEPIN[cat], OUTPUT);
		digitalWrite(CATHODEPIN[cat], HIGH);
	}
}

void loop()
{
    // while (Serial.available() > 0)
    // {
    //   char id = Serial.read();
    //   int num = Serial.parseInt();

    //   if(id == '1')
    //   {
    //     playerActivePos = num;
    //   }
    //   else if(id == '2')
    //   {
    //     playerNotActivePos = num;
    //   }
    //   else if(id == '3')
    //   {
    //     phase = num;
    //   }
    // }

    while (Serial.available())
    { 
      char c = Serial.read();

      if (c == '\r') continue;

      if (c == '\n')
      {
        buffer[index] = '\0';

        int active, notActive, p;

        if (sscanf(buffer, "%d,%d,%d", &active, &notActive, &p) == 3)
        {
            playerActivePos = active;
            playerNotActivePos = notActive;
            phase = p;
        }

        index = 0;
      }
      else if (index < sizeof(buffer) - 1)
      {
        buffer[index++] = c;
      }
    }

    for(int pwm = 0; pwm < BRIGHTNESS_LEVEL; pwm++)
    {
      for(int row = 0; row < 8; row++)
      {
          // 全消灯
          for(int i = 0; i < 8; i++)
          {
              digitalWrite(ANODEPIN[i], LOW);
              digitalWrite(CATHODEPIN[i], HIGH);
          }

          // 明るさ判定
          for(int col = 0; col < 8; col++)
          {
              int brightNum = mapData[row][col] * MAP_BRIGHTNESS;
              if(row == playerRoute[playerNotActivePos].y && col == playerRoute[playerNotActivePos].x)
              {
                brightNum = (phase == MAP) ? PLAYER_NOT_ACTIVE : PLAYER_IN_GAME;
              }
              if(row == playerRoute[playerActivePos].y && col == playerRoute[playerActivePos].x)
              {
                brightNum = (phase == MAP) ? PLAYER_ACTIVE : PLAYER_IN_GAME;
              }

              if(brightNum > pwm)
              {
                  digitalWrite(ANODEPIN[col], HIGH);
              }
          }

          digitalWrite(CATHODEPIN[row], LOW);

          delayMicroseconds(50);

          digitalWrite(CATHODEPIN[row], HIGH);
      }
    }
}