#include <CurieBLE.h>
/* Initialize variables */
BLEPeripheral blePeripheral;       // BLE Peripheral Device
BLEService distanceService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE Battery Service

// BLE Battery Level Characteristic"
BLEUnsignedIntCharacteristic distanceFloat("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite); //characteristic UUID
// get notifications if this characteristic changes

int oldDistance = 0;  // last battery level reading from analog input
long previousMillis = 0;  // last time the battery level was checked, in ms

void setup() {
  Serial.begin(9600);    // initialize serial communication
  //pinMode(13, OUTPUT);   // initialize the LED on pin 13 to indicate when a central is connected

  /* Set local name */
  blePeripheral.setLocalName("ReadDistance");
  blePeripheral.setAdvertisedServiceUuid(distanceService.uuid());  // add the service UUID
  blePeripheral.addAttribute(distanceService);   // Add the BLE service
  blePeripheral.addAttribute(distanceFloat); // add the distance characteristic
  distanceFloat.setValue(oldDistance);   // initial value for this characteristic

  /* Activate BLE */
  blePeripheral.begin();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);

    // check the battery level every 200ms
    // as long as the central is still connected:
    while (central.connected()) {
      long currentMillis = millis();
      // if 200ms have passed, check the battery level:
      if (currentMillis - previousMillis >= 200) {
        previousMillis = currentMillis;
        updateDistanceReading();
      }
    }
    // when the central disconnects, turn off the LED:
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}

void updateDistanceReading() {
  /* Read voltage from pin A0 */
  float volts = analogRead(A0)*0.0032226563;  // value from sensor * (3.3/1024)
  int distance = 13*pow(volts, -1);

  if (distance != oldDistance) {      // if the distance has changed
    Serial.print("Reading distance: ");
    Serial.println(distance);
    distanceFloat.setValue(distance);  // and update the distance characteristic
    oldDistance = distance;           // update old value
  }
}
