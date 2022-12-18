#include "Scheduler2.h"
#define ON0MSEC		0
#define ON500MS		1
#define ON1000MS	2
int led1 = 13;
int led2 = 12;
unsigned long list[] = {500,1000};
Scheduler scheduler(list,2);
int count = 0;

void onMaxSpeedEvents();
void onHalfSecondEvents();
void onSecondEvents();
void epochScheduler();
 
void periodicBlink(int led) {
  Serial.print("printing periodic blink led ");
  Serial.println(led);

  digitalWrite(led, !digitalRead(led));
}

void epochScheduler(){
	if(count < 10){
		Serial.print("Doing stuff... ");
		Serial.println(count);
	}else if(count == 10){
		Serial.println("Disable all... ");
		scheduler.disableEvent(1,ON1000MS);
		scheduler.disableEvent(1,ON500MS);
		digitalWrite(led1, LOW);
		digitalWrite(led2, LOW);
		Serial.println("Ending timers...");
	}else if(count < 20){
		Serial.print("Frized... ");
		Serial.println(count);
	}else if(count >= 20){
		Serial.print("Enable all... ");
		Serial.println(count);
		scheduler.enableEvent(1,ON1000MS);
		scheduler.enableEvent(1,ON500MS);
		Serial.println("Starting timers...");
		count = -1;
	}
  count++;
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
	Serial.println(F("Scheduler init"));
	Serial.print(F("Time base: "));
	Serial.println(scheduler.getTimebase());
	//scheduler.addEvent(onMaxSpeedEvents, 1, ON0MSEC);
	scheduler.addEvent(onHalfSecondEvents, 1, ON500MS);
	scheduler.addEvent(onSecondEvents, 1, ON1000MS);
	scheduler.addEvent(epochScheduler, 2, ON1000MS);
}
 
void loop() {
	scheduler.scheduleAll();
	delay(10);
}
