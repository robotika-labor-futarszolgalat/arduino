#include <Bridge.h>
#include <HttpClient.h>
#include <SoftwareSerial.h>

/***** bluetooth stuff*/
void locate_beacons();
void end_scan();

unsigned long time = 0;

char charbuffer[50];
int buffer_end = 0;
boolean bluetooth_reading = false;
int devices = 0;
boolean processed = false;

unsigned long read_start = 0;
int countdown = 0;

String bt_names = "";
String bt_signals = "";
String myip_port= "192.168.10.10/mydir";
String message = "";

char inputbuffer[50];

int bluetoothTx = 7;
int bluetoothRx = 4;  // RX-I pin of bluetooth mate, Arduino D3

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

/******control stuff ***/
int mleft_sensor = 2;
int mright_sensor = 3;

int left_motor = 9;
int right_motor = 11;
int left_dir = 8;
int right_dir = 10;

int button_pin = 12;

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

  pinMode(button_pin, INPUT);
  pinMode(12, INPUT);
  attachInterrupt(digitalPinToInterrupt(button_pin), button_press, FALLING);

  attachInterrupt(digitalPinToInterrupt(mleft_sensor), lstep_interrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(mright_sensor), rstep_interrupt, FALLING);

  Bridge.begin();
  Serial.begin(9600);
  bluetooth.begin(115200);

  bluetooth.print("$");
  bluetooth.print("$");
  bluetooth.print("$");

  delay(100);
  bluetooth.println("U,9600,N");

  bluetooth.begin(9600);
  delay(100);
  bluetooth.print("$");
  bluetooth.print("$");
  bluetooth.print("$");
}

void loop()
{
  HttpClient client;
  if ((millis() - time) > 10000 ) {
    time = millis();
    locate_beacons();
  }
  if ((bt_names != "") && (bt_signals != ""))
  {
     message += myip_port;
     message += "?";
     message += bt_names;
     message += "_";
     message += bt_signals;
     message += "_";     
     
     client.get(message);
     delay(200);
     
     message= "";
     bt_names = "";
     bt_signals = "";
  }

  if (Serial.available())
  {
    int action = Serial.parseInt();

    switch (action) {
      case 1:
        forward(Serial.parseInt());
        Serial.println("elore");
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

  if (bluetooth.available())
  {
    char c = bluetooth.read();
    if (bluetooth_reading) {
      charbuffer[buffer_end++] = c;

      if (c == '\n')
      {
        charbuffer[buffer_end - 2] = '\0';
        if (strstr(charbuffer, "CMD") != NULL )
        {
          Serial.println("command mode");
          processed = true;
        }
        if (strstr(charbuffer, "Done") != NULL )
        {
          end_scan();
          processed = true;

        }
        if (devices > 0)
        {
          int current_word = 0;
          int current_pos = 0;
          char current_buffer[10];

          for (int i = 0; i < buffer_end; ++i)
          {
            current_buffer[current_pos++] = charbuffer[i];
            if (charbuffer[i] == ',' || i == buffer_end - 1)
            {
              current_buffer[current_pos - 1] = '\0';
              if (current_word == 1)
              {
                Serial.println();
                Serial.print("BT nev: ");
                Serial.println(current_buffer);
                bt_names += current_buffer;
                bt_names += "_";
              }

              if (current_word == 3)
              {
                Serial.print("BT jel: ");
                Serial.println(current_buffer);
                Serial.println();
                bt_signals += current_buffer;
                bt_signals += "_";
              }
              current_word++;
              current_pos = 0;
            }
          }
          --devices;
          processed = true;
        }

        if (strstr(charbuffer, "Found") != NULL )
        {
          if (strstr(charbuffer, "No Devices"))
          {
            devices = 0;
            Serial.println("nincsenek elerheto eszközök ");
            end_scan();
          }
          else
          {
            char * number = charbuffer + 6;
            int num = atoi(number);
            Serial.print("talált eszközök: ");
            Serial.println(num);
            devices = num;
          }
          processed = true;
        }

        if (!processed)
        {
          Serial.println(charbuffer);
        }
        processed = false;

        buffer_end = 0;
      }
    }
  }
  if (stop_flag) {
    stop();
    Serial.println("stop");
    stop_flag = false;
    left_target = left_steps;
    right_target = right_steps;
  }
  if (button_pressed) {
    Serial.println("button pressed");
    Serial.println(button_debounce++);
    button_pressed = false;
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

void button_press()
{
  button_debounce = button_debounce > 1000 ? 0 : ++button_debounce;
  button_pressed = true;
  button_debounce == 0;
}

void end_scan()
{
  Serial.println("kereses vege");
  Serial.println("----------------------------------");
  Serial.println();
  Serial.println();
  bluetooth_reading = false;
}

void locate_beacons()
{
  bluetooth_reading = true;
  bluetooth.print("IQ");
  bluetooth.print('\n');
}
