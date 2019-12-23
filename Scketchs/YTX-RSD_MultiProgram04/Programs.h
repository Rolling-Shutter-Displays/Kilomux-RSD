// Programs //////////////////////////////////////////////////////////////////////////////

struct Program {
  unsigned int pot[4];
  unsigned int potValue[4];
  bool potState[4];
  
  bool buttonState[4];
  bool buttonLastState[4];
  char buttonPushCounter[4];   // counter for the number of button presses

  char buttonPausePushCounter;
  bool paused = false;
  
  bool led[4];
  
  virtual void draw() {}
  virtual void updateState() {}
  virtual void pause() {}
  virtual void play() {}
  virtual void reset() {}
};
