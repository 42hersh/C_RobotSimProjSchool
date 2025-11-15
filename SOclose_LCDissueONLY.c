const int LEFT_LINE_SENSOR = 0;
const int RIGHT_LINE_SENSOR = 4;
const int COLOR_SENSOR = 8;

const int MOTOR_FORWARD = 90;
const int MOTOR_TURN = 60;
const int LINE_THRESHOLD = 400;
const int BLUE_TARGET = 180;           // Center of blue
const int BLUE_TOLERANCE = 10;         // ±10 range

bool pickedOnce = false;
bool started = false;
bool carryingObject = false;

void setup() {
    lcd("Press sw1 to begin");
    keep_down();
}

void loop() {
    if (!started && sw1() == 0) {
        beep();
        lcd("Starting...");
        sleep(1000);
        started = true;
        lcd("Following line...");
    }

    if (started) {
        int left = analog(0);
        int right = analog(4);
        int color = analog(8);

        // --- PICK UP ONLY BLUE (170–190) ---
        if (!carryingObject && !pickedOnce &&
            color >= (BLUE_TARGET - BLUE_TOLERANCE) &&
            color <= (BLUE_TARGET + BLUE_TOLERANCE)) {
            
            motor(1, 0);
            motor(2, 0);
            lcd("Blue object!");
            sleep(500);
            keep_up();
            sleep(1200);
            carryingObject = true;
            pickedOnce = true;
            lcd("Picked up");
            sleep(800);
        }

        // --- DROP ON SW2 PRESS ---
        if (carryingObject && sw2() == 0) {
            motor(1, 0);
            motor(2, 0);
            lcd("Dropping...");
            sleep(500);
            keep_down();
            sleep(1200);
            carryingObject = false;
            lcd("Dropped");
            sleep(800);
        }

        // --- LINE FOLLOWING (UNCHANGED) ---
        followLine(left, right);
        sleep(10);
    }
}

// --- LINE FOLLOWING: DO NOT CHANGE ---
void followLine(int L, int R) {
    bool leftOnBlack = (L < LINE_THRESHOLD);
    bool rightOnBlack = (R < LINE_THRESHOLD);

    if (leftOnBlack && rightOnBlack) {
        motor(1, MOTOR_FORWARD);
        motor(2, MOTOR_FORWARD);
    }
    else if (leftOnBlack && !rightOnBlack) {
        motor(1, -MOTOR_TURN);
        motor(2, MOTOR_FORWARD/2);
    }
    else if (!leftOnBlack && rightOnBlack) {
        motor(1, MOTOR_FORWARD/2);
        motor(2, -MOTOR_TURN);
    }
    else {
        motor(1, MOTOR_FORWARD);
        motor(2, MOTOR_FORWARD);
        if (carryingObject) lcd("At box. Press sw2");
        else lcd("Following Line...");
    }
}