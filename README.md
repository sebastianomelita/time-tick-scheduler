# time-tick-scheduler

Schedulatore di compiti basato sui time tick

Simulazione su ESP32 con Wokwi di un esempio di schedulazione periodica: https://wokwi.com/projects/352057010320512001

Simulazione su ESP32 con Wokwi di un esempio di timer asincrono: https://wokwi.com/projects/352243906494838785

		
    		```C++	Scheduler()```
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
