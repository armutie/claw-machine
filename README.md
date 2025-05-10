# RobotC Claw Machine Controller

This repository contains the RobotC source code for a custom-built claw machine. The machine features coin detection, joystick-controlled gantry movement, an automated claw grab-and-lift sequence, and a homing procedure to return the claw and dispense any potential prize.

## Features

*   **Coin Detection:**
    *   Utilizes an EV3 Touch Sensor (`S1`) to detect coin insertion.
    *   Allows for multiple "tries" or attempts if multiple coins/activations are detected within `COIN_INPUT_TIME` (10 seconds).
    *   Provides on-screen feedback for coin input time and attempts.
    *   Plays a sound upon successful coin detection.
*   **Joystick Control:**
    *   Manual control of the gantry's X-axis (`motorA`) and Y-axis (`motorB`) using joystick buttons (Up, Down, Left, Right).
    *   Movement is constrained by software limits defined by motor encoder values (`y_limit`, `x_limit`, `MARGIN`).
*   **Automated Grab & Lift Sequence (`grab_procedure`):**
    *   Triggered by the "Enter" button on the joystick.
    *   Lowers the claw mechanism (`motorC`) to a defined depth (`z_limit`).
    *   Closes the claw/gripper (`motorD`) to a defined tightness (`grip_limit`).
*   **Automated Homing & Dispense (`home`):**
    *   Lifts the claw mechanism (`motorC`) to its top position.
    *   Returns the gantry's X-axis (`motorA`) and Y-axis (`motorB`) to their home positions (near encoder value 0 or `MARGIN`).
    *   Opens the claw/gripper (`motorD`) to release the prize.
*   **Game Timer:**
    *   A `TAKING_TOO_LONG` timer (60 seconds per attempt) to prevent indefinite play. If the timer expires, the current attempt ends, and the claw homes.
*   **Safety Feature:**
    *   Prevents the `grab_procedure` from activating if the claw is likely positioned over the prize chute/return area (based on X and Y encoder values), ensuring it only drops in the playfield.
    *   A short joystick "Enter" button press (less than 3 seconds) triggers the grab sequence. A longer press might be interpreted as an abort or to trigger homing immediately (depending on `time1[T2]` logic).
*   **On-Screen Display:** Provides feedback on game state, attempts, and coin input time using `displayTextLine`.

## Hardware Components

*   **Microcontroller:** LEGO Mindstorms EV3 Brick (acts as the central controller and hosts the physical buttons used by the custom joystick).
*   **Motors (all LEGO Mindstorms EV3 Motors with Integrated Encoders):**
    *   `motorA`: X-axis gantry movement (e.g., LEGO EV3 Large Motor)
    *   `motorB`: Y-axis gantry movement (e.g., LEGO EV3 Large Motor)
    *   `motorC`: Z-axis claw lowering/lifting (e.g., LEGO EV3 Medium Motor)
    *   `motorD`: Claw open/close mechanism (e.g., LEGO EV3 Medium Motor)
*   **Sensors:**
    *   `S1`: LEGO EV3 Touch Sensor (used for coin detection).
    *   Integrated Motor Encoders on all four motors (used for position control and defining movement limits).
*   **Input:**
    *   **Custom LEGO Joystick:** A custom-built joystick constructed from LEGO parts. This joystick is mechanically designed to press the physical buttons directly on the LEGO EV3 Brick (Up, Down, Left, Right, and Center/Enter buttons) to provide user input for movement and actions.
*   **Mechanical Structure:**
    *   **Frame:** Custom-built wooden frame, approximately 40cm x 40cm x 60cm.
    *   **Gantry System:**
        *   **X-axis Movement:** Utilizes a rack and pinion system driven by `motorA`. 
        *   **Y-axis Movement:** Utilizes a 3D-printed rack driven by a pinion gear connected to `motorB`.
        *   **Z-axis Movement (Claw Lift/Lower):** Employs a string wound by `motorC` (simple pulley system) for vertical movement of the claw.
    *   **Claw/Gripper:** Custom-designed and built using LEGO Technic parts.
    *   **Coin Mechanism:** Custom-built using cardboard, housing the EV3 Touch Sensor (`S1`) for coin detection.
    *   **Prize Chute:** Constructed using cardboard.


