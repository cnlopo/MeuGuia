// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Namespace.hpp>

#include <stddef.h>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

template <size_t X, size_t Y, bool MaxIsX = (X > Y)>
struct Max {};

template <size_t X, size_t Y>
struct Max<X, Y, true> {
  static const size_t value = X;
};

template <size_t X, size_t Y>
struct Max<X, Y, false> {
  static const size_t value = Y;
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
