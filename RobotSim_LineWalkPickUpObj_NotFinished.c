const int LEFT_LINE_SENSOR = 0;
const int RIGHT_LINE_SENSOR = 4;
const int COLOR_SENSOR = 8;

const int MOTOR_FORWARD = 90;
const int MOTOR_TURN = 60;          // Reverse wheel speed for sharp turn
const int LINE_THRESHOLD = 400;     // <400 = ON BLACK
const int BLUE_COLOR = 3;

bool pickedOnce = false;
bool started = false;
bool carryingObject = false;


void setup() {
    lcd("Press sw1 to begin");
    keep_down();  // Gripper open initially
}


void loop() {

    // --- START ---
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

        // --- PICK UP BLUE OBJECT ---
        if (!carryingObject && analog(8) > 1 && !pickedOnce) {
            keep_up();
            motor(1, 0);
            motor(2, 0);
            lcd("Blue object!");
            sleep(500);
            keep_up();          // Close gripper & lift object
            sleep(1200);
            carryingObject = true;
            lcd("Picked up");
            sleep(800);
            pickedOnce = true;
        }

        // --- DROP BLUE OBJECT ON SW2 PRESS ---
        if (carryingObject && sw2() == 0) {
            motor(1, 0);
            motor(2, 0);
            lcd("Dropping...");
            sleep(500);
            keep_down();        // Open gripper & release object
            sleep(1200);
            carryingObject = false;
            lcd("Dropped");
            sleep(800);
        }

        // --- LINE FOLLOWING ---
        followLine(left, right);

        sleep(10);  // Small delay for stability
    }
}


// --- LINE FOLLOWING: SHARP TURNS (UNCHANGED) ---
void followLine(int L, int R) {

    bool leftOnBlack = (L < LINE_THRESHOLD);
    bool rightOnBlack = (R < LINE_THRESHOLD);

    if (leftOnBlack && rightOnBlack) {

        // BOTH ON BLACK → STRAIGHT
        motor(1, MOTOR_FORWARD);
        motor(2, MOTOR_FORWARD);
    }
    else if (leftOnBlack && !rightOnBlack) {

        // LEFT ON, RIGHT OFF → TURN RIGHT SHARPLY
        motor(1, -MOTOR_TURN);        // Right wheel backward
        motor(2, MOTOR_FORWARD / 2);  // Left wheel slower forward
    }
    else if (!leftOnBlack && rightOnBlack) {

        // LEFT OFF, RIGHT ON → TURN LEFT SHARPLY
        motor(1, MOTOR_FORWARD / 2);  // Right wheel slower forward
        motor(2, -MOTOR_TURN);        // Left wheel backward
    }
    else {

        // BOTH OFF (WHITE) → KEEP GOING FORWARD
        motor(1, MOTOR_FORWARD);
        motor(2, MOTOR_FORWARD);

        if (carryingObject)
            lcd("At box. Press sw2");
        else
            lcd("Following Line...");
    }
}
