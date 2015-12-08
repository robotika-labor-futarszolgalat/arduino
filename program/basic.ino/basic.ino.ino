#include <Bridge.h>
#include <YunClient.h>

#define id 00
#define PORT 255
#define LEDP 5
#define LEDZ 6
#define BUTTON 4

YunClient client;

void setup()
{
  Bridge.begin();
  Serial.begin(9600);
  pinMode(LEDP,OUTPUT);
  pinMode(LEDZ,OUTPUT);
  while (!Serial);
}

void loop()
{
  IPAddress addr(192, 168, 42, 185);
  client.connect(addr, PORT);

  while(client.connected())
  {
    //bluetooth beolvasás
    //infratávmérő olvasás
    //gomb olvasás
    int buttonState = digitalRead(BUTTON);
    //|id|bt1|bt2|bt3|bt4|error|gomb|
    client.println("Something...");
    delay(250);
    ///////////////////////////////
    while (client.available())
    {
      //|id|irány|cm/fok|
      //|int|int|int|
      String fogadott = client.read();
      
      
      Serial.print(c);
    }
    Serial.flush();
    client.stop();
  }
  //megszűnt a szerver, vagy nem lehet hozzá kapcsolódni
  
  delay (3000);
}
