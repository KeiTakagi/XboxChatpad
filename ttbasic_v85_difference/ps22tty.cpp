//
// チャットパッドの入力
// （キーリピート対応）
//

#include <XboxChatpad.h>
#include "tTVscreen.h"

//ボタンを押してから2個目の文字が表示するまでのカウント
#define REPEATCNT1 120000
//ボタンのリピートカウント
#define REPEATCNT2 10000

XboxChatpad kb;
static boolean keyDown = false;
static char keyCode = 0;
static uint32_t Nexttime;

void setupPS2(uint8_t kb_type = false) {
  // チャットパッドのセットアップ
  kb.begin(Serial2);
}

void endPS2() {
  kb.end();
}

// キー入力文字の取得
uint8_t ps2read() {
  char c=0;
  keyEvent k; // キー入力情報 
  k = kb.read();
  if ( k.code && k.code != KEY_ERROR) {
    if (k.BREAK==0){
      //キーを押した
      keyDown = true;
      keyCode = k.code;
      c = keyCode;
      Nexttime = REPEATCNT1;
    }else{
      //キーを離した
      keyDown = false;
      keyCode = 0; 
    }
  }
  if(0<Nexttime)Nexttime--;
  if(keyDown == true && Nexttime == 0){
    c = keyCode;
    Nexttime=REPEATCNT2;
  }
  return c;
}
