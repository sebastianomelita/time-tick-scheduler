#include "Scheduler2.h"
int led1 = 13;
int led2 = 12;
int led3 = 14;
int pulsante=21;
byte precval;
byte stato= LOW;

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
		if(str.indexOf("accendi")>=0 ){
			digitalWrite(led3, HIGH);
		}
		if(str.indexOf("spegni")>=0){
			digitalWrite(led3, LOW);
		}
	}
}

void on50msEvents(){
	byte val = digitalRead(pulsante);		//pulsante collegato in pulldown
	//val = digitalRead(!pulsante);	//pulsante collegato in pullup
	if(precval==LOW && val==HIGH){ 	//rivelatore di fronte di salita
		stato = !stato; 							//impostazione dello stato del toggle	
		Serial.println(stato);
		digitalWrite(led3, stato);
	}
	precval=val;	
}
 
void periodicBlink(int led) {
  //Serial.print("printing periodic blink led ");
  //Serial.println(led);
  digitalWrite(led, !digitalRead(led));
}

void epoch20sec(){
		periodicBlink(led2);
}

void onHalfSecondEvents(){
	periodicBlink(led1);
}
void onSecondEvents(){
	periodicBlink(led2);
}
 
void setup() {
	pinMode(led1, OUTPUT);
	pinMode(led2, OUTPUT);
	pinMode(led3, OUTPUT);
	Serial.setTimeout(10);
	Serial.begin(115200); 
	scheduler.addPeriodicEvent(on50msEvents, 1, 100);
	scheduler.addPeriodicEvent(onMaxSpeedEvents, 1, 0);
	scheduler.addPeriodicEvent(onHalfSecondEvents, 1, 500);
	scheduler.addAsyncEvent(epoch20sec, 1, 6000, 5000, 500, true);
	Serial.println(F("Scheduler init"));
	Serial.print(F("Time base: "));
	Serial.println(scheduler.getTimebase());
	Serial.print(F("Nsteps: "));
	Serial.println(scheduler.getNsteps());
}
 
void loop() {
	scheduler.scheduleAll();
	delay(10);
}
