#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define SS_PIN 4
#define RST_PIN 2
#define LCD_LIGHT 3
#define LOCK_PIN 13

MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

String dataString;
int line;
unsigned int lockTimeout = 60000;

void setup() {
	Serial1.begin(250000);
	Serial.begin(115200);
	String pritnToSerial = "";
	while(!pritnToSerial.startsWith("bravaBegin")){
		pritnToSerial = Serial1.readStringUntil('\n');
		if(pritnToSerial.length() > 2)
			Serial.println(pritnToSerial);
	}
	SPI.begin();
	lcd.begin(20, 4);
	mfrc522.PCD_Init();
	pinMode(LCD_LIGHT, OUTPUT);
	pinMode(LOCK_PIN, OUTPUT);
	digitalWrite(LCD_LIGHT, HIGH);
	lcd.print("hello, world");
}

void loop() {
	if(mfrc522.PICC_IsNewCardPresent()){
		if(mfrc522.PICC_ReadCardSerial()){
			Serial1.print("rfid:");
			Serial1.println(mfrc522.PICC_ReturnUidString(&(mfrc522.uid)));
		}
	}

	if(Serial.available()){
		delay(5);
		Serial1.print("fromSerial:")
		Serial1.println(Serial.readStringUntil('\n'));
	}

	if(Serial1.available()){
		delay(5); int a = int(Serial1.read() - '0');
		switch(a){
			case 0:
				digitalWrite(LOCK_PIN, HIGH);
				lockTimeout = 0;
				break;
			case 2:
				//lcd stuff
				line = char(Serial1.read()) - '0';
				dataString = Serial1.readStringUntil('\n');
				if(line < 4){
					if(dataString.length() == 20){
						lcd.setCursor(0,line);
						lcd.print(dataString);
					}
					else{
						Serial.println(dataString);
					}
				}
				else{
					int val = dataString.toInt();
					analogWrite(LCD_LIGHT, val);
				}
				break;
			default:
				Serial1.print("wrong case ");
				Serial1.println(a);
				break;
		}
		while(Serial1.available()) Serial1.read();
	}

	if(lockTimeout <= 150){
		lockTimeout++;
		if(lockTimeout == 150)
			digitalWrite(LOCK_PIN, LOW);
	}
/*	if(Serial.available()){
		String data = Serial.readStringUntil('\n');
		data = data + "\r\n";
		Bluetooth.print(data);
	}
	while(Bluetooth.available()){
		Serial.write(Bluetooth.read());
	}/**/
}