// Arduino入門編㉖ UART通信について！
// https://burariweb.info
// LED点灯テスト①(受信側スケッチ)
 
const int LED_PIN = 13;
 
void setup(){
  
  Serial.begin(9600);                // シリアル通信の開始(ボーレート9600bps)
  pinMode(LED_PIN, OUTPUT);
}

 
void loop(){
 
  while(Serial.available() > 0){    // 入力された文字が何バイトか調べその回数分繰り返す
    int val = Serial.read();        // 1バイト分のデータを読み取る
    
    if(val == '1'){                 // "1"ならLEDを点灯,"0"ならLEDを消灯
      digitalWrite(LED_PIN, HIGH);
    } else if(val == '0'){
      digitalWrite(LED_PIN, LOW);
    }
  }
}