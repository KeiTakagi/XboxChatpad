# Arduino STM32 Xbox Chatpad ライブラリ

***※本バージョンはβ版です.大幅に仕様変更する可能性があります.***

- [豊四季TinyBASIC Ver0.86β](https://github.com/Tamakichi/ttbasic_arduino_stm32)に対応しました <span style="color:red;">**(NEW)**</span>
- [日本語表記](#日本語表記)のチャットパッドに対応しました


## これは何？
Xbox ChatpadをArduino化したSTM32F103でキーボードとして使うためのライブラリです.
Microsoft製のXbox Chatpadで動作しますが、Microsoft製のXbox Chatpadが入手しにくいため、比較的入手しやすいクローン(例えば、TYX-517PCB1 Ver2.5)が代替品として使用出来ます.  
Microsoft製のXbox Chatpadは、クローンより高速に起動します.  

****<span style="color:red;">※動画のように豊四季 Tiny BASICで動作させるたためには、豊四季 Tiny BASICの[改造](#改造)が必要です.本ライブラリを置き換えただけではTiny BASICでの動作はできません.ご注意ください.</span>****

****写真をクリックすると動画再生します****

[![操作動画](./img/xboxchatpad001.jpg)](https://www.youtube.com/watch?v=LU9IOJWQZ0k&vl=ja)

本ライブラリはSTM32F103CBを使用したBlue Pill、Black Pillで動作の確認をしています.
本ライブラリは次を参考に作成しています.
> - たま吉さん      https://github.com/Tamakichi/ArduinoSTM32_PS2Keyboard
> - Cliff L. Biffle http://cliffle.com/project/chatpad

## 著作権/使用条件(License)
**このアーカイブに含まれるソースコード、テキスト及びデータ**の著作権は**Kei Takagi**が所有しています.  
Xbox Chatpadの改造や、本アプリケーションの使用は、使用者個人の責任のもとに行ってください.  
本アプリケーションを実行した結果について何らかの不具合が発生したとしても、作者は一切の責任を負いませんので、あらかじめご了承ください.  
著作権者の同意なしに金銭を伴う再配布を行う事を固く禁じます.  
この条件のもとで、利用、複写、改編、再配布を認めます.  

**「豊四季 Tiny BASIC」**の著作権は開発者の**Tetsuya Suzuki**氏にあります.  
「豊四季 Tiny BASIC」を使用した場合のプログラム利用については、オリジナル版の著作権者の配布条件に従うものとします.  
著作権者の同意なしに経済的な利益を得てはいけません.  
この条件のもとで、利用、複写、改編、再配布を認めます.  

## Xbox Chatpadの改造
### 1.ケース
![ケース](./img/xboxchatpad002.jpg)
### 2.前面
![前面](./img/xboxchatpad003.jpg)
### 3.背面
![背面](./img/xboxchatpad004.jpg)
### 4.分解します
![配線１](./img/xboxchatpad004a.jpg)
### 5.基盤を取り出します
![配線１](./img/xboxchatpad005.jpg)
### 6.不要な配線を取り除きます.必要な配線は以下の写真を参考にしてください.
写真では、取り外したネジとワッシャーで止めていますが、キーボードの中心のボタンを強く押すと、意図しないキー反応があります.  
キーボードを押しても曲がらないよう、硬い板を当てるなど工夫してください.
![配線２](./img/xboxchatpad006.jpg)

配線ですが、上から、
- ブラック : 3.3Vへ接続
- オレンジ : STM32F103のSerial2であるPA2へ接続
- グレー : STM32F103のSerial2であるPA3へ接続
- グリーン : GNDへ接続

になります.
****<span style="color:red;">（配線の色に惑わされないよう注意）</span>****

## 使い方
基本的な使い方のサンプルは、XboxChatpad_exsample.inoに置いておきます.  
最初にサンプルでXbox Chatpadの動作を確認してから豊四季 Tiny BASIC等で使用してください.  
Xbox Chatpad クローンの場合、電源投入から2～5秒程使用出来ない時間があります.  
気になる方はXboxChatpad.hを修正してください.  

<a name="改造"></a>

## たま吉さんの豊四季 Tiny BASIC V0.86βで使用したい場合
****[豊四季 Tiny BASIC for Arduino STM32 V0.86β](https://github.com/Tamakichi/ttbasic_arduino_stm32)の手順に沿ってインストールしたあと、[差替ファイル](https://github.com/KeiTakagi/XboxChatpad/blob/master/ttbasic_difference/ps22tty.cpp)を差し替えてください.****


## キーボードに印刷されていないキー操作について

|**動作**|**操作**|
|--------|--------|
|上移動|緑□ + →|
|下移動|緑□ + ←|
|実行中のプログラム停止1|緑□ + C|
|実行中のプログラム停止2|人 + E|
|画面初期化|人 + L|
|!|shift + 1|
|"|shift + 2|
|#|shift + 3|
|$|shift + 4|
|%|shift + 5|
|&|shift + 6|
|'|shift + 7|
|(|shift + 8|
|)|shift + 9|

<a name="日本語表記"></a>

## 日本語表記のXbox Chatpadを使用する場合
「カナひら」「漢字」を押すとキーボードの印刷にない「.」「,」を表示します.
XboxChatpad.hの下記のように『JAPAN_KEY 0』(英語版)から『JAPAN_KEY 1』（日本語版）へ修正してください
```
// 日本語表記のチャットパット対応
// #define JAPAN_KEY 0 ←英語版(デフォルト)
// #define JAPAN_KEY 1 ←日本語版
#define JAPAN_KEY 1
```

## キーボードの定義を変えたい場合
キーボードの定義を変更したい場合はXboxChatpad.hのsAsciiTable[] を変更してください.  
配列の順番は、Normal, shif, Green, orange , Peopleになります.


## 豊四季 Tiny BASIC以外で使いたい場合
Serialからの入力や、PS/2キーボードライブラリと置き換えて使用する際は、Sirialx.available() Sirialx.read()と置き換えることになるかと思います.  
使用例を参考に置き換えてください.
あわせて、[豊四季TinyBasic Ver0.85以前のバージョンで使用する場合](#TTBASIC086)も対応してください.
```
使用例）

#include <XboxChatpad.h>

XboxChatpad kb;
inline char c_getch(void) {
  uint8_t c = 0;
  keyEvent k = kb.read();
  if ( k.code && k.code != KEY_ERROR) {
    if (k.BREAK)c = k.code; // キーを離した
  }
  return c;
}
#define c_kbhit( ) kb.available()

Sirialx.available() ← c_kbhit()で置き換え
Sirialx.read()      ← c_getch()で置き換え
```

<a name="TTBASIC086"></a>
## 豊四季TinyBasic Ver0.85以前のバージョンで使用する場合
デフォルトは豊四季TinyBasic Ver0.86以降の設定にしています。
XboxChatpad.hの下記のように『TTBASIC086 1』(豊四季TinyBasic 0.86以降)から『TTBASIC086 0』（豊四季TinyBasic 0.85以前）へ修正してください
```
// 豊四季TinyBasick個別バージョン対応
// #define TTBASIC086 0 ←豊四季TinyBasic 0.85以前
// #define TTBASIC086 1 ←豊四季TinyBasic 0.86以降 デフォルト
#define TTBASIC086 0
```

## 問題点
- キーリピートを実装したところ、処理中にキーを離した状態を取得出来ないため見送ります.そのため、キーを離したタイミングで文字を表示する仕様とします.


## 今後の予定について
- なし