
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

int initialtime = 0;
uint8_t value[4];
uint32_t timer = 0;

#define SERVICE_UUID        "b907675a-c901-4fa1-8224-2e9a69fa70a1"
#define CHARACTERISTIC_UUID "82352aca-4d9e-4c11-b6b9-4a9889e7f4aa"


#define buzzer 9
#define Led 6

#define WeaponOut 2

union
{
int Timer_;
uint8_t union_data[4];
}converter_union;

int WeaponState = 0;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};


void setup() {
  
  Serial.begin(115200);

  BLEDevice::init("MyESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");

  while (deviceConnected == false)
  {
    delay(1000);
    Serial.println("Waiting to connect");
  }
  initialtime = millis();

  
 //  pinMode(WeaponOut, OUTPUT);
 // pinMode(Led, OUTPUT);
 // pinMode(buzzer, OUTPUT);

  // sound / led checkup
  // tone(buzzer,1500);
  // digitalWrite(Led, HIGH);
  delay(300);
 // noTone(buzzer);
 // digitalWrite(Led, LOW);

}

void loop() {

    
  //WeaponState = digitalRead(WeaponOut);

  if (WeaponState == 1)
  {
    if (deviceConnected)
      {
         Report_toServer(); 
      }
      else
      {
    //led and buzzer warning
      }
      
    ActivateDisplays();
    RestartBout(); 
  }

  delay(1000);
  timer += 1000;
  /*
  if (! timer == millis() - initialtime)
  {
    timer = millis() - initialtime;
  }*/
  //converter_union.timer = timer;  //timer into byte array
      Serial.println("Reached manual byte writing");

      /*
      value[0] = 0x61;
      value[1] = 0x62;
      value[2] = 0x63;
      value[3] = 0x64;
      */
      converter_union.Timer_ = timer;
      value[0] = converter_union.union_data[3];
      value[1] = converter_union.union_data[2];
      value[2] = converter_union.union_data[2];
      value[3] = converter_union.union_data[0];

      Serial.print("Timer value is  ");
      Serial.println(timer);
      Serial.print("Union Timer value is  ");
      Serial.println(converter_union.Timer_);
      Serial.println("");
      
      Serial.print("Bytes are    ");
      Serial.print(value[0]);
      Serial.print(" - ");
      Serial.print(value[1]);
      Serial.print(" - ");
      Serial.print(value[2]);
      Serial.print(" - ");
      Serial.print(value[3]);
      Serial.println(" End bytes ");
      
  pCharacteristic->setValue(value, 4);   //update data
  }

void ActivateDisplays() // activating sound and lights 
{
 // tone(buzzer,1500);
 // digitalWrite(Led, HIGH);
  delay(5000);
 // noTone(buzzer);
  delay(5000);
  digitalWrite(Led, LOW);
}
void RestartBout() // reseting the equipment and resynching with the servers
{
  //TODO
  //tone(buzzer,100);
 // digitalWrite(Led, HIGH);
  delay(500);
  //noTone(buzzer);
 // digitalWrite(Led, LOW);

}

void Report_toServer()  //reporting to the central device monitoring both fencers and keeping score
{       

      timer = millis() - initialtime;
      converter_union.Timer_ = timer;
      value[0] = converter_union.union_data[3];
      value[1] = converter_union.union_data[2];
      value[2] = converter_union.union_data[2];
      value[3] = converter_union.union_data[0];
      
        pCharacteristic->setValue(value, 4);   //notify server and update data
        pCharacteristic->notify();
        delay(10);
       
}

