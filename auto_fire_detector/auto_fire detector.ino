int Pump = D1; // GPIO5
int Buzzer = D2; // GPIO4
int Sensor = D6; // GPIO12

void setup() {
  Serial.begin(9600);
  pinMode(Pump, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Sensor, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(Sensor) == LOW) {
    Serial.println("Flame Detection!!");  
    digitalWrite(Pump, HIGH);
    digitalWrite(Buzzer, HIGH);
    delay(1000);
  } else {
    Serial.println("No dangerous!"); 
    digitalWrite(Pump, LOW);
    digitalWrite(Buzzer, LOW);
    delay(500);
  }
}
