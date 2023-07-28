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
	//step=1;
	nt = 0;
	timerFlag = false;
	addTime(0);
}

long Scheduler::getTime(unsigned long when){
	unsigned t = 0;
	int p = timeSearch(when, tasks);
	if(p >= 0){
		t = tasks[p].step;
	}
	return t;
}

void Scheduler::setTimes(){
	Serial.println("setTimes: -------------");
	tbase = findGCD();
	Serial.print("mcm: ");Serial.println(mcm);
	Serial.print("tbase: ");Serial.println(tbase);
	for(int i=0; i < nt-1; i++){
		//if(tbase > 0){
		tasks[i].step = tasks[i].time / tbase; 
		Serial.print("step: ");Serial.println(tasks[i].step);
		////tasks[i].prec = 0;
		//}
	}
	////prec = 0;
	Serial.println("Fine setTimes: -----------------");
}

unsigned Scheduler::getTimebase(){
	return tbase;
}

void Scheduler::scheduleAll(){// scheduler engine. Place this in loop().
	// max speed scheduled events
	for(int j=0; j < tasks[nt-1].fe; j++){// only the first time
		if(tasks[nt-1].events[j]->enabled)
			(*tasks[nt-1].events[j]->pevent)();// event callback function call
		
	}
	
	if(millis()-prec >= tbase){ //schedulatore per tempo base 
		prec += tbase;
		// variely timed scheduled events
		for(int i=0; i < nt-1; i++){// all times except the first
			//Serial.println("------------------------------------------");
			//Serial.print("i: ");Serial.println(i);
			//Serial.print("time: ");
			//Serial.println(tasks[i].time);
			//if(!(step % tasks[i].step)){
			//Serial.print("diff: ");Serial.println(prec - tasks[i].prec);
			if(prec - tasks[i].prec > tasks[i].time){
				tasks[i].prec += tasks[i].time; // aggiornamento prec del task
				//Serial.print("time: ");Serial.println(tasks[i].time);
				//Serial.print("diff: ");Serial.println(prec - tasks[i].prec);
				//Serial.print("fe: ");Serial.println(tasks[i].fe);
				//Serial.println("++++++++++++++++++++++++++++++++++++++");
				for(int j=0; j < tasks[i].fe; j++){// fino all'ultimo della lista
					//Serial.print(" j: ");Serial.println(j);
					if(tasks[i].events[j]->enabled)// se l'evento è abilitato
						tasks[i].events[j]->doEvent(tasks[i].step);// event callback function call	
				}
			}
		}
	}
}


// https://www.ics.uci.edu/~givargis/pubs/C50.pdf
void Scheduler::scheduleAllISRFlagged(bool noflag){// scheduler engine. Place this in loop().	
	// max speed scheduled events
	for(int j=0; j < tasks[nt-1].enabled; j++){// only the first time
		(*tasks[nt-1].events[j]->pevent)();// event callback function call
	}
	
	if(timerFlag || noflag){ //schedulatore per tempo base 
		// variely timed scheduled events
		for(int i=0; i < nt-1; i++){// all times except the first
			//Serial.println("------------------------------------------");
			//Serial.print("i: ");Serial.println(i);
			//Serial.print("Steplist: ");
			//Serial.println(tasks[i].step);
			if(tasks[i].elapsed >= tasks[i].time){
				//Serial.println("++++++++++++++++++++++++++++++++++++++");
				for(int j=0; j < tasks[i].fe; j++){// fino all'ultimo della lista
					//Serial.print(" j: ");
					//Serial.println(j);
					if(tasks[i].events[j]->enabled)// se l'evento è abilitato
						tasks[i].events[j]->doEvent(tasks[i].step);// event callback function call	
				}
				tasks[i].elapsed = 0;
			}
			tasks[i].elapsed += tbase;
		}
		timerFlag = false;
	}
}

void Scheduler::timerISR(void) {
   if (timerFlag) {
		for(int i=0; i < nt; i++){// all times except the first
			tasks[i].elapsed += tbase;
		}	
   }
   else {
      timerFlag = true;
   }
   return;
}

int TCB::getNEvents(){
	return fe + 1;
}

int TCB::cerca(uint8_t order, Evnt **list){
   int min=0;
   int max=fe-1;
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
// ordinamento crescente
void TCB::sort(Evnt **evnts){
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
		Serial.print("fe prima add ");Serial.println(fe);
		fe++;// first empty
		Serial.print("fe dopo add ");Serial.println(fe);
		if(evnt->enabled){
			enabled++; // counter of enabled increment
		}else{
			evnt->order = evnt->order + DISABLED;
		}
		sort(events);// sort by priority
		ok = true;
	}
	return ok;
}

