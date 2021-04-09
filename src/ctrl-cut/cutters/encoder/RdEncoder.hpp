#ifndef HPGL_ENCODER_H_
#define HPGL_ENCODER_H_

#include <iostream>
#include <vector>
#include "cut/Cut.hpp"

class RdEncoder {

public:
  static void encode(std::ostream &out, Cut& model);
  
  enum Laser {
    LaserOne = '\x00',
    LaserTwo = '\x20',
  };

  enum Power {
    PowerMin = '\x01',
    PowerMax = '\x02',
  };

  enum BoundingCoords {
    BoundingMin = '\x03',
    BoundingMax = '\x07',
  };
  
  enum Devices {
    NoDevices = '\x00',
    AirBlower = '\x01',
  };

  inline static std::vector<char> encodeInt(int64_t integer, int64_t length) ;

  inline static void writeInstructionAbsolute(std::ostream &out, uint8_t instruction, int32_t x, int32_t y);
  inline static void writeInstructionRelative(std::ostream &out, uint8_t instruction, int16_t x, int16_t y);
  inline static void writeInstructionSingleRelative(std::ostream &out, uint8_t instruction, int16_t c);

  inline static void writeMoveAbsolute(std::ostream &out, int32_t x, int32_t y);
  inline static void writeMoveRelative(std::ostream &out, int16_t x, int16_t y);
  inline static void writeMoveRelativeX(std::ostream &out, int16_t x);
  inline static void writeMoveRelativeY(std::ostream &out, int16_t y);

  inline static void writeCutAbsolute(std::ostream &out, int32_t x, int32_t y);
  inline static void writeCutRelative(std::ostream &out, int16_t x, int16_t y);
  inline static void writeCutRelativeX(std::ostream &out, int16_t x);
  inline static void writeCutRelativeY(std::ostream &out, int16_t y);

  inline static void writeSetPower(std::ostream &out, uint8_t selectorByte, int16_t power);
  inline static void writeSetPowerLayer(std::ostream &out, uint8_t selectorByte, uint8_t layer, int16_t power);

  inline static void writeSetSpeed(std::ostream &out, int32_t speed);
  inline static void writeSetSpeedLayer(std::ostream &out, uint8_t layer, int32_t speed);

  inline static void writeDeviceFlags(std::ostream &out, uint8_t deviceFlags);
  
  inline static void writeSetLayer(std::ostream &out, uint8_t layer);
  inline static void writeSetMaximumLayer(std::ostream &out, uint8_t maximumLayer);
  inline static void writeSetLayerColor(std::ostream &out, uint8_t layer, uint8_t r, uint8_t g, uint8_t b);

  // used outside of translation unit therefore no explicit `inline`
  static void writeSetBoundingCoords(std::ostream &out, uint8_t selectorByte, int32_t x, int32_t y);
};

#endif /* HPGL_ENCODER_H_ */