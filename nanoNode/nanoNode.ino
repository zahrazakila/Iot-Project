void setup() {
  Serial.begin(9600);  // Memulai komunikasi serial dengan baud rate 9600
}

void loop() {
  Serial.println("Hello from Nano");  // Mengirim pesan serial
  delay(1000);                        // Menunggu selama 1 detik
}
