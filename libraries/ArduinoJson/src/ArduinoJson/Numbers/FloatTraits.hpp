// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <ArduinoJson/Configuration.hpp>
#include <ArduinoJson/Polyfills/alias_cast.hpp>
#include <ArduinoJson/Polyfills/math.hpp>
#include <ArduinoJson/Polyfills/pgmspace_generic.hpp>
#include <ArduinoJson/Polyfills/preprocessor.hpp>
#include <ArduinoJson/Polyfills/type_traits.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

template <typename T, size_t = sizeof(T)>
struct FloatTraits {};

template <typename T>
struct FloatTraits<T, 8 > {
  using mantissa_type = uint64_t;
  static const short mantissa_bits = 52;
  static const mantissa_type mantissa_max =
      (mantissa_type(1) << mantissa_bits) - 1;

  using exponent_type = int16_t;
  static const exponent_type exponent_max = 308;

  static const size_t binaryPowersOfTen = 9;

  static pgm_ptr<T> positiveBinaryPowersOfTen() {
    ARDUINOJSON_DEFINE_PROGMEM_ARRAY(
        uint64_t, factors,
        {
            0x4024000000000000,
            0x4059000000000000,
            0x40C3880000000000,
            0x4197D78400000000,
            0x4341C37937E08000,
            0x4693B8B5B5056E17,
            0x4D384F03E93FF9F5,
            0x5A827748F9301D32,
            0x75154FDD7F73BF3C,
        });
    return pgm_ptr<T>(reinterpret_cast<const T*>(factors));
  }

  static pgm_ptr<T> negativeBinaryPowersOfTen() {
    ARDUINOJSON_DEFINE_PROGMEM_ARRAY(
        uint64_t, factors,
        {
            0x3FB999999999999A,
            0x3F847AE147AE147B,
            0x3F1A36E2EB1C432D,
            0x3E45798EE2308C3A,
            0x3C9CD2B297D889BC,
            0x3949F623D5A8A733,
            0x32A50FFD44F4A73D,
            0x255BBA08CF8C979D,
            0x0AC8062864AC6F43
        });
    return pgm_ptr<T>(reinterpret_cast<const T*>(factors));
  }

  static T nan() {
    return forge(0x7ff8000000000000);
  }

  static T inf() {
    return forge(0x7ff0000000000000);
  }

  static T highest() {
    return forge(0x7FEFFFFFFFFFFFFF);
  }

  template <typename TOut>
  static T highest_for(
      enable_if_t<is_integral<TOut>::value && is_signed<TOut>::value &&
                      sizeof(TOut) == 8,
                  signed>* = 0) {
    return forge(0x43DFFFFFFFFFFFFF);
  }

  template <typename TOut>
  static T highest_for(
      enable_if_t<is_integral<TOut>::value && is_unsigned<TOut>::value &&
                      sizeof(TOut) == 8,
                  unsigned>* = 0) {
    return forge(0x43EFFFFFFFFFFFFF);
  }

  static T lowest() {
    return forge(0xFFEFFFFFFFFFFFFF);
  }

  static T forge(uint64_t bits) {
    return alias_cast<T>(bits);
  }
};

template <typename T>
struct FloatTraits<T, 4 > {
  using mantissa_type = uint32_t;
  static const short mantissa_bits = 23;
  static const mantissa_type mantissa_max =
      (mantissa_type(1) << mantissa_bits) - 1;

  using exponent_type = int8_t;
  static const exponent_type exponent_max = 38;

  static const size_t binaryPowersOfTen = 6;

  static pgm_ptr<T> positiveBinaryPowersOfTen() {
    ARDUINOJSON_DEFINE_PROGMEM_ARRAY(uint32_t, factors,
                                     {
                                         0x41200000,
                                         0x42c80000,
                                         0x461c4000,
                                         0x4cbebc20,
                                         0x5a0e1bca,
                                         0x749dc5ae
                                     });
    return pgm_ptr<T>(reinterpret_cast<const T*>(factors));
  }

  static pgm_ptr<T> negativeBinaryPowersOfTen() {
    ARDUINOJSON_DEFINE_PROGMEM_ARRAY(uint32_t, factors,
                                     {
                                         0x3dcccccd,
                                         0x3c23d70a,
                                         0x38d1b717,
                                         0x322bcc77,
                                         0x24e69595,
                                         0x0a4fb11f
                                     });
    return pgm_ptr<T>(reinterpret_cast<const T*>(factors));
  }

  static T forge(uint32_t bits) {
    return alias_cast<T>(bits);
  }

  static T nan() {
    return forge(0x7fc00000);
  }

  static T inf() {
    return forge(0x7f800000);
  }

  static T highest() {
    return forge(0x7f7fffff);
  }

  template <typename TOut>
  static T highest_for(
      enable_if_t<is_integral<TOut>::value && is_signed<TOut>::value &&
                      sizeof(TOut) == 4,
                  signed>* = 0) {
    return forge(0x4EFFFFFF);
  }

  template <typename TOut>
  static T highest_for(
      enable_if_t<is_integral<TOut>::value && is_unsigned<TOut>::value &&
                      sizeof(TOut) == 4,
                  unsigned>* = 0) {
    return forge(0x4F7FFFFF);
  }

  template <typename TOut>
  static T highest_for(
      enable_if_t<is_integral<TOut>::value && is_signed<TOut>::value &&
                      sizeof(TOut) == 8,
                  signed>* = 0) {
    return forge(0x5EFFFFFF);
  }

  template <typename TOut>
  static T highest_for(
      enable_if_t<is_integral<TOut>::value && is_unsigned<TOut>::value &&
                      sizeof(TOut) == 8,
                  unsigned>* = 0) {
    return forge(0x5F7FFFFF);
  }

  static T lowest() {
    return forge(0xFf7fffff);
  }
};

template <typename TFloat, typename TExponent>
inline TFloat make_float(TFloat m, TExponent e) {
  using traits = FloatTraits<TFloat>;

  auto powersOfTen = e > 0 ? traits::positiveBinaryPowersOfTen()
                           : traits::negativeBinaryPowersOfTen();
  auto count = traits::binaryPowersOfTen;

  if (e <= 0)
    e = TExponent(-e);

  for (uint8_t index = 0; e != 0; index++) {
    if (index >= count)
      return traits::nan();
    if (e & 1)
      m *= powersOfTen[index];
    e >>= 1;
  }
  return m;
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
