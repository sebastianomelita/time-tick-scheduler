#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__
#include <string.h>
#include <Arduino.h>
#define NEVENTS		20
#define NTIMES		10
#define PERIODIC 0
#define ASYNC_A	 1
#define ASYNC_B	 2

// https://en.wikibooks.org/wiki/Computer_Science_Design_Patterns/Mediator
//per disaccoppiare Scheduler e gli eventi è necessario far derivare entrambi 
//da genitori che non hanno riferimenti incrociati (pattern mediatore).
//Inoltre, per poter invocare doEvent() su eventi diversi è necessario
//realizzare un late binding con un metodo virtuale

using PEventCallback = void (*)(); //type aliasing
#define DISABLED	128

class Sched{
	public:
	uint8_t id;
};

class Evnt{
	public:
		Sched *sch;
		unsigned order;
		bool enabled;
		bool tochange;
		uint8_t type;
		uint8_t id;
		//uint8_t pos;
		unsigned long time;
		unsigned long counter;
		PEventCallback pevent;
		Evnt(Sched *s, unsigned long when, PEventCallback x, uint8_t priority, bool enable, uint8_t etype){
			time = when;
			sch = s;
			pevent = x;
			id = priority;
			order = priority;
			enabled = enable;
			type = etype;
			counter = 0;
		};
		virtual void doEvent(unsigned long step);	//late binding
};

			//late binding
class PeriodicEvnt: public Evnt{
	public:
		void doEvent(unsigned long step){
			if(pevent != NULL)
				(*pevent)();
		};
		PeriodicEvnt(Sched *x, unsigned long y, PEventCallback z, uint8_t u, bool v, uint8_t t):Evnt(x,y,z,u,v,t){};
};
/*
class AsyncEvntA: public Evnt{
	public:
		unsigned long slaveTime;
		uint8_t slaveId;
		unsigned long duration;
		unsigned long interval;
		bool periodic;
		void doEvent(unsigned long step);
		
		AsyncEvntA(Sched *x, unsigned long y, unsigned long z, uint8_t m, PEventCallback n, uint8_t k, bool u, uint8_t v, unsigned long howlong, unsigned long every, bool repeat):Evnt(x,y,n,k,u,v){
			duration = howlong; 
			interval = every; 
			periodic = repeat; 
			slaveTime = z;
			slaveId = m;
		};
};
*/
class AsyncEvntB: public Evnt{
	public:
		unsigned long slaveTime;
		uint8_t slaveId;
		unsigned long duration;
		unsigned long interval;
		bool periodic;
		void doEvent(unsigned long step);
		
		AsyncEvntB(Sched *x, unsigned long y, unsigned long z, uint8_t m, PEventCallback n, uint8_t k, bool u, uint8_t v, unsigned long howlong, unsigned long every, bool repeat):Evnt(x,y,n,k,u,v){
			duration = howlong; 
			interval = every; 
			periodic = repeat; 
			slaveTime = z;
			slaveId = m;
		};
};

class TCB{
	public:
		Evnt *events[NEVENTS];
		//uint8_t pos;
		uint8_t fe;// first empty
		uint8_t enabled;
		unsigned long step;
		unsigned long time;
		TCB(){step = 0; time = 0; enabled = 0; fe = 0;};
		bool addEvent(Evnt *);
		void sort(Evnt **, uint8_t);
		void scambia(Evnt **, Evnt **);
		int cerca(uint8_t, Evnt **, int);
		bool getEventState(uint8_t order);
		bool enableEvent(uint8_t);
		bool disableEvent(uint8_t);
		//bool disableEventPos(uint8_t pos);
		//bool enableEventPos(uint8_t pos);
};		

class Scheduler: public Sched{
	private:
		uint8_t nt;
		TCB tasks[NTIMES];
		unsigned long tbase;
		unsigned long prec=0;
		volatile unsigned long step;
		int gcd(int, int);
		unsigned long nsteps;
		unsigned findGCD();
		void timeSort(TCB*, uint8_t);
		int timeSearch(unsigned long, TCB*, int);
		void maxstepCalc();
		void setTimes();
		int addTime( unsigned long);

	public:
		Scheduler();
		bool addPeriodicEvent(PEventCallback pevnt, uint8_t priority, unsigned long when);
		bool addAsyncEvent(PEventCallback pevnt, uint8_t priority, unsigned long when, unsigned long howlong, unsigned long every, bool repeat);
		void scheduleAll();
		unsigned getTimebase();
		unsigned long getNsteps();
		long getTime(unsigned long when);
		bool getEventState(uint8_t order, unsigned long);
		bool setEventState(uint8_t order, bool state, unsigned long);
		bool enableEvent(uint8_t, unsigned long);
		bool disableEvent(uint8_t, unsigned long);
};

#endif
