/*
Test del metodo deletePeriodicEvent(uint8_t priority, unsigned long every).
Scrivere un programma che ascolti un input da seriale per due comandi "metti"con il quale 
creare, in sequenza, 3 task diversi e  "togli" col il quale cancellare in sequenza 
gli stessi task.
*/
#include "Scheduler2.h"
int led1 = 13;
int led2 = 12;
int led3 = 14;

Scheduler scheduler;
int count = 0;
bool delflag = false;
bool addflag = false;

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
		if(str.indexOf("metti")>=0){
			if(count < 3){
				addflag = true;
			}
		}
		if(str.indexOf("togli")>=0){
			if(count > 0){
				delflag = true;
			}
		}
	}
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
	periodicBlink(led3);
}
 
void setup() {
	pinMode(led1, OUTPUT);
	pinMode(led2, OUTPUT);
	pinMode(led3, OUTPUT);
	Serial.setTimeout(10);
	Serial.begin(115200); 
	scheduler.addPeriodicEvent(onMaxSpeedEvents, 1, 0);
	scheduler.addPeriodicEvent(onHalfSecondEvents, 1, 500);
	scheduler.addPeriodicEvent(onSecondEvents, 1, 1000);
	scheduler.addAsyncEvent(epoch20sec, 2, 6000, 5000, 500, true);
	Serial.println(F("Scheduler init"));
	Serial.print(F("Time base: "));
	Serial.println(scheduler.getTimebase());
	count = 3;
}
 
void loop() {
	scheduler.scheduleAll();
	if(delflag){
		delflag = false;
		Serial.print("Cancella: ");Serial.println(count);
		if(count == 1){
			scheduler.deletePeriodicEvent(1, 500);
		}else if(count == 2){
			scheduler.deletePeriodicEvent(2, 500);
		}else if(count == 3){
			scheduler.deletePeriodicEvent(1, 1000);
		}
		count--;
	}
	if(addflag){
		addflag = false;
		count++;
		Serial.print("Inserisci: ");Serial.println(count);
		if(count == 1)
			scheduler.addPeriodicEvent(onHalfSecondEvents, 1, 500);
		else if(count == 2)
			scheduler.addAsyncEvent(epoch20sec, 2, 6000, 5000, 500, true);
		else if(count == 3)
			scheduler.addPeriodicEvent(onSecondEvents, 1, 1000);
	}
	delay(1);
}
