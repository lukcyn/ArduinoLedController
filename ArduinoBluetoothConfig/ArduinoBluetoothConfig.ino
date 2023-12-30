#define BLUETOOTH_DEVICE_NAME "LedController"

void sendCommandToHC05(String command) {
  Serial.print("Sending command: ");
  Serial.println(command);
  Serial.println("Response:");

  Serial.println(command);
  delay(1000);

  // Read and print the response
  while (Serial.available()) {
    char c = Serial.read();
    Serial.print(c);
  }

  Serial.println("\n-------------------------");
  delay(1000);
}


void setupBluetooth() {
  Serial.println("Initializing Bluetooth Module");
  Serial.println("Make sure that HC-05 is in AT mode");
  delay(1000);

  sendCommandToHC05("AT");

  Serial.println("Disconnecting Paired Device...");
  sendCommandToHC05("AT+RMAAD");

  Serial.println("Setting Device Name...");
  char command[30];
  strcpy(command, "AT+NAME=");
  strcat(command, BLUETOOTH_DEVICE_NAME);
  sendCommandToHC05(command);

  Serial.println("Setting Role to Slave...");
  sendCommandToHC05("AT+ROLE=0");

  Serial.println("Setting Password...");
  sendCommandToHC05("AT+PSWD=1234");

  Serial.println("Bluetooth configutation Complete");
}

void setup() {
  Serial.begin(38400);
  setupBluetooth();
}

void loop() {

}