// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Namespace.hpp>
#include <ArduinoJson/Polyfills/utility.hpp>

#include <stdlib.h>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

template <typename TSource, typename Enable = void>
struct Reader {
 public:
  Reader(TSource& source) : source_(&source) {}

  int read() {
    return source_->read();
  }

  size_t readBytes(char* buffer, size_t length) {
    return source_->readBytes(buffer, length);
  }

 private:
  TSource* source_;
};

template <typename TSource, typename Enable = void>
struct BoundedReader {
};

ARDUINOJSON_END_PRIVATE_NAMESPACE

#include <ArduinoJson/Deserialization/Readers/IteratorReader.hpp>
#include <ArduinoJson/Deserialization/Readers/RamReader.hpp>
#include <ArduinoJson/Deserialization/Readers/VariantReader.hpp>

#if ARDUINOJSON_ENABLE_ARDUINO_STREAM
#  include <ArduinoJson/Deserialization/Readers/ArduinoStreamReader.hpp>
#endif

#if ARDUINOJSON_ENABLE_ARDUINO_STRING
#  include <ArduinoJson/Deserialization/Readers/ArduinoStringReader.hpp>
#endif

#if ARDUINOJSON_ENABLE_PROGMEM
#  include <ArduinoJson/Deserialization/Readers/FlashReader.hpp>
#endif

#if ARDUINOJSON_ENABLE_STD_STREAM
#  include <ArduinoJson/Deserialization/Readers/StdStreamReader.hpp>
#endif

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

template <typename TInput>
Reader<remove_reference_t<TInput>> makeReader(TInput&& input) {
  return Reader<remove_reference_t<TInput>>{detail::forward<TInput>(input)};
}

template <typename TChar>
BoundedReader<TChar*> makeReader(TChar* input, size_t inputSize) {
  return BoundedReader<TChar*>{input, inputSize};
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
