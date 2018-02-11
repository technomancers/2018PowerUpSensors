#include <hcsr04.h>
#include <Wire.h>

// Ultrasonic setup
const int numUltras = 4;
HCSR04 ultrasonics[numUltras] = {
  {7, 2},
  {8, 3},
  {9, 4},
  {10, 5}
  //{11, 6}
};
int distances[numUltras];

//Temperature setup
const int thermPin = A0;
//Thermistor thermistor = Thermistor::FromCelsius(A0, 10000, 25, 4300, 98800);

//Setup I2C
const int SLAVE_ADDRESS = 0x2A;
byte registerMapTemp[numUltras*2], registerMap[numUltras*2]; //2 bytes per distance
bool newDataAvailable;

void setup() {
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);         // Starts the serial communication. Only needed for debug comment before deploy
  analogReference(EXTERNAL);  //Reduce the noise on the analog read
}

void loop() {
  float temp = temperatureC();
  Serial.print(temp);
  Serial.print("; ");
  float speedOfSound = (331.3 + 0.606 * temp) / 10000; // Calculate the speed of sound using temperature in cm/us

  // Go through Round Robin and calculate the distance
  for (int i = 0; i < numUltras; i++) {
    delayMicroseconds(100); // High signals must be greater than 10 microseconds
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
  storeData();
  newDataAvailable = true;
}

void requestEvent(){
  if (newDataAvailable) {
    for (int i = 0; i < numUltras*2;i++){
      registerMap[i] = registerMapTemp[i];
    }
  }
  Wire.write(registerMap, numUltras*2);
  newDataAvailable = false;
}

void storeData(){
  byte *bytePointer;
  byte arrayindex = 0;
  for (int d = 0; d < numUltras; d++){
    bytePointer = (byte*)&distances[d];
    for (int i = 1; i > -1; i--){
      registerMapTemp[arrayindex] = bytePointer[i];
      arrayindex++;
    }
  }
}

float temperatureC() {
  int tempReading = analogRead(thermPin);
  float voltage = tempReading * 3.23;
  voltage /= 1024.0;
  return (voltage - 0.5) * 100.0;
}

