//
// ファイル: XboxChatpad.h
// 概    要: Arduino STM32 Xbox Chatpad Clone ライブラリ用
// このソースコードは次のソースコードを参考に作成しました
//   たま吉さん      https://github.com/Tamakichi/ArduinoSTM32_PS2Keyboard
//   Cliff L. Biffle http://cliffle.com/project/chatpad
// 作 成 者: Kei Takagi
// 作 成 日: 2017/08/28
//
//

// 2017/09/16 修正, 全体的な見直し、受信バケットの異常処理等追加 By Kei Takagi
// 2017/10/28 修正: By Kei Takagi
//            タイマ割り込みを使った監視、矢印キー、キー出力タイミング
// 2017/10/29 修正: By Kei Takagi
//            キーリピート、キークリック時点での表示の廃止：
//            理が重いと受信バケットを取り逃がすため、キーを離したタイミングがとれない
//            テスト機以外の構成でキーを押したときのチャタリングが起きやすい
// 2017/10/31 修正: SD使用時にフリーズ回避のためタイマ割り込の方法をオーバーフローへ変更 By Kei Takagi
//

#ifndef __XBOXCHATPAD_H__
#define __XBOXCHATPAD_H__

#include <stdint.h>
#include "HardwareSerial.h"
#include "wiring_private.h"
#include "wirish_time.h"
#include "Arduino.h"

// ハードウェアシリアルポート
static HardwareSerial *_serial;

// 状態管理用
#define BREAK_CODE       0x0100  // BREAKコード
#define KEY_CODE         0x0200  // KEYコード
#define SHIFT_CODE       0x0400  // SHIFTあり
#define CTRL_CODE        0x0800  // CTRLあり
#define ALT_CODE         0x1000  // ALTあり
#define GUI_CODE         0x2000  // GUIあり

#define KEY_NONE          0      // 継続またはバッファに変換対象なし
#define KEY_ERROR         255    // キーコードエラー

#define KEY_CTRLC         3
#define KEY_BACKSPACE     8
#define KEY_TAB           9
#define KEY_LINE_FEED     10
#define KEY_CTRLL         12
#define KEY_ENTER         13
#define KEY_ESC           27
#define KEY_RIGHTARROW    131
#define KEY_LEFTARROW     130
#define KEY_UPARROW       129
#define KEY_DOWNARROW     128

#define KEY_DELETE        127
#define KEY_HOME          132
#define KEY_INSERT        134
#define KEY_PAGEDOWN      135
#define KEY_PAGEUP        136
#define KEY_END           137

// キーボードイベント構造体
typedef struct  {
  uint8_t code  : 8; // code
  uint8_t BREAK : 1; // BREAKコード
  uint8_t KEY   : 1; // KEYコード判定
  uint8_t SHIFT : 1; // SHIFTあり
  uint8_t CTRL  : 1; // CTRLあり
  uint8_t ALT   : 1; // ALTあり
  uint8_t GUI   : 1; // GUIあり
  uint8_t dumy  : 2; // ダミー
} keyEvent;

// キーボードイベント共用体
typedef union {
  uint16_t  value;
  keyEvent  kevt;
} keyinfo;

// クラス定義
class XboxChatpad {
  private:
    uint8_t _buffer[8];
    uint8_t _last_key0;
    uint8_t _last_key1;
    uint32_t _last_ping;
    uint8_t toggle;
  public:
    // キーボード利用開始
    uint8_t begin(HardwareSerial &);
    // キーボード利用終了
    void end();
    // キーボード初期化
    uint8_t init();
    // シリアルポートに何バイトのデータが到着しているかを返す
    int available(void);
    // キーボード入力の読み込み
    keyEvent read();
    // キーボード上LED制御
    uint8_t ctrl_LED(uint8_t swCaps, uint8_t swNum, uint8_t swScrol);
    // 監視コマンド送信
    void GetUp();
};

#endif