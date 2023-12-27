#ifndef Component_h
#define Component_h

class Component {
public:
  Component(int pin);
  Component(int pin, int offSignal, int onSignal);

  void begin();
  void on();
  void off();

  bool isOn();
  bool isOff();

  void toggle();

  void setState(bool isOn);

private:
  int pin;
  int state;
  int onSignal;
  int offSignal;
};

#endif
