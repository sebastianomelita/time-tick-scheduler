#include "Scheduler2.h"
#define nsteps          12000        // numero di fasi massimo di un periodo generico

PeriodicEvnt::PeriodicEvnt(PEventCallback evnt, uint8_t priority, bool enbld){
	order = priority;
	pevent = evnt;
	enabled = enbld;
}

Scheduler::Scheduler(unsigned long timeList[], unsigned ntimes){
	if(ntimes < NTIMES){
		this->ntimes = ntimes;
		setTimes(timeList, ntimes);
		prec=0;
		step=0;
		memset(fe, 0, NTIMES);// number of events for times
		memset(enabled, 0, NTIMES);// number of enabled events for times
		for(int i=0; i<ntimes; i++){
			memset(events[i], NULL, NEVENTS);
		}
	}else{
		Serial.println("ERRORE: Numero di tempi eccessivo");
	}
}

unsigned Scheduler::getTime(uint8_t n){
	return steplist[n];
}

void Scheduler::setTimes(unsigned long timeList[], unsigned ntimes){
	tlen = ntimes;
	tbase = findGCD(timeList, tlen);
	for(int i=0; i < tlen; i++){
		steplist[i] = timeList[i] / tbase; 
		Serial.println(steplist[i]);
	}
}

unsigned Scheduler::getTimebase(){
	return tbase;
}

void Scheduler::scheduleAll(){// scheduler engine. Place this in loop().
	// max speed scheduled events
	for(int j=0; j < enabled[0]; j++){// only the first time
		(*events[0][j]->pevent)();// event callback function call
	}
	unsigned long diff = millis()-prec;
	if(diff > tbase){ //schedulatore per tempo base 
		prec = millis();
		if(diff > 2*tbase){// right timeslot elapsed
			unsigned long ofst = diff/tbase;// right timeslot search
			step = (step + ofst) % nsteps;// right timeslot placing
		}else{// right timeslot not elapsed
			step = (step + 1) % nsteps;
		}
		// variely timed scheduled events
		for(int i=1; i < tlen+1; i++){// all times except the first
			//Serial.print("Steplist: ");
			//Serial.print(steplist[i-1]);
			if(!(step % steplist[i-1])){
				//Serial.println(" si");
				for(int j=0; j < enabled[i]; j++){
					//Serial.print(" j: ");
					//Serial.print(j);
					if(events[i][j] != NULL){//protezione del valore
						//Serial.println(" do");
						(*events[i][j]->pevent)();// event callback function call
					}
				}
			}else{
				//Serial.println(" no");
			}
		}
	}
}
// nt: event index. 0:max_speed, 1:first_time_scheduled, 2:second_time_scheduled,...
bool Scheduler::addEvent(PEventCallback pevnt, uint8_t priority, int nt){// periodic events reggistration. Place this in setup().
	bool ok = false;
	
	if(nt >= 0 && nt < ntimes+1){
		if(fe[nt] < NEVENTS){
			events[nt][fe[nt]] = new PeriodicEvnt(pevnt, priority, true);// default enabled
			fe[nt]++;// first empty
			enabled[nt]++;// default enabled
			sort(events[nt], fe[nt]);// sort by priority
			ok = true;
		}
	}else{
		Serial.println("ERRORE: indice di un tempo fuori range");
	}
	return ok;
}

bool Scheduler::disableEvent(uint8_t order, int nt){// call as needed everywhere on runtime
	bool ok = false;
	int pos; 
	
	if(nt >= 0 && nt < ntimes+1){
		pos = cerca(order,events[nt],fe[nt]);
		if(pos >= 0 && pos < fe[nt] && events[nt][pos]->enabled){// check if enabled for first
			events[nt][pos]->enabled = false;
			events[nt][pos]->order = events[nt][pos]->order + DISABLED;
			sort(events[nt], fe[nt]);// place disabled on greatest order zone
			enabled[nt]--;
			ok = true;
		}
	}
	return ok;
}

bool Scheduler::enableEvent(uint8_t order, int nt){// call as needed everywhere on runtime
	bool ok = false;
	int pos; 
	
	if(nt >= 0 && nt < ntimes+1){
		pos = cerca(order + DISABLED,events[nt],fe[nt]);
		//Serial.print("pos ");Serial.println(pos);		
		if(pos >= 0 && pos < fe[nt] && !events[nt][pos]->enabled){// check if disabled for first
			events[nt][pos]->enabled = true;
			events[nt][pos]->order = events[nt][pos]->order - DISABLED;
			sort(events[nt], fe[nt]);// place enabled on lower order zone
			enabled[nt]++;
			ok = true;
		}
	}
	return ok;
}
//HELPER FUNCTIONS---------------------------------------------------------------------------------------------
int Scheduler::gcd(int a, int b)
{
  if (a == 0)
    return b;
  return gcd(b % a, a);
}

unsigned Scheduler::findGCD(unsigned long arr[], uint8_t n){
  unsigned result = arr[0];
  for (uint8_t i = 1; i < n; i++)
  {
    result = gcd(arr[i], result);
  }
  return result;
}

void Scheduler::sort(PeriodicEvnt **evnts, uint8_t fe){
    int i,j;
	for(i=fe-1; i>=0; i--) {
        for(j=0; j<i; j++){
            if(evnts[j]->order > evnts[j+1]->order){
              scambia(&evnts[j], &evnts[j+1]);
            }  
        }
    }   
}

void Scheduler::scambia(PeriodicEvnt **a,PeriodicEvnt **b){
    PeriodicEvnt* c=*a;
    *a=*b;
    *b=c;
}

int Scheduler::cerca(uint8_t order, PeriodicEvnt **list,int pempty){
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
//END HELPER FUNCTIONS---------------------------------------------------------------------------------------------