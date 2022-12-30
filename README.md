# time-tick-scheduler

Schedulatore di compiti basato sui [time tick](https://github.com/sebastianomelita/ArduinoBareMetal/blob/master/tasksched.md)

Scegliere i vari **tempi in gioco** in maniera opportuna, tale da consentire un agevole calcolo automatico sia del **tempo base** (in modo che non risulti troppo piccolo) sia dei **contatori sentinella** dei time tick degli eventi (multipli del tempo base).

Gli **eventi** possono essere **inseriti**, associati ai loro tempi, **senza** seguire un **ordine** particolare utilizzando una delle due funzioni ```addPeriodicEvent()``` e ```addAsyncEvent()```. 

Da **tenere presente** che:
- Per **ogni tempo** definito nello schedulatore è possibile inserire fino a **127 eventi**, limite imposto dalla profondità in bit scelta per alcune variabili di conteggio.
- ```#define NEVENTS``` nel file Scheduler2.h definisce il numero massimo attuale, per **default impostato a 20**
- ```#define NTIMES``` nel file Scheduler2.h definisce il numero massimo attuale, per **default impostato a 20**

Il **tempo base** dei time tick() è scelto automaticamente calcolando il **massimo comune divisore** dei vari tempi in gioco.

Simulazione su ESP32 con Wokwi di un esempio di **schedulazione periodica**: https://wokwi.com/projects/352057010320512001

Simulazione su ESP32 con Wokwi di un esempio di **timer asincrono**: https://wokwi.com/projects/352243906494838785

Simulazione su ESP32 con Wokwi di un esempio con scheduling ad **ogni loop**: https://wokwi.com/projects/352419045941500929

Simulazione su ESP32 con Wokwi di un esempio di debouncer per **pulsante toggle**: https://wokwi.com/projects/352463685677474817


### **Metodi** della classe **```Scheduler()```**:		
    		
- **```Scheduler()```** Costruttore
- **```bool addPeriodicEvent(PEventCallback pevnt, uint8_t priority, unsigned long every)```**. Aggiunge un evento periodico definito da:
	- ```pevnt```: callback evento da schedulare
	- ```priority``` ordine di esecuzione oppure id del task in un certo slot temporale
	- ```every```: slot temporale in cui l'evento deve essere ripetuto. La funzione definisce un nuovo slot se non esiste uno con un tempo uguale, oppure si limita ad associare l'evento ad uno slot esistente. In unione al campo priority **identifica** l'evento periodico.
- **```bool addAsyncEvent(PEventCallback pevnt, uint8_t priority, unsigned long when, unsigned long howlong, unsigned long every, bool repeat)```**. Aggiunge un evento aperiodico definito in più da:
	- ```when```: slot temporale in cui l'evento comincia. La funzione definisce un nuovo slot se non ne esiste uno con un tempo uguale, oppure si limita ad associare l'evento ad uno slot esistente. 
	- ```howlong```: quanto tempo dura la schedulazione
	- ```every```: slot temporale in cui l'evento deve essere ripetuto. La funzione definisce un nuovo slot se non ne esiste uno con un tempo uguale, oppure si limita ad associare l'evento ad uno slot esistente. In unione al campo priority **identifica** l'evento aperiodico.
- **```void scheduleAll()```**. Esegue tutti i task. Da inserire nel loop() o in una callback richiamata ad intervalli regolari (interrupt).
- **```unsigned getTimebase()```**. Restituisce il tempo base dei time tick.
- **```unsigned long getNsteps()```**. Restituisce il massimo valore del conteggio circolare dei time tick.
- **```long getTime(unsigned long when)```**. Restituisce il conteggio in time tick del tempo fornito come parametro.
- **```bool getEventState(uint8_t priority, unsigned long every)```**. Restituisce lo stato (abilitato o disabilitato) di un evento. Un evento è identificato dalla coppia (tempo in millisecondi, ordine).
- **```bool setEventState(uint8_t priority, bool state, unsigned long every)```**. Imposta lo stato (abilitato o disabilitato) di un evento.
- **```bool enableEvent(uint8_t, unsigned long every)```**. Abilita un evento identificato dalla coppia (tempo in millisecondi, ordine).
- **```bool disableEvent(uint8_t, unsigned long every)```**. Disabilita un evento identificato dalla coppia (tempo in millisecondi, ordine).
