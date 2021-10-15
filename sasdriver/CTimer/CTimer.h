#ifndef TIMER_H
#define TIMER_H
namespace TIMER
{
 struct TalarmData
 {
  unsigned int flipControl;
  unsigned int alarmTime;
  bool alarm; 
  bool enabled;
 };

 int SetAlarm(TalarmData *alarmData, unsigned int currentTime, unsigned int alarmTime);
 bool Alarm(TalarmData *alarmData, unsigned int currentTime);
}
#endif