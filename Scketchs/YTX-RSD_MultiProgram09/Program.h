// Programs //////////////////////////////////////////////////////////////////////////////

struct Program {
  unsigned int pot[4];
  unsigned int potValue[4];
  bool potState[4];
  
  bool buttonState[4];
  bool buttonLastState[4];
  char buttonPushCounter[4];   // counter for the number of button presses

  bool paused = false;
  
  bool led[4];

  virtual void setup() {}
  virtual void draw() {}
  virtual void updateState() {}
  
  virtual void pause() { paused = true; }
  virtual void play() { paused = false;}
  virtual void reset() {}

  //MIDI
  
};
