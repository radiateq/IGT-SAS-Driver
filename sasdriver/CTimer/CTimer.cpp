#include "CTimer.h"
/*Example
TIMER::SetAlarm(&SASClass->AFTlockAlarm, SASClass->GetTicks(), SASClass->AFTLockAndStatus.lockTimeout);
if (TIMER::Alarm(&this->AFTlockAlarm, this->GetTicks()))
*/


//Function accepts:
// 1. pointer to alarmData struct
// 2. current time in ticks
// 3. ticks to pass before alarm is reported
//Function returns 
// 0 on success
// -1 on failure
int TIMER::SetAlarm(TalarmData *alarmData, unsigned int currentTime, unsigned int alarmTime)
{
 (*alarmData).enabled     = true;
 (*alarmData).alarm       = false;
 (*alarmData).flipControl = currentTime;
 (*alarmData).alarmTime   = currentTime + alarmTime;
  return 1;
}

//Function accepts:
// 1. pointer to alarmData struct
// 2. current time in ticks 
//Function returns 
// false when alarm is not activated or timer disabled
// true when alarm is activated
bool TIMER::Alarm(TalarmData *alarmData, unsigned int currentTime)
{
 if (!(*alarmData).enabled)
  return false;
 if ((*alarmData).alarm)
  return true;
 if ((currentTime >= (*alarmData).alarmTime) && ((*alarmData).alarmTime >= (*alarmData).flipControl))
 {
  (*alarmData).alarm = true;
  return true;
 }  
 else if (currentTime < (*alarmData).flipControl)
  (*alarmData).flipControl = 0;
 return false;
}
