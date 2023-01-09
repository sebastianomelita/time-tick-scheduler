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

void epoch10secDisable(){
	scheduler.disableEvent(1,1000);
	scheduler.disableEvent(1,500);
	scheduler.enableEvent(1,10000);
	scheduler.disableEvent(2,10000);
	digitalWrite(led1, LOW);
	digitalWrite(led2, LOW);
	Serial.println("Ending timers...");
	Serial.print("Frized... ");
}

void epoch10secEnable(){
	Serial.print("Enable all... ");
	Serial.println(count);
	scheduler.enableEvent(1,1000);
	scheduler.enableEvent(1,500);
	scheduler.disableEvent(1,10000);
	scheduler.enableEvent(2,10000);
	Serial.println("Starting timers...");
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
	//scheduler.addEvent(onMaxSpeedEvents, 1, 0);
	scheduler.addPeriodicEvent(onHalfSecondEvents, 1, 500);
	scheduler.addPeriodicEvent(epoch10secEnable, 1, 10000);
	scheduler.addPeriodicEvent(epoch10secDisable, 2, 10000);
	scheduler.addPeriodicEvent(onSecondEvents, 1, 1000);
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
