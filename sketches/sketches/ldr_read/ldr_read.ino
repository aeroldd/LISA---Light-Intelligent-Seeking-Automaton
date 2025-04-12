// LDR READING

const int ldrPins[4] = {A0, A1, A2, A3};
int ldrAnalog[4] = {0,0,0,0};

void setup() {
  // initialise ldrs as inputs
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  // begin serial communication
  Serial.begin(9600);
}


void readLDR() {
  for(int i = 0; i < 4; i++) {
    int sensorValue = analogRead(ldrPins[i]);
    ldrAnalog[i] = sensorValue;
    // compare with each light level
  }
}

void printLDRReadings() {
  for(int i = 0; i < 4; i++) {
    Serial.print(ldrAnalog[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void loop() {
  readLDR();
  printLDRReadings();
}

