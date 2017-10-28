//
// ファイル: XboxChatpad.cpp
// 概    要: Arduino STM32 Xbox Chatpad Clone ライブラリ用
// このソースコードは次のソースコードを参考に作成しました
//   たま吉さん      https://github.com/Tamakichi/ArduinoSTM32_PS2Keyboard
//   Cliff L. Biffle http://cliffle.com/project/chatpad
// 作 成 者: Kei Takagi
// 作 成 日: 2017/08/28
//
//

// 2017/09/16 修正: 全体的な見直し、受信バケットの異常処理等追加 By Kei Takagi
// 2017/10/28 修正: By Kei Takagi
//            タイマ割り込みを使った監視、矢印キー、キー出力タイミング
//

#include "XboxChatpad.h"

// タイマ割込間隔 1sec
#define RATE 1000000
// 日本語対応
#define JAPAN_KEY 1

static const uint8_t ShiftMask = (1 << 0);
static const uint8_t GreenSquareMask = (1 << 1);
static const uint8_t OrangeCircleMask = (1 << 2);
static const uint8_t PeopleMask = (1 << 3);

// Xbox Chatpad 初期化コマンド
static const uint8_t InitMessage[]      = { 0x87, 0x02, 0x8C, 0x1F, 0xCC };
// Xbox Chatpad 起きているか監視コマンド
static const uint8_t KeepAwakeMessage[]  = { 0x87, 0x02, 0x8C, 0x1B, 0xD0 };

// Normal, Shift  , Ctrl , Alt    , Win
// Normal, Shift  , Green, Orange , People
static const uint8_t AsciiTable[] PROGMEM = {
  '7', '\'', 0 ,  0 ,  0 ,      /* 11 Key7 */
  '6', '&',  0 ,  0 ,  0 ,      /* 12 Key6 */
  '5', '%',  0 ,  0 ,  0 ,      /* 13 Key5 */
  '4', '$',  0 ,  0 ,  0 ,      /* 14 Key4 */
  '3', '#',  0 ,  0 ,  0 ,      /* 15 Key3 */
  '2', '"',  0 ,  0 ,  0 ,      /* 16 Key2 */
  '1', '!',  0 ,  0 ,  0 ,      /* 17 Key1 */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 18 Unused */

  'u', 'U', '&',  0 ,  0 ,      /* 21 KeyU */
  'y', 'Y', '^',  0 ,  0 ,      /* 22 KeyY */
  't', 'T', '%',  0 ,  0 ,      /* 23 KeyT */
  'r', 'R', '#', '$',  0 ,      /* 24 KeyR */
  'e', 'E',  0 ,  0 , KEY_ESC,  /* 25 KeyE */
  'w', 'W', '@',  0 ,  0 ,      /* 26 KeyW */
  'q', 'Q', '!',  0 ,  0 ,      /* 27 KeyQ */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 28 Unused */

  'j', 'J', '\'', '"',  0 ,     /* 31 KeyJ */
  'h', 'H', '/', '\\',  0 ,     /* 32 KeyH */
  'g', 'G',  0 ,  0 ,  0 ,      /* 33 KeyG */
  'f', 'F', '}', '?',  0 ,      /* 34 KeyF */
  'd', 'D', '{',  0 ,  0 ,      /* 35 KeyD */
  's', 'S',  0 ,  0 ,  0 ,      /* 36 KeyS */
  'a', 'A', '~',  0 ,  0 ,      /* 37 KeyA */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 38 Unused */

  'n', 'N', '<',  0 ,  0 ,      /* 41 KeyN */
  'b', 'B', '|', '+',  0 ,      /* 42 KeyB */
  'v', 'V', '-', '_',  0 ,      /* 43 KeyV */
  'c', 'C', KEY_ESC,  0 ,  0 ,  /* 44 KeyC */
  'x', 'X',  0 ,  0 ,  0 ,      /* 45 KeyX */
  'z', 'Z', '`',  0 ,  0 ,      /* 46 KeyZ */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 47 Unused */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 48 Unused */

  KEY_RIGHTARROW,  0 , KEY_UPARROW,  0 ,  0 , /* 51 KeyRight */
  'm', 'M', '>',  0 ,  0 ,      /* 52 KeyM */
  '.', '.', '?',  0 ,  0 ,      /* 53 KeyPeriod */
  ' ', ' ', ' ', ' ',  0 ,      /* 54 KeySpace  */
  KEY_LEFTARROW,  0 , KEY_DOWNARROW,  0 ,  0 , /* 55 KeyLeft */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 56 Unused */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 57 Unused */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 58 Unused */

   0 ,  0 ,  0 ,  0 ,  0 ,      /* 61 Unused */
  ',', ',', ':', ';',  0 ,      /* 62 KeyComma */
  KEY_ENTER,  0 ,  0 ,  0 ,  0 ,/* 63 KeyEnter */
  'p', 'P', ')', '=',  0 ,      /* 64 KeyP */
  '0',  0 ,  0 ,  0 ,  0 ,      /* 65 Key0 */
  '9', ')',  0 ,  0 ,  0 ,      /* 66 Key9 */
  '8', '(',  0 ,  0 ,  0 ,      /* 67 Key8 */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 68 Unused */

  KEY_BACKSPACE, KEY_DELETE,  0 ,  0 ,  0 , /* 71 KeyBackspace */
  'l', 'L', ']',  0 , KEY_CTRLL,/* 72 KeyL */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 73 Unused */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 74 Unused */
  'o', 'O', '(',  0 ,  0 ,      /* 75 KeyO */
  'i', 'I', '*',  0 ,  0 ,      /* 76 KeyI */
  'k', 'K', '[',  0 ,  0 ,      /* 77 KeyK */
   0 ,  0 ,  0 ,  0 ,  0        /* 78 Unused */
};

