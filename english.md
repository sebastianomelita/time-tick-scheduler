# time-tick-scheduler 

>[Versione italiana](README.md)

Task scheduler based on [time ticks](https://github.com/sebastianomelita/ArduinoBareMetal/blob/master/tasksched.md)

Choose the various **times in play** in an appropriate manner, such as to allow easy automatic calculation of both the **base time** (so that it is not too small) and the **sentinel counters** of the time ticks of the events (multiples of the base time).

**Events** can be **inserted**, associated with their times, **without** following a particular **order** of invocations using one of the two functions ```addPeriodicEvent()``` and ` ``addAsyncEvent()```.

To ** bear in mind ** that:
- For **each time** defined in the scheduler it is possible to enter up to **127 events**, a limit imposed by the bit depth chosen for some counting variables.
- ```#define NEVENTS``` in the Scheduler2.h file defines the current maximum number, for **default set to 20**
- ```#define NTIMES``` in the Scheduler2.h file defines the current maximum number, for **default set to 20**

The **time base** of the time ticks() is automatically chosen by calculating the **greatest common divisor** of the various times involved.

If several tasks with **different periodicity** occur at the same time (tick), priority is given to the **slower** ones so that if any **delay** of a **fast task** were to spread across several fast ticks, the introduced **time error** would affect only the following **first short tick** and would have no effect on **slow ticks** (of greater periodicity) since they are always **served first* *.

The library allows an **alternative** to **resampling in the loop(**) to compensate for **delays** of a task, exploiting the forced **prerelease** property of a task that has interrupts. If the ticks are **interrupt based** then they always occur in the exact time assigned to them even in the presence of a task that **delays** its execution.

**Examples:**

- Simulation on ESP32 with Wokwi of an example of **periodic scheduling**: https://wokwi.com/projects/352057010320512001

- Simulation on ESP32 with Wokwi of an example of **asynchronous timer**: https://wokwi.com/projects/352243906494838785

- Simulation on ESP32 with Wokwi of an example with **every loop** scheduling: https://wokwi.com/projects/352419045941500929

- Simulation on ESP32 with Wokwi of a debouncer example for **toggle button**: https://wokwi.com/projects/352463685677474817

- Simulation on ESP32 with Wokwi of a test example for **tasks with random delay**: https://wokwi.com/projects/352549012781285377

- ESP32 simulation with Wokwi test for **random delay** tasks with ISR: https://wokwi.com/projects/353217720393475073

- Simulation on ESP32 with Wokwi of **interrupt-scheduled tasks** in an ISR: https://wokwi.com/projects/353402752887006209

- Simulation on ESP32 with Wokwi of **tasks scheduled via toggleEvent()**: https://wokwi.com/projects/353405525014893569

### **Methods** of class **```Scheduler()```**:
    		
- **```Scheduler()```** Costruttore
- **```bool addPeriodicEvent(PEventCallback pevnt, uint8_t priority, unsigned long every)```**. Aggiunge un evento periodico definito da:
	- ```pevnt```: callback event to be scheduled
	- ```priority``` execution order or id of the task in a certain time slot
	- ```every```: time slot in which the event should be repeated. The function defines a new slot if there is no one with the same time, or simply associates the event to an existing slot. Together with the priority field, **identifies** the periodic event.
- **```bool addAsyncEvent(PEventCallback pevnt, uint8_t priority, unsigned long when, unsigned long howlong, unsigned long every, bool repeat)```**.Adds an aperiodic event additionally defined by:
	- ```when```: time slot in which the event begins. The function defines a new slot if one with the same time does not exist, or simply associates the event to an existing slot.
	- ```howlong```: how long does the schedule last. A new slot is not defined for this time but for every+howlong time.
	- ```every```: time slot in which the event should be repeated. The function defines a new slot if one with the same time does not exist, or simply associates the event to an existing slot. Together with the priority field, **identifies** the aperiodic event.
- **```void scheduleAll()```**. Perform all tasks. To be inserted in the loop() or in a callback both called at regular intervals.
- **```unsigned getTimebase()```**. Returns the base time of time ticks.
- **```long getTime(unsigned long when)```**.Returns the time tick count of the time supplied as a parameter.
- **```bool getEventState(uint8_t priority, unsigned long every)```**. Returns the status (enabled or disabled) of an event. An event is identified by the pair (time every in milliseconds, priority).
- **```bool setEventState(uint8_t priority, bool state, unsigned long every)```**. Set the status (enabled or disabled) of an event.
- **```bool enableEvent(uint8_t priority, unsigned long every)```**. Enable an event identified by the pair (time every in milliseconds, priority).
- **```bool disableEvent(uint8_t priority, unsigned long every)```**. Disable an event identified by the pair (time every in milliseconds, priority).
- **```bool toggleEvent(uint8_t priority, unsigned long every)```**.  Change the state of an event identified by the pair (time every in milliseconds, priority).
- **```void scheduleAllISRFlagged(bool noflag=false)```**. Execute all tasks upon signaling a flag asserted by ```void timerISR()``` . To be placed in the ```loop()``` where the flag is polled every loop. A call executes the tasks associated with a tick only if the flag is asserted, otherwise it does nothing.
	- ```noflag```: disable flag check of running task. Defaults to false. Set it to false if ```void scheduleAllISRFlagged(true)``` can be executed directly inside an isochronous time-base callback. Several consecutive calls each make a tick by invoking all the tasks associated with it.
- **```void timerISR(void)```**. ISR called at regular intervals by a HW timer (interrupt).

### **Sitography:**

- https://www.ics.uci.edu/~givargis/pubs/C50.pdf
- https://www.cs.ucr.edu/~vahid/rios/

>[Versione italiana](README.md)
