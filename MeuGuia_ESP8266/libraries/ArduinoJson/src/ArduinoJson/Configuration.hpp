// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#pragma once

#ifndef ARDUINOJSON_ENABLE_STD_STREAM
#  ifdef __has_include
#    if __has_include(<istream>) && \
    __has_include(<ostream>) && \
    !defined(min) && \
    !defined(max)
#      define ARDUINOJSON_ENABLE_STD_STREAM 1
#    else
#      define ARDUINOJSON_ENABLE_STD_STREAM 0
#    endif
#  else
#    ifdef ARDUINO
#      define ARDUINOJSON_ENABLE_STD_STREAM 0
#    else
#      define ARDUINOJSON_ENABLE_STD_STREAM 1
#    endif
#  endif
#endif

#ifndef ARDUINOJSON_ENABLE_STD_STRING
#  ifdef __has_include
#    if __has_include(<string>) && !defined(min) && !defined(max)
#      define ARDUINOJSON_ENABLE_STD_STRING 1
#    else
#      define ARDUINOJSON_ENABLE_STD_STRING 0
#    endif
#  else
#    ifdef ARDUINO
#      define ARDUINOJSON_ENABLE_STD_STRING 0
#    else
#      define ARDUINOJSON_ENABLE_STD_STRING 1
#    endif
#  endif
#endif

#ifndef ARDUINOJSON_ENABLE_STRING_VIEW
#  ifdef __has_include
#    if __has_include(<string_view>) && __cplusplus >= 201703L
#      define ARDUINOJSON_ENABLE_STRING_VIEW 1
#    else
#      define ARDUINOJSON_ENABLE_STRING_VIEW 0
#    endif
#  else
#    define ARDUINOJSON_ENABLE_STRING_VIEW 0
#  endif
#endif

#ifndef ARDUINOJSON_SIZEOF_POINTER
#  if defined(__SIZEOF_POINTER__)
#    define ARDUINOJSON_SIZEOF_POINTER __SIZEOF_POINTER__
#  elif defined(_WIN64) && _WIN64
#    define ARDUINOJSON_SIZEOF_POINTER 8
#  else
#    define ARDUINOJSON_SIZEOF_POINTER 4
#  endif
#endif

#ifndef ARDUINOJSON_USE_DOUBLE
#  if ARDUINOJSON_SIZEOF_POINTER >= 4
#    define ARDUINOJSON_USE_DOUBLE 1
#  else
#    define ARDUINOJSON_USE_DOUBLE 0
#  endif
#endif

#ifndef ARDUINOJSON_USE_LONG_LONG
#  if ARDUINOJSON_SIZEOF_POINTER >= 4
#    define ARDUINOJSON_USE_LONG_LONG 1
#  else
#    define ARDUINOJSON_USE_LONG_LONG 0
#  endif
#endif

#ifndef ARDUINOJSON_DEFAULT_NESTING_LIMIT
#  define ARDUINOJSON_DEFAULT_NESTING_LIMIT 10
#endif

#ifndef ARDUINOJSON_SLOT_ID_SIZE
#  if ARDUINOJSON_SIZEOF_POINTER <= 2
#    define ARDUINOJSON_SLOT_ID_SIZE 1
#  elif ARDUINOJSON_SIZEOF_POINTER == 4
#    define ARDUINOJSON_SLOT_ID_SIZE 2
#  else
#    define ARDUINOJSON_SLOT_ID_SIZE 4
#  endif
#endif

#ifndef ARDUINOJSON_POOL_CAPACITY
#  if ARDUINOJSON_SLOT_ID_SIZE == 1
#    define ARDUINOJSON_POOL_CAPACITY 16
#  elif ARDUINOJSON_SLOT_ID_SIZE == 2
#    define ARDUINOJSON_POOL_CAPACITY 128
#  else
#    define ARDUINOJSON_POOL_CAPACITY 256
#  endif
#endif

#ifndef ARDUINOJSON_INITIAL_POOL_COUNT
#  define ARDUINOJSON_INITIAL_POOL_COUNT 4
#endif

#ifndef ARDUINOJSON_AUTO_SHRINK
#  if ARDUINOJSON_SIZEOF_POINTER <= 2
#    define ARDUINOJSON_AUTO_SHRINK 0
#  else
#    define ARDUINOJSON_AUTO_SHRINK 1
#  endif
#endif

