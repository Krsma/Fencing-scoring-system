
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

uint8_t value[4];
uint32_t timer = 0;

#define SERVICE_UUID        "b907675a-c901-4fa1-8224-2e9a69fa70a1"
#define CHARACTERISTIC_UUID "82352aca-4d9e-4c11-b6b9-4a9889e7f4aa"


#define buzzer 9
#define Led 6

#define WeaponOut 2

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

  
  
  pinMode(WeaponOut, OUTPUT);
  pinMode(Led, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // sound / led checkup
//  tone(buzzer,1500);
  digitalWrite(Led, HIGH);
  delay(300);
 // noTone(buzzer);
  digitalWrite(Led, LOW);

}

void loop() {

  if (deviceConnected && !oldDeviceConnected)
    {
        //  connecting to ble client
        oldDeviceConnected = deviceConnected;
        //TODO led blink
    }

    
  WeaponState = digitalRead(WeaponOut);

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

  delay(1);
  timer++;
}

void ActivateDisplays() // activating sound and lights 
{
 // tone(buzzer,1500);
  digitalWrite(Led, HIGH);
  delay(5000);
 // noTone(buzzer);
  delay(5000);
  digitalWrite(Led, LOW);
}
void RestartBout() // reseting the equipment and resynching with the servers
{
  //TODO
  //tone(buzzer,100);
  digitalWrite(Led, HIGH);
  delay(500);
  //noTone(buzzer);
  digitalWrite(Led, LOW);

}

void Report_toServer()  //reporting to the central device monitoring both fencers and keeping score
{       //TODO change value to signed int base 16 byte
        
        value[0] = timer;
        value[1] = timer>>8;
        value[2] = timer>>8;
        value[3] = timer>>8;
        
        pCharacteristic->setValue(value, 4);   //notify server and update data
        pCharacteristic->notify();
        delay(10);
}

