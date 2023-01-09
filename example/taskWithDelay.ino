#include "Scheduler2.h"
int led1 = 13;
int led2 = 12;
int led3 = 14;
int pulsante=27;
byte precval;
byte stato= LOW;
unsigned long prevMillis = 0;

Scheduler scheduler;
int count = 0;

void onMaxSpeedEvents();
void onHalfSecondEvents();
void onSecondEvents();
void epochScheduler();
void epoch10sec();
void epoch20sec();

void onMaxSpeedEvents(){
	if(Serial.available() > 0 ){//anche while va bene!
		//parsing di una stringa dal flusso di dati 
		String str = Serial.readString();
		//parsing di un comando (sottostringa) all'interno di una stringa	
		if(str.indexOf("accendi")>=0){
			digitalWrite(led3, HIGH);
		}
		if(str.indexOf("spegni")>=0){
			digitalWrite(led3, LOW);
		}
	}
}

void periodicBlink(int led) {
  //Serial.print("printing periodic blink led ");
  //Serial.println(led);
  digitalWrite(led, !digitalRead(led));
}

void ontwosec(){
	  unsigned long now = millis();
		unsigned long diff = now-prevMillis;
		//diff = diff%50;
		Serial.print("ontwosec: ");Serial.println(diff);
		periodicBlink(led2);
		prevMillis = now;
}

void onHalfSecondEvents(){
	unsigned randomDelay = random(1, 200);
	Serial.print("delay: ");Serial.println(randomDelay);
	delay(randomDelay);
	periodicBlink(led1);
}

void epoch20sec(){
		periodicBlink(led3);
}

void on50msEvents(){

}
 
void setup() {
	randomSeed(analogRead(0));
	pinMode(led1, OUTPUT);
	pinMode(led2, OUTPUT);
	pinMode(led3, OUTPUT);
	Serial.setTimeout(10);
	Serial.begin(115200); 
	scheduler.addPeriodicEvent(on50msEvents, 1, 50);
	scheduler.addPeriodicEvent(onHalfSecondEvents, 1, 500);
	scheduler.addPeriodicEvent(ontwosec, 1, 2000);
	scheduler.addAsyncEvent(epoch20sec, 2, 6000, 5000, 500, true);
	Serial.println(F("Scheduler init"));
	Serial.print(F("Time base: "));
	Serial.println(scheduler.getTimebase());
}
 
void loop() {
	scheduler.scheduleAll();
	delay(1);
}
