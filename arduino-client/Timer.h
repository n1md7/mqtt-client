#ifndef Timer_h
#define Timer_h

#include "TimerStatus.h"

class Timer {
public:
  Timer(unsigned long defaultValue);

  void start();
  void stop();
  void reset();
  bool isActive();
  bool isStopped();
  void handle();
  TimerStatus getStatus();

  void toggle();

  void set(unsigned long seconds);
  void set(int minutes, int seconds);

  unsigned long getValue();

  static const unsigned int MAX_TIME;
  static const unsigned int MIN_TIME;

private:
  TimerStatus status;

  unsigned long initialTime;
  unsigned long remainingTime;
  unsigned long lastTickedAt;
};

#endif
