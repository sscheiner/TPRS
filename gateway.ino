/*
 *  Sam Scheiner 4/2017
 *  Gateway node source for tactical position reporting system
 *  
*/
#include <Arduino.h>
#include <RadioHead.h>
#include <radio_config_Si4460.h>
#include <RHCRC.h>
#include <RHDatagram.h>
#include <RHGenericDriver.h>
#include <RHGenericSPI.h>
#include <RHHardwareSPI.h>
#include <RHMesh.h>
#include <RHNRFSPIDriver.h>
#include <RHReliableDatagram.h>
#include <RHRouter.h>
#include <RHSoftwareSPI.h>
#include <RHSPIDriver.h>
#include <RHTcpProtocol.h>
#include <RH_ASK.h>
#include <RH_CC110.h>
#include <RH_MRF89.h>
#include <RH_NRF24.h>
#include <RH_NRF51.h>
#include <RH_NRF905.h>
#include <RH_RF22.h>
#include <RH_RF24.h>
#include <RH_RF69.h>
#include <RH_RF95.h>
#include <RH_Serial.h>
#include <RH_TCP.h>
#include <SPI.h>
#include <RH_RF69.h>
#include "node.h"
#include "nodelist.h"

/************ Radio Setup ***************/

// set frequency for network, should always be 915.0
#define NETWORK_FREQ 915.0

#if defined (__AVR_ATmega32U4__) // Feather 32u4 w/Radio
  #define RFM69_CS      8
  #define RFM69_INT     7
  #define RFM69_RST     4
  #define LED           13
#endif

#if defined(ARDUINO_SAMD_FEATHER_M0) // Feather M0 w/Radio
  #define RFM69_CS      8
  #define RFM69_INT     3
  #define RFM69_RST     4
  #define LED           13
#endif

#if defined (__AVR_ATmega328P__)  // Feather 328P w/wing
  #define RFM69_INT     3  // 
  #define RFM69_CS      4  //
  #define RFM69_RST     2  // "A"
  #define LED           13
#endif

#if defined(ESP8266)    // ESP8266 feather w/wing
  #define RFM69_CS      2    // "E"
  #define RFM69_IRQ     15   // "B"
  #define RFM69_RST     16   // "D"
  #define LED           0
#endif

#if defined(ESP32)    // ESP32 feather w/wing
  #define RFM69_RST     13   // same as LED
  #define RFM69_CS      33   // "B"
  #define RFM69_INT     27   // "A"
  #define LED           13
#endif

/* Teensy 3.x w/wing
#define RFM69_RST     9   // "A"
#define RFM69_CS      10   // "B"
#define RFM69_IRQ     4    // "C"
#define RFM69_IRQN    digitalPinToInterrupt(RFM69_IRQ )
*/
 
/* WICED Feather w/wing 
#define RFM69_RST     PA4     // "A"
#define RFM69_CS      PB4     // "B"
#define RFM69_IRQ     PA15    // "C"
#define RFM69_IRQN    RFM69_IRQ
*/

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);
//create datagram for this instance, address is 0 for gateway
RHDatagram manager(rf69, 0);
//self node instance
Node self;
//gateway needs no parent instance, only list of children
NodeList childList;

int16_t packetnum = 0;  // packet counter, we increment per xmission
const uint8_t layer = 0; //gateway is always layer 0 in the tree mesh

void setup() 
{

  //set maximum baud rate for serial connection
  Serial.begin(115200);
  //while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer

  pinMode(LED, OUTPUT);     //setup LED (13 for feather) for visual update of status
  pinMode(RFM69_RST, OUTPUT); //setup radio reset
  digitalWrite(RFM69_RST, LOW); //initialize reset to low

  Serial.println("TPRS Gateway node initializing\n");

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  
  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(NETWORK_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);
  
  pinMode(LED, OUTPUT);

  Serial.print("RFM69 radio @");  Serial.print((int)NETWORK_FREQ);  Serial.println(" MHz");
}



void loop() {

  
  delay(1000);  // Wait 1 second between transmits, could also 'sleep' here!
  handshake();
  
}

void Blink(byte PIN, byte DELAY_MS, byte loops) {
	for (byte i = 0; i < loops; i++) {
		digitalWrite(PIN, HIGH);
		delay(DELAY_MS);
		digitalWrite(PIN, LOW);
		delay(DELAY_MS);
	}


}

void handshake() {

	uint8_t address = manager.thisAddress;
	uint8_t sendBuffer[RH_RF69_MAX_MESSAGE_LEN];
	uint8_t messageLength = sizeof(sendBuffer);
	//we want to send current layer and address for handshake
	

	//print handshake data being sent 
	Serial.print("Sending "); Serial.println(sendBuffer);

	// cast as unsigned 8 bit integer and send
	rf69.send((uint8_t *)sendBuffer, strlen(sendBuffer));
	rf69.waitPacketSent();

	// Now wait for a reply and store in receive buffer
	uint8_t receiveBuffer[RH_RF69_MAX_MESSAGE_LEN];


	if (rf69.waitAvailableTimeout(500)) {
		// Should be a reply message for us now 
		//check that it has been received 
		if (rf69.recv(receiveBuffer, &messageLength)) {

			Serial.print("Got a reply: ");
			Serial.println((char*)receiveBuffer);

			//signal that a reply has been received
			Blink(LED, 50, 3);
		}
		else {
			Serial.println("Receive failed");
		}
	}
	else {
		Serial.println("No reply, is another RFM69 listening?");
	}

}