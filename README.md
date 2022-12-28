# time-tick-scheduler

Schedulatore di compiti basato sui [time tick](https://github.com/sebastianomelita/ArduinoBareMetal/blob/master/tasksched.md)

Scegliere i vari **tempi in gioco** in maniera opportuna, tale da consentire un agevole calcolo automatico sia del **tempo base** (in modo che non risulti troppo piccolo) sia dei **contatori sentinella** dei time tick degli eventi (multipli del tempo base).

Il **tempo base** dei time tick() è scelto automaticamente calcolando il **massimo comune divisore** dei vari tempi in gioco.

Simulazione su ESP32 con Wokwi di un esempio di **schedulazione periodica**: https://wokwi.com/projects/352057010320512001

Simulazione su ESP32 con Wokwi di un esempio di **timer asincrono**: https://wokwi.com/projects/352243906494838785

**Metodi** della classe **```Scheduler()```**:		
    		
- **```Scheduler()```** Costruttore
- **```bool addPeriodicEvent(PEventCallback pevnt, uint8_t priority, unsigned long when)```**. Aggiunge un evento periodico definito da:
	- ```pevnt```: callback evento da schedulare
	- ```priority``` ordine di esecuzione oppure id del task in un certo slot temporale
	- ```when```: slot temporale in cui l'evento deve essere ripetuto. La funzione definisce un nuovo slot se non esiste uno con un tempo uguale, oppure si limita ad associare l'evento ad uno slot esistente.
- **```bool addAsyncEvent(PEventCallback pevnt, uint8_t priority, unsigned long when, unsigned long howlong, unsigned long every, bool repeat)```**. Aggiunge un evento aperiodico definito in più da:
	- ```when```: slot temporale in cui l'evento comincia. La funzione definisce un nuovo slot se non ne esiste uno con un tempo uguale, oppure si limita ad associare l'evento ad uno slot esistente.
	- ```howlong```: quanto tempo dura la schedulazione
	- ```every```: slot temporale in cui l'evento deve essere ripetuto. La funzione definisce un nuovo slot se non ne esiste uno con un tempo uguale, oppure si limita ad associare l'evento ad uno slot esistente.
- **```void scheduleAll()```**. Esegue tutti i task
- **```unsigned getTimebase()```**. Restituisce il tempo base dei time tick.
- **```unsigned long getNsteps()```**. Restituisce il massimo valore del conteggio circolare dei time tick.
- **```long getTime(unsigned long when)```**. Restituisce il conteggio in time tick del tempo fornito come parametro.
- **```bool getEventState(uint8_t order, unsigned long)```**. Restituisce lo stato (abilitato o disabilitato) di un evento. Un evento è identificato dalla coppia (tempo in millisecondi, ordine).
- **```bool setEventState(uint8_t order, bool state, unsigned long)```**. Imposta lo stato (abilitato o disabilitato) di un evento.
- **```bool enableEvent(uint8_t, unsigned long)```**. Abilita un evento identificato dalla coppia (tempo in millisecondi, ordine).
- **```bool disableEvent(uint8_t, unsigned long)```**. Disabilita un evento identificato dalla coppia (tempo in millisecondi, ordine).
