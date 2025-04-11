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

const int ldrPins[3] = {A0, A1, A2};
int ldrAnalog[3] = {0,0,0};
int ldrDigital[3] = {0,0,0};
int ldrSensors = B000;
float ldrSensorsLevels[3] = {0,0,0}; // a value from 0 to 4, where 5 is the strongest - aka light from 3cm and 0 is the weakest where light is greater than 19cm
int sensorThresholds[5][3]= {
  {90,94,82},
  {240,163,170},
  {362,260,260},
  {450,350,320},
  {550,420,383}
};
int lightThreshold = 500;

void setup() {
  // initialise ldrs as inputs
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  // begin serial communication
  Serial.begin(9600);
}

void set_motor_pwm(int pwm, int IN1_PIN, int IN2_PIN)
{
  if (pwm < 0) {  // reverse speeds
    analogWrite(IN1_PIN, -pwm);
    digitalWrite(IN2_PIN, LOW);

  } else { // stop or forward
    digitalWrite(IN1_PIN, LOW);
    analogWrite(IN2_PIN, pwm);
  }
}
// ================================================================================
/// Set the current on both motors.
///
/// \param pwm_A  motor A PWM, -255 to 255
/// \param pwm_B  motor B PWM, -255 to 255

void set_motor_currents(int pwm_A, int pwm_B)
{
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
void spin_and_wait(int pwm_A, int pwm_B, int duration)
{
  set_motor_currents(pwm_A, pwm_B);
  delay(duration);
}

void readLDR() {
  ldrSensors = B000;
  for(int i = 0; i < 3; i++) {
    int sensorValue = analogRead(ldrPins[i]);
    ldrAnalog[i] = sensorValue;
    // compare with each light level
    for(int j = 4; j >= 0; j--) {
      if(sensorValue >= sensorThresholds[j][i]) {
        ldrSensorsLevels[i] = j;
        break;
      }
    }
    
    // digital sensor readings
    ldrDigital[i] = (sensorValue <= lightThreshold)? 1:0;
  }
}

void printLDRReadings() {
  for(int i = 0; i < 3; i++) {
    Serial.print(ldrSensorsLevels[i]);
    Serial.print(" - ");
    Serial.print(ldrAnalog[i]);
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
  error = ((ldrSensorsLevels[2])-(ldrSensorsLevels[0]));
  dE = lastError - error;
  smoothedError = error * (1+abs(dE)) * 50;
  Serial.print(" Directional error: ");
  Serial.print(error);
  Serial.print("smoothed error: ");
  Serial.print(smoothedError);

  // base speed
  baseSpeed = ldrSensorsLevels[1] * ldrSensorsLevels[1] * (MAX_SPEED/16);

  // Serial.print("Right: ");
  // Serial.print(ldrSensorsLevels[0]);
  // Serial.print(" (");
  // Serial.print(ldrAnalog[0]);
  // Serial.print(") ");
  Serial.print(" Middle: ");
  Serial.print(ldrSensorsLevels[1] * ldrSensorsLevels[1]);
    Serial.print(" (");
  Serial.print(ldrAnalog[1]);
  Serial.print(") ");
  // Serial.print(" Left: ");
  // Serial.print(ldrSensorsLevels[2]);
  //   Serial.print(" (");
  // Serial.print(ldrAnalog[2]);
  // Serial.print(") ");


  // Serial.print(" Base speed: ");
  // Serial.print(baseSpeed);

  leftServoSpeed = constrain(baseSpeed + smoothedError + motorOffset, -MAX_SPEED + motorOffset, MAX_SPEED + motorOffset);
  rightServoSpeed = constrain(baseSpeed - smoothedError - motorOffset, -MAX_SPEED - motorOffset, MAX_SPEED - motorOffset);

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