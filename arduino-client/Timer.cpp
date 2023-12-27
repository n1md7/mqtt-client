#include <Time.h>
#include <TimeLib.h>
#include <Arduino.h>

#include "Timer.h"

#define TickInterval 1000  // 1 second

const unsigned int Timer::MAX_TIME = 60 * 60;  // 1 hour in seconds
const unsigned int Timer::MIN_TIME = 0;        // 0 second

Timer::Timer(unsigned long defaultValue) {
  initialTime = defaultValue;
  remainingTime = defaultValue;
  status = TimerStatus::OFF;
  lastTickedAt = Timer::MIN_TIME;
}

TimerStatus Timer::getStatus() {
  return status;
}

void Timer::start() {
  status = TimerStatus::ON;
  lastTickedAt = millis();
}

void Timer::stop() {
  status = TimerStatus::OFF;
}

void Timer::toggle() {
  if (isActive()) return stop();

  start();
}

void Timer::reset() {
  remainingTime = initialTime;
}

bool Timer::isActive() {
  return status == TimerStatus::ON;
}

bool Timer::isStopped() {
  return status == TimerStatus::OFF;
}

void Timer::handle() {
  if (status == TimerStatus::ON) {
    unsigned long currentTime = millis();

    unsigned long delta = currentTime - lastTickedAt;
    if (delta >= TickInterval) {
      remainingTime -= delta / TickInterval;
      lastTickedAt = currentTime;
    }

    if (remainingTime <= Timer::MIN_TIME) {
      return reset();
    }
  }
}

void Timer::set(unsigned long seconds) {
  if (seconds > Timer::MAX_TIME || seconds < Timer::MIN_TIME) {
    return;
  }

  remainingTime = seconds;
}

void Timer::set(int minutes, int seconds) {
  // Convert to seconds
  set((unsigned long)minutes * 60 + seconds);
}

unsigned long Timer::getValue() {
  return remainingTime;
}
