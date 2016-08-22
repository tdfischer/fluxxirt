#pragma once
#include "Display.h"
#include "Input.h"

class Animation {
public:
    virtual void handleEvent(const InputEvent& event) {}
    virtual void update() {}
    virtual void render(Display* dpy) = 0;
    virtual void stop() {}
    virtual void start() {}
};

class Animator {
public:
    Animator();
    void handleEvent(const InputEvent& event);
    void setAnimation(Animation* anim) {
        if (m_currentAnim) {
            m_currentAnim->stop();
        }
        m_currentAnim = anim;
        if (m_currentAnim) {
            m_currentAnim->start();
        }
    }

    void update() {
        if (m_currentAnim) {
            m_currentAnim->update();
        }
    }

    void render(Display* dpy) {
        if (m_currentAnim) {
            m_currentAnim->render(dpy);
        }
    }
private:
    Animation* m_currentAnim;
};
