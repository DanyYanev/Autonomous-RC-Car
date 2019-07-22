#define FR_SPEED 150
#define BK_SPEED 100
#define TURN_TIME 3000
//Distance to obsticle before which to initiate a turn
#define TURN_TRESHOLD 100
#define BREAK_TIME 100
#define SOUND_SPEED 0.034029

#define FR 3
#define BK 5
#define LF 6
#define RT 9

#define FR_ECHO 7
#define LF_ECHO 8
#define RT_ECHO 10

#define FR_TRIG 11
#define LF_TRIG 12
#define RT_TRIG 13

#define DEFAULT_ROUTINE &Drive

class CarAI;

typedef bool (CarAI::*RoutinePtr)();

class CarAI {

  bool IsGoingForward = false;
  bool IsTurning = false;
  bool ShouldStabilize = false;

  //Func ptr to curr and next routine.
  RoutinePtr CurrentRoutine = DEFAULT_ROUTINE;
  RoutinePtr NextRoutine;

  //All purpose variable for timers;
  unsigned RoutineTimer = 0;
  bool IsInRoutine = false;

public:

  CarAI(){};

  void SetupAI(){
    pinMode(FR_TRIG, OUTPUT);
    pinMode(LF_TRIG, OUTPUT);
    pinMode(RT_TRIG, OUTPUT);
    pinMode(FR, OUTPUT);
    pinMode(BK, OUTPUT);
    pinMode(LF, OUTPUT);
    pinMode(RT, OUTPUT);
    pinMode(FR_ECHO, INPUT);
    pinMode(LF_ECHO, INPUT);
    pinMode(RT_ECHO, INPUT);
  }

  void SetStabilize (bool Value){
    ShouldStabilize = Value;
  }

  void Run(){
    if((this->*CurrentRoutine)()){
      if(NextRoutine == nullptr){
        SetNextRoutine(DEFAULT_ROUTINE);
      }
      CurrentRoutine = NextRoutine;
      NextRoutine = nullptr;
      IsInRoutine = false;
    }
  }

protected:
  void SetCurrentRoutine(RoutinePtr p){
    CurrentRoutine = p;
  }

  void SetNextRoutine(RoutinePtr p){
    NextRoutine = p;
  }

  float CalcDist(int sens, int trig){
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(100);
    digitalWrite(trig, LOW);  
    int dur = pulseIn(sens, HIGH);
    float len = ((float)dur*SOUND_SPEED)/2;
    return len;
  }

  void Stop(){
    SetCurrentRoutine(&Break);
  }

private:
  
  bool Break(){
    if(!IsInRoutine){
      //First run
      analogWrite(BK, BK_SPEED);
      RoutineTimer = millis();
      IsInRoutine = true;
    }
    
    if(millis() - RoutineTimer > BREAK_TIME){
      //exit routine
      analogWrite(BK, 0);
      IsInRoutine = false;
      return 1;
    }
    return 0;
  }

  bool Drive(){
    if(!IsInRoutine){
      analogWrite(FR, FR_SPEED);
      IsInRoutine = true;
    }

    if(CalcDist(FR_ECHO, FR_TRIG) < TURN_TRESHOLD){
      if(CalcDist(LF_ECHO, LF_TRIG) > CalcDist(RT_ECHO, RT_TRIG)){
        SetNextRoutine(&TurnLeft);
      }else {
        SetNextRoutine(&TurnRight);
      }
      analogWrite(FR, 0);
      return 1;
    }

    return 0;
  }

  bool TurnLeft(){
    if(!IsInRoutine){
      analogWrite(FR, FR_SPEED);
      analogWrite(LF, 255);
      RoutineTimer = millis();
      IsInRoutine = true;
    }

    if(millis() - RoutineTimer > TURN_TIME){
      //exit routine
      analogWrite(FR, 0);
      analogWrite(LF, 0);
      //analogWrite(RT, 255);
      //analogWrite(RT, 0);
      IsInRoutine = false;
      return 1;
    }

    return 0;
  }

   bool TurnRight(){
    if(!IsInRoutine){
      analogWrite(FR, FR_SPEED);
      analogWrite(RT, 255);
      RoutineTimer = millis();
      IsInRoutine = true;
    }

    if(millis() - RoutineTimer > TURN_TIME){
      //exit routine
      analogWrite(FR, 0);
      analogWrite(RT, 0);
      //analogWrite(LF, 255);
      //analogWrite(LF, 0);
      IsInRoutine = false;
      return 1;
    }

    return 0;
  }
  
};
