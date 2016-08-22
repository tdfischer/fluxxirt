#pragma once

#include "Arduino.h"
#include <FastLED.h>

template<typename T> struct Coordinates {
  Coordinates(T _x, T _y) : x(_x), y(_y) {}
  T x;
  T y;
};

struct VirtualCoordinates: Coordinates<uint8_t> {
  VirtualCoordinates(uint8_t _x, uint8_t _y) : Coordinates(_x, _y) {}
};

struct PhysicalCoordinates: Coordinates<uint8_t> {
  PhysicalCoordinates(uint8_t _x, uint8_t _y) : Coordinates(_x, _y) {}
};

template<typename CoordType> struct Interpolated {
  CoordType pos;
  fract8 scale;
};

class PixelRow {
  public:
    PixelRow(double pixelCount)
      : m_size(pixelCount) {}

    double size() const {
      return m_size;
    }

  private:
    double m_size;
};


class Display {
  public:
  
    struct AliasedPixel {
      Interpolated<PhysicalCoordinates> upLeft;
      Interpolated<PhysicalCoordinates> upRight;
      Interpolated<PhysicalCoordinates> downLeft;
      Interpolated<PhysicalCoordinates> downRight;
      Display& dpy;

      AliasedPixel& operator=(const CRGB& rgb) {
#define SCALED(rgb, scale) CRGB(scale8(rgb.r,scale), scale8(rgb.g,scale), scale8(rgb.b,scale));
        dpy.pixelAt(upLeft.pos) = SCALED(rgb, upLeft.scale);
        dpy.pixelAt(upRight.pos) = SCALED(rgb, upRight.scale);
        dpy.pixelAt(downLeft.pos) = SCALED(rgb, downLeft.scale);
        dpy.pixelAt(downRight.pos) = SCALED(rgb, downRight.scale);
        return *this;
      }
    };

    Display(PixelRow* rows, int rowCount, CRGB* pixelBacking)
      : m_rows(rows), m_rowCount(rowCount), m_pixelBacking(pixelBacking) {}

    CRGB& pixelAt(const PhysicalCoordinates coords) {
      int stripOffset = 0;
      for(uint8_t i = 0; i < coords.y;i++) {
        stripOffset += m_rows[i].size();
      }
      return m_pixelBacking[(int)ceil(stripOffset) + (int)coords.x];
    }

    AliasedPixel pixelAt(const VirtualCoordinates coords) {
      return AliasedPixel {
        nearestPixel(coords, UpLeft),
        nearestPixel(coords, UpRight),
        nearestPixel(coords, DownLeft),
        nearestPixel(coords, DownRight),
        *this
      };
    }

    enum NearestNeighbor {
      UpLeft,
      UpRight,
      DownLeft,
      DownRight
    };

    PhysicalCoordinates getNeighbor(const PhysicalCoordinates pos, const NearestNeighbor dir) {
      PhysicalCoordinates ret(pos);
      switch(dir) {
        // FIXME: Needs to consider the row length of adjacent rows when moving +/- y axis
        case UpLeft:
          ret.y -= 0;
          ret.x -= 1;
          break;
        case UpRight:
          ret.y -= 1;
          ret.x += 1;
          break;
        case DownLeft:
          ret.y += 1;
          ret.x -= 1;
          break;
        case DownRight:
          ret.y += 0;
          ret.x += 1;
          break;
      }
      ret.y %= m_rowCount;
      ret.x %= (int)ceil(m_rows[(int)ceil(ret.y)].size());
      return ret;
    }

    VirtualCoordinates physicalToVirtualCoords(const PhysicalCoordinates localCoords) {
      const uint8_t scaledX = ((float)localCoords.x / (float)m_rows[localCoords.y].size()) * 255.;
      const uint8_t scaledY = ((float)localCoords.y / (float)m_rowCount) * 255.;
      return VirtualCoordinates{scaledX, scaledY};
    }

    PhysicalCoordinates virtualToPhysicalCoords(const VirtualCoordinates virtualCoords) {
      uint8_t nearestRow = scale8(virtualCoords.y, m_rowCount);
      const PixelRow& row = m_rows[nearestRow];
      const uint8_t nearestColumn = scale8(virtualCoords.x, row.size());
      return PhysicalCoordinates{nearestColumn, nearestRow};
    }

    Interpolated<PhysicalCoordinates> nearestPixel(const VirtualCoordinates virtualCoords, NearestNeighbor dir) {
      const PhysicalCoordinates pixelCoords = virtualToPhysicalCoords(virtualCoords);
      const VirtualCoordinates pixelInVirtualCoords = physicalToVirtualCoords(pixelCoords);
      const PhysicalCoordinates neighborPixel = getNeighbor(pixelCoords, dir);
      const VirtualCoordinates virtualNeighbor = physicalToVirtualCoords(neighborPixel);

      const int16_t virtualXDistance = virtualNeighbor.x - pixelInVirtualCoords.x;
      const int16_t virtualYDistance = virtualNeighbor.y - pixelInVirtualCoords.y;
      const double virtualDistance = sqrt((virtualXDistance * virtualXDistance) + (virtualYDistance * virtualYDistance));
      const fract8 scaledDistance = min(255, virtualDistance * 255.0);
      return Interpolated<PhysicalCoordinates> {
        neighborPixel,
        scaledDistance
      };
    }

  private:
    PixelRow* m_rows;
    int m_rowCount;
    CRGB* m_pixelBacking;
};

