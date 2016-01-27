
#include <SoftwareSerial.h>

int bluetoothTx = 5;  // TX-O pin of bluetooth mate, Arduino D2
int bluetoothRx = 4;  // RX-I pin of bluetooth mate, Arduino D3

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

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

char inputbuffer[50];

void setup()
{
  delay(1000);
  Serial.begin(9600);
  while (!Serial);

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
  if (countdown > 0 && (millis() - read_start) > 1000 ) {
    read_start = millis();
    Serial.print(countdown--);
    Serial.print("... ");
    if (countdown == 0)
      Serial.println();
  }

  if (Serial.available())
  {
    char command = Serial.read();
    
    if (command == 'k') {
      read_start = millis();
      countdown = 7;
      locate_beacons();
    }

    if (command == 'r')
    {
      devices = 0;
      buffer_end = 0;
      bluetooth_reading = false;
    }
  }

  if (bluetooth.available()) // If the bluetooth sent any characters
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
              }

              if (current_word == 3)
              {
                Serial.print("BT jel: ");
                Serial.println(current_buffer);
                Serial.println();
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
          } else {

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