#ifndef ARDUINOJSON_STRING_LENGTH_SIZE
#  if ARDUINOJSON_SIZEOF_POINTER <= 2
#    define ARDUINOJSON_STRING_LENGTH_SIZE 1
#  else
#    define ARDUINOJSON_STRING_LENGTH_SIZE 2
#  endif
#endif

#ifdef ARDUINO

#  ifndef ARDUINOJSON_ENABLE_ARDUINO_STRING
#    define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
#  endif

#  ifndef ARDUINOJSON_ENABLE_ARDUINO_STREAM
#    define ARDUINOJSON_ENABLE_ARDUINO_STREAM 1
#  endif

#  ifndef ARDUINOJSON_ENABLE_ARDUINO_PRINT
#    define ARDUINOJSON_ENABLE_ARDUINO_PRINT 1
#  endif

#  ifndef ARDUINOJSON_ENABLE_PROGMEM
#    define ARDUINOJSON_ENABLE_PROGMEM 1
#  endif

#else

#  ifndef ARDUINOJSON_ENABLE_ARDUINO_STRING
#    define ARDUINOJSON_ENABLE_ARDUINO_STRING 0
#  endif

#  ifndef ARDUINOJSON_ENABLE_ARDUINO_STREAM
#    define ARDUINOJSON_ENABLE_ARDUINO_STREAM 0
#  endif

#  ifndef ARDUINOJSON_ENABLE_ARDUINO_PRINT
#    define ARDUINOJSON_ENABLE_ARDUINO_PRINT 0
#  endif

#  ifndef ARDUINOJSON_ENABLE_PROGMEM
#    ifdef __AVR__
#      define ARDUINOJSON_ENABLE_PROGMEM 1
#    else
#      define ARDUINOJSON_ENABLE_PROGMEM 0
#    endif
#  endif

#endif

#ifndef ARDUINOJSON_DECODE_UNICODE
#  define ARDUINOJSON_DECODE_UNICODE 1
#endif

#ifndef ARDUINOJSON_ENABLE_COMMENTS
#  define ARDUINOJSON_ENABLE_COMMENTS 0
#endif

#ifndef ARDUINOJSON_ENABLE_NAN
#  define ARDUINOJSON_ENABLE_NAN 0
#endif

#ifndef ARDUINOJSON_ENABLE_INFINITY
#  define ARDUINOJSON_ENABLE_INFINITY 0
#endif

#ifndef ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD
#  define ARDUINOJSON_POSITIVE_EXPONENTIATION_THRESHOLD 1e7
#endif

#ifndef ARDUINOJSON_NEGATIVE_EXPONENTIATION_THRESHOLD
#  define ARDUINOJSON_NEGATIVE_EXPONENTIATION_THRESHOLD 1e-5
#endif

#ifndef ARDUINOJSON_LITTLE_ENDIAN
#  if defined(_MSC_VER) ||                           \
      (defined(__BYTE_ORDER__) &&                    \
       __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || \
      defined(__LITTLE_ENDIAN__) || defined(__i386) || defined(__x86_64)
#    define ARDUINOJSON_LITTLE_ENDIAN 1
#  else
#    define ARDUINOJSON_LITTLE_ENDIAN 0
#  endif
#endif

#ifndef ARDUINOJSON_ENABLE_ALIGNMENT
#  if defined(__AVR)
#    define ARDUINOJSON_ENABLE_ALIGNMENT 0
#  else
#    define ARDUINOJSON_ENABLE_ALIGNMENT 1
#  endif
#endif

#ifndef ARDUINOJSON_TAB
#  define ARDUINOJSON_TAB "  "
#endif

#ifndef ARDUINOJSON_STRING_BUFFER_SIZE
#  define ARDUINOJSON_STRING_BUFFER_SIZE 32
#endif

#ifndef ARDUINOJSON_DEBUG
#  ifdef __PLATFORMIO_BUILD_DEBUG__
#    define ARDUINOJSON_DEBUG 1
#  else
#    define ARDUINOJSON_DEBUG 0
#  endif
#endif

#if ARDUINOJSON_USE_LONG_LONG || ARDUINOJSON_USE_DOUBLE
#  define ARDUINOJSON_USE_EXTENSIONS 1
#else
#  define ARDUINOJSON_USE_EXTENSIONS 0
#endif

#if defined(nullptr)
#  error nullptr is defined as a macro. Remove the faulty #define or #undef nullptr
#endif