void handler_getup(void);

//
// 利用開始(初期化)
// 引数
//   serial    : シリアル番号
// 戻り値
//  0:正常終了 0以外: 異常終了
uint8_t XboxChatpad::begin(HardwareSerial &serial) {
  uint8_t i, err = 0;
  err = init();

  if ( err != 0 )goto ERROR;

  _serial = &serial;

  // 電源投入後500ms以上待って
  // InitMessage（87 02 8C 1F CC）を送信する必要がある
  // 私が行った実験ではXbox Chatpad clone (TYX-517PCB1 VER2.5 2015-12-08)
  // は、初期化伝文を2回以上送信しないと起動しない
  // Microsoft純正のチャットパッドでは1回で動作する。
  //
  // あなたの環境で、チャットパッドクローンが動作しない場合
  // 初期化の回数、電源投入後の時間を調整する必要がある
  //
  // Cliff L. Biffle氏は500ms待たずに初期を1回だけ実行している
  // これはMicrosoft純正のチャットパッドで試しているためだと考えられる
  //
  // チャットパッドクローンの場合、キーボードが使用できるまで電源ONから約2～5秒必要
  //

  _serial->begin(19200);
  while (!_serial) delay(100);
  delay(500);
  for (i = 0; i < 3; i++) {
    _serial->write(InitMessage, sizeof(InitMessage));
    delay(30);
  }
  Timer3.resume(); // タイマ割込開始

ERROR:
  return err;
}

// 利用終了
// 引数
//  なし
// 戻り値
//  なし
//
void XboxChatpad::end() {
  if (_serial == NULL)return;
  _serial->end();
  Timer3.pause();                   // タイマ停止
  Timer3.detachCompare1Interrupt(); // コンパレータ1解放
  return;
}

// キーボード初期化
// 引数
//  なし
// 戻り値
//  0:正常終了、 0以外:異常終了
//
uint8_t XboxChatpad::init() {
  _serial = NULL;
  _last_key0 = 0;
  _last_key1 = 0;
 
  // タイマーの設定
  Timer3.pause();                                // タイマ停止
  Timer3.setChannel1Mode(TIMER_OUTPUTCOMPARE);   //
  Timer3.setPeriod(RATE);                        // in microseconds
  Timer3.setCompare1(1);                         // overflow might be small
  Timer3.attachCompare1Interrupt(handler_getup); // コンパレータ1にて割り込み発生
	Timer3.refresh();                              // タイマ更新

  return 0;
}

// シリアルポートに何バイトのデータが到着しているかを返す
// 戻り値 シリアルバッファにあるデータのバイト数を返す
int XboxChatpad::available(void) {
  if (_serial == NULL)return 0;
  return _serial->available();
}

