#include "../Animation.h"
#include "../Display.h"

class SafetyLights: public Animation {
public:
    void update() {
        m_frame += 1;
    }

    void render(Display* dpy) {
        const int brightness = sin8(m_frame) / 10 + 60;
        for(int i = 0; i < 254; i++) {
            dpy->pixelAt(VirtualCoordinates(i, 255)) = CHSV(255, 255, brightness);
        }
    }
private:
    uint8_t m_frame;
};
