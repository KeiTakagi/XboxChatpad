//
// ファイル: XboxChatpad.h
// 概    要: Arduino STM32 Xbox Chatpad Clone ライブラリ用
// 作 成 者: Kei Takagi
// 作 成 日: 2017/08/28
//
// このソースコードは次のソースコードを参考に作成しました
//   たま吉さん      https://github.com/Tamakichi/ArduinoSTM32_PS2Keyboard
//

#include <XboxChatpad.h>

// Xbox Chatpad Keyboard object
XboxChatpad kb;

void setup() {

  // USBシリアル通信の初期設定
  // ※USB経由のシリアル通信を使わない場合は、要修正
  Serial.begin(115200);
  while (!Serial.isConnected()) delay(100);
  
  Serial.println("XboxChatpad Test(v1.0):");

  // Xbox Chatpad キーボードの利用開始
  // begin()の引数： シリアル通信番号
  if ( kb.begin(Serial2)) {
    // 初期化に失敗時はエラーメッセージ表示
    Serial.println("Xbox Chatpad initialize error.");
  }
}

void loop() {
  uint8_t  c;
  keyEvent k; // キー入力情報
  
  // キー入力情報(キーイベント構造体のメンバーは下記の通り)
  // k.code  : アスキーコード or キーコード(TKeyboard.h内の#define参照)
  //            0の場合は入力無し、255の場合はエラー
  // k.BREAK : キー押し情報                  => 0: 押した、　　　　1:離した
  // k.KEY   : キーコード/アスキーコード種別 => 0: アスキーコード、1:キーコード
  // k.SHIFT : SHIFTキー押し判定             => 0: 押していない 、 1:押している
  // k.CTRL  : CTRLキー押し判定              => 0: 押していない 、 1:押している
  // k.ALT   : ALTキー押し判定               => 0: 押していない 、 1:押している
  // k.GUI   : GUI(Windowsキー)押し判定      => 0: 押していない 、 1:押している

  if (Serial.available()) {
    // 動作確認 コマンド
    //  i: PS/2 キーボードの初期化
    //  l: CapsLock、NumLock、ScrolLock用LEDの点灯
    //  m: CapsLock、NumLock、ScrolLock用LEDの消灯

    // シリアルからの入力コマンドチェック
    c = Serial.read();    
    if (c == 'i') {
      // キーボードの初期化(この処理はbegin()後、いつでも呼び出して初期化可能)
      kb.init();          
    } else if (c == 'l') {
      // LEDの点灯 (引数：led_caps,led_num, led_scrol)
      kb.ctrl_LED(1,1,1);  
    } else if (c == 'm') {
      // LEDの消灯 (引数：led_caps,led_num, led_scrol)
      kb.ctrl_LED(0,0,0);  
    }
  }   

  // キーボードからの入力情報取得
  k = kb.read();
  if ( k.code && k.code != KEY_ERROR) {  // ※入力無しの場合(0)、処理エラー(KEY_ERROR)以外の時に入力値の評価を行う
    if (k.BREAK)
      Serial.print("[Break]"); // キーを離した
    if (k.KEY)
      Serial.print("[KEY]");   // codeの内容はキーコードである
    if (k.SHIFT)
      Serial.print("[Shift]"); // シフトキーが同時に押されている
    if (k.CTRL)
      Serial.print("[Ctrl]");  // Ctrlキーが同時に押されている
    if (k.ALT)
      Serial.print("[Alt]");   // Altキーが同時に押されている
    if (k.GUI)
      Serial.print("[Win]");   // Windowsキーが同時に押されている

    if (k.KEY) {
      // 入力キーがASCIIコード化出来ない場合は、キーコードを表示する
      Serial.print("key_code=");
      Serial.println(k.code);
    } else {
      // アスキーコードの場合は、その文字を表示する
      Serial.print("ascii:");
      Serial.write(k.code);
      Serial.println();
    }
  }
}
