#Arduino STM32 Xbox Chatpad Clone ���C�u����

���܂��A����r���ł��B�啝�Ɏd�l���ύX�ɂȂ�\��������܂��B���������������B

���̃��C�u�����[�́AXBOX Chatpad clone(TYX-517PCB1 Ver2.5)��Arduino������STM32F103���V���A���Őڑ����A�{�^����������K�v�ȃL�����N�^�[�R�[�h�������́A�L�[�R�[�h�֕ϊ����܂��B
Xbox Chatpad��3.3V�œ��삵�܂��B
3.3V�œ��삷��Arduino DUE�ŁA���삷�邩������܂��񂪊m�F�͂��Ă��܂���B

���̃��C�u�����͎����Q�l�ɍ쐬���܂���
   ���܋g����      https://github.com/Tamakichi/ArduinoSTM32_PS2Keyboard
   Cliff L. Biffle http://cliffle.com/project/chatpad


�L�[�{�[�h�̒�`��ύX�������ꍇ��XboxChatpad.cpp��sAsciiTable[] ��ύX���Ă��������B
�z��̏��Ԃ́ANormal, shifted, Green, orange , People�ɂȂ�܂��B


STM32F103�̃s����Serial2�ł���PA2(TX),�APA3(RX)���g�p���Ă��܂��B
Chatpad��RX(in)�s����STM32F103��TX(out)�s���ցAChatpad��TX(out)�s����STM32F103��RX(in)�s���֐ڑ����܂��B
�܂��AChatpad��3.3V��GND�����l��STM32F103��3.3V��GND�֐ڑ����Ă��������B

��{�I�Ȏg�����̃T���v���́AXboxChatpad_exsample.ino�ɒu���Ă����܂��B
Chatpad�͓d����������2�b�قǂ͎g�p�ł��܂���B�����ӂ��������B

TynyBasic�ł�Serial����̓��͂�APS/2�L�[�{�[�h������ێg�p����ۂ́ASirialx.available() Sirialx.read()�ƒu�������邱�ƂɂȂ邩�Ǝv���܂��B
�g�p����Q�l�ɒu���ւ��Ă��������B

�g�p��j
#include <XboxChatpad.h>

XboxChatpad kb;

inline char c_getch(void) {
  char c = 0;
  keyEvent k = kb.read();
  if ( k.code && k.code != KEY_ERROR) {
     if (k.BREAK)c = k.code;
  }
  return c;
}
#define c_kbhit( ) kb.available()


Sirialx.available() �� c_kbhit()�Œu������
Sirialx.read()      �� c_getch()�Œu������



