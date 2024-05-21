package lab9;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Magazine {
    private final int maxCapacity;
    private final Map<String, Integer> items = new HashMap<>();
    private int currentCapacity = 0;
    private final Lock lock = new ReentrantLock();
    private final Condition notFull = lock.newCondition();

    public Magazine(int maxCapacity) {
        this.maxCapacity = maxCapacity;
    }

    public void addItem(String type, int amound) throws InterruptedException {
        lock.lock();
        try {
            while (currentCapacity + amound > maxCapacity) {
                notFull.await();
            }
            items.put(type, items.getOrDefault(type, 0) + amound);
            currentCapacity += amound;
            System.out.println("Collected item: " + type + ", amound: " + amound);
        } finally {
            lock.unlock();
        }
    }

    public void getItem(String type, int amound) throws InterruptedException {
        lock.lock();
        try {
            if (items.getOrDefault(type, 0) == 0) {
                System.out.println("Lack of item: " + type);
                return;
            }
            int available = items.get(type);
            if (available >= amound) {
                items.put(type, available - amound);
                currentCapacity -= amound;
                System.out.println("Collected item: " + type + ", amound: " + amound);
            } else {
                items.put(type, 0);
                currentCapacity -= available;
                System.out.println("Collected item: " + type + ", amound: " + available);
            }
            notFull.signalAll();
        } finally {
            lock.unlock();
        }
    }
}
