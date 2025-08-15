                                    CSE 773 Assignment 1

Suraj Patil 24m0837
Shubh Nagar 24m0764
Abhishek Singh 24m0808

Task 2A

- Change the threshold values by running the calibration.c program to get the values 
  for cache hit cycles.

Synchronization:-

- Recieverfirst, it waits for sender in a loop until it gets 20 hits at a predetermined offset.

- When sender is executed, it first accesses the address to sync with the receiver for 40 times, which triggers the receiver to exit from wait.

Accuracy :- Accuracy is a bit uncertain for each execution it changes, but on average the accuracy was around 93 to 97%.

Bandwidth:- We were able to achieve a max bandwidth of 8.8Kbits per second but on an expense of high varying accuracy.Atlast we achieve bandwidth of 2kbits per second with low variance in accuracy.
 
 Findings:-
 
 - Channel interval 1 million gives around 2207kbits per second and around 99 percent accuracy(non-deterministic).
 - Maximum bandwidth of 4.41k bits per second achieved at an accuracy of 98 percent.
 - 8.8 kbits per second at an accuracy of 80 percent.

Task 2B

Initial Setup:-
    
- Here tsend.c and rsend.c programs are used to get the average values for counter to decide if sender is sending bit 1 or 0.
- Run the calibrate.sh script to get the average values of counter for bit 1 and 0.
- ./calibrate.sh $array_increment_step_size_for_sender.
- Once we get the counters then change then threshold values of receiver.c program.

Data Transmission:-

- Then execute sender.c and receiver.c program to start transmitting the data.
- `Sender program keeps on thrashing for each time window based on bit 1 or 0 i.e how much arr
  array to trash.
- Receiver prgram keeps on accessing its array for each time window and calculates counter value
- Then we compare it with the average counter values to deduce if the  sender is sending bit 1 or 0.
    
For task 2B we were able to receive only this part of the message and were able to achieve a bandwidth of 10-15 bits per second.

Hi Simran,

I’m Raj. I’ve noticed you a few times and thought
