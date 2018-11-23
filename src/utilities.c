float fmod(float x, float y)
{
	int q = floor(x / y);
	return x - (float)q * y;
}

float degToRad(float degrees)
{
	return degrees * PI / 180;
}

float radToDeg(float radians)
{
	return radians * 180 / PI;
}

float nearAngle(float angle, float reference)
{
	return round((reference - angle) / (2 * PI)) * (2 * PI) + angle;
}

float simplifyAngle(float angle)
{
	float baseAng = fmod(gPosition.a,PI*2);
	return (baseAng > PI)? (baseAng - PI*2) : baseAng;
}

void stopAllButCurrentTasks()
{
	// DO NOT MAKE A FOR LOOP, WILL NOT WORK
	tHog();
	STOP_TASK_NOT_CUR(1);
	STOP_TASK_NOT_CUR(2);
	STOP_TASK_NOT_CUR(3);
	STOP_TASK_NOT_CUR(4);
	STOP_TASK_NOT_CUR(5);
	STOP_TASK_NOT_CUR(6);
	STOP_TASK_NOT_CUR(7);
	STOP_TASK_NOT_CUR(8);
	STOP_TASK_NOT_CUR(9);
	STOP_TASK_NOT_CUR(10);
	STOP_TASK_NOT_CUR(11);
	STOP_TASK_NOT_CUR(12);
	STOP_TASK_NOT_CUR(13);
	STOP_TASK_NOT_CUR(14);
	STOP_TASK_NOT_CUR(15);
	STOP_TASK_NOT_CUR(16);
	STOP_TASK_NOT_CUR(17);
	STOP_TASK_NOT_CUR(18);
	STOP_TASK_NOT_CUR(19);
	STOP_TASK_NOT_CUR(20);
	tRelease();
}
