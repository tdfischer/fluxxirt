#include "Animation.h"

Animator::Animator()
  : m_currentAnim(nullptr)
{
}

void
Animator::handleEvent(const InputEvent& event)
{
    if (m_currentAnim) {
        m_currentAnim->handleEvent(event);
    }
}
