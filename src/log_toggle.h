/*Drive*/
#if defined(DRIVE_LOGS)
	#define LOG_DRIVE(line) writeDebugStreamLine line; 
#else
	#define LOG_DRIVE(line){ }
#endif

#if defined(DRIVE_STATE_LOGS)
	#define LOG_DRIVE_STATE(line) writeDebugStreamLine line; 
#else
	#define LOG_DRIVE_STATE(line){ }
#endif

/*Intake*/
#if defined(INTAKE_LOGS)
	#define LOG_INTAKE(line) writeDebugStreamLine line; 
#else
	#define LOG_INTAKE(line){ }
#endif

#if defined(INTAKE_STATE_LOGS)
	#define LOG_INTAKE_STATE(line) writeDebugStreamLine line; 
#else
	#define LOG_INTAKE_STATE(line){ }
#endif

/*Decapper*/
#if defined(DECAPPER_LOGS)
	#define LOG_DECAPPER(line) writeDebugStreamLine line; 
#else
	#define LOG_DECAPPER(line){ }
#endif

#if defined(DECAPPER_STATE_LOGS)
	#define LOG_DECAPPER_STATE(line) writeDebugStreamLine line; 
#else
	#define LOG_DECAPPER_STATE(line){ }
#endif

/*Angler*/
#if defined(ANGLER_LOGS)
	#define LOG_ANGLER(line) writeDebugStreamLine line; 
#else
	#define LOG_ANGLER(line){ }
#endif

#if defined(ANGLER_STATE_LOGS)
	#define LOG_ANGLER_STATE(line) writeDebugStreamLine line; 
#else
	#define LOG_ANGLER_STATE(line){ }
#endif

/*Shooter*/
#if defined(SHOOTER_LOGS)
	#define LOG_SHOOTER(line) writeDebugStreamLine line; 
#else
	#define LOG_SHOOTER(line){ }
#endif

/*Macro*/
#if defined(MACRO_LOGS)
	#define LOG_MACRO(line) writeDebugStreamLine line; 
#else
	#define LOG_MACRO(line){ }
#endif

/*Auto*/
#if defined(AUTO_LOGS)
	#define LOG_AUTO(line) writeDebugStreamLine line; 
#else
	#define LOG_AUTO(line){ }
#endif
