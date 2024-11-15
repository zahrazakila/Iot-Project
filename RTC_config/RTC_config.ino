#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu"};

void setup() {
  Serial.begin(9600);
  delay(3000); // Waktu tunggu untuk memastikan RTC siap

  Wire.begin(); // Mulai I2C
  if (!rtc.begin()) {
    Serial.println("Tidak dapat menemukan RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC kehilangan daya! Atur waktu dari komputer.");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // Atau atur waktu secara manual jika diperlukan:
    // rtc.adjust(DateTime(2019, 1, 25, 10, 30, 0)); // Contoh: 25 Januari 2019, jam 10:30:00
  }
}

void loop() {
  DateTime now = rtc.now();

  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]); // Hari
  Serial.print(", ");
  Serial.print(now.day(), DEC); // Tanggal
  Serial.print('/');
  Serial.print(now.month(), DEC); // Bulan
  Serial.print('/');
  Serial.print(now.year(), DEC); // Tahun
  Serial.print(' ');
  Serial.print(now.hour(), DEC); // Jam
  Serial.print(':');
  Serial.print(now.minute(), DEC); // Menit
  Serial.print(':');
  Serial.print(now.second(), DEC); // Detik
  Serial.println();

  delay(1000); // Tunggu 1 detik sebelum memperbarui waktu lagi
}
