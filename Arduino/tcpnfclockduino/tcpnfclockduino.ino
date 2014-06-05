/*
 * Arduino nfc door lock with remote card database on webserver.
 * Copyright 2014 Alberto Panu alberto[at]panu.it
 * 
 * Pin layout for the MFRC522 board should be as follows:
 * Signal     Pin          
 *            Arduino Uno 
 * ------------------------
 * Reset      8
 * SPI SS     9
 * SPI MOSI   11
 * SPI MISO   12
 * SPI SCK    13
 *
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com. 
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Ethernet.h>

/* DEBUG levels
   0 no
   1 normal
   2 verbose */
#define DEBUG 2
/* tentativi set ne try connection number to webserver */
#define tentativi 5
/* pin_della_serratura is the lock pin */
#define pin_della_serratura 5
/* chose the ip setup 0 for DHCP */
#define IPSETUP 1
/* portaserver set the remote server port */
#define portaserver 80
/* rficodemaxlenght rfid uuid max lenght, i don't read the nfc spec */ 
#define rficodemaxlenght 16

/* MFRC522 module variables */
#define SS_PIN 9
#define RST_PIN 8
MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance.

unsigned char lettura;
unsigned char rfidcode[rficodemaxlenght];
unsigned char rficodelenght;
char risposta;
String reponse = "test";
unsigned int prove;
EthernetClient client;
/* remote server ip, change wth your server ip */
byte server[] = { 81, 208, 22, 71 };
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x4D, 0x49 };

// DHCP
#if IPSETUP == 0 
#warning Ethernet configured by DHCP
#endif

// Lavoro
#if IPSETUP == 1 
#warning Ethernet Lavoro
byte ip[] = { 10, 63, 0, 183 };
byte mydns[] = { 8, 8, 8, 8 };
byte gateway[] = { 10, 63, 0, 1 };
byte netmask[] = { 255, 255, 254, 0 };
#endif

// Casa
#if IPSETUP == 2 
#warning Ethernet Casa
byte ip[] = { 192, 168, 1, 17 };
byte mydns[] = { 8, 8, 8, 8 };
byte gateway[] = { 192, 168, 1, 1 };
byte netmask[] = { 255, 255, 255, 0 };
#endif

void setup() {
        pinMode(pin_della_serratura,OUTPUT);
        digitalWrite(pin_della_serratura,LOW);
#if DEBUG > 0  
	Serial.begin(9600);	// Initialize serial communications with the PC
#endif
//      removed because already done by ethernet
//	SPI.begin();			// Init SPI bus
#if IPSETUP > 0
  Ethernet.begin(mac, ip, mydns, gateway, netmask);
#else
  while ( Ethernet.begin(mac) == 0 ) {
#if DEBUG > 0
     Serial.println("DHCP configuration error!");
     Serial.println("retry in 5 seconds");
#endif
     delay(5000);
  } 
#if DEBUG > 0
  Serial.println("Ethernet DHCP configured.");
#endif
#endif
  delay(1000);
  mfrc522.PCD_Init();	// Init MFRC522 card
#if DEBUG > 0 
  Serial.println("Nfc toker reader ready!");
#endif
}

void loop() {
	// Look for new cards
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

        rficodelenght = mfrc522.uid.size;
        if ( rficodelenght > rficodemaxlenght ) {
#if DEBUG > 0 
            Serial.println("Rfid uid too long!");
#endif
        mfrc522.PICC_HaltA();
        return;
        }
        for (byte i = 0; i < mfrc522.uid.size; i++) {
                rfidcode[i]=mfrc522.uid.uidByte[i];
	}
        mfrc522.PICC_HaltA();
#if DEBUG > 0
        Serial.print("Card UID:");
        for ( byte i = 0 ; i < rficodelenght; i++) {
        	Serial.print(rfidcode[i] < 0x10 ? "0" : "");
		Serial.print(rfidcode[i], HEX);
        }
	Serial.println();
#endif
      prove = tentativi;
      while (prove > 0) {
      risposta='E';
//      EthernetClient client;
#if DEBUG > 1  
      Serial.println("connecting...");
#endif
        if ( client.connect(server, portaserver) ) {
#if DEBUG > 1
        Serial.println("connected");
#endif 
//      this should be the page on your server that check the rfid code ar return in text act=O for open or act=C for close
        client.print("GET /tcpnfclockduino/x.shtml?rfid=");
//      this is a test page on my webserver that randomly respond open or close and do not check the rfid code
//      client.print("GET /tcpnfclockduino/random.shtml?rfid=");
//      this is a test page on my webserver that every time open the door and do not check the rfid code
//      client.print("GET /tcpnfclockduino/lock_open.html?rfid=");
//      this is a test page on my webserver that every time don't open the door and do not check the rfid code
//      client.print("GET /tcpnfclockduino/lock_close.html?rfid=");
        for ( byte i = 0; i < rficodelenght; i++) {
          if (rfidcode[i] < 10) 
          client.print("0");
          client.print(rfidcode[i],HEX);
        }
        client.println(" HTTP/1.1");
        client.println("Host: www.panu.it");
        client.println("");
        delay(200);
        reponse = "";
// Be aware the response must be in the fist 340 characters!
        while (client.available() && reponse.length() < 350) {
          risposta = client.read();
          reponse = reponse + risposta;
#if DEBUG > 1
          Serial.print(risposta);
#endif  
        }
#if DEBUG > 1
        Serial.println("");
        Serial.println("-----------------------");
        Serial.println(reponse);
        Serial.println("-----------------------");
#endif
        client.stop();
        
        if ( reponse.indexOf("act=O") > 0 ) {
// Here we open the door
          digitalWrite(pin_della_serratura,HIGH);
          delay(100);
          digitalWrite(pin_della_serratura,LOW);
#if DEBUG > 0 
          Serial.println("Door opened!");
#endif
          break;
         }
#if DEBUG > 0 
         else if ( reponse.indexOf("act=C") > 0) {
           Serial.println("Unauthorized card");
           break;
         } else {
           Serial.print("Door command not found on page");
         }    
#endif
      }
#if DEBUG > 0     
     else {
        Serial.println("connection failed");
        }
#endif
#if DEBUG > 1
          Serial.print("Connect server try number: ");
          Serial.print(prove);
          Serial.println(" falied!");
#endif
          prove--;
        }
#if DEBUG > 0
        if (prove == 0) {
          Serial.println("Remote server connect error!");
        }
#endif
}
