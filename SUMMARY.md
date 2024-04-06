### Problem 1
###### Method
For this problem I attempted to implement a Lazy Linked List. I decided on this structure because it although it has two lock nodes, current and predicessor, for add and remove, it saves contention with contains. This locking structure would be helpful as each thread/servant would be able to make changes to the chain concurrently as long as nodes were not shared in the operation. It was difficult to maintain the lock acquisition which is a major reason why my implementation is failing.

###### Correctness
In practice, my structure does not work, but in theory the Lazy Linked List maintains correctness for all add, remove, and contains methods. This is done through adding a marked variable to nodes so that contain can be validated and add and remove are guarenteed by locking the current and predicessor and validating to ensure that every object verified before the operation is performed.

###### Efficiency
This strategy would be efficient when implemented correctly because all of the servants could be making changes and in turn progress with processing all the presents and thank yous. Although there may be some lock contention, especially when the list is small, as it grows the threads are less likely to contest. Until near the end, when the chain is limited in the number of presents available, where contention will rise again. The act of alternating may keep the chain smaller than desired, and increasing contention.

###### Experimental Evaluation
It crashes - no data

### Problem 2
###### Method
I chose to perform a type of blocking synchronization, where every thread would have to wait at the start and end of every step for the other threads. By doing this, all threads will be placing the data in the correct interval without any jumping ahead, but slows down the program because threads cannot preprocess anything past the next interval. When it is a threads turn to place their interval data in it is one value at a time.

###### Correctness
Because the threads always have to be in sync to perform there next scan, the threads will never jump ahead or miss an interval. Additionally, because the threads have to meet up at both the start and end of the method, they will return to being in sync for every iteration. (interval differences are broken)

###### Effeciency
This strategy is very ineffecient because of the overhead with synching and not being able to process any scan information in advance. Constantly waiting for threads to sync up and then put in values sequentially limits any benefit of being able to process values concurrently. Although the threads remain synced it only manages to slow down the program.

###### Experimental Evaluation
Testing with a few different numbers of threads I get:
```
1 thread runs 24000 "minutes" in 0.02 seconds
3 threads run 24000 "minutes" in 0.81 seconds
5 threads run 24000 "minutes" in 4.96 seconds
8 threads run 24000 "minutes" in 12.35 seconds
```

Here we see just how ineffecient the multithreading is. As more threads are included the slower the program runs, after 8 threads it was not terminating.