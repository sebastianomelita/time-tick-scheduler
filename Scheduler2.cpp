#include "Scheduler2.h"

void AsyncEvntB::doEvent(unsigned long step){
	//Serial.println("AsyncEvntA: ");
	counter += time; // aggiorna il contatore della durata della temporizzazione
	//Serial.print("counter: ");Serial.println(counter);
	if(counter < slaveTime){
		//do nothing
	}else if(counter < slaveTime + duration){
		if(pevent != NULL)
			(*pevent)();
	}else{ // disabilita le ripetizioni temporizzate
		counter = 0;
		if(!periodic){
			//static_cast<Scheduler*>(sch)->enableEvent(slaveId,slaveTime); // abilita il suo master (ripetizioni temporizzate)
			static_cast<Scheduler*>(sch)->disableEvent(id,time); // disabilita se stesso
		}
	}
};
/*
void AsyncEvntA::doEvent(unsigned long step){

};
*/
Scheduler::Scheduler(){
	prec=0;
	step=0;
	nt = 1;
}

long Scheduler::getTime(unsigned long when){
	unsigned t = -1;
	int p = timeSearch(when, tasks, nt);
	if(p >= 0){
		t = tasks[p].step;
	}
	return t;
}

void Scheduler::setTimes(){
	tbase = findGCD();
	Serial.print("mcm: ");Serial.println(mcm);
	//Serial.print("tbase: ");Serial.println(tbase);
	for(int i=1; i < nt; i++){
		tasks[i].step = tasks[i].time / tbase; 
		Serial.print("step: ");Serial.println(tasks[i].step);
	}
	maxstepCalc();
}

unsigned Scheduler::getTimebase(){
	return tbase;
}

unsigned long Scheduler::getNsteps(){
	return nsteps;
}

void Scheduler::scheduleAll(){// scheduler engine. Place this in loop().
	bool go = false;
	
	// max speed scheduled events
	for(int j=0; j < tasks[0].enabled; j++){// only the first time
		(*tasks[0].events[j]->pevent)();// event callback function call
	}
	unsigned long diff = millis()-prec;
	if(diff > tbase){ //schedulatore per tempo base 
		prec += diff;
		//unsigned long ofst = diff / tbase; // right timeslot search
		//unsigned long remain = diff % tbase; 
		//step = (step + 1) % nsteps; // right timeslot placing
		//Serial.println(step);
		// variely timed scheduled events
		for(int i=1; i < nt; i++){// all times except the first
			//Serial.println("------------------------------------------");
			//Serial.print("i: ");Serial.println(i);
			//Serial.print("Steplist: ");
			//Serial.println(tasks[i].step);
			//if(!(step % tasks[i].step)){
			if(prec - tasks[i].prec > tasks[i].time){
				//Serial.print(" diff(");Serial.print(i);Serial.print(")");Serial.println(step - tasks[i].prec);
				tasks[i].prec = prec;
				
				//Serial.println("++++++++++++++++++++++++++++++++++++++");
				for(int j=0; j < tasks[i].enabled; j++){
					//Serial.print(" j: ");
					//Serial.println(j);
					tasks[i].events[j]->doEvent(step);// event callback function call	
				}
			}
		}
	}
}

int TCB::cerca(uint8_t order, Evnt **list,int pempty){
   int min=0;
   int max=pempty-1;
   int med;
   int c;
   
   while(min<=max){
       med=(min+max)/2;
       if(order < list[med]->order){
           max=med-1;
       }else if(order > list[med]->order){
           min=med+1;
       }else{
           min=max+1;
       }
   }
   if(order != list[med]->order){
       med=-1;
   }
   return med;
}

void TCB::sort(Evnt **evnts, uint8_t fe){
    int i,j;
	for(i=fe-1; i>=0; i--) {
        for(j=0; j<i; j++){
            if(evnts[j]->order > evnts[j+1]->order){
              scambia(&evnts[j], &evnts[j+1]);
			  // ogni oggetto deve sapere il suo indice
			  //evnts[j]->pos = j;		
			  //evnts[j+1]->pos = j+1;
            }  
        }
    }   
}

