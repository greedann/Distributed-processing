package lab9;

import java.util.Random;

public class Producer implements Runnable {
    private final Magazine magazine;
    private final Random random = new Random();
    private final String[] types = { "Type 1", "Type 2", "Type 3" };

    public Producer(Magazine magazine) {
        this.magazine = magazine;
    }

    @Override
    public void run() {
        try {
            while (true) {
                String type = types[random.nextInt(types.length)];
                int amound = random.nextInt(2) + 1;
                magazine.addItem(type, amound);
                Thread.sleep(random.nextInt(1000));
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }
}
