#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__
#include <string.h>
#include <Arduino.h>
#define NEVENTS		20
#define NTIMES		10

using PEventCallback = void (*)(); //type aliasing
#define DISABLED	128

class PeriodicEvnt{
	public:
		unsigned order;
		bool enabled;
		PEventCallback pevent;
		PeriodicEvnt(PEventCallback, uint8_t, bool);
};		

class Scheduler{
	private:
		PeriodicEvnt *events[NTIMES][NEVENTS];
		uint8_t ntimes;
		uint8_t fe[NTIMES];// first empty
		uint8_t enabled[NTIMES];
		unsigned long steplist[NTIMES];
		unsigned long tbase;
		unsigned tlen;
		unsigned long prec=0;
		volatile unsigned long step;
		int gcd(int, int);
		unsigned long nsteps;
		unsigned findGCD(unsigned long [], uint8_t);
		void sort(PeriodicEvnt **, uint8_t);
		void scambia(PeriodicEvnt **, PeriodicEvnt **);
		int cerca(uint8_t, PeriodicEvnt **, int);
		void maxstepCalc();

	public:
		Scheduler(unsigned long [], unsigned);
		bool addEvent(PEventCallback , uint8_t, int);
		bool enableEvent(uint8_t, int);
		bool disableEvent(uint8_t, int);
		void scheduleAll();
		unsigned getTimebase();
		unsigned long getNsteps();
		unsigned getTime(uint8_t);
		void setTimes(unsigned long [], unsigned);
		bool getEventState(uint8_t order, int nt);
		bool setEventState(uint8_t order, bool state, int nt);
};

#endif
