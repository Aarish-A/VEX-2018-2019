/* Drive */
#define JOY_DRIVE_THROTTLE Ch3
#define JOY_DRIVE_TURN Ch4

/* Shooter Controls */
#define BTN_SHOOT Btn7D

/* Intake Controls */
#define BTN_INTAKE_UP Btn6U
#define BTN_INTAKE_DOWN Btn6D

/* Angler Controls */
#define JOY_ANGLER Ch2
//#define BTN_ANGLER_PICKUP
#define BTN_ANGLER_GROUND_PICKUP Btn7R
#define BTN_ANGLER_CAP_PICKUP Btn7U
//#define BTN_ANGLER_LOW_PF_PICKUP

/* Decapper Controls */
#define JOY_DECAPPER Ch1
#define BTN_DECAPPER_MOVE Btn7L

/* Macros */
	//Shot Trigger
#define BTN_SHOOT_FRONT_TOP Btn8U
#define BTN_SHOOT_FRONT_MID Btn8R
#define BTN_SHOOT_BACK_TOP Btn8L
#define BTN_SHOOT_BACK_MID Btn8D
	//Shot Queue
#define BTN_FLAG_LT Btn5U
#define BTN_FLAG_LM Btn5D
#define BTN_FLAG_RT Btn6U
#define BTN_FLAG_RM Btn6D

#define BTN_SHIFT Btn5U

void enableAllButtons()
{
	for (TVexJoysticks btn = Btn5D; btn <= Btn7R; btn++)
	{
		//writeDebugStreamLine("%d", (int)btn);
		enableJoystick(btn);
	}
}
