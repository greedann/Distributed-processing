package lab9;

import java.util.Random;

public class Consumer implements Runnable {
    private final Magazine magazine;
    private final Random random = new Random();
    private final String[] types = { "Type 1", "Type 2", "Type 3" };

    public Consumer(Magazine magazine) {
        this.magazine = magazine;
    }

    @Override
    public void run() {
        try {
            while (true) {
                String type = types[random.nextInt(types.length)];
                int amound = random.nextInt(10) + 1;
                magazine.getItem(type, amound);
                Thread.sleep(random.nextInt(1000));
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }
}
