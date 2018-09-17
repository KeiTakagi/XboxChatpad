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
// 2018/09/17 修正: 豊四季Tiny Basic 0.86対応 By Kei Takagi
//

#ifndef __XBOXCHATPAD_H__
#define __XBOXCHATPAD_H__

#include <stdint.h>
#include "Arduino.h"
#include "HardwareSerial.h"

// 電源投入からの待ち時間
// チャットパットは電源投入後500ms以上待って
// 監視コマンドを送信する必要がある
#define STARTWAIT 500

// 日本語表記のチャットパット対応
// #define JAPAN_KEY 0 ←英語版(デフォルト)
// #define JAPAN_KEY 1 ←日本語版
#define JAPAN_KEY 0

// タイマー割り込みを1:使う 0:使わない
#define TIMER 1

// 豊四季TinyBasick個別バージョン対応
// #define TTBASIC086 0 ←豊四季TinyBasic 0.85以前
// #define TTBASIC086 1 ←豊四季TinyBasic 0.86以降 デフォルト
#define TTBASIC086 1

// デバッグを1:使う 0:使わない
#define DEBUG 0

#if TIMER == 1
#include "wiring_private.h"
#include "wirish_time.h"
#endif

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

#if TTBASIC086 == 0
//豊四季TinyBasic 0.85以前
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

#else
//豊四季TinyBasic 0.86以降
#define KEY_RIGHTARROW    0x1E
#define KEY_LEFTARROW     0x1F
#define KEY_UPARROW       0x1C
#define KEY_DOWNARROW     0x1D
#define KEY_DELETE        127
#define KEY_HOME          0x01
#define KEY_INSERT        134
#define KEY_PAGEDOWN      135
#define KEY_PAGEUP        136
#define KEY_END           0x0F

#endif




#if TIMER == 1
// タイマ割込間隔 1sec
#define RATE 1000000
#endif

#if DEBUG == 1
#define LED_PIN PC13
uint8_t toggle;
#endif


// Xbox Chatpad 初期化コマンド
static const uint8_t InitMessage[]      = { 0x87, 0x02, 0x8C, 0x1F, 0xCC };
// Xbox Chatpad 監視コマンド
static const uint8_t KeepAwakeMessage[]  = { 0x87, 0x02, 0x8C, 0x1B, 0xD0 };

#if JAPAN_KEY == 1
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

  'u', 'U', '(', '+',  0 ,      /* 21 KeyU */
  'y', 'Y', '&','\'',  0 ,      /* 22 KeyY */
  't', 'T', '%', '|',  0 ,      /* 23 KeyT */
  'r', 'R', '$',  0 ,  0 ,      /* 24 KeyR */
  'e', 'E', '#',  0 , KEY_ESC,  /* 25 KeyE */
  'w', 'W', '"','\'',  0 ,      /* 26 KeyW */
  'q', 'Q', '_', '@',  0 ,      /* 27 KeyQ */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 28 Unused */

  'j', 'J','\\', '{',  0 ,      /* 31 KeyJ */
  'h', 'H', ']', '>',  0 ,      /* 32 KeyH */
  'g', 'G', '[', '<',  0 ,      /* 33 KeyG */
  'f', 'F','\\', '.',  0 ,      /* 34 KeyF */
  'd', 'D',  0 ,  0 ,  0 ,      /* 35 KeyD */
  's', 'S',  0 ,  0 ,  0 ,      /* 36 KeyS */
  'a', 'A',  0 ,  0 ,  0 ,      /* 37 KeyA */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 38 Unused */

  'n', 'N','\'',  0 ,  0 ,      /* 41 KeyN */
  'b', 'B', '?',  0 ,  0 ,      /* 42 KeyB */
  'v', 'V', '!',  0 ,  0 ,      /* 43 KeyV */
  'c', 'C', KEY_ESC,  0 ,  0 ,  /* 44 KeyC */
  'x', 'X',  0 ,  0 ,  0 ,      /* 45 KeyX */
  'z', 'Z',  0 ,  0 ,  0 ,      /* 46 KeyZ */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 47 Unused */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 48 Unused */

  KEY_LEFTARROW,  0 , KEY_DOWNARROW,  0 ,  0 , /* 51 KeyLeft */
  'm', 'M', '.', ',',  0 ,      /* 52 KeyM */
  '.',  0 ,  0 ,  0 ,  0 ,      /* 53 Keyカタ */
  ' ', ' ', ' ', ' ',  0 ,      /* 54 KeySpace  */
  KEY_BACKSPACE, KEY_DELETE,  0 ,  0 ,  0 , /* 55 Backspace */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 56 Unused */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 57 Unused */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 58 Unused */

   0 ,  0 ,  0 ,  0 ,  0 ,      /* 61 Unused */
  ',',  0 ,  0 ,  0 ,  0 ,      /* 62 Key漢字 */
  KEY_ENTER, 0 ,  0 ,  0 ,  0 ,/* 63 KeyEnter */
  'p', 'P', '-', '=',  0 ,      /* 64 KeyP */
  '0',  0 ,  0 ,  0 ,  0 ,      /* 65 Key0 */
  '9', ')',  0 ,  0 ,  0 ,      /* 66 Key9 */
  '8', '(',  0 ,  0 ,  0 ,      /* 67 Key8 */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 68 Unused */

  KEY_RIGHTARROW,  0 , KEY_UPARROW,  0 ,  0 , /* 71 KeyRight */ 
  'l', 'L', ':', ';', KEY_CTRLL,/* 72 KeyL */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 73 Unused */
   0 ,  0 ,  0 ,  0 ,  0 ,      /* 74 Unused */
  'o', 'O', '^', '~',  0 ,      /* 75 KeyO */
  'i', 'I', ')', '*',  0 ,      /* 76 KeyI */
  'k', 'K', '/', '}',  0 ,      /* 77 KeyK */
   0 ,  0 ,  0 ,  0 ,  0        /* 78 Unused */
};
#else

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
  'y', 'Y', '`',  0 ,  0 ,      /* 22 KeyY */
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
  KEY_ENTER, 0 ,  0 ,  0 ,  0 , /* 63 KeyEnter */
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
#endif

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