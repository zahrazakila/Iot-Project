void setup() {
  Serial.begin(9600);  // Memulai komunikasi serial dengan baud rate 9600
  Serial.setTimeout(500); // Menetapkan timeout untuk pembacaan serial
  Serial.println("NodeMCU Initialized");
}

void loop() {
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');
    Serial.print("Received: ");
    Serial.println(message);
  }
}
