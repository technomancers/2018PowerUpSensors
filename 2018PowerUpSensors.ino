#include <thermistor.h>
#include <hcsr04.h>

// Ultrasonic setup
const int numUltras = 5;
HCSR04 ultrasonics[numUltras] = {
  {7, 2},
  {8, 3},
  {9, 4},
  {10, 5},
  {11, 6}
};
int distances[numUltras];

//Temperature setup
Thermistor thermistor = Thermistor::FromCelsius(A0, 10000, 25, 4300, 98800);

void setup() {
  Serial.begin(9600);         // Starts the serial communication. Only needed for debug comment before deploy
  analogReference(EXTERNAL);  //Reduce the noise on the analog read
}

void loop() {
  float temp = thermistor.temperatureC();
  Serial.print(temp);
  Serial.print("; ");
  float speedOfSound = (331.3 + 0.606 * temp) / 10000; // Calculate the speed of sound using temperature in cm/us

  // Go through Round Robin and calculate the distance
  for (int i = 0; i < numUltras; i++) {
    delayMicroseconds(10); // High signals must be greater than 10 microseconds
    distances[i] = ultrasonics[i].distance(speedOfSound);
    Serial.print(i);
    Serial.print(":");
    Serial.print(distances[i]);
    // Make debugging easier to read
    if (i == numUltras - 1) {
      Serial.println();
    } else {
      Serial.print("; ");
    }
  }
}

