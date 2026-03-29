#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define TRIG_PIN 6
#define ECHO_PIN 7
#define RELAY_PIN 8
#define LED_PIN 10
#define BUTTON_PIN 4

const int tempThreshold = 23;
const int distanceThreshold = 10;

bool systemActive = false;
bool lastButtonState = LOW;

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  // --- Toggle system on button press ---
  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (currentButtonState == HIGH && lastButtonState == LOW) {
    systemActive = !systemActive;
    Serial.println(systemActive ? "System ACTIVATED" : "System DEACTIVATED");
    delay(300); // debounce delay
  }
  lastButtonState = currentButtonState;

  // --- Only run sensors if system is active ---
  if (systemActive) {
    float temperature = dht.readTemperature();

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    int distance = duration * 0.034 / 2;

    Serial.print("Temp: ");
    Serial.print(temperature);
    Serial.print("°C, Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // --- Control Relay and LED based on both conditions ---
    if (temperature > tempThreshold && distance <= distanceThreshold) {
      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(LED_PIN, LOW);
    }

  } else {
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }

  delay(100); // minimal delay for better responsiveness
}
