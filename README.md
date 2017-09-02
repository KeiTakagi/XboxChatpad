# Arduino STM32 Xbox Chatpad Clone ライブラリ   
**※ベータ版です。大幅に仕様が変更になる可能性があります。ご了承ください。**  
このライブラリーは、XBOX Chatpad clone(TYX-517PCB1 Ver2.5)とArduino化したSTM32F103をシリアルで接続し、ボタン押下情報を必要なキャラクターコードもしくは、キーコードへ変換します。  
Xbox Chatpadは3.3Vで動作します。
3.3Vで動作するArduino DUEで動作するかもしれませんが確認はしていません。

このライブラリは次を参考に作成しました  
	たま吉さん      https://github.com/Tamakichi/ArduinoSTM32_PS2Keyboard  
	Cliff L. Biffle http://cliffle.com/project/chatpad  
  
## STM32F103との接続について ##
STM32F103のピンはSerial2であるPA2(TX),、PA3(RX)を使用しています。 
ChatpadのRX(in)ピンをSTM32F103のTX(out)ピンへ、ChatpadのTX(out)ピンをSTM32F103のRX(in)ピンへ接続します。 
また、Chatpadの3.3VとGNDも同様にSTM32F103の3.3VとGNDへ接続してください。 
  
## キーボードの定義について ##  
キーボードの定義を変更したい場合はXboxChatpad.cppのsAsciiTable[] を変更してください。
配列の順番は、Normal, shifted, Green, orange , Peopleになります。

## 注意事項 ##  
Chatpadは電源投入から2～5秒ほどは使用できません。
　　
## 基本的な使い方のサンプルについて ##  
基本的な使い方のサンプルは、XboxChatpad_exsample.inoに置いておきます。 
　　
## Serialxを使ったキーボードの入力と本ライブラリを置き換える場合について ##  
TynyBasicではSerialからの入力や、PS/2キーボードから実際使用する際は、Sirialx.available() Sirialx.read()と置き換えることになるかと思います。 
使用例を参考にして置き替えてください。 

使用例）
#include <XboxChatpad.h>  

XboxChatpad kb;  

inline char c_getch(void) {  
  char c = 0;  
  keyEvent k = kb.read();  
  if ( k.code && k.code != KEY_ERROR) {  
     if (k.BREAK)c = k.code;//キーが押された時のキャラクターコードもしくは、キーコードを取得
  }  
  return c;  
}  
#define c_kbhit( ) kb.available()  
  
Sirialx.available() ← c_kbhit()で置き換え  
Sirialx.read()      ← c_getch()で置き換え  

