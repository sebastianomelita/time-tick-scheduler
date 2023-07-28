# time-tick-scheduler 

>[English version](english.md)

Schedulatore di compiti basato sui [time tick](https://github.com/sebastianomelita/ArduinoBareMetal/blob/master/tasksched.md)

Lo schedulatore permette la **creazione** e la **cancellazione** dinamica **a runtime** dei task, così come la **abilitazione** e la **disabilitazione** degli stessi, sempre **a runtime**.

Scegliere i vari **tempi in gioco** in maniera opportuna, tale da consentire un agevole calcolo automatico sia del **tempo base** (in modo che non risulti troppo piccolo) sia dei **contatori sentinella** dei time tick degli eventi (multipli del tempo base).

Gli **eventi** possono essere **inseriti**, associati ai loro tempi, **senza** seguire un **ordine** particolare delle invocazioni utilizzando una delle due funzioni ```addPeriodicEvent()``` e ```addAsyncEvent()```. 

Da **tenere presente** che:
- Per **ogni tempo** definito nello schedulatore è possibile inserire fino a **127 eventi**, limite imposto dalla profondità in bit scelta per alcune variabili di conteggio.
- ```#define NEVENTS``` nel file Scheduler2.h definisce il numero massimo attuale, per **default impostato a 20**
- ```#define NTIMES``` nel file Scheduler2.h definisce il numero massimo attuale, per **default impostato a 20**

Il **tempo base** dei time tick() è scelto automaticamente calcolando il **massimo comune divisore** dei vari tempi in gioco.

Se più task con **periodicità diversa** occorrono nello stesso tempo (tick), viene data priorità a quelli **più lenti** dimodochè se un eventuale **ritardo** di un **task veloce** dovesse spalmarsi su più tick rapidi, l'**errore di tempo** introdotto coinvolgerebbe solo il **primo tick breve** successivo e non avrebbe effetto sui **tick lenti** (di periodicità più grande) dato che sono sempre **serviti prima**.

La libreria permette un'**alternativa** al **ricampionamento nel loop(**) per compensare i **ritardi** di un task, sfruttando la proprietà di **prerilascio** forzato di un task che possiedono gli interrupt. Se i tick sono **interrupt based** allora essi accadono sempre e comunque nel tempo esatto a loro deputato anche in presenza di un task che **ritarda** la sua esecuzione. 

**Esempi:**

- Simulazione su ESP32 con Wokwi di un esempio di **schedulazione periodica**: https://wokwi.com/projects/352057010320512001

- Simulazione su ESP32 con Wokwi di un esempio di **timer asincrono**: https://wokwi.com/projects/352243906494838785

- Simulazione su ESP32 con Wokwi di un esempio con scheduling ad **ogni loop**: https://wokwi.com/projects/352419045941500929

- Simulazione su ESP32 con Wokwi di un esempio di debouncer per **pulsante toggle**: https://wokwi.com/projects/352463685677474817

- Simulazione su ESP32 con Wokwi di un esempio di test per **task con delay random**: https://wokwi.com/projects/352549012781285377

- Simulazione su ESP32 con Wokwi di test per **task con delay random** con ISR: https://wokwi.com/projects/353217720393475073

- Simulazione su ESP32 con Wokwi di **task schedulati da interrupt** in una ISR: https://wokwi.com/projects/353402752887006209

- Simulazione su ESP32 con Wokwi di **task schedulati mediante toggleEvent()**: https://wokwi.com/projects/353405525014893569

- Simulazione su ESP32 con Wokwi di **task creati e cancellati dinamicamente a runtime**: https://wokwi.com/projects/371341648643932161


### **Metodi** della classe **```Scheduler()```**:		
    		
- **```Scheduler()```** Costruttore
- **```bool addPeriodicEvent(PEventCallback pevnt, uint8_t priority, unsigned long every, bool enabled = true)```**. Aggiunge un evento periodico definito da:
	- ```pevnt```: callback evento da schedulare
	- ```priority``` ordine di esecuzione oppure id del task in un certo slot temporale
	- ```every```: slot temporale in cui l'evento deve essere ripetuto. La funzione definisce un nuovo slot se non esiste uno con un tempo uguale, oppure si limita ad associare l'evento ad uno slot esistente. In unione al campo priority **identifica** l'evento periodico.
	- ```enabled```: se ```true``` (valore di default) l'evento è registrato e attivato, se se ```false``` l'evento è registrato ma non è attivato
- **```bool addAsyncEvent(PEventCallback pevnt, uint8_t priority, unsigned long when, unsigned long howlong, unsigned long every, bool repeat)```**. Aggiunge un evento aperiodico definito in più da:
	- ```when```: slot temporale in cui l'evento comincia. La funzione non definisce un nuovo slot, gestisce un timer aggiornato in poling con la periodicità dello slot ```every```.
	- ```howlong```: quanto tempo dura la schedulazione. Non viene definito un nuovo slot per questo tempo ma soltanto per il tempo ```every```.
	- ```every```: slot temporale **base** in cui l'evento deve essere **ripetuto** (periodicità base). La funzione definisce un nuovo slot se non ne esiste uno con un tempo uguale, oppure si limita ad associare l'evento ad uno slot esistente. In unione al campo priority **identifica** l'evento aperiodico. Per **cancellarlo**, l'evento si può individuare con la coppia (```priority```, ```every```).
- **```bool Scheduler::deletePeriodicEvent(uint8_t priority, unsigned long every, bool test = false)```**. Cancella un evento periodico identificato in base alla sua priorità e alla sua periodicità base ```every```. Cancella sia eventi periodici che asincroni. Il parametro opzionale test esegue l'operazione "a secco" (dry) per motivi di test cioè verificando la sua fatttibilità senza eseguire nessuna cancellazzione (restituisce solo il parametro booleano di avvenuta cancellazione. Per impostazione predefinita, la funzione di test è disabilitata. 
- **```void scheduleAll()```**. Esegue tutti i task. Da inserire nel loop() o in una callback richiamata ad intervalli regolari.
- **```unsigned getTimebase()```**. Restituisce il tempo base dei time tick.
- **```long getTime(unsigned long when)```**. Restituisce il conteggio in time tick del tempo fornito come parametro.
- **```bool getEventState(uint8_t priority, unsigned long every)```**. Restituisce lo stato (abilitato o disabilitato) di un evento. Un evento è identificato dalla coppia (tempo every in millisecondi, priority).
- **```bool setEventState(uint8_t priority, bool state, unsigned long every)```**. Imposta lo stato (abilitato o disabilitato) di un evento.
- **```bool enableEvent(uint8_t priority, unsigned long every)```**. Abilita un evento identificato dalla coppia (tempo every in millisecondi, priority).
- **```bool disableEvent(uint8_t priority, unsigned long every)```**. Disabilita un evento identificato dalla coppia (tempo every in millisecondi, priority).
- **```bool toggleEvent(uint8_t priority, unsigned long every)```**.  cambia lo stato di un evento identificato dalla coppia (tempo every in millisecondi, priority).
- **```void scheduleAllISRFlagged(bool noflag=false)```**. Esegue tutti i task su segnalazione di un flag asserito dalla ```void timerISR()``` . Da inserire nel ```loop()``` dove viene eseguito il polling del flag ad ogni ciclo. Una chiamata esegue i task associati ad un tick solo se il flag è asserito, altrimenti non esegue nulla.
	- ```noflag```: disabilita il check del flag di task in esecuzione. Di default a false. Settarlo a false se ```void scheduleAllISRFlagged(true)``` può essere eseguita direttamente dentro una callback isocrona col tempo base. Più chiamate consecutive realizzano ciascuna un tick invocando tutti i task ad esso associati.
- **```void timerISR(void)```**. ISR richiamata ad intervalli regolari da un timer HW (interrupt).

### **Sitografia:**

- https://www.ics.uci.edu/~givargis/pubs/C50.pdf
- https://www.cs.ucr.edu/~vahid/rios/

>[English version](english.md)