void TCB::scambia(Evnt **a, Evnt **b){
    Evnt* c=*a;
    *a=*b;
    *b=c;
}
// must be NEVENTS < DISABLED !!!
bool TCB::addEvent(Evnt *evnt){
	bool ok = false;
	if(fe < NEVENTS){
		//evnt->pos = fe;
		events[fe] = evnt;// default enabled
		fe++;// first empty
		if(evnt->enabled){
			enabled++; // counter of enabled increment
		}else{
			evnt->order = evnt->order + DISABLED;
		}
		sort(events, fe);// sort by priority
		ok = true;
	}
	return ok;
}

bool TCB::getEventState(uint8_t order){
	bool ok = false;
	int pos = cerca(order,events,fe);
	if(pos >= 0){
		ok = events[pos]->enabled;
	}
	return ok;
}

bool TCB::enableEvent(uint8_t order){
	bool ok = false;
	int pos = cerca(order + DISABLED,events,fe);
	//Serial.print("pos ");Serial.println(pos);		
	if(pos >= 0 && !events[pos]->enabled){// check if disabled for first
		//Serial.print("Enable made ");Serial.println(pos);	
		events[pos]->enabled = true;
		events[pos]->order = events[pos]->order - DISABLED;
		sort(events, fe);// place enabled on lower order zone
		enabled++;
		ok = true;
	}
	return ok;
}

bool TCB::disableEvent(uint8_t order){
	bool ok = false;
	int pos = cerca(order,events,fe);
	//Serial.print("pos ");Serial.println(pos);	
	if(pos >= 0 && events[pos]->enabled){// check if enabled for first
		//Serial.print("Dis made ");Serial.println(pos);	
		events[pos]->enabled = false;
		events[pos]->order = events[pos]->order + DISABLED;
		sort(events, fe);// place disabled on greatest order zone
		enabled--;
		ok = true;
	}
	return ok;
}

int Scheduler::addTime(unsigned long when){
	int p = -1;
	
	if(when==0){
		p=0;
	}else{
		p = timeSearch(when, tasks, nt);
	}
	
	if(p<0){ // se non lo trova
		if(nt < NTIMES){
			tasks[nt].time = when; // lo inserisce
			nt++;
			timeSort(tasks, nt); // ordina
			p = timeSearch(when, tasks, nt);
		}
	}
	return p;
}

// nt: event index. 0:max_speed, 1:first_time_scheduled, 2:second_time_scheduled,...
bool Scheduler::addPeriodicEvent(PEventCallback pevnt, uint8_t priority, unsigned long every){// periodic events reggistration. Place this in setup().
	bool ok = true;
	
	int p = addTime(every);
	if(p>=0){
		//Serial.print("when: ");Serial.println(when);
		//Serial.print("p add: ");Serial.println(p);
		if(tasks[p].addEvent(new PeriodicEvnt(this, every, pevnt, priority, true, PERIODIC))){
			setTimes();
		}
	}else{
		ok = false;
		Serial.println("ERRORE: indice di un tempo fuori range");
	}
	
	for(int i=0; i<nt; i++) {
		Serial.println(tasks[i].time);
	}

	return ok;
}

bool Scheduler::addAsyncEvent(PEventCallback pevnt, uint8_t priority, unsigned long when, unsigned long howlong, unsigned long every, bool repeat){// periodic events reggistration. Place this in setup().
	bool ok = true;
	
	int p = addTime(every);
	if(p>=0){
		AsyncEvntB *slave = new AsyncEvntB(this, every, when, priority, pevnt, priority, true, ASYNC_B, howlong, every, repeat);
		//p = addTime(every);
		if(p>=0){
			tasks[p].addEvent(slave); // add async event as disabled
			//setTimes();
		}
		PeriodicEvnt *dummy = new PeriodicEvnt(this, when+howlong, NULL, DISABLED-1, false, PERIODIC);// not enabled
		p = addTime(when+howlong);
		if(p>=0){
			tasks[p].addEvent(dummy); // add async event as disabled
			setTimes();
		}
	}else{
		ok = false;
		Serial.println("ERRORE: indice di un tempo fuori range");
	}
	Serial.print("every: ");Serial.println(every);	
	Serial.print("dummy: ");Serial.println(when+howlong); 	
	for(int i=0; i<nt; i++) {
		Serial.println(tasks[i].time);
	}
	return ok;
}

