const int COIN_INPUT_TIME = 10000;
const int TAKING_TOO_LONG = 60000;
int MARGIN = 5;

int coin_sense();
int joy();
void xyMove(int button);
void grab_procedure();
void home();
bool took_too_long(float current_time);


int coin_sense() //detects whether a coin was inserted into the slot
{
	int additions = 0;
	clearTimer(T1);

	while (time1[T1] < COIN_INPUT_TIME && joy() == -1)
	{
		while (SensorValue[S1] == 0)
		{
			displayTextLine(2, "ENTER COINS NOW");
			displayTextLine(3, "Attempts you will have: %i", additions+1);
			displayTextLine(4, "Time left to put coins: %d", (abs(time1[T1]-COIN_INPUT_TIME)/1000.0));
			if (time1[T1] >= COIN_INPUT_TIME || joy() != -1)
				break;
		}
		eraseDisplay();

		while (SensorValue[S1] == 1 && time1[T1] < COIN_INPUT_TIME)
		{
			displayTextLine(3, "Coin detected...");
			displayTextLine(4, "Time left to put coins: %d", (abs(time1[T1]-COIN_INPUT_TIME)/1000.0));
			if (time1[T1] >= COIN_INPUT_TIME)
				break;
		}
		playSound(soundUpwardTones);
		while (bSoundActive)
		{}
		additions++;
	}

	eraseDisplay();
	return additions;
}


int joy() //converts button presses into integer values
{
	if (getButtonPress(buttonAny))
	{
		if (getButtonPress(buttonUp))
			return 0;
		else if (getButtonPress(buttonRight))
			return 1;
		else if (getButtonPress(buttonDown))
			return 2;
		else if (getButtonPress(buttonLeft))
			return 3;
		else if (getButtonPress(buttonEnter))
			return 4;
	}
	else
		return -1;
}


void xyMove(int button) 
{
	int y_limit = -25000;
	int x_limit = 1700;

	if (button == 0 && nMotorEncoder(motorB) > y_limit)
		motor[motorB] = -100;
	else if (button == 2 && nMotorEncoder(motorB) < 0)
		motor[motorB] = 100;
	else if (button == 1 && nMotorEncoder(motorA) < x_limit)
		motor[motorA] = 30;
	else if (button == 3 && nMotorEncoder(motorA) > MARGIN)
		motor[motorA] = -30;
	else
		motor[motorA] = motor[motorB] = motor[motorC] = 0;
}


void grab_procedure()
{
	int z_limit = 2150;
	int grip_limit = 1000;

	while (nMotorEncoder(motorC) < z_limit)
	{
		motor[motorC] = 20;
	}
	motor[motorC] = 0;

	while (nMotorEncoder(motorD) < grip_limit)
	{
		motor[motorD] = 100;
	}
	motor[motorD] = 0;
}


void home() //homing procedure
{
	while (nMotorEncoder(motorC) > 0)
	{
		motor[motorC] = -20;
	}
	motor[motorC] = 0;

	while (nMotorEncoder(motorA) > MARGIN)
	{
		motor[motorA] = -50;
	}
	motor[motorA] = 0;

	while (nMotorEncoder(motorB) < 0)
	{
		motor[motorB] = 100;
	}
	motor[motorB] = 0;

	while (nMotorEncoder(motorD) > (2*MARGIN))
	{
		motor[motorD] = -30;
		displayTextLine(7, "motor D encoder  is %d", nMotorEncoder(motorD));
	}
	motor[motorD] = 0;
}


bool took_too_long(float current_time)
{
	if (current_time > TAKING_TOO_LONG)
		return true;

	return false;
}


task main()
{
	bool slowpoke = false;

	while (true)
	{
		nMotorEncoder(motorA) = 0;
		nMotorEncoder(motorB) = 0;
		nMotorEncoder(motorC) = 0;
		nMotorEncoder(motorD) = 0;
		SensorType[S1] = sensorEV3_Touch;

		int pressing = -1;
		int tries = 0;

		displayTextLine(2, "Enter a coin to start the game... ");
		while (SensorValue[S1] == 0)
		{}
		while (SensorValue[S1] == 1)
		{}
		eraseDisplay();
		tries += coin_sense();
		wait1Msec(100);
		int count = 0;
		clearTimer(T3);
		slowpoke = false;

		while (count < tries)
		{
			displayTextLine(3, "ATTEMPT #%i", count+1);
			pressing = joy();

			if (pressing != 4 && !slowpoke)
			{
				xyMove(pressing);
				if (took_too_long(time1[T3]))
					slowpoke = true;
					clearTimer(T3);
			}

			else if (!slowpoke)
			{
				clearTimer(T2);

				while (getButtonPress(buttonEnter))
				{}
				if (time1[T2] < 3000 && (nMotorEncoder[motorA] > 800 || nMotorEncoder[motorB] < -19000)) //makes sure it doesn't drop while above chute
				{
					grab_procedure();
					home();
					count++;
					clearTimer(T3);
				}
				else if(time1[T2] > 3000)
				{
					home();
					break;
				}
			}

			else
			{
				clearTimer(T3);
				home();
				count++;
				slowpoke = false;
			}
		}
	}
}