bool TCB::delEvent(uint8_t order, bool test){
	bool ok = false;	
	
	Serial.print("test: ");Serial.println(test);
	int pos = cerca(order,events);
	if(pos < 0){
		pos = cerca(order + DISABLED,events);
	}
	
	if(pos >= 0){
		Serial.print("fe prima canc ");Serial.println(fe);
		Serial.print("evento trovato in posizione ");Serial.println(pos);
		if(!test){
			delete events[pos];// garbace of the deleted event object
			for(int i = pos; i+1 < fe; i++){
				events[i] = events[i+1];
			}
			fe--;// first empty
			Serial.println("Del event: evento cancellato");
			Serial.print("fe: ");Serial.println(fe);
			Serial.println("Inizio stampa eventi--: ");
			for(int i=0; i<fe; i++) {
				Serial.println(events[i]->order);
			}
			Serial.println("Fine stampa eventi--: ");
		}
		ok = true;
	}else{
		Serial.println("evento non trovato");
	}
	return ok;
}

bool TCB::getEventState(uint8_t order){
	bool ok = false;
	int pos = cerca(order,events);
	if(pos >= 0){
		ok = events[pos]->enabled;
	}
	return ok;
}

bool TCB::enableEvent(uint8_t order){
	bool ok = false;
	int pos = cerca(order + DISABLED,events);
	//Serial.print("pos ");Serial.println(pos);		
	if(pos >= 0 && !events[pos]->enabled){// check if disabled for first
		//Serial.print("Enable made ");Serial.println(pos);	
		events[pos]->enabled = true;
		events[pos]->order = events[pos]->order - DISABLED;
		sort(events);// place enabled on lower order zone
		enabled++;
		ok = true;
	}
	return ok;
}

bool TCB::disableEvent(uint8_t order){
	bool ok = false;
	int pos = cerca(order,events);
	//Serial.print("pos: ");Serial.println(pos);	
	if(pos >= 0 && events[pos]->enabled){// check if enabled for first
		//Serial.print("Dis made ");Serial.println(pos);	
		events[pos]->enabled = false;
		events[pos]->order = events[pos]->order + DISABLED;
		sort(events);// place disabled on greatest order zone
		enabled--;
		ok = true;
	}
	return ok;
}

int Scheduler::addTime(unsigned long when){
	int p = -1;
	/*
	if(when==0){
		p=0;
	}else{
		p = timeSearch(when, tasks);
	}
	*/
	Serial.print("add time when: ");Serial.println(when);	
	p = timeSearch(when, tasks);
	Serial.print("add time pos: ");Serial.println(p);	
	if(p<0){ // se non lo trova
		if(nt < NTIMES){
			tasks[nt].time = when; // lo inserisce
			tasks[nt].elapsed = tasks[nt].time; // time init
			tasks[nt].prec = prec -tasks[nt].time; // time init
			tasks[nt].fe = 0; // reset first empty
			tasks[nt].step = 1; 
			nt++;
			timeSort(tasks); // ordina
			setTimes();
			p = timeSearch(when, tasks);
		}
		Serial.print("nt: ");Serial.println(nt);	
	}
	return p;
}

bool Scheduler::delTime(unsigned long when){
	int p = -1;

	//Serial.print("when: ");Serial.println(when);	
	p = timeSearch(when, tasks);
	//Serial.print("pos1: ");Serial.println(p);	
	return delTimeByPos(p);
}
// in posizione nt-1 ci sta sempre il tempo 0!
bool Scheduler::delTimeByPos(int pos){
	bool erased = false;
	//Serial.println("pos--");Serial.println(pos);
	//Serial.println("nt del");Serial.println(nt);
	if(pos >= 0 && pos < nt-1 && tasks[pos].fe == 0){ // se è entro il range degli inseriti (nt è escluso perchè il tempo 0 non si deve cancellare)
		for(int i = pos; i+1 < nt; i++){
			tasks[i] = tasks[i+1]; // cancellazione per spostamento a sin
			//tasks[i].prec = 0; // time init
		}
		//if(nt > 0)
		nt--;
		setTimes();
		//Serial.print("nt--: ");Serial.println(nt);
	    //Serial.println("Stampa tempi--: ");
		//for(int i=0; i<nt; i++) {
		//	Serial.println(tasks[i].time);
		//}
		erased = true;
		Serial.println("Cancelled");
	}
	return erased;
}

// nt: event index. 0:max_speed, 1:first_time_scheduled, 2:second_time_scheduled,...
bool Scheduler::addPeriodicEvent(PEventCallback pevnt, uint8_t priority, unsigned long every, bool enabled){// periodic events reggistration. Place this in setup().
	bool ok = true;
	
	int p = addTime(every);
	if(p>=0){
		//Serial.print("when: ");Serial.println(when);
		//Serial.print("p add: ");Serial.println(p);
		if(tasks[p].addEvent(new PeriodicEvnt(this, every, pevnt, priority, true, PERIODIC))){
			if(!enabled)
				tasks[p].disableEvent(priority);
		}
	}else{
		ok = false;
		Serial.println("ERRORE: indice di un tempo fuori range");
	}
	Serial.println("Stampa tempi:");
	for(int i=0; i<nt; i++) {
		Serial.println(tasks[i].time);
	}
	Serial.println("Fine Stampa tempi:");
	return ok;
}

