#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <RtcDS3231.h>
#include <Wire.h>

// Menginisialisasi RTC menggunakan library RtcDS3231
RtcDS3231<TwoWire> Rtc(Wire);

// Konfigurasi DNS
const byte DNS_PORT = 53;
IPAddress apIP(10, 10, 10, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

// Definisi pin untuk buzzer dan LED
#define buzzer D8
#define LED1_PIN D7
#define LED2_PIN D5
#define BUTTON_PIN D6

String hr, minut, sec; // Variabel untuk menyimpan jam dan menit alarm
int hr1, minut1, sec1;
String new_time;

// Status alarm
bool alarmActive = false;

// Halaman HTML untuk tampilan awal
String home_screen = 
   "<!DOCTYPE html><html lang=\"en\">"
   "<head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
   "<title>IoT Alarm Clock</title>"
   "<style>"
   "body {font-family: Arial, sans-serif; background-color: #f0f0f0; margin: 0; padding: 0;}"
   ".container {max-width: 600px; margin: 50px auto; padding: 20px; background-color: #fff; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);}"
   "h1 {text-align: center; color: #333;}"
   "form {text-align: center;}"
   "select {width: 20%; padding: 10px; margin: 10px 0; border-radius: 5px; border: 1px solid #ccc; box-sizing: border-box;}"
   "input[type=\"submit\"] {width: 40%; background-color: #4caf50; color: #fff; padding: 10px; border: none; border-radius: 5px; cursor: pointer; transition: background-color 0.3s ease;}"
   "input[type=\"submit\"]:hover {background-color: #45a049;}"
   "</style>"
   "</head>"
   "<body>"
   "<div class=\"container\">"
   "<h1>IoT Alarm Clock</h1>"
   "<form action=\"/confirmation_screen.php\" method=\"get\">"
   "<h2>Please select your next Alarm time:</h2>"
   "<div>"
   "<label for=\"hour\">Hour:</label><br>"
   "<select name=\"hour\" id=\"hour\">"
   "<option value=\"00\">00</option>"
   "<option value=\"01\">01</option>"
   "<option value=\"02\">02</option>"
   "<option value=\"03\">03</option>"
   "<option value=\"04\">04</option>"
   "<option value=\"05\">05</option>"
   "<option value=\"06\">06</option>"
   "<option value=\"07\">07</option>"
   "<option value=\"08\">08</option>"
   "<option value=\"09\">09</option>"
   "<option value=\"10\">10</option>"
   "<option value=\"11\">11</option>"
   "<option value=\"12\">12</option>"
   "<option value=\"13\">13</option>"
   "<option value=\"14\">14</option>"
   "<option value=\"15\">15</option>"
   "<option value=\"16\">16</option>"
   "<option value=\"17\">17</option>"
   "<option value=\"18\">18</option>"
   "<option value=\"19\">19</option>"
   "<option value=\"20\">20</option>"
   "<option value=\"21\">21</option>"
   "<option value=\"22\">22</option>"
   "<option value=\"23\">23</option>"
   "</select>"
   "</div>"
   "<div>"
   "<label for=\"minute\">Minute:</label><br>"
   "<select name=\"minute\" id=\"minute\">"
   "<option value=\"00\">00</option>"
   "<option value=\"01\">01</option>"
   "<option value=\"02\">02</option>"
   "<option value=\"03\">03</option>"
   "<option value=\"04\">04</option>"
   "<option value=\"05\">05</option>"
   "<option value=\"06\">06</option>"
   "<option value=\"07\">07</option>"
   "<option value=\"08\">08</option>"
   "<option value=\"09\">09</option>"
   "<option value=\"10\">10</option>"
   "<option value=\"11\">11</option>"
   "<option value=\"12\">12</option>"
   "<option value=\"13\">13</option>"
   "<option value=\"14\">14</option>"
   "<option value=\"15\">15</option>"
   "<option value=\"16\">16</option>"
   "<option value=\"17\">17</option>"
   "<option value=\"18\">18</option>"
   "<option value=\"19\">19</option>"
   "<option value=\"20\">20</option>"
   "<option value=\"21\">21</option>"
   "<option value=\"22\">22</option>"
   "<option value=\"23\">23</option>"
   "<option value=\"24\">24</option>"
   "<option value=\"25\">25</option>"
   "<option value=\"26\">26</option>"
   "<option value=\"27\">27</option>"
   "<option value=\"28\">28</option>"
   "<option value=\"29\">29</option>"
   "<option value=\"30\">30</option>"
   "<option value=\"31\">31</option>"
   "<option value=\"32\">32</option>"
   "<option value=\"33\">33</option>"
   "<option value=\"34\">34</option>"
   "<option value=\"35\">35</option>"
   "<option value=\"36\">36</option>"
   "<option value=\"37\">37</option>"
   "<option value=\"38\">38</option>"
   "<option value=\"39\">39</option>"
   "<option value=\"40\">40</option>"
   "<option value=\"41\">41</option>"
   "<option value=\"42\">42</option>"
   "<option value=\"43\">43</option>"
   "<option value=\"44\">44</option>"
   "<option value=\"45\">45</option>"
   "<option value=\"46\">46</option>"
   "<option value=\"47\">47</option>"
   "<option value=\"48\">48</option>"
   "<option value=\"49\">49</option>"
   "<option value=\"50\">50</option>"
   "<option value=\"51\">51</option>"
   "<option value=\"52\">52</option>"
   "<option value=\"53\">53</option>"
   "<option value=\"54\">54</option>"
   "<option value=\"55\">55</option>"
   "<option value=\"56\">56</option>"
   "<option value=\"57\">57</option>"
   "<option value=\"58\">58</option>"
   "<option value=\"59\">59</option>"
   "</select>"
   "</div>"
   "<div>"
   "<label for=\"second\">Second:</label><br>"
   "<select name=\"second\" id=\"second\">"
   "<option value=\"00\">00</option>"
   "<option value=\"01\">01</option>"
   "<option value=\"02\">02</option>"
   "<option value=\"03\">03</option>"
   "<option value=\"04\">04</option>"
   "<option value=\"05\">05</option>"
   "<option value=\"06\">06</option>"
   "<option value=\"07\">07</option>"
   "<option value=\"08\">08</option>"
   "<option value=\"09\">09</option>"
   "<option value=\"10\">10</option>"
   "<option value=\"11\">11</option>"
   "<option value=\"12\">12</option>"
   "<option value=\"13\">13</option>"
   "<option value=\"14\">14</option>"
   "<option value=\"15\">15</option>"
   "<option value=\"16\">16</option>"
   "<option value=\"17\">17</option>"
   "<option value=\"18\">18</option>"
   "<option value=\"19\">19</option>"
   "<option value=\"20\">20</option>"
   "<option value=\"21\">21</option>"
   "<option value=\"22\">22</option>"
   "<option value=\"23\">23</option>"
   "<option value=\"24\">24</option>"
   "<option value=\"25\">25</option>"
   "<option value=\"26\">26</option>"
   "<option value=\"27\">27</option>"
   "<option value=\"28\">28</option>"
   "<option value=\"29\">29</option>"
   "<option value=\"30\">30</option>"
   "<option value=\"31\">31</option>"
   "<option value=\"32\">32</option>"
   "<option value=\"33\">33</option>"
   "<option value=\"34\">34</option>"
   "<option value=\"35\">35</option>"
   "<option value=\"36\">36</option>"
   "<option value=\"37\">37</option>"
   "<option value=\"38\">38</option>"
   "<option value=\"39\">39</option>"
   "<option value=\"40\">40</option>"
   "<option value=\"41\">41</option>"
   "<option value=\"42\">42</option>"
   "<option value=\"43\">43</option>"
   "<option value=\"44\">44</option>"
   "<option value=\"45\">45</option>"
   "<option value=\"46\">46</option>"
   "<option value=\"47\">47</option>"
   "<option value=\"48\">48</option>"
   "<option value=\"49\">49</option>"
   "<option value=\"50\">50</option>"
   "<option value=\"51\">51</option>"
   "<option value=\"52\">52</option>"
   "<option value=\"53\">53</option>"
   "<option value=\"54\">54</option>"
   "<option value=\"55\">55</option>"
   "<option value=\"56\">56</option>"
   "<option value=\"57\">57</option>"
   "<option value=\"58\">58</option>"
   "<option value=\"59\">59</option>"
   "</select>"
   "</div>"
   "<input type=\"submit\" value=\"Set Alarm\">"
   "</form>"
   "</div>"
   "</body>"
   "</html>";

String confirmation_screen;

// Fungsi untuk menangani pengaturan alarm
void handleSetAlarm() {
  hr = webServer.arg("hour");
  minut = webServer.arg("minute");
  sec = webServer.arg("second");
  new_time = hr + ":" + minut + ":" + sec;

  // Simpan waktu alarm
  hr1 = hr.toInt();
  minut1 = minut.toInt();
  sec1 = sec.toInt();

  // Aktifkan alarm
  alarmActive = true;

  // Buat halaman konfirmasi
  confirmation_screen = 
      "<!DOCTYPE html><html lang=\"en\">"
      "<head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
      "<title>Alarm Confirmation</title></head>"
      "<body><h1>Alarm Set!</h1><p>Your alarm is set for " + new_time + ".</p>"
      "<form action=\"/\"><input type=\"submit\" value=\"Back\"></form></body></html>";

  webServer.send(200, "text/html", confirmation_screen);
  Serial.print("Alarm set for: ");
  Serial.println(new_time);
}

void setup() {
  Serial.begin(115200);

  // Setup pins
  pinMode(buzzer, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Menginisialisasi RTC
  Wire.begin();
  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  if (!Rtc.IsDateTimeValid()) {
    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
  }
  if (!Rtc.GetIsRunning()) {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }
  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  } else if (now > compiled) {
    Serial.println("RTC is newer than compile time. (this is expected)");
  } else if (now == compiled) {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }

  // Setup WiFi in AP mode
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("AlarmClockAP");

  Serial.println("Access Point \"AlarmClockAP\" started");
   Serial.print("IP Address: ");
   Serial.println(WiFi.softAPIP());

  // Setup DNS server
  dnsServer.start(DNS_PORT, "*", apIP);

     // Atur routing untuk halaman yang tidak ditemukan
   webServer.onNotFound([]() {
      webServer.sendHeader("Location", String("http://10.10.10.1/home_screen.php"), true);
      webServer.send(302, "text/plain", "");
   });
   
   // Handle permintaan untuk halaman beranda
   webServer.on("/home_screen.php", []() {
      webServer.send(200, "text/html", home_screen);
   });

   // Handle permintaan untuk halaman konfirmasi
   webServer.on("/confirmation_screen.php", []() {
        if (webServer.args() > 0) {
            new_time = webServer.arg("hour") + ":" + webServer.arg("minute") + ":" + webServer.arg("second");
            webServer.send(200, "text/html", confirmation_screen); // Tampilkan halaman konfirmasi
            hr = new_time.substring(0, 2); // Ambil jam
            hr1 = hr.toInt(); // Konversi ke integer
            minut = new_time.substring(3, 5); // Ambil menit
            minut1 = minut.toInt(); // Konversi ke integer
            sec = new_time.substring(6, 8); // Ambil menit
            sec1 = sec.toInt(); // Konversi ke integer
            alarmActive = true; // Set alarm menjadi aktif
            Serial.print("Alarm set for: ");
            Serial.print(hr1);
            Serial.print(":");
            Serial.println(minut1);
            Serial.print(":");
            Serial.println(sec1);
        } else {
            webServer.send(200, "text/html", home_screen);
        }
    });

   // Mulai server HTTP
   webServer.begin();
   Serial.println("HTTP server started");
}


 

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();

  RtcDateTime now = Rtc.GetDateTime();
  Serial.print("Current time: ");
  Serial.print(now.Hour());
  Serial.print(":");
  Serial.print(now.Minute());
  Serial.print(":");
  Serial.println(now.Second());

  if (alarmActive) {
    checkButton();
    if (now.Hour() == hr1 && now.Minute() == minut1 && now.Second() == sec1) {
      Serial.println("Alarm ON - Alarming");
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, HIGH); // Hidupkan LED
        Buzzer(); // Bunyikan buzzer
        Buzzer();
        Buzzer();
        Buzzer();
      
    }
  } else {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, LOW);
  }

  // Cek tombol untuk mematikan alarm
  if (digitalRead(BUTTON_PIN) == LOW) {
    Serial.println("Alarm stopped by button press.");
    digitalWrite(buzzer, LOW);
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    alarmActive = false;
    delay(1000); // Debounce delay
  }

  delay(1000); // Update every second
}

void Buzzer() {
   digitalWrite(buzzer, HIGH);
   delay(500);
   digitalWrite(buzzer, LOW);
   delay(500);
}

void checkButton() {
   // Baca status tombol
   int buttonState = digitalRead(BUTTON_PIN);

   // Jika tombol ditekan (logika negatif karena pull-up resistor)
   if (buttonState == LOW) {
      // Matikan alarm
      alarmActive = false;
      // Matikan LED
      digitalWrite(LED2_PIN, LOW);
   }
}