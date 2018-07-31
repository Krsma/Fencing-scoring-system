/**
 * A BLE client example that is rich in capabilities.
 */

#include "BLEDevice.h"
//#include "BLEScan.h"

static BLEUUID serviceUUID1("91bad492-b950-4226-aa2b-4ede9fa42f59");  //TODO check perif and central device UUIDs
static BLEUUID    charUUID1("0d563a58-196a-48ce-ace2-dfec78acc814");

static BLEUUID serviceUUID2("dbf07658-78d4-40af-be4b-ef230e0def62");
static BLEUUID    charUUID2("afa9c82b-1443-409d-bb42-1c51adc6abc7");

static BLEAddress *pServerAddress1;
static BLEAddress *pServerAddress2;

bool found1 = false;
bool found2 = false;

static boolean doConnect = false;
static boolean connected = false;

static BLERemoteCharacteristic* pRemoteCharacteristic1;
static BLERemoteCharacteristic* pRemoteCharacteristic2;

int LocalTimer1 = 0;
int LocalTimer2 = 0;
int TimeDiff = 0;

int LastSynchedTimestamp = 0;

int touch1 = 0;
int touch2 = 0;
bool Hit1 = false;
bool Hit2 = false;

bool AlreadyNotified = false;


union
{
int Timer_;
uint8_t union_data[4];
}converter_union;

//TODO implement system fir reading data - didnt touch notify yet
static void notifyCallback( BLERemoteCharacteristic* pBLERemoteCharacteristic,uint8_t* pData,size_t length,bool isNotify)
{
  if (AlreadyNotified == false)
  {
    AlreadyNotified = true;
    int touch1 = 0;
    int touch2 = 0;
    // Serial.print("Notify callback for characteristic ");
    //Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    //Serial.print(" of data length ");
    // Serial.println(length);
    ReadTimerData();
    touch1 = LocalTimer1;
    touch2 = LocalTimer2 + TimeDiff; // leveling two timer , taking in account the time diff
    //if LocalTimer1 is bigger the LT2 then the addition increases the LT2, if its the other way around and 
    //the negative value of TimeDiff takes off the advantadge of LT2
    delay(40);
    ReadTimerData();  //reading timer again after the 40ms interval to check for hits; after the hit the timer gets locked and we check that
    if (touch1 + 40 > LocalTimer1)
    {
      Hit1 = true;
    }
    if (touch2 + 40 > LocalTimer2)
    {
      Hit2 = true;
    }
    delay(5000);

  }
    


}

bool connectToServer(BLEAddress pAddress1, BLEAddress pAddress2) {
    Serial.print("Forming a connection to ");
    //Serial.println(pAddress.toString().c_str());
    
    BLEClient*  pClient1  = BLEDevice::createClient();
    BLEClient*  pClient2  = BLEDevice::createClient();

    Serial.println(" - Created clients");

    // Connect to the remove BLE Server.
    pClient1->connect(pAddress1);
    pClient2->connect(pAddress2);
    Serial.println(" - Connected to servers");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService1 = pClient1->getService(serviceUUID1);
    if (pRemoteService1 == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID1.toString().c_str());
      return false;
    }
    BLERemoteService* pRemoteService2 = pClient2->getService(serviceUUID2);
    if (pRemoteService2 == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID2.toString().c_str());
      return false;
    }

    Serial.println(" - Found our services");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic1 = pRemoteService1->getCharacteristic(charUUID1);
    if (pRemoteCharacteristic1 == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID1.toString().c_str());
      return false;
    }
    pRemoteCharacteristic2 = pRemoteService2->getCharacteristic(charUUID2);
    if (pRemoteCharacteristic2 == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID2.toString().c_str());
      return false;
    }



    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    //std::string value = pRemoteCharacteristic->readValue();
    //Serial.print("The characteristic value was: ");
    //Serial.println(value.c_str());

    pRemoteCharacteristic1->registerForNotify(notifyCallback);
    pRemoteCharacteristic2->registerForNotify(notifyCallback);

}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(serviceUUID1)) {

      // 
      found1 = true;
      Serial.print("Found our device1!  address: "); 
      if (found1 && found2)
      {
        advertisedDevice.getScan()->stop();
      }

      pServerAddress1 = new BLEAddress(advertisedDevice.getAddress());


    }
    else if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(serviceUUID2))
    {


      found2 = true;
      Serial.print("Found our device2!  address: "); 
      if (found1 && found2)
      {
        advertisedDevice.getScan()->stop();
      }

      pServerAddress2 = new BLEAddress(advertisedDevice.getAddress());


    }

    doConnect = found1 && found2;
  } 
}; 


void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 30 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);
} // End of setup.


// This is the Arduino main loop function.
void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer(*pServerAddress1,*pServerAddress2)) {
      Serial.println("We are now connected to the BLE Servers.");
      connected = true;
      delay(2000);
      GetLocalTimers();
      LastSynchedTimestamp = millis();
      //TODO Visual indication all is ok
    } else {
      Serial.println("We have failed to connect to the servers; there is nothin more we will do.");
    }
    doConnect = false;
  }


  if (connected) {
      int Timenow = millis();
      if (LastSynchedTimestamp + 15000 < Timenow) //synching due to millis() drift
      {
        GetLocalTimers();
        LastSynchedTimestamp = Timenow;
      }
  //  String newValue = "Time since boot: " + String(millis()/1000);
   // Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    
    // Set the characteristic's value to be the array of bytes that is actually a string.
   // pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  }
  
  delay(1000); // Delay a second between loops.
} // End of loop

void GetLocalTimers()
{
  ReadTimerData();

  TimeDiff = LocalTimer1 - LocalTimer2;
  //LocalTimer1
  //LocalTimer2
  //time diff = timer1 - timer2
}
void ReadTimerData()
{
  std::string TValue1 = pRemoteCharacteristic1->readValue();    //more work needed fo conversion from string to int
  std::string TValue2 = pRemoteCharacteristic2->readValue();


  converter_union.union_data[0] = TValue1[0];
  converter_union.union_data[1] = TValue1[1];
  converter_union.union_data[2] = TValue1[2];
  converter_union.union_data[3] = TValue1[3];
  LocalTimer1 = converter_union.Timer_;

  converter_union.union_data[0] = TValue2[0];
  converter_union.union_data[1] = TValue2[1];
  converter_union.union_data[2] = TValue2[2];
  converter_union.union_data[3] = TValue2[3];
  LocalTimer2 = converter_union.Timer_;
  
}
