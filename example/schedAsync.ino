#include "Scheduler2.h"
int led1 = 13;
int led2 = 12;

Scheduler scheduler;
int count = 0;

void onMaxSpeedEvents();
void onHalfSecondEvents();
void onSecondEvents();
void epochScheduler();
void epoch10sec();
void epoch20sec();
 
void periodicBlink(int led) {
  Serial.print("printing periodic blink led ");
  Serial.println(led);

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
	Serial.begin(115200); 
	scheduler.addPeriodicEvent(onHalfSecondEvents, 1, 500);
	scheduler.addAsyncEvent(epoch20sec, 1, 6000, 5000, 500, true); 	  // repeat
	//scheduler.addAsyncEvent(epoch20sec, 2, 6000, 5000, 500, false); // no repeat
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