// unica periodicità sul tempo every
bool Scheduler::addAsyncEvent(PEventCallback pevnt, uint8_t priority, unsigned long when, unsigned long howlong, unsigned long every, bool repeat){// periodic events reggistration. Place this in setup().
	bool ok = true;
	
	int p = addTime(every);
	if(p>=0){
		AsyncEvntB *slave = new AsyncEvntB(this, every, when, priority, pevnt, priority, true, ASYNC_B, howlong, every, repeat);
		if(p>=0){
			tasks[p].addEvent(slave); // add async event as disabled
		}
	}else{
		ok = false;
		Serial.println("ERRORE: indice di un tempo fuori range");
	}
	Serial.print("every: ");Serial.println(every);	
	for(int i=0; i<nt; i++) {
		Serial.println(tasks[i].time);
	}
	return ok;
}

bool Scheduler::deletePeriodicEvent(uint8_t priority, unsigned long every, bool test){// periodic events delete
	bool ok = false;
	int p = -1;
	
	Serial.print("every: ");Serial.println(every);	
	p = timeSearch(every, tasks);
	Serial.print("pos: ");Serial.println(p);
	if(p>=0){
		if(tasks[p].delEvent(priority, test)){
			if(tasks[p].fe==0){// se il tempo non contiene alcun evento cancellalo
				if(!test){
					if(delTimeByPos(p))
						Serial.print("tempo ");Serial.print(every);Serial.println(" cancellato.");		
				}			
			}
			ok = true;
		}else{
			Serial.print("ERRORE: evento con priorità ");Serial.print(priority);Serial.println(" non trovato.");
		}
	}else{
		Serial.print("ERRORE: tempo ");Serial.print(every);Serial.println(" non trovato.");
	}
	Serial.println("dopo 1");
	return ok;
}

bool Scheduler::getEventState(uint8_t priority, unsigned long every){
	bool ok = false;
	
	int p = timeSearch(every, tasks);
	//Serial.print("p: ");Serial.println(p);
	if(p >= 0){
		ok = tasks[p].getEventState(priority);
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

bool Scheduler::toggleEvent(uint8_t priority, unsigned long every){
	bool state = getEventState(priority, every);
	Serial.print("state: ");Serial.println(state);
	if(state){
		disableEvent(priority,every);
		
	}else{
		enableEvent(priority, every);
	}
	return getEventState(priority, every);
}

bool Scheduler::disableEvent(uint8_t priority, unsigned long every){// call as needed everywhere on runtime
	bool ok = false;
	int p = timeSearch(every, tasks);
	//Serial.print("dis time pos: ");Serial.println(p);
	if(p >= 0){
		tasks[p].disableEvent(priority);
	}
	return ok;
}

bool Scheduler::enableEvent(uint8_t priority, unsigned long every){// call as needed everywhere on runtime
	bool ok = false;
	int p = timeSearch(every, tasks);
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
  unsigned long mcd = tasks[0].time;
  unsigned long mcd2 = tasks[0].time;
  mcm = tasks[0].time;
  if(tasks[0].time  != 0){
	  //Serial.println("-----------------------------------------------");
	  for (uint8_t i = 1; i < nt-1; i++)
	  {
		  //if(tasks[i].time != 0){
		  mcd = gcd(tasks[i].time, mcd);
		  mcd2 = gcd(tasks[i].time, mcm);
		  mcm = lcm(tasks[i].time, mcm, mcd2);
		  //}
		  //Serial.print("mcm ");Serial.println(mcm);
	  }
  }else{
	  mcd = 1;
  }
  return mcd;
}

int Scheduler::timeSearch(unsigned long tosearch, TCB* list){
   int min=0;
   int max=nt-1;
   int med;
   
   //Serial.println("Begin sort ----------------------------------------------------------");
   Serial.println("------------------");
   Serial.print("nt: ");Serial.println(nt);
   Serial.println("Stampa tempi: ");
   Serial.println("------------------");
   for(int i=0; i<nt; i++) {
		Serial.println(tasks[i].time);
	}
   Serial.println("------------------");
   while(min<=max){
       med=(min+max)/2;
	   Serial.println(med);
       if(tosearch > list[med].time){
           max=med-1;
       }else if(tosearch < list[med].time){
           min=med+1;
       }else{
           min=max+1;
       }
   }
   //Serial.println("End search ----------------------------------------------------------");
   if(tosearch != list[med].time){
       med=-1;
   }
   Serial.print("p: ");Serial.println(med);
   return med;
}
// ordinamento decrescente
void Scheduler::timeSort(TCB* list){
    int i,j;
	for(i=nt-1; i>=0; i--) {
        for(j=0; j<i; j++){
            if(list[j].time < list[j+1].time){
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
/*
void Scheduler::maxstepCalc(){
	//unsigned long div = nsteps / tasks[i].step;
	Serial.print("mcm: ");Serial.println(mcm);
	if(tbase>0)
		nsteps = mcm / tbase;
	Serial.print("nsteps: ");Serial.println(nsteps);
}
*/
//END HELPER FUNCTIONS---------------------------------------------------------------------------------------------
