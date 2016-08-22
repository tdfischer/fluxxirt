#include "../Animation.h"
#include "../Display.h"

class TestAnimation: public Animation {
public:
    void handleEvent(const InputEvent& evt) override {
        if (evt.source() == InputEvent::Acceleration) {
            if (evt.value() > 1) {
                m_brightness += 15;
            }
            m_hue += scale8(evt.value(), 128);
        }
    }

    void update() override {
        m_brightness -= 3;
        m_brightness = max(0, min(255, m_brightness));
        m_x += 4;
        if (m_x % 12 == 0) {
          m_y += 28;
        }
        m_saturation++;
        m_saturation %= 155;
    }

    void render(Display* dpy) override {
        for(uint8_t col = 0; col < 3; col++) {
            for (uint8_t i = 0; i < 254; i+=10) {
                dpy->pixelAt(VirtualCoordinates{m_x + (col * (254 / 3)), i + m_y}) = CHSV(m_hue, m_saturation + 100, scale8(i, m_brightness));
            }
        }
    }

    void stop() override {}
    void start() override {}
private:
    int m_hue;
    int m_saturation;
    int m_brightness;
    uint8_t m_x;
    uint8_t m_y;
};
