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
// 2017/10/29 修正: By Kei Takagi
//            ・キーリピート、キークリック時点での表示の廃止：
//              理が重いと受信バケットを取り逃がすため、キーを離したタイミングがとれない
//              テスト機以外の構成でキーを押したときのチャタリングが起きやすい
//            ・日本語表記のチャットパット対応対応
// 2017/10/31 修正: SD使用時にフリーズ回避のためタイマ割り込の方法をオーバーフローへ変更 By Kei Takagi
// 2018/09/17 修正: 豊四季Tiny Basic 0.86対応 By Kei Takagi
//

#include "XboxChatpad.h"

static const uint8_t ShiftMask = (1 << 0);
static const uint8_t GreenSquareMask = (1 << 1);
static const uint8_t OrangeCircleMask = (1 << 2);
static const uint8_t PeopleMask = (1 << 3);

#if TIMER == 1
void handler_getup(void);
#endif

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
  delay(STARTWAIT);
  for (i = 0; i < 3; i++) {
    _serial->write(InitMessage, sizeof(InitMessage));
    delay(30);
  }
#if TIMER == 1
Timer3.resume(); // タイマ割込開始
#endif
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
  if (!_serial)return;
  _serial->end();
#if TIMER == 1
Timer3.pause();                   // タイマ停止
#endif
  _serial = NULL;
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

#if TIMER == 1
  Timer3.pause();                   // タイマー停止
  Timer3.setPrescaleFactor(7200);   // システムクロック 72MHzを10ｋHzに分周
  Timer3.setOverflow(10000);        // 最大値を1秒に設定
 
  Timer3.attachInterrupt(           // 割り込みハンドラの登録
      TIMER_UPDATE_INTERRUPT,       // 呼び出し条件は、カウンターオーバーフロー更新時
      handler_getup                 // 呼び出す関数
    );  

  Timer3.setCount(0);               // カウンタを0に設定
  Timer3.refresh();                 // タイマ更新
  Timer3.resume();                  // タイマースタート

#endif

#if DEBUG == 1
pinMode(LED_PIN, OUTPUT);
#endif
  return 0;
}

// シリアルポートに何バイトのデータが到着しているかを返す
// 戻り値 シリアルバッファにあるデータのバイト数を返す
int XboxChatpad::available(void) {
  int ret=0;
  if (_serial)ret = _serial->available();
  return ret;
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

  if (!_serial)goto ERROR;

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
    for (i = 0; i < 8; i++)_buffer[i] = _serial->read();
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

    if (code < 0x11)goto ERROR;
    code = code - 0x11;

    _last_key0 = key0;
    _last_key1 = key1;

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
#if TIMER == 0
  GetUp();
#endif
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
#if TIMER == 1
void handler_getup(void) {
  // KeepAwakeMessageを定期的に送信する必要がある。
  // 送信しない場合、チャットパッドはスリープ状態に戻る
  // 毎秒KeepAwakeMessageを送信する
  // 監視コマンド送信
#if DEBUG == 1
  toggle ^= 1;
  digitalWrite(LED_PIN, toggle);
#endif
  if (_serial)_serial->write(KeepAwakeMessage, sizeof(KeepAwakeMessage));
}
#endif

// 起きろコマンド送信
void XboxChatpad::GetUp() {
#if TIMER == 0
  // KeepAwakeMessageを定期的に送信する必要がある。
  // 送信しない場合、チャットパッドはスリープ状態に戻る
  // 毎秒KeepAwakeMessageを送信する
  // 監視コマンド送信
  if(!_serial)return;
  uint32_t time = millis();
  if (time - _last_ping > 1000) {
#if DEBUG == 1
    toggle ^= 1;
    digitalWrite(LED_PIN, toggle);
#endif
    _last_ping = time;
    _serial->write(KeepAwakeMessage, sizeof(KeepAwakeMessage));
  }
#endif
}