#include "Arduino.h"
#include "Input.h"

Input::Input()
{
}

InputEvent
Input::read()
{
  const int gravity = max(max(analogRead(7), analogRead(8)), analogRead(9));
  const int accel = abs(gravity - m_lastGravity);

  m_lastGravity = gravity;

  //if (accel > 0) {
      return InputEvent(InputEvent::Acceleration, accel);
  //}

  return InputEvent();
}
