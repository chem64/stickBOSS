//Run with at least Serial & Joystick USB modes
//Teensy LC
//Joystick X axis = A0
//Joystick Y axis = A1
//Joystick Z axis = A2
//Rotary Encoder = pins 11 and 12
//Joystick Button = pin0
//Button 1 = pin1
//Button 2 = pin2
//Button 3 = pin3
//Button 4 = pin4
//
//Joystick = 4-Axis Joystick Potentiometer Button Controller for JH-D400X-R4 10K 4D
//Teensy = Teensy LC USB Microcontroller Development Board ARM MKL26Z64 Without Pins
//Rotary Encoder = Electronic Handwheel MPG 6 Terminals Pulse Generator Encoder 100P/R 5V 60mm
//Buttons = 28MM One Piece Arcade Push Button (wired from ground thru switch to teensy pin)

typedef struct button
{
  int pin;
  int button;
} button_t;

void joystick_x(unsigned int value)
{
  Joystick.X(value);
}

void joystick_y(unsigned int value)
{
  Joystick.Y(value);
}

void joystick_z(unsigned int value)
{
  Joystick.Z(value);
}

void joystick_zrot(unsigned int value)
{
  Joystick.Zrotate(value);
}

typedef struct joystick
{
  int pin_x;
  int pin_y;
  int pin_z;
  void (*func_x)(unsigned int);
  void (*func_y)(unsigned int);
  void (*func_z)(unsigned int);
  void (*func_zrot)(unsigned int);
} joystick_t;

//Buttons
button_t buttons[] = {
  {0, 1}, //trigger
  {1, 2}, 
  {2, 3}, 
  {3, 4}, 
  {4, 5}, 
  {5, 6}, 
  {6, 7}, 
  {7, 8}, 
  {8, 9}, 
  {9, 10} 
};

int buttons_size = sizeof(buttons) / sizeof(button_t);

//Joysticks
joystick_t joysticks[] = {
  {A0, A1, A2, joystick_x, joystick_y, joystick_z, joystick_zrot} 
};

int joysticks_size = sizeof(joysticks) / sizeof(joystick_t);

#include "EncoderStepCounter.h"

#define ENCODER_PIN1 11
#define ENCODER_INT1 digitalPinToInterrupt(ENCODER_PIN1)
#define ENCODER_PIN2 12
#define ENCODER_INT2 digitalPinToInterrupt(ENCODER_PIN2)

EncoderStepCounter encoder(ENCODER_PIN1, ENCODER_PIN2);

int i;
int position = 512;
int posRes = 10;

void setup() 
{
  Serial.begin(9600);

  //Digital pins buttons
  for (i = 0; i < buttons_size; i++)
  {
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }
  encoder.begin();
  attachInterrupt(ENCODER_INT1, interrupt, CHANGE);
  attachInterrupt(ENCODER_INT2, interrupt, CHANGE);
}

// Call tick on every change interrupt
void interrupt() 
{
  encoder.tick();
}

void loop()
{
  //rotary encoder
  signed char pos = encoder.getPosition();
  if (pos != 0) 
  {
    if(pos>0) position += posRes;
    else position -= posRes;
    if(position < 0) position = 0;
    if(position > 1024) position = 1024;
    Serial.println(position);
    encoder.reset();
  }
  else position = 512;
  
  //Digital pins buttons
  for (i = 0; i < buttons_size; i++)
  {
    if (digitalRead(buttons[i].pin) == LOW)
    {
      Joystick.button(buttons[i].button, 1);
    }
    else
    {
      Joystick.button(buttons[i].button, 0);
    }
  }

  //assign joystick values
  for (i = 0; i < joysticks_size; i++)
  {
    joystick_t joy = joysticks[i];
    joy.func_x(1023-analogRead(joy.pin_x));
    joy.func_y(1023-analogRead(joy.pin_y));
    joy.func_z(analogRead(joy.pin_z));
    joy.func_zrot(position);
  }
  delay(5);
}
