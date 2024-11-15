// Definisikan pin untuk lampu dan tombol
#define LED_PIN D7
#define BUTTON_PIN D6

// Inisialisasi status lampu
bool lampuNyala = false;

void setup() {
  Serial.begin(115200);
  
  // Set pin mode untuk lampu dan tombol
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  // Baca status tombol
  int buttonState = digitalRead(BUTTON_PIN);

  // Jika tombol ditekan (logika negatif karena pull-up resistor)
  if (buttonState == LOW) {
    // Ubah status lampu
    lampuNyala = !lampuNyala;

    // Nyalakan atau matikan lampu sesuai dengan status
    digitalWrite(LED_PIN, lampuNyala ? HIGH : LOW);

    // Tunggu sebentar sebelum membaca tombol lagi untuk menghindari bouncing
    delay(200);
  }
}
