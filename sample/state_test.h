#define ACCEL_TIME 75 //amnt of MS that must elapse before starting to check vel safety

/* Universal State Macros */
#define NOT_T_O(machineIn) ( (machineIn##Timeout <= 0)? 1 : (npgmTime < machineIn##Timeout) )

#define WHILE(machineIn, condition) machineIn##Blocked = true; \
	while( NOT_T_O(machineIn) && machineIn##VelSafetyCount < 10 && (condition) )

#define DO_WHILE(machineIn, condition) while( NOT_T_O(machineIn) && machineIn##VelSafetyCount < 10 && (condition) ); \
machineIn##Blocked = true

#define LOG(machineIn) if(machineIn##Logs) writeDebugStreamLine

#define VEL_CHECK_INC(machineIn, safetyType) machineIn##VelSafetyCheck(safetyType); \
machineIn##StateCycCount++

#define MACHINE_AWAIT(machineIn) while (machineIn##Blocked) sleep(10)

typedef enum _tVelDir
{
	velEither = -1,
	velUp = 0,
	velDown = 1
}tVelDir;
typedef enum _tVelType
{
	velSensor,
	velLocalY,
	velAngle
} tVelType;
/* /////////////// State Machine Macros (For X States) ////////////////// */
/* Create machine using:
	CREATE_MACHINE (-----)
	{
		---
	}
*/

/*	Macro for Machine w/ 3 States	*/
#define CREATE_MACHINE_3(machine, sensor, state0, state1, state2, type1, arg1Name, type2, arg2Name) \
const int machine##StateCount = 3; \
typedef enum _tStates##machine \
{ \
	machine##state0, \
	machine##state1, \
	machine##state2 \
}tStates##machine; \
\
tStates##machine machine##State = machine##state0; \
bool machine##Blocked = false; \
float machine##VelSafetyThresh = -1; \
tVelDir machine##VelSafetyDir = -1; \
unsigned long machine##Timeout; \
type1 machine##arg1Name; \
type2 machine##arg2Name; \
int machine##VelSafetyCount = 0; \
unsigned long machine##StateStartTime = 0; \
unsigned long machine##StateCycCount = 0; \
bool machine##Logs = 0; \
void machine##StateChange(int stateIn, bool await =  0, long timeout = -1, float velSafetyThresh = -1, tVelDir velDir = -1, type1 arg1In = -1, type2 arg2In = -1) \
{ \
	if (machine##State != stateIn) \
	{ \
		machine##State = stateIn; \
		machine##Blocked = await; \
		writeDebugStreamLine("Await?%d, machineBlocked?%d",  await, machine##Blocked); \
		unsigned long curTime = npgmtime; \
		if (timeout <= 0) \
		{ \
			machine##Timeout = 0; \
		} \
		else \
		{ \
			machine##Timeout = ( timeout + curTime ); \
		} \
		\
		machine##VelSafetyCount = 0; \
		machine##StateStartTime = curTime; \
		machine##StateCycCount = 0; \
		machine##VelSafetyThresh = velSafetyThresh; \
		machine##VelSafetyDir = velDir; \
		machine##arg1Name = arg1In; \
		machine##arg2Name = arg2In;  \
		writeDebugStreamLine ("%d" #machine "State:%d, TO:%d velS:%f, %d, %d", npgmTime, machine##State, machine##timeout, machine##VelSafetyThresh, machine##arg1Name, machine##arg2Name); \
		writeDebugStreamLine("Before await - Await?%d, machineBlocked?%d",  await, machine##Blocked); \
		if (await) MACHINE_AWAIT(machine); \
	} \
} \
\
void machine##VelSafetyCheck (tVelType velType = velSensor) \
{ \
	if (machine##VelSafetyThresh != -1 && machine##VelSafetyThresh != 0) \
	{ \
		if (machine##VelSafetyDir == velEither || machine##VelSafetyDir == velUp) \
			machine##VelSafetyThresh = abs(machine##VelSafetyThresh); \
		else if (machine##VelSafetyDir == velDown) \
			machine##VelSafetyThresh = -1 * abs(machine##VelSafetyThresh); \
		\
		tHog(); \
		float out = 0; \
		bool goodVel = false; \
		switch (velType) \
		{ \
			case velSensor: \
				velocityCheck(sensor); \
				out = gSensor[sensor].velocity; \
				goodVel = true; \
				break; \
			case velLocalY: \
				out = gVelocity.localY; \
				break; \
			case velAngle: \
				out = gVelocity.a; \
				goodVel = true; \
				break; \
		} \
		unsigned long curTime = npgmTime; \
		if (goodVel && curTime-machine##StateStartTime > ACCEL_TIME) \
		{ \
				if (machine##VelSafetyDir == velEither) \
				{ \
					if ( abs(out) < abs(machine##VelSafetyThresh) ) \
					{ \
						machine##VelSafetyCount ++; \
						if(machine##Logs) writeDebugStreamLine("%d:"#machine"velSafety trip(either)%f", npgmtime, out); \
					} \
				} \
				else \
				{ \
					if ( (sgn(machine##VelSafetyThresh) == 1)? (out < machine##VelSafetyThresh) :  (out > machine##VelSafetyThresh) ) \
					{ \
						machine##VelSafetyCount ++; \
						if(machine##Logs) writeDebugStreamLine("%d:"#machine"velSafety trip(dir)%f", npgmtime, out); \
					} \
				} \
		} \
	} \
} \
\
void machine##SafetyCheck(int timedOutState = machine##state0, type1 machine##arg1Name = -1, type2 machine##arg2Name = -1) \
{ \
		bool timedOut = false; \
		bool velSafety = false; \
		if (!NOT_T_O(machine)) \
			timedOut = true; \
		if (machine##VelSafetyCount >= 10) \
			velSafety = true; \
		\
		if (velSafety || timedOut) \
		{ \
			writeDebugStreamLine("%d" #machine "safety: Timedout? %d at %d VelSafety? %d", npgmTime, timedout, machine##Timeout, velSafety); \
			machine##StateChange(timedOutState, false, machine##arg1Name, machine##arg2Name); \
		} \
}

/* Test macros: */
/*
CREATE_MACHINE(thing, Idle, Hold, Manual, Move, MoveSimple, int, A, int, B)
{
	thingState = thingIdle;
	while (true)
	{
		sleep(10);
	}
}
*/

/* /////////////// Macros to Convert Functions to States ////////////////// */

//Add other funcs to a machine starting at __ val - call in header
#define ADD_FUNCS_TO_MACHINE_1(machine, func0) \
typedef enum _tFuncStates##machine \
{ \
	machine##func0 = machine##StateCount \
}tFuncStates##machine; \
const int func0##Loc = machine##StateCount

#define ADD_FUNCS_TO_MACHINE_5(machine, func1, func2, func3, func4, func5) \
typedef enum _tFuncStates##machine \
{ \
	machine##func1 = machine##StateCount, \
	machine##func2, \
	machine##func3, \
	machine##func4, \
	machine##func5 \
}tFuncStates##machine; \
const int func1##Loc = machine##StateCount + 0; \
const int func2##Loc = machine##StateCount + 1; \
const int func3##Loc = machine##StateCount + 2; \
const int func4##Loc = machine##StateCount + 3; \
const int func5##Loc = machine##StateCount + 4

//Create global variables for all args of a func - call in header
/* Macros for 4 Param Functions */
//Create global variables for all args of a func - TO BE CALLED IN HEADER
#define PREP_FUNC_STATE_4(funcType, func, type1, arg1, type2, arg2, type3, arg3, type4, arg4) \
funcType func (type1 arg1, type2 arg2, type3 arg3, type4 arg4); \
funcType func##Ret; \
const int func##ArgCount = 4; \
type1 func##Arg1 = -1; \
type2 func##Arg2 = -1; \
type3 func##Arg3 = -1; \
type4 func##Arg4 = -1

#define PREP_FUNC_STATE_VOID_4(funcType, func, type1, arg1, type2, arg2, type3, arg3, type4, arg4) \
funcType func (type1 arg1, type2 arg2, type3 arg3, type4 arg4); \
const int func##ArgCount = 4; \
type1 func##Arg1 = -1; \
type2 func##Arg2 = -1; \
type3 func##Arg3 = -1; \
type4 func##Arg4 = -1

//Assign to all func args - TO BE CALLED RIGHT BEFORE PUTTING MACHINE INTO STATE
#define ASSIGN_FUNC_STATE_4(func, arg1In, arg2In, arg3In, arg4In) \
func##Arg1 = arg1In; \
func##Arg2 = arg2In; \
func##Arg3 = arg3In; \
func##Arg4 = arg4In

//Add function to the machine - TO BE CALLED IN STATE MACHINE SWITCH
#define CALL_FUNC_STATE_4(func) func(func##Arg1, func##Arg2, func##Arg3, func##Arg4)

#define ADD_FUNC_TO_SWITCH_4(func, machine, nextState, safetyState) \
case (func##Loc): \
{ \
	int curState = machine##State; \
	machine##Blocked = true; \
	func##Ret = CALL_FUNC_STATE_4(func); \
	machine##SafetyCheck(safetyState); \
	if (machine##State == curState) \
		machine##StateChange(nextState, 0); \
	break; \
}

#define ADD_FUNC_TO_SWITCH_VOID_4(func, machine, nextState, safetyState) \
case (func##Loc): \
{ \
	int curState = machine##State; \
	machine##Blocked = true; \
	CALL_FUNC_STATE_4(func); \
	machine##SafetyCheck(safetyState); \
	if (machine##State == curState) \
		machine##StateChange(nextState); \
	break; \
}