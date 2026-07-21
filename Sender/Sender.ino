// 仕様：ROW側がカソード、COLOMN側がアノードの8x8マトリックスLED
int ANODEPIN[8]   = { 10, 11, 12, 14, 15, 16, 17, 18 };
int CATHODEPIN[8] = {  2,  3,  4,  5,  6,  7,  8,  9 };

void setup()
{
  Serial.begin(9600);

	// アノードを、LOWで初期化
	for( int ano = 0; ano < 8; ano++ )
	{
		pinMode( ANODEPIN[ano], OUTPUT );
		digitalWrite( ANODEPIN[ano], LOW );
	}

	// カソードを、HIGHで初期化
	for( int cat = 0; cat < 8; cat++ )
	{
		pinMode( CATHODEPIN[cat], OUTPUT );
		digitalWrite( CATHODEPIN[cat], HIGH );
	}
}

void loop()
{
	// カソードのループ
	for( int cat = 0; cat < 8; cat++ )
	{
		digitalWrite( CATHODEPIN[cat], LOW );	// LOWに変更
		// アノードのループ
		for( int ano = 0; ano < 8; ano++ )
		{
			digitalWrite( ANODEPIN[ano], HIGH );	// HIGHに変更
			delay(100);	//点灯時間
			digitalWrite( ANODEPIN[ano], LOW );	// LOWに戻す
		}
		digitalWrite( CATHODEPIN[cat], HIGH );	// HIGHに戻す
	}
}