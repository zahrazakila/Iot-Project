#include <U8g2lib.h>
#include <Wire.h>

// Inisialisasi untuk OLED 128x64 menggunakan I2C
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup(void) {
  // Memulai komunikasi I2C
  u8g2.begin();
}

void loop(void) {
  u8g2.clearBuffer();                // Menghapus buffer
  u8g2.setFont(u8g2_font_ncenB14_tr); // Mengatur font
  u8g2.drawStr(0, 30, "Zahra Cantikk");  // Menulis teks di buffer
  u8g2.sendBuffer();               // Mengirim buffer ke OLED
  delay(1000);                      // Tunggu selama 1 detik
}
