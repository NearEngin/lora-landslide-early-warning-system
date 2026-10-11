
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// LoRa pins
#define LORA_SS    5
#define LORA_RST   14
#define LORA_DIO0  26
#define LORA_FREQ  433E6

// OLED I2C pins
#define OLED_SDA   21
#define OLED_SCL   22

// 1.3-inch, 128x64 SH1106 OLED
Adafruit_SH1106G display(128, 64, &Wire, -1);

const int GRAPH_POINTS = 48;
int rssiHistory[GRAPH_POINTS];

int sampleCount = 0;
int writeIndex = 0;
unsigned long packetCount = 0;

int lastRSSI = -120;
String lastMessage = "Waiting for packets";

void addRSSI(int rssi) {
  rssiHistory[writeIndex] = rssi;
  writeIndex = (writeIndex + 1) % GRAPH_POINTS;

  if (sampleCount < GRAPH_POINTS) {
    sampleCount++;
  }
}

void drawRSSIGraph() {
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);

  // Header
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LoRa RSSI Monitor");

  // Current RSSI and packet count
  display.setCursor(0, 10);
  display.print("RSSI: ");
  display.print(lastRSSI);
  display.print(" dBm");

  display.setCursor(0, 20);
  display.print("Packets: ");
  display.print(packetCount);

  // Graph border
  const int gx = 15;
  const int gy = 30;
  const int gw = 112;
  const int gh = 30;

  display.drawRect(gx, gy, gw, gh, SH110X_WHITE);

  // Horizontal reference lines
  for (int i = 1; i <= 2; i++) {
    int y = gy + i * gh / 3;
    for (int x = gx + 1; x < gx + gw - 1; x += 4) {
      display.drawPixel(x, y, SH110X_WHITE);
    }
  }

  // Plot oldest to newest
  for (int i = 1; i < sampleCount; i++) {
    int idx1 = (writeIndex - sampleCount + i - 1
                + GRAPH_POINTS) % GRAPH_POINTS;
    int idx2 = (writeIndex - sampleCount + i
                + GRAPH_POINTS) % GRAPH_POINTS;

    // Fixed scale: -120 to -30 dBm
    float v1 = constrain(rssiHistory[idx1], -120, -30);
    float v2 = constrain(rssiHistory[idx2], -120, -30);

    int x1 = gx + 1 + (i - 1) * (gw - 3)
             / (GRAPH_POINTS - 1);
    int x2 = gx + 1 + i * (gw - 3)
             / (GRAPH_POINTS - 1);

    int y1 = gy + gh - 2 -
        (v1 + 120.0) / 90.0 * (gh - 3);
    int y2 = gy + gh - 2 -
        (v2 + 120.0) / 90.0 * (gh - 3);

    display.drawLine(x1, y1, x2, y2, SH110X_WHITE);
  }

  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // Initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(0x3C, true)) {
    Serial.println("OLED initialization failed!");
    while (true) delay(1000);
  }

  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 20);
  display.println("Starting LoRa RX...");
  display.display();

  // Initialize LoRa
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("LoRa initialization failed!");

    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("LoRa FAILED!");
    display.display();

    while (true) delay(1000);
  }

  // Must match TX settings
  LoRa.setSyncWord(0x12);
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.enableCrc();

  Serial.println("LoRa RX + RSSI graph ready");
  drawRSSIGraph();
}

void loop() {
  int packetSize = LoRa.parsePacket();

  if (packetSize > 0) {
    String message = "";

    while (LoRa.available()) {
      message += (char)LoRa.read();
    }

    lastMessage = message;
    lastRSSI = LoRa.packetRssi();
    packetCount++;

    addRSSI(lastRSSI);

    Serial.print("Message: ");
    Serial.print(lastMessage);
    Serial.print(" | RSSI: ");
    Serial.print(lastRSSI);
    Serial.println(" dBm");

    drawRSSIGraph();
  }
}
