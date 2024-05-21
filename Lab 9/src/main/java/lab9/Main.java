package lab9;

public class Main {
    public static void main(String[] args) {
        Magazine magazine = new Magazine(10);

        Thread producer1 = new Thread(new Producer(magazine));
        Thread producer2 = new Thread(new Producer(magazine));
        Thread consumer1 = new Thread(new Consumer(magazine));
        Thread consumer2 = new Thread(new Consumer(magazine));

        producer1.start();
        producer2.start();
        consumer1.start();
        consumer2.start();
    }
}
