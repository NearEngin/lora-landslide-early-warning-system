
#include <SPI.h>
#include <LoRa.h>

#define LORA_SS    5
#define LORA_RST   14
#define LORA_DIO0  26

#define LORA_FREQ  433E6

unsigned long counter = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("LoRa initialization FAILED!");
    while (true) {
      delay(1000);
    }
  }

  LoRa.setSyncWord(0x12);
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.enableCrc();

  Serial.println("LoRa TX initialized");
}

void loop() {
  String message = "Hello from TX: " + String(counter++);

  LoRa.beginPacket();
  LoRa.print(message);
  int result = LoRa.endPacket();

  if (result == 1) {
    Serial.print("Sent: ");
    Serial.println(message);
  } else {
    Serial.println("Transmission failed");
  }

  delay(2000);
}
