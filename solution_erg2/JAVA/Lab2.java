
   
public class Lab2 {

    public static void main(String[] args) {

        int numThreads = 4; // local variable (in main) | not passed
		
		int n = 0; // local variable (in main) | pass-by-value
	    int[] a = new int[numThreads]; // local variable (in main) | pass-by-reference
	    for (int i = 0; i < numThreads; i++)
			a[i] = 0; 
		
		CounterThread counterThreads[] = new CounterThread[numThreads]; // local variable (in main) | not passed
		for (int i = 0; i < numThreads; i++) { // local variable (in main) | pass-by-value
		    counterThreads[i] = new CounterThread(i, n, a); // i (στοίχιση), n: pass-by-value arguments, a: pass-by-reference argument
		    counterThreads[i].start();
		}

        for (int i = 0; i < numThreads; i++) {
            try {
				counterThreads[i].join();
            }
            catch (InterruptedException e) {}
		}
		
		System.out.println("n = "+n);
		
		for (int i = 0; i < numThreads; i++)
			System.out.println("a["+i+"] = "+a[i]); 	
    }

}

class CounterThread extends Thread {
  	
    int threadID; // instance variable
    int n; // instance variable
    int[] a; // instance variable
    
    public CounterThread(int tid, int n, int[] a) { // tid, n: value parameters, a: reference paramter
        this.threadID = tid;
        this.n = n;
        this.a = a;
    }
  	
    public void run() {
        
        n = n + threadID ;
        a[threadID] = a[threadID] + n ;
        System.out.println("Thread "+threadID+ " my a = "+a[threadID]); 
    }
}


/* Lab2 is more straightforward than Lab1, mostly because we don't have any global variables.
 * The i and n arguments are passed by value, so n won't change and will remain 0 for the
 * duration of the processes runtime (also because it isn't returned by e.g. run). On the
 * other hand, a is obviously passed by reference. So every index will get a new value by the end.
 * Inside every thread, n will just become the value of threadId, and since a[0..3] = 0,
 * the value of every index of the array will essentially have the threadID number.
 * So the general output will be:
 * 
 * Thread i my a = i
 * n = 0
 * a[0] = 0
 * a[1] = 1
 * a[2] = 2
 * a[3] = 3
 * 
 * which proves to be true. Example output:
 * Thread 3 my a = 3
 * Thread 0 my a = 0
 * Thread 1 my a = 1
 * Thread 2 my a = 2
 * n = 0
 * a[0] = 0
 * a[1] = 1
 * a[2] = 2
 * a[3] = 3
 */