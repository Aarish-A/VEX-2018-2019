#include state_test.h
void foo(int a, int b, int c, int d)
{
	while (npgmtime < 10000)
	{
		sleep(10);
	}
	writeDebugStreamLine("time passed - exit foo");
}

PREP_STATE_VOID_4(void, foo, int, a, int, b, int, c, int, d);

task main()
{
	foo();
	sleep(10);
}