bool Scheduler::getEventState(uint8_t priority, unsigned long every){
	bool ok = false;
	int pos; 
	
	int p = timeSearch(every, tasks, nt);
	if(p >= 0){
		tasks[p].getEventState(priority);
	}
	return ok;
}

bool Scheduler::setEventState(uint8_t priority, bool state, unsigned long every){
	if(state){
		enableEvent(priority,every);
	}else{
		disableEvent(priority, every);
	}
}

bool Scheduler::disableEvent(uint8_t priority, unsigned long every){// call as needed everywhere on runtime
	bool ok = false;
	int p = timeSearch(every, tasks, nt);
	//Serial.print("dis time pos: ");Serial.println(p);
	if(p >= 0){
		tasks[p].disableEvent(priority);
	}
	return ok;
}

bool Scheduler::enableEvent(uint8_t priority, unsigned long every){// call as needed everywhere on runtime
	bool ok = false;
	int p = timeSearch(every, tasks, nt);
	if(p >= 0){
		tasks[p].enableEvent(priority);
	}
	return ok;
}
//HELPER FUNCTIONS---------------------------------------------------------------------------------------------
unsigned long Scheduler::gcd(unsigned long a, unsigned long b)
{
  if (a == 0)
        return b;
    return gcd(b % a, a);
}

unsigned long Scheduler::lcm(unsigned long m, unsigned long n, unsigned long gcd){
  return m / gcd * n;
}

unsigned long Scheduler::findGCD(){
  unsigned long mcd = tasks[1].time;
  unsigned long mcd2 = tasks[1].time;
  mcm = tasks[1].time;
  //Serial.println("-----------------------------------------------");
  for (uint8_t i = 2; i < nt; i++)
  {
	  mcd = gcd(tasks[i].time, mcd);
	  mcd2 = gcd(tasks[i].time, mcm);
	  mcm = lcm(tasks[i].time, mcm, mcd2);
	  //Serial.print("mcm ");Serial.println(mcm);
  }
  return mcd;
}

int Scheduler::timeSearch(unsigned long tosearch, TCB* list, int pempty){
   int min=0;
   int max=pempty-1;
   int med;
   
   while(min<=max){
       med=(min+max)/2;
       if(tosearch < list[med].time){
           max=med-1;
       }else if(tosearch > list[med].time){
           min=med+1;
       }else{
           min=max+1;
       }
   }
   if(tosearch != list[med].time){
       med=-1;
   }
   return med;
}

void Scheduler::timeSort(TCB* list, uint8_t fe){
    int i,j;
	for(i=fe-1; i>=0; i--) {
        for(j=0; j<i; j++){
            if(list[j].time > list[j+1].time){
              TCB app = list[j];
			  list[j] = list[j+1];
			  list[j+1] = app;
			  // ogni oggetto deve sapere il suo indice
			  //list[j].pos = j;
			  //list[j+1].pos = j+1;
            }  
        }
    }   
}

void Scheduler::maxstepCalc(){
	/*
	nsteps = tasks[1].step;
	int i;
	for(i=2; i<nt; i++) {
		if(nsteps < tasks[i].step){
			nsteps = tasks[i].step;
		}
	}*/
	//unsigned long div = nsteps / tasks[i].step;
	Serial.print("mcm: ");Serial.println(mcm);
	if(tbase>0)
		nsteps = mcm / tbase;
	Serial.print("nsteps: ");Serial.println(nsteps);
}
//END HELPER FUNCTIONS---------------------------------------------------------------------------------------------