// 入力キー情報の取得(CapsLock、NumLock、ScrollLockを考慮)
// 仕様
//  ・入力したキーに対応するASCIIコード文字またはキーコードと付随する情報を返す
//    - エラーまたは入力なしの場合、下位8ビットに以下の値を設定する
//     0x00で入力なし、0xFFでエラー
// 引数
//  なし
// 戻り値: 入力情報
// キー入力情報(キーイベント構造体のメンバーは下記の通り)
// k.code  : アスキーコード or キーコード(AsciiTable参照)
//            0の場合は入力無し、255の場合はエラー
// k.BREAK : キー押し情報                  => 0: 押した、        1:離した
// k.KEY   : キーコード/アスキーコード種別 => 0: アスキーコード、1:キーコード
// k.SHIFT : SHIFTキー押し判定             => 0: 押していない 、 1:押している
// k.CTRL  : CTRLキー押し判定              => 0: 押していない 、 1:押している
// k.ALT   : ALTキー押し判定               => 0: 押していない 、 1:押している
// k.GUI   : GUI(Windowsキー)押し判定      => 0: 押していない 、 1:押している
//
keyEvent XboxChatpad::read() {
  keyinfo  in;  // キーボード状態
  uint8_t i, code, checksum = 0, err;
  uint16_t index;
  int len;

  if (_serial == NULL)goto ERROR;

  // キーコードの初期化
  in.value = KEY_NONE;
  in.kevt.KEY = 1;

  // チャットパッドは、最大2つの同時キーを検出可能
  // 押された1番目のキーのキーコードは4バイト目
  // 2つのキーが押されていると、
  // 押された2番目のキーのキーコードは5バイト目に格納
  len = available();
  if (8 <= len ) {
    //Chatpadからシリアル通信で8バイトのバケットを受取る
    err = 1;
    for (i = 0; i < 8; i++) {
      //受信データを1バイト読み込みますが、バッファ中の読み取り位置は変更しない
      //受信データから0xB4を見つけるまでパケットを捨てる
      if ( _serial->peek() == 0xB4 ) {
        if (i == 0) {
          err = 0;
          break;
        }
      } else {
        _serial->read();
      }
    }
    if (err == 1)goto ERROR;

    //Chatpadからシリアル通信で8バイトのバケットを受取る
    for (i = 0; i < 8; i++) _buffer[i] = _serial->read();
    //0xA5で始まる「ステータスレポート」パケットは使い方が不明なので破棄する
    //0xB4から始まらないパケットは無視する
    //2バイト目が0xC5以外のパケットも無視する
    if (_buffer[1] != 0xC5) goto ERROR;

    //チェックサムの確認
    //7バイト目はチェックサム
    //0-6バイトを合計し、結果を否定（2の補数）することによって計算する
    for (i = 0; i < 7; i++) checksum += _buffer[i];
    checksum = -checksum;
    if (checksum != _buffer[7])goto ERROR;

    //正常なパケットの処理
    uint8_t modifiers = _buffer[3];
    uint8_t key0      = _buffer[4];
    uint8_t key1      = _buffer[5];

    code = 0;
    if (key0 && key0 != _last_key0 && key0 != _last_key1) {
      code = key0;
      in.kevt.BREAK = 0; //0:押した
    }
    if (key1 && key1 != _last_key0 && key1 != _last_key1) {
      code = key1;
      in.kevt.BREAK = 0; //0:押した
    }
    if (_last_key0 && _last_key0 != key0 && _last_key0 != key1) {
      code = _last_key0;
      in.kevt.BREAK = 1; //1:離した
    }
    if (_last_key1 && _last_key1 != key0 && _last_key1 != key1) {
      code = _last_key1;
      in.kevt.BREAK = 1; //1:離した
    }
    _last_key0 = key0;
    _last_key1 = key1;

    code = code - 0x11;
    index = ((code & 0x70) >> 1) | (code & 0x7);
    if (index >= (sizeof(AsciiTable) / 5)) goto ERROR;

    in.kevt.SHIFT = 0;  // Shiftキー
    in.kevt.CTRL  = 0;  // Ctrlキー
    in.kevt.ALT   = 0;  // Altキー
    in.kevt.GUI   = 0;  // Windowsキー
    if ( modifiers & ShiftMask )in.kevt.SHIFT      = 1; // Shiftキー ON
    if ( modifiers & GreenSquareMask )in.kevt.CTRL = 1; // Ctrlキー ON
    if ( modifiers & OrangeCircleMask )in.kevt.ALT = 1; // Altキー ON
    if ( modifiers & PeopleMask )in.kevt.GUI       = 1; // Windowsキー ON

    //Xbox Chatpadの入力値を文字コードに変換
    index = index * 5;

    if (in.kevt.SHIFT == 1)index += 1;
    else if (in.kevt.CTRL == 1)index += 2;
    else if (in.kevt.ALT == 1) index += 3;
    else if (in.kevt.GUI == 1) index += 4;
    in.kevt.code = pgm_read_byte_near(AsciiTable + index);
    if (0x20 <= in.kevt.code && in.kevt.code <= 0x7e)in.kevt.KEY = 0;//表示可能文字コード
  }

  goto DONE;
ERROR:
  in.value = KEY_ERROR;
DONE:
  return in.kevt;
}

// キーボードLED点灯設定
// 引数
//  swCaps : CapsLock   LED制御 0:off 1:on
//  swNum  : NumLock    LED制御 0:off 1:on
//  swScrol: ScrollLock LED制御 0:off 1:on
// 戻り値
//  0:正常 1:異常
//
uint8_t XboxChatpad::ctrl_LED(uint8_t swCaps, uint8_t swNum, uint8_t swScrol) {
  return 0;
}

// 監視コマンド送信
// 引数
//  なし
// 戻り値
//  なし
//
void handler_getup(void) {
  if (_serial == NULL)return;
  // KeepAwakeMessageを定期的に送信する必要がある。
  // 送信しない場合、チャットパッドはスリープ状態に戻る
  // 毎秒KeepAwakeMessageを送信する
  if (_serial == NULL)return;
  // 監視コマンド送信
  _serial->write(KeepAwakeMessage, sizeof(KeepAwakeMessage));
}

