#include "Scheduler2.h"
#include <Ticker.h>
Ticker periodicTicker1;
int led1 = 13;
int led2 = 12;
int led3 = 14;
int pulsante=27;
byte precval;
byte stato= LOW;
unsigned long prevMillis = 0;
unsigned long tbase;

Scheduler scheduler;
int count = 0;

void onMaxSpeedEvents();
void onHalfSecondEvents();
void onSecondEvents();
void epochScheduler();
void epoch10sec();
void epoch20sec();

void periodicBlink(int led) {
  //Serial.print("printing periodic blink led ");
  //Serial.println(led);
  digitalWrite(led, !digitalRead(led));
}

void ontwosec(){
		periodicBlink(led2);
}

void onHalfSecondEvents(){
	periodicBlink(led1);
}

void epoch20sec(){
		periodicBlink(led3);
}

void on50msEvents(){
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
	Serial.println(F("Scheduler init"));
	Serial.print(F("Time base: "));
	tbase = scheduler.getTimebase();
	Serial.println(tbase);
	Serial.print(F("Nsteps: "));
	Serial.println(scheduler.getNsteps());
	periodicTicker1.attach_ms(tbase, timerISR);
}
 
void timerISR(void) {
  scheduler.scheduleAllISRFlagged(true);
}

void loop()
{
	delay(1);
	// il codice eseguito al tempo massimo della CPU va qui
}
