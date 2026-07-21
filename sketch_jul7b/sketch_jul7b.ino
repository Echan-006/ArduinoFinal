const int pressurePin = A0;
#define BUZZER 8

//イカジャンプ

void setup()
{
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  int pressureValue = analogRead(pressurePin);

  Serial.println(pressureValue);
  int frequency = map(pressureValue, 0, 1023, 200, 2000);

    if (pressureValue > 20) // 少しでも押されたら鳴らす
    {
        tone(BUZZER, frequency);
    }
    else
    {
        noTone(BUZZER);
    }

  delay(50);
}