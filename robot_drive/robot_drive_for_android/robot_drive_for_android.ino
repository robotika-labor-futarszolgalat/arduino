int left_motor = 9;
int right_motor = 11;
int left_dir = 8;
int right_dir = 10;

int mleft_sensor = 2;
int mright_sensor = 3;

int forward_speed = 200;
int turn_speed = 150;

volatile long left_steps = 0;
volatile long right_steps = 0;

long left_target = 0;
long right_target = 0;

volatile bool stop_flag = false;

volatile bool button_pressed = false;
volatile int button_debounce = 0;

void lstep_interrupt();
void rstep_interrupt();
void forward(int unit);
void backward(int unit);
void turn(int direction, int unit);
void stop();
void button_press();

void setup()
{
  pinMode(mleft_sensor, INPUT);
  pinMode(mright_sensor, INPUT);
  pinMode(left_motor, OUTPUT);
  pinMode(right_motor, OUTPUT);
  pinMode(left_dir, OUTPUT);
  pinMode(right_dir, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(mleft_sensor), lstep_interrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(mright_sensor), rstep_interrupt, FALLING);
  Serial.begin(9600);
}

void loop()
{
  if (Serial.available())
  {
    int action = Serial.parseInt();
    switch (action) {
      case 1:
        forward(Serial.parseInt());
        Serial.println(left_target);
        Serial.println(left_steps);
        break;
      case 2:
        turn(Serial.parseInt(), Serial.parseInt());
      case 3:
        backward(Serial.parseInt());
        break;
      case 4:
        stop_flag = true;
        break;
    }
  }

  if (stop_flag)
  {
    stop();
    stop_flag = false;
    left_target = left_steps;
    right_target = right_steps;
  }
}

void stop() {
  analogWrite(left_motor, 0);
  analogWrite(right_motor, 0);
}

void forward(int unit)
{
  int leftpos = left_steps;
  int rightpos = right_steps;

  digitalWrite(left_dir, 0);
  digitalWrite(right_dir, 0);

  analogWrite(left_motor, forward_speed);
  analogWrite(right_motor, forward_speed);
  left_target = leftpos + unit;
  right_target = rightpos + unit;
}

void backward(int unit)
{
  int leftpos = left_steps;
  int rightpos = right_steps;

  digitalWrite(left_dir, 1);
  digitalWrite(right_dir, 1);

  analogWrite(left_motor, forward_speed);
  analogWrite(right_motor, forward_speed);

  left_target = leftpos + unit;
  right_target = rightpos + unit;
}

void turn(int direction, int unit)
{
  int leftpos = left_steps;
  int rightpos = right_steps;

  if (direction) {
    digitalWrite(left_dir, 0);
    digitalWrite(right_dir, 1);
  } else {
    digitalWrite(left_dir, 1);
    digitalWrite(right_dir, 0);
  }

  analogWrite(left_motor, turn_speed);
  analogWrite(right_motor, turn_speed);

  left_target = leftpos + unit;
  right_target = rightpos + unit;
}

void lstep_interrupt()
{
  ++left_steps;
  stop_flag = left_steps > left_target;
}

void rstep_interrupt()
{
  ++right_steps;
  stop_flag = right_steps > right_target;
}



