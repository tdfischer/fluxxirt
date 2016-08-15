#include <FastLED.h>

#define LED_NUM 300

CRGB leds[LED_NUM];

class PixelRow {
  public:
    PixelRow(unsigned int pixelCount)
      : m_size(pixelCount) {}

    unsigned int size() const {
      return m_size;
    }

  private:
    unsigned int m_size;
};

struct Coordinates {
  uint8_t x;
  uint8_t y;
};

struct InterpolatedCoordinates {
  Coordinates pos;
  fract8 scale;
};

template<int RowCount> class Display {
  public:
    Display(PixelRow rows[RowCount], CRGB* pixelBacking)
      : m_rows(rows), m_pixelBacking(pixelBacking) {}

    CRGB& nearestPixel(const Coordinates coords) {
      const Coordinates physicalCoords = virtualToPhysicalCoords(coords);
      int stripOffset;

      for (uint8_t i = 0; i < physicalCoords.y; i++) {
        stripOffset += m_rows[i].size();
      }
      return m_pixelBacking[stripOffset + physicalCoords.x];
    }

    enum NearestNeighbor {
      Up,
      Down,
      Left,
      Right
    };

    Coordinates getPhysicalNeighbor(const Coordinates pos, const NearestNeighbor dir) {
      switch(dir) {
        // FIXME: Needs to consider the row length of adjacent rows when moving +/- y axis
        case Up:
          return Coordinates{pos.x, pos.y - 1};
        case Down:
          return Coordinates{pos.x, pos.y + 1};
        case Left:
          return Coordinates{pos.x-1, pos.y};
        case Right:
          return Coordinates{pos.x+1, pos.y};
      }
    }

    Coordinates physicalToVirtualCoords(const Coordinates localCoords) {
      const uint8_t scaledX = ((float)localCoords.x / (float)m_rows[localCoords.y].size()) * 255.;
      const uint8_t scaledY = ((float)localCoords.y / (float)RowCount) * 255.;
      return Coordinates{scaledX, scaledY};
    }

    Coordinates virtualToPhysicalCoords(const Coordinates virtualCoords) {
      const uint8_t nearestRow = scale8(virtualCoords.y, RowCount);
      const PixelRow& row = m_rows[nearestRow];
      const uint8_t nearestColumn = scale8(virtualCoords.x, row.size());
      return Coordinates{nearestColumn, nearestRow};
    }

    /*InterpolatedCoordinates nearestPixel(const Coordinates virtualCoords, NearestNeighbor dir) {
      const Coordinates physicalCoords = virtualToPhysicalCoords(virtualCoords);
      const Coordinates physcalCoordsInVirtualSpace = physicalToVirtualCoords(physicalCoords);
      const Coordinates physicalNeighbor = getPhysicalNeighbor(coords, dir);
      const Coordinates physicalNeighborInVirtualSpace = physicalToVirtualCoords(physicalNeighbor);

      const uint8_t virtualXDistance = abs(physicalNeighborInVirtualSpace.x - physcalCoordsInVirtualSpace.x);
      const uint8_t virtualYDistance = abs(physicalNeighborInVirtualSpace.y - physcalCoordsInVirtualSpace.y);
      const fract8 xScale = virtualXDistance
    }*/

  private:
    CRGB* m_pixelBacking;
    PixelRow* m_rows;
};

PixelRow rows[] = {
  PixelRow(29),
  PixelRow(30),
  PixelRow(29),
  PixelRow(29)
};

constexpr uint8_t rowCount = sizeof(rows) / sizeof(PixelRow);

Display<rowCount> dpy(rows, leds);

void setup() {
  FastLED.addLeds<WS2812B, 13, GRB>(leds, LED_NUM);
  for (int i = 0; i < LED_NUM; i++) {
    leds[i] = CRGB(0, 0, 0);
  }
  FastLED.show();
}

uint8_t x = 0;
uint8_t hue = 0;
uint8_t saturation = 0;

void loop() {
  for(int i = 0; i < LED_NUM;i++) {
    leds[i] = CRGB(0, 0, 0);
  }
  for (int col = 0; col < 5; col++) {
    for (int i = 0; i < 255; i++) {
      dpy.nearestPixel(Coordinates{col * (255 / 5) + x, i}) = CHSV(hue, saturation + 100, 255);
      //dpy.nearestPixel(Coordinates{255 - (col * (255 / 5) + x), i}) = CHSV(hue, saturation + 100, 255);
    }
  }
  x++;
  hue++;
  saturation++;
  saturation %= 155;
  FastLED.show();
  delay(30);
}