## Software & Logic Overview (RobotC)

The program is structured around a main game loop that manages different states:

1.  **Initialization:** Motor encoders are reset, and the coin sensor type is set.
2.  **Coin Input (`coin_sense` function):**
    *   Waits for a coin (Touch Sensor `S1` press).
    *   Manages a `COIN_INPUT_TIME` window for the user to insert coins, incrementing `tries` for each valid detection.
3.  **Gameplay Loop (per `try`):**
    *   Displays the current attempt number.
    *   Monitors joystick input using the `joy()` function, which translates button presses into numerical codes.
    *   **Manual Movement (`xyMove` function):** If directional buttons are pressed, motors A and B are driven until joystick buttons are released or encoder limits are reached.
    *   **Grab Trigger:** If the "Enter" button (`pressing == 4`) is pressed briefly (and not over the chute):
        *   The `grab_procedure()` is called to lower and close the claw.
        *   The `home()` procedure is then called to lift, return to the home position, and open the claw.
    *   **Timeout (`took_too_long` function):** A 60-second timer (`T3`) runs during each attempt. If it expires (`slowpoke` becomes true), the attempt ends, and the `home()` procedure is called.
    *   A long press of the "Enter" button also seems to trigger an early `home()` and ends the attempts.
4.  **Homing (`home` function):** Systematically moves each motor (C, A, B, D) back to its defined home position using encoder feedback.

### Key Constants:

*   `COIN_INPUT_TIME = 10000`: Max time (ms) to input coins for one game session.
*   `TAKING_TOO_LONG = 60000`: Max time (ms) per attempt before auto-homing.
*   `MARGIN = 5`: Small encoder margin for homing, likely to ensure motors stop cleanly at the zero point.
*   Motor Encoder Limits in `xyMove` and `grab_procedure`: These define the operational boundaries of the claw machine's mechanics.

## Code Structure

The code is contained within a single `.c` file and includes:

*   Global constants for timing and margins.
*   Function prototypes.
*   **`coin_sense()`:** Handles coin detection and counting attempts.
*   **`joy()`:** Translates joystick button presses into integer codes.
*   **`xyMove()`:** Manages X and Y gantry movement based on joystick input and encoder limits.
*   **`grab_procedure()`:** Automates the claw lowering and gripping sequence.
*   **`home()`:** Automates the claw lifting, gantry return, and claw release sequence.
*   **`took_too_long()`:** Checks the gameplay timer for the current attempt.
*   **`task main()`:** The main program entry point, orchestrating the overall game flow and state transitions.

## Setup & Usage Notes 

*   **Motor Connections:**
    *   `motorA`: X-axis motor
    *   `motorB`: Y-axis motor
    *   `motorC`: Z-axis (claw up/down) motor
    *   `motorD`: Claw grip motor
*   **Sensor Connections:**
    *   `S1`: EV3 Touch Sensor for coin input.
*   **RobotC Environment:** This code is intended to be compiled and run using the RobotC IDE for LEGO Mindstorms.
*   **Encoder Resets:** Encoders are reset at the beginning of each full game cycle. Ensure the machine is in a known (preferably home) position if restarting mid-operation, or that the homing procedure can reliably find its way.
*   **Tuning:** The various `_limit` values in `xyMove` and `grab_procedure`, as well as `MARGIN`, `COIN_INPUT_TIME`, and `TAKING_TOO_LONG` may need to be tuned based on your specific claw machine's mechanical design and desired gameplay.

## Potential Future Improvements

*   Implement limit switches for more robust homing and end-stop detection.
*   Add a sensor to detect if a prize was successfully grabbed or dispensed.
*   More sophisticated claw pressure control
*   Different game modes or difficulty levels.
*   Enhanced sound effects or music.
