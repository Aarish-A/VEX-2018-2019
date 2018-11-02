/* Functions */
void initCycle(sCycleData& data, unsigned long period, const string name)
{
	data.period = period;
	data.startTime = nPgmTime;
	data.count = 0;
	data.time = 0;
	data.name = name;
}

void endCycle(sCycleData& data)
{
	++data.count; // Increment the cycle count

	// Make sure the cycle hasn't taken more than the max time and sleep so it takes exactly that time
	unsigned long now = nPgmTime;
	data.time = now - data.startTime;
	if (data.time <= data.period)
	{
		sleep(data.period - data.time);
	}
	else if (data.time > data.period)
	{
		//writeDebugStream(data.name);
		writeDebugStreamLine(" %d Cycle %s took %dms instead of %dms", nPgmTime, data.name, data.time, data.period);
	}
	data.startTime = nPgmTime;
}
