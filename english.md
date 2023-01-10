Task scheduler based on time ticks

Choose the various times in play in an appropriate manner, such as to allow easy automatic calculation of both the base time (so that it is not too small) and the sentinel counters of the time ticks of the events (multiples of the base time).

Events can be inserted, associated with their times, without following a particular order of invocations using one of the two functions addPeriodicEvent() and addAsyncEvent().

It should be noted that:

For each time defined in the scheduler it is possible to enter up to 127 events, a limit imposed by the bit depth chosen for some counting variables.
#define NEVENTS in the Scheduler2.h file defines the current maximum number, by default set to 20
#define NTIMES in the Scheduler2.h file defines the current maximum number, by default set to 20
The time base of the time tick() is chosen automatically by calculating the greatest common divisor of the various times in play.

If several tasks with different periodicity occur in the same time (tick), priority is given to the slower ones so that if any delay of a fast task were to spread over several fast ticks, the time error introduced would involve only the first successive short tick and would have no effect on slow ticks (of higher periodicity) since they are always served first.

The library allows an alternative to resampling in loop() to compensate for task delays, by exploiting the forced prerelease property of a task that has interrupts. If the ticks are interrupt based then they always occur in the exact time assigned to them even in the presence of a task that delays its execution.
