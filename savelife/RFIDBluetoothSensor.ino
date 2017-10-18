#include "DHT.h"                           // DHT.h 라이브러리를 포함한다.
#include <Servo.h>
#include <SoftwareSerial.h>
#include <MFRC522.h>
#include <SPI.h>
#include <ArduinoJson.h>

#define uchar    unsigned char                                             
#define uint     unsigned int
#define DHTPIN   8                           // DHTPIN을 디지털 2번핀으로 정의한다.
#define DHTTYPE  DHT11                      // DHTTYPE을 DHT11로 정의한다.
#define RST_PIN  9                           // reset 핀 설정
#define SS_PIN   10                           // 데이터를 주고받는 역할의 핀 (SS = Slave Select)
#define BLUE_TX  3                             // Tx (보내는 핀 설정)
#define BLUE_RX  2                             // Rx (받는 핀 설정)
#define SERVOPIN 4
#define ID_SIZE  11                         // 군번은 최대 병사의경우 10자리다.
#define REDPIN   5
#define GREENPIN 6
#define BLUEPIN  7

MFRC522 mfrc522(SS_PIN, RST_PIN);             // MFR522 방식의 RFID 모듈 컨트롤 위한 mfrc 객체 생성
Servo servo1;                               // 서보 모터 사용을 위한 변수 선언
SoftwareSerial mySerial(BLUE_TX, BLUE_RX);   // 시리얼 통신을 위한 객체 선언
DHT dht(DHTPIN, DHTTYPE);                  // DHT설정 - (디지털 8, DHT11)

  // Now a card is selected. The UID and SAK is in mfrc522.uid.
  byte sector         = 2;
  byte valueBlockA    = 8;
  byte valueBlockB    = 9;
  byte valueBlockC    = 10;
  byte trailerBlock   = 11;
  byte status;

void setup()
{
  pinMode(REDPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);

  servo1.attach(SERVOPIN);
  randomSeed(analogRead(0));

  Serial.begin(9600);                   // 시리얼 통신을 시작한다. 속도는9600, 시리얼 모니터
  mySerial.begin(9600);                  // 블루투스 시리얼

  SPI.begin();

  mfrc522.PCD_Init();
}

void chkRFID()
{
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  
  if(!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
  {
    delay(500);
    return;
  }
  
  // Authenticate using key A.
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  
  // Authenticate using key B.
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  // Dump PICC type
  byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.print("PICC type: ");
  Serial.println(mfrc522.PICC_GetTypeName(piccType));
  if (        piccType != MFRC522::PICC_TYPE_MIFARE_MINI 
           &&        piccType != MFRC522::PICC_TYPE_MIFARE_1K
           &&        piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
           //Serial.println("This sample only works with MIFARE Classic cards.");
           return;
        }  
}

void loop() {
  // 블루투스에 읽을 값이 있으면 위험상황이므로 알맞은 센서동작을 해준다.
  while(mySerial.available()){
    char danger = mySerial.read();
    if( danger == '!' ) alert();
  }

  // 새로운 RFID가 연결되는지를 확인한다.
  chkRFID();

  // 새로운 RFID가 등록될 수 있으므로 매번 id를 확인한다.
  // id가 설정된 위치의 블록 값은 10이다.(3번째 섹터의 3번째 블록)     
  byte buffer[100];
  byte size = sizeof(buffer);
  String id="";
  status = mfrc522.MIFARE_Read(valueBlockC, buffer, &size);

  for(int i=0; i<ID_SIZE; ++i) {
    uint buf = buffer[i];
    if( buf == 0xff ) break;
    buf = buf + 48;
    char c[10];
    itoa(buf, c, 10);
    id += c;
  }

  // id를 읽는데 실패하면 센서측정을 실시하지 않는다.
  if( id.length()==0 ){
    Serial.println("Read ID is FAILED...");
    return;
  }

  // 센서 값을 읽고 서버에 보낸다.
  readSensor(id);
}

void readSensor(String id)
{  
  StaticJsonBuffer<50> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  int temperature  = dht.readTemperature();
  int randomNumber = random(0,250);
  temperature+=randomNumber;

  root["id"]   = id;
  root["data"] = temperature;
  root.printTo(mySerial);

  digitalWrite(BLUEPIN, HIGH);
  delay(2500);
  digitalWrite(BLUEPIN, LOW);
  delay(500);
}

void alert()
{
  uint angle = 0;   // 서보모터 제어 위한 변수 선언 및 초기화
  uint reply = 0;   // 서보모터 반복 제어 위한 변수 선언 및 초기화
  
  Serial.println("ALERT!!!");
  for (reply = 0; reply < 3; reply++)
  {
    for (angle = 0; angle < 60; angle++)
    {
      servo1.write(angle);
      delay(2);
      }
      for (angle = 60; angle > 0; angle--)
      {
        servo1.write(angle);
        delay(2);
      }
   }
   digitalWrite(REDPIN, HIGH);
   delay(1500);
   digitalWrite(REDPIN, LOW);
   delay(500);
}
