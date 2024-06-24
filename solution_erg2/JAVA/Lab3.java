
   
public class Lab3 {
  
    public static void main(String[] args) {

        int numThreads = 4; // local variable (in main) | pass-by-value
		
		SharedCounter count = new SharedCounter(numThreads); // local variable (in main) | pass-by-reference
			
		CounterThread counterThreads[] = new CounterThread[numThreads]; // local variable (in main) | not passed
		for (int i = 0; i < numThreads; i++) { // local variable (in main) | pass-by-value (στοίχιση)
		    counterThreads[i] = new CounterThread(i, count); // i: pass-by-value argument (στοίχιση), count: pass-by-reference argument
		    counterThreads[i].start();
		}

        for (int i = 0; i < numThreads; i++) {
            try {
				counterThreads[i].join();
				count.n = count.n + counterThreads[i].threadN;
            }
            catch (InterruptedException e) {}
		} 
        for (int i = 0; i < numThreads; i++)
			System.out.println("a["+i+"] = "+count.a[i]);
			
		System.out.println("n = "+count.n); 
    }
}

class SharedCounter {
	
	int n; // instance variable | pass to instance variable that returns val to main
    int[] a; // instance variable | pass to instance variable that returns val to main
    
    public SharedCounter (int numThreads) {
		
			this.n = 0;
			this.a = new int[numThreads];
	    
			for (int i = 0; i < numThreads; i++) // local variable (outside of main)
				this.a[i] = 0; 
	}		

}    

class CounterThread extends Thread {
  	
    int threadID; // instance variable
    int threadN; // instance variable
    SharedCounter threadCount; // instance variable (global in the context of every CounterThread instance)
    
    public CounterThread(int tid, SharedCounter c) { // tid: pass-by-value parameter, c: pass-by-reference parameter
        this.threadID = tid;
        this.threadCount = c;
        this.threadN = threadCount.n;
        
    }
  	
    public void run() {
  
        threadN = threadN + 1 + threadID;  
        threadCount.a[threadID] = threadCount.a[threadID] + 1 + threadID;
		System.out.println("Thread "+threadID+" n = "+ threadN +"  a["+threadID+"] ="+ threadCount.a[threadID]); 
    }
}

/* Looking at it, it kinda seems like the most difficult to understand, because of the
 * usage of the SharedCounter class, which acts like a global in the context of every
 * CounterThread instance, since the same pointer to the class is passed to every
 * CounterThread object. So everything is readable and writable from SharedCounter
 * inside CounterThread. So when it's instance variables n and a are accessed in main
 * on lines 20 and 25 respectively, the values there will be a byproduct of whatever
 * processing happens in the different CounterThread objects.
 * 
 * However the different operations happening in run, and the way that the variables
 * are managed I believe doesn't really change the way it will behave from the other
 * examples. Variable threadN is once again defined only in a local context, so
 * any change to it's value inside CounterThread won't affect it inside SharedCounter.
 * So when main accesses it, it will always get threadID + 1. Thus, n at the end of main
 * will once again be 1 + 2 + 3 + 4. The same goes for a, since it's passed
 * by reference, any change inside CounterThread will affect it inside SharedCounter, 
 * and when main accesses it, it will have this form in memory a = {1, 2, 3, 4} (threadID + 1).
 * I believe an example output will be:
 * 
 * T 2 n = 3 a[2] = 3
 * T 0 n = 1 a[0] = 1
 * T 1 n = 2 a[1] = 2
 * T 3 n = 4 a[3] = 4
 * a[2] = 3
 * a[0] = 1
 * a[1] = 2
 * a[3] = 4
 * n = 10
 * 
 * We run it and we are proven right. Example output:
 * Thread 0 n = 1  a[0] =1
 * Thread 1 n = 2  a[1] =2
 * Thread 2 n = 3  a[2] =3
 * Thread 3 n = 4  a[3] =4
 * a[0] = 1
 * a[1] = 2
 * a[2] = 3
 * a[3] = 4
 * n = 10n = 10
 */