// DECLARE motor pins
const int left_slp_pin = 31;
const int left_dir_pin = 29;
const int left_pwm_pin = 40;
const int right_slp_pin = 11;
const int right_dir_pin = 30;
const int right_pwm_pin = 39;

// DECLARE reflectance pins
int pin_refL[8] = {65, 48, 64, 47, 52, 68, 53, 69};

// DECLARE LED pins
const int even_led = 45;
const int odd_led = 61;

// DECLARE led weights
const float sensor_weights[] = {-1.75, -1.25, -.75, -0.25, 0.25, 0.75, 1.25, 1.75};

// DECLARE states
bool reached_black_line = false; // indicates when a horizontal black line has been reached
bool reached_first_line = false; // indicates when one end of the track has been reached

// DECLARE speed values
const float right_straight = 90;
const float left_straight = 86;

const float Kp = 14.5;
const float turn_base = 65;

// declare LED values
int refL[8];


void setup() {
  // put your setup code here, to run once:

//  Serial.begin(9600);
  
  // INIT motor pins
  pinMode(left_slp_pin, OUTPUT);
  pinMode(left_dir_pin, OUTPUT);
  pinMode(left_pwm_pin, OUTPUT);
  pinMode(right_slp_pin, OUTPUT);
  pinMode(right_dir_pin, OUTPUT);
  pinMode(right_pwm_pin, OUTPUT);

  digitalWrite(left_dir_pin, LOW);
  digitalWrite(right_dir_pin, LOW);
  digitalWrite(left_slp_pin, HIGH);
  digitalWrite(right_slp_pin, HIGH);

  // INIT LED pins
  pinMode(odd_led, OUTPUT);
  pinMode(even_led, OUTPUT);
  digitalWrite(odd_led, HIGH);
  digitalWrite(even_led, HIGH);
}

void loop() {
  read_sensor_values();
  
  // For Kd (derivative controller)
  // prev_error = error;

  float weighted_value = 0;
  for (int i = 0; i < 8; i++) {
    weighted_value += (refL[i] * sensor_weights[i]); 
  }

  // check if reached black line
  reached_black_line = all_high(refL);

  if (reached_black_line) {
    if (reached_first_line) {
      analogWrite(left_pwm_pin, 0);
      analogWrite(right_pwm_pin, 0);
      while(true) {}
    }
    else {
      reached_black_line = false;
      reached_first_line = true;
      turn_around(); 
    }
  }

   float error = Kp * weighted_value /*Kd*(error-prev_error)*/;


   int dir = get_turn_direction(weighted_value);

   adjust_movement(dir, error);
}

void read_sensor_values() {
  for (int i = 0; i < 8; i++) {
    refL[i] = get_sensor_value(pin_refL[i], 700);
  }
}

// get_sensor_value: 
// inputs: an int pin number and an int delay length
// outputs: 1 if black, 0 if white
int get_sensor_value(int pin, int delay_length) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delayMicroseconds(10);
  pinMode(pin, INPUT);
  delayMicroseconds(delay_length);
  int result = digitalRead(pin);
  return result;
}

// get_turn_direction: 
// inputs: weighted sensor value
// outputs: -1 if car should turn left, 1 if car should turn right, 0 if car should go straight 
int get_turn_direction(float sensor_val) {
    if (sensor_val > .75) { // left sensors high
      return -1; 
    } else if (sensor_val < -.75) { // right sensors high
      return 1; 
    } else { // middle sensors high
      return 0; 
    } 
}

// all_high:
// inputs: array of reflector outputs
// outputs: 1 if all of the outputs are high, 0 if not 
int all_high (int arr[]) {
  int result = true;
  for (int i = 0; i < 8; i++) {
    if (arr[i] == 0) {
      result = false;
    }
  }
  return result;
}

// adjust_movement:
// inputs: integer representing direction to turn
// outputs: none
void adjust_movement (int dir, int error) {
   switch(dir) {
    case 0:
      analogWrite(right_pwm_pin, right_straight);
      analogWrite(left_pwm_pin, left_straight);
      break;
    case 1:
    case -1:
      analogWrite(left_pwm_pin, turn_base-error);
      analogWrite(right_pwm_pin,turn_base+error);
      break;
  }
}

void turn_around() {
  // start turning
    digitalWrite(left_dir_pin, HIGH);
    digitalWrite(right_dir_pin, LOW);
    analogWrite(left_pwm_pin, 120);
    analogWrite(right_pwm_pin, 120);
    delay(490);
  // "reset"
    digitalWrite(left_dir_pin, LOW);
    digitalWrite(right_dir_pin, LOW);
    analogWrite(left_pwm_pin, 0);
    analogWrite(right_pwm_pin, 0);
    analogWrite(left_pwm_pin, right_straight);
    analogWrite(right_pwm_pin, left_straight);
    return;
}
