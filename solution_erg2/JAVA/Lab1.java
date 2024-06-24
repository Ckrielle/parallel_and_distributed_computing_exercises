
   
public class Lab1 {

    static int n = 0; // class variable | global | pass to instance variable (threadN l. 44) that returns val to main (l. 26)
    static int[] a; // class variable | pass-by-global

    public static void main(String[] args) {

        int numThreads = 4; // local variable (in main) | not passed
		
	    a = new int[numThreads];
	    
	    for (int i = 0; i < numThreads; i++) // local variable (in main)
			a[i] = 0;
			
		CounterThread counterThreads[] = new CounterThread[numThreads]; // local variable (in main) | not passed
		for (int i = 0; i < numThreads; i++) {
		    counterThreads[i] = new CounterThread(i); // value arguments | pass-by-value (στοίχιση)
		    counterThreads[i].start();
		}

        for (int i = 0; i < numThreads; i++) {
            try {
				counterThreads[i].join();
				n = n + counterThreads[i].threadN;
            }
            catch (InterruptedException e) {}
		} 
        for (int i = 0; i < numThreads; i++)
			System.out.println("a["+i+"] = "+a[i]);
			
		System.out.println("n = "+n); 
    }
 }   

class CounterThread extends Thread {
	  	
  	int threadID; // instance variable
	int threadN; // instance variable
	    
	public CounterThread(int tid) {  // value parameter
	    this.threadID = tid;
	    this.threadN = Lab1.n;
	}
	  	
	public void run() {
	  
	    threadN = threadN + 1 + threadID;  
	    Lab1.a[threadID] = Lab1.a[threadID] + 1 + threadID;
		System.out.println("Thread "+threadID+" n = "+ threadN +"  a["+threadID+"] ="+ Lab1.a[threadID]); 
	}
}

/* Let's attempt and think of how the program will behave before executing it.
 * In lines 14-15 the global array a is initialized with only 0's. So every thread
 * that has a reference to it will find 0 in all of it's indexes, but will also be
 * able to change these values. In line 17 the declaration of the object array variable
 * counterThreads happens, which is of type CounterThread, the threading class of this example.
 * There are numThreads (4) threads being created. In lines 18-20 we find the threads
 * initialization loop, which creates an object counterThreads for every index, and starts the threads.
 * 
 * Since for every CounterThread instance the instance variable threadN gets the value
 * of the global Lab 1 variable n, which is 0, I think every thread will have threadN = 0.
 * Let's take for example the first thread. What will happen in the run method is
 * 
 * threadN = 0 + 1 + 0 = 1
 * Lab1.a[0] = 0 + 1 + 0 = 1
 * stdout -> Thread 0 n = 1 a[0] = 1
 * 
 * Since a is a global array, it's 0th element has changed in memory to 1, and so
 * since every other thread has a pointer to the same memory area, they will also
 * have this change. The global variable n however I believe is different.
 * Since n is passed as a value to the instance variable threadN, even though it's
 * value has changed to 1, this change wasn't written on the address of the global variable n.
 * So I believe the correct way the second thread will behave is this
 * 
 * threadN = 0 (because 0 was passed in the constructor) + 1 + 1 (idx of thread 2) = 2
 * Lab1.a[1] = 0 + 1 + 1 = 2
 * stdout -> Thread 1 n = 2 a[1] = 2
 * 
 * However an interesting part is line 26, where the value of the n global var is
 * changed, and is the value of the addition between the value of the local instance
 * variable threadN, and the value of the Lab1 class variable n. One could think
 * that this will affect the value of threadN , however I think it's safe as it is
 * written. When the thread creation happens and execution starts, all the local
 * threadN's will have the value of 0. Since they have all been initialized with 0,
 * and inside the thread only the local instance var threadN is used (i.e. all
 * instances don't point to the gloabl address), when the time comes for n to 
 * essentially increment, the change won't affect any other thread. It would be
 * interesting to see how moving around the code in line 26 would affect the program
 * (for example to place it before the join or after start).
 * 
 * So after the threads have executed, main will probably print
 * 
 * a[0] = 1
 * a[1] = 2
 * a[2] = 3
 * a[3] = 4
 * n = 10
 * 
 * Running the code shows that we were right in our general train of thought, obviously
 * mixing up the execution of the threads
 * 
 * example output
 * Thread 2 n = 3  a[2] =3
 * Thread 3 n = 4  a[3] =4
 * Thread 0 n = 1  a[0] =1
 * Thread 1 n = 2  a[1] =2
 * a[0] = 1
 * a[1] = 2
 * a[2] = 3
 * a[3] = 4
 * n = 10
 */