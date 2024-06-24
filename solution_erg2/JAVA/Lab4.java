
public class Lab4 {

    public static void main(String[] args) {

        int numThreads = 4; // local variable (in main) | not passed
	
	    int n = 0; // local variable (in main) |  pass-by-value | pass to instance variable that returns val to main (l. 18)
		CounterThread counterThreads[] = new CounterThread[numThreads]; // local variable (in main) | not passed
		for (int i = 0; i < numThreads; i++) { // local variable (in main) | pass-by-value
		    counterThreads[i] = new CounterThread(i, n); // pass-by-value arguments (i -> στοίχιση)
		    counterThreads[i].start();
		}
	
        for (int i = 0; i < numThreads; i++) {
            try {
				counterThreads[i].join();
				n += counterThreads[i].threadN;
            }
            catch (InterruptedException e) {}
		} 
		System.out.println("Main n = "+n); 
    }

}

class CounterThread extends Thread {
  	
    int threadID; // instance variable
    int threadN;  // instance variable
    
    public CounterThread(int tid, int n) { // pass-by-value parameters
        this.threadID = tid;
        this.threadN = n;
    }
  	
    public void run() {
              
        threadN = threadN + 1 + threadID;
        System.out.println("Thread "+threadID+ " n = "+threadN); 
    }
}

/* In this example, we don't have an array like in lab1,2 and 3, and instead we have
 * just a local variable n, which is passed by value to our thread class. In this case
 * however, the value is returned to main in line 18, and is added to the local variable
 * n, through the CounterThread instance variable threadN. I believe this case is the
 * same as Lab1, where since every instance has been initialized with 0, and especially
 * since n isn't global, the value inside every instance won't change. So we will have
 * an output like this:
 * 
 * T 3 n = 4
 * T 0 n = 1
 * T 1 n = 2
 * T 2 n = 3
 * Main n = 10
 * 
 * which proves to be the case. Example output: 
 * Thread 0 n = 1
 * Thread 2 n = 3
 * Thread 1 n = 2
 * Thread 3 n = 4
 * Main n = 10
 */