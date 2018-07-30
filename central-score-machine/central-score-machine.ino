/**
 * A BLE client example that is rich in capabilities.
 */

#include "BLEDevice.h"
//#include "BLEScan.h"

// The remote service we wish to connect to.
static BLEUUID serviceUUID1("91bad492-b950-4226-aa2b-4ede9fa42f59");  //TODO check perif and central device UUIDs
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID1("0d563a58-196a-48ce-ace2-dfec78acc814");

static BLEUUID serviceUUID2("dbf07658-78d4-40af-be4b-ef230e0def62");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID2("afa9c82b-1443-409d-bb42-1c51adc6abc7");

static BLEAddress *pServerAddress1;
static BLEAddress *pServerAddress2;

bool found1 = false;
bool found2 = false;

static boolean doConnect = false;
static boolean connected = false;

static BLERemoteCharacteristic* pRemoteCharacteristic1;
static BLERemoteCharacteristic* pRemoteCharacteristic2;


static void notifyCallback( //TODO implement system fir reading data - didnt touch notify yet
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
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
    } else {
      Serial.println("We have failed to connect to the servers; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
  //  String newValue = "Time since boot: " + String(millis()/1000);
   // Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    
    // Set the characteristic's value to be the array of bytes that is actually a string.
   // pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  }
  
  delay(1000); // Delay a second between loops.
} // End of loop
