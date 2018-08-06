#pragma config(Sensor, dgtl1,  solenoid,       sensorDigitalOut)
#pragma config(Motor,  port2,           lA,            tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           lB,            tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           lC,            tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           rA,            tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           rB,            tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           rC,            tmotorVex393_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define LIM_TO_VAL(val, lim) ( (abs(val) < lim)? val : lim * sgn(val) )
#define DRIVE_DZ 20
#define TURN_DZ 10
void setDrive(word l, word r)
{
	motor[lA] = motor[lB] = motor[lC] = LIM_TO_VAL(l, 127);
	motor[rA] = motor[rB] = motor[rC] = LIM_TO_VAL(r, 127);
}


task main()
{
	bool sol = 1;
	bool btn, lstBtn;
	word throttle, turn;
	while (true)
	{
		SensorValue[solenoid] = sol;
		btn = vexRT[Btn6U];

		throttle = vexRT[Ch3];
		if (abs(throttle) < DRIVE_DZ)
			throttle = 0;
		turn = vexRT[Ch4];
		if (abs(turn) < TURN_DZ)
			turn = 0;
		setDrive(throttle+turn, throttle-turn);

		if (btn && !lstBtn)
			sol = !sol;

		lstBtn = btn;
		sleep(10);

	}
}
