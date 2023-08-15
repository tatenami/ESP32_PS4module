//#include <ps4.h>
//#include <ps4_int.h>
#include <PS4Controller.h>

uint8_t buf[8], pre_buf[8], check_sum = 0;
int8_t stick[4];
bool change_check_flag = 0;

// 電文データの初期化
void ps4_init() {
  buf[0] = 0x80;
  buf[1] = 0x00;
  buf[2] = 0x00;
  buf[7] = 0x00;
  for (int i = 3; i < 7; i++) {
    buf[i] = 0x40;
  }

  for (int i = 0; i < 8; i++) {
    pre_buf[i] = buf[i];
  }
}

void setup() { 
  //while(!Serial){}
  ps4_init();
  Serial.begin(115200);
  PS4.begin("0c:b8:15:77:09:d4"); // ESP32マイコン・DS4共通のMACアドレス
  while (!PS4.isConnected()) {}
  delay(1000);
  //TxSerial.begin(9600);

  //PS4.setLed(0, 0xff, 0); // 緑色に変更
  //PS4.sendToController();
}

void loop() {

  // Below has all accessible outputs from the controller
  if (PS4.isConnected()) {
    buf[2] = (PS4.Up() * 0x01) + (PS4.Down() * 0x02)
             + (PS4.Right() * 0x04) + (PS4.Left() * 0x08)
             + (PS4.Triangle() * 0x10) + (PS4.Cross() * 0x20)
             + (PS4.Circle() * 0x40) + (PS4.Options() * 0x03)
             + (PS4.Share() * 0x0c);

    buf[1] = (PS4.Square() * 0x01) + (PS4.L1() * 0x02)
             + (PS4.L2() * 0x04) + (PS4.R1() * 0x08)
             + (PS4.R2() * 0x10);

    stick[0] = PS4.LStickX();
    stick[1] = PS4.LStickY();
    stick[2] = PS4.RStickX();
    stick[3] = PS4.RStickY();

    for (int a = 0; a < 4; a++) {
      if (abs(stick[a]) < 7) buf[a + 3] = 0x40;
      else buf[a + 3] = (stick[a] + 128) / 2;
    }

    for (int i = 1; i < 7; i++) {
      check_sum += buf[i];
    }
    buf[7] = check_sum;
    check_sum = 0;

    for (int a = 0; a < 8; a++) {
      if (pre_buf[a] != buf[a]) {
        change_check_flag = 1;
        break;
      }
    }

    for (int i = 0; i < 8; i++) {
      pre_buf[i] = buf[i];
    }

    if (change_check_flag) {
      Serial.write(buf, 8);
      delay(10);
      Serial.write(buf, 8);
      change_check_flag = 0;
    }

    //delay(20);
  }
}
