// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#pragma once

#ifdef _MSC_VER

#  define FORCE_INLINE

#  ifndef ARDUINOJSON_DEPRECATED
#    define ARDUINOJSON_DEPRECATED(msg) __declspec(deprecated(msg))
#  endif

#elif defined(__GNUC__)

#  define FORCE_INLINE __attribute__((always_inline))

#  ifndef ARDUINOJSON_DEPRECATED
#    if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
#      define ARDUINOJSON_DEPRECATED(msg) __attribute__((deprecated(msg)))
#    else
#      define ARDUINOJSON_DEPRECATED(msg) __attribute__((deprecated))
#    endif
#  endif

#else

#  define FORCE_INLINE

#  ifndef ARDUINOJSON_DEPRECATED
#    define ARDUINOJSON_DEPRECATED(msg)
#  endif

#endif

#if defined(__has_attribute)
#  if __has_attribute(no_sanitize)
#    define ARDUINOJSON_NO_SANITIZE(check) __attribute__((no_sanitize(check)))
#  else
#    define ARDUINOJSON_NO_SANITIZE(check)
#  endif
#else
#  define ARDUINOJSON_NO_SANITIZE(check)
#endif
