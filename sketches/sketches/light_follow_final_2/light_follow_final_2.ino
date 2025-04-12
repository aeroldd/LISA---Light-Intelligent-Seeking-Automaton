#define MOT_A1_PIN 5
#define MOT_A2_PIN 6
#define MOT_B1_PIN 9
#define MOT_B2_PIN 10

boolean reverseL = 0;
boolean reverseR = 0;

/* variables to keep track of current speed of motors */
int leftServoSpeed = 0;
int rightServoSpeed = 0;
float motorOffset = 12;  // Adjust this value as needed

const int MAX_SPEED = 200;

// error things for updating directions
float error;
int correction;
int scaleFactor = 50;

// LDR READING

// right, middle, left, rear
const int ldrPins[4] = { A0, A1, A2, A3 };
int ldrAnalog[4] = { 0, 0, 0, 0 };
int ldrDigital[4] = { 0, 0, 0, 0 };
int ldrSensors = B0000;
float ldrSensorsLevels[4] = { 0, 0, 0, 0 };  // a value from 0 to 4, where 4 is the strongest - aka light from 3cm and 0 is the weakest where light is greater than 19cm
int sensorThresholds[7][4] = {
  { 90, 94, 82, 90 },     // 3cm
  { 240, 163, 170, 170 }, // 7cm
  { 362, 260, 260, 260 }, // 11cm
  { 450, 350, 320, 350 }, // 15cm
  { 550, 420, 383, 400 }, // 19cm
  { 503, 414, 403, 463 }, // 23cm
  { 557, 469, 506, 538 }  // 27cm
};

// int brightRoomSensorThresholds[5][4] = {
//   { 86, 58, 80, 72 },      //3cm
//   { 195, 132, 151, 146 },  //7cm
//   { 262, 180, 200, 212 },  //11cm
//   { 295, 200, 222, 220 },  //15cm
//   { 315, 214, 230, 222 }   //19cm
// };

int brightRoomThreshold[4] = {
  341, 280, 249, 222
};

int ambientLightThreshold[4];

// check what brightness the room is to use those thresholds
void checkAmbientLighting() {
  readLDR();
  for (int i = 0; i < 4; i++) {
    ambientLightThreshold[i] = ldrAnalog[i];
  }
}

int lightThreshold = 300;

void setup() {
  // initialise ldrs as inputs
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  // begin serial communication
  Serial.begin(9600);

  checkAmbientLighting();
  delay(1000);
}

void set_motor_pwm(int pwm, int IN1_PIN, int IN2_PIN) {
  if (pwm < 0) {  // reverse speeds
    analogWrite(IN1_PIN, -pwm);
    digitalWrite(IN2_PIN, LOW);

  } else {  // stop or forward
    digitalWrite(IN1_PIN, LOW);
    analogWrite(IN2_PIN, pwm);
  }
}
// ================================================================================
/// Set the current on both motors.
///
/// \param pwm_A  motor A PWM, -255 to 255
/// \param pwm_B  motor B PWM, -255 to 255

void set_motor_currents(int pwm_A, int pwm_B) {
  set_motor_pwm(pwm_A, MOT_A1_PIN, MOT_A2_PIN);
  set_motor_pwm(pwm_B, MOT_B1_PIN, MOT_B2_PIN);

  //Print a status message to the console.
  // Serial.print("Set motor A PWM = ");
  // Serial.print(pwm_A);
  // Serial.print(" motor B PWM = ");
  // Serial.println(pwm_B);
}

// ================================================================================
/// Simple primitive for the motion sequence to set a speed and wait for an interval.
///
/// \param pwm_A  motor A PWM, -255 to 255
/// \param pwm_B  motor B PWM, -255 to 255
/// \param duration delay in milliseconds
void spin_and_wait(int pwm_A, int pwm_B, int duration) {
  set_motor_currents(pwm_A, pwm_B);
  delay(duration);
}

void readLDR() {
  ldrSensors = B000;
  for (int i = 0; i < 4; i++) {
    int sensorValue = analogRead(ldrPins[i]);
    ldrAnalog[i] = sensorValue;
    // compare with each light level
    for (int j = 6; j >= 0; j--) {
      if (sensorValue >= sensorThresholds[j][i]) {
        ldrSensorsLevels[i] = 6 - j;
        break;
      }
    }

    // digital sensor readings
    ldrDigital[i] = (sensorValue <= lightThreshold) ? 1 : 0;
  }
}

void printLDRReadings() {
  for (int i = 0; i < 6; i++) {
    Serial.print(ldrSensorsLevels[i]);
    Serial.print(" - ");
    Serial.print(ldrAnalog[i]);
    Serial.print(" -  ");
    Serial.print(ldrDigital[i]);
    Serial.print(" ");
  }
  Serial.println();
}

int baseSpeed;
float lastError;
float dE;
float smoothedError;

// updates the motor speed and directions depending on the readings from the sensors
void updateDirection() {
  // rotational error
  lastError = error;

  // error, a value between -4 and 4, is the difference between left and right sensors
  // error > 0 -> light is coming from the left side
  // error < 0 -> light is coming from the right side
  // error == 0 -> buggy is facing the light source
  error = ((ldrSensorsLevels[2]) - (ldrSensorsLevels[0]));

  // difference in error dE
  dE = lastError - error;

  // if bigger difference in error (light direction suddenly changed)
  // there is a bigger correction
  smoothedError = error * (1 + abs(dE)) * 50;

  // base speed depends on the middle sensor's intensity level
  // if its greater than the max threshold OR is in the ambient room readings
  if (ldrSensorsLevels[1] == 0 || ldrAnalog[1] >= ambientLightThreshold[1]) {
    leftServoSpeed = 0;
    rightServoSpeed = 0;
  }

  else {
    baseSpeed = ((6-ldrSensorsLevels[1]) * (6-ldrSensorsLevels[1])* (MAX_SPEED / 20));

    // new speed calculations -> combines the effects of directional light
    // and light source's distance from middle sensor
    int leftSpeed = baseSpeed + smoothedError + motorOffset;
    int rightSpeed = baseSpeed - smoothedError - motorOffset;

    // constrain used to make sure that the values remain within the max speed settings
    leftServoSpeed = constrain(leftSpeed, -MAX_SPEED + motorOffset, MAX_SPEED + motorOffset);
    rightServoSpeed = constrain(rightSpeed, -MAX_SPEED - motorOffset, MAX_SPEED - motorOffset);
  }


  // if the rear sensor detects light it stops everything
  if (ldrDigital[3]) {
    leftServoSpeed = 0;
    rightServoSpeed = 0;
  }

  Serial.print(" ");
  Serial.print(leftServoSpeed);
  Serial.print(" ");
  Serial.print(rightServoSpeed);
  Serial.println();
}

void loop() {
  readLDR();
  //printLDRReadings();
  updateDirection();
  spin_and_wait(leftServoSpeed, rightServoSpeed, 100);
}