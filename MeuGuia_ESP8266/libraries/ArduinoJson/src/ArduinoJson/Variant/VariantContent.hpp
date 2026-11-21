// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#pragma once

#include <stddef.h>

#include <ArduinoJson/Array/ArrayData.hpp>
#include <ArduinoJson/Numbers/JsonFloat.hpp>
#include <ArduinoJson/Numbers/JsonInteger.hpp>
#include <ArduinoJson/Object/ObjectData.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

enum class VariantTypeBits : uint8_t {
  OwnedStringBit = 0x01,
  NumberBit = 0x08,
#if ARDUINOJSON_USE_EXTENSIONS
  ExtensionBit = 0x10,
#endif
  CollectionMask = 0x60,
};

enum class VariantType : uint8_t {
  Null = 0,
  TinyString = 0x02,
  RawString = 0x03,
  LinkedString = 0x04,
  OwnedString = 0x05,
  Boolean = 0x06,
  Uint32 = 0x0A,
  Int32 = 0x0C,
  Float = 0x0E,
#if ARDUINOJSON_USE_LONG_LONG
  Uint64 = 0x1A,
  Int64 = 0x1C,
#endif
#if ARDUINOJSON_USE_DOUBLE
  Double = 0x1E,
#endif
  Object = 0x20,
  Array = 0x40,
};

inline bool operator&(VariantType type, VariantTypeBits bit) {
  return (uint8_t(type) & uint8_t(bit)) != 0;
}

const size_t tinyStringMaxLength = 3;

union VariantContent {
  VariantContent() {}

  float asFloat;
  bool asBoolean;
  uint32_t asUint32;
  int32_t asInt32;
#if ARDUINOJSON_USE_EXTENSIONS
  SlotId asSlotId;
#endif
  ArrayData asArray;
  ObjectData asObject;
  CollectionData asCollection;
  const char* asLinkedString;
  struct StringNode* asOwnedString;
  char asTinyString[tinyStringMaxLength + 1];
};

#if ARDUINOJSON_USE_EXTENSIONS
union VariantExtension {
#  if ARDUINOJSON_USE_LONG_LONG
  uint64_t asUint64;
  int64_t asInt64;
#  endif
#  if ARDUINOJSON_USE_DOUBLE
  double asDouble;
#  endif
};
#endif

ARDUINOJSON_END_PRIVATE_NAMESPACE
