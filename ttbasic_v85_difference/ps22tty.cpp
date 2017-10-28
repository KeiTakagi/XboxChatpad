//
// チャットパッドの入力
//

#include <XboxChatpad.h>
#include "tTVscreen.h"

XboxChatpad kb;

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
	  if (k.BREAK)c = k.code;
	}
  return c;
}
