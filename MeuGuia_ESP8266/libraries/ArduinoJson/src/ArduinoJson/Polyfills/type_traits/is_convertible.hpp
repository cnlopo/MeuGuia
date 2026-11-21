// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#pragma once

#include "declval.hpp"

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4244)
#endif

#ifdef __ICCARM__
#pragma diag_suppress=Pa093
#endif

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

template <typename From, typename To>
struct is_convertible {
 protected:
  static int probe(To);
  static char probe(...);

  static const From& from_;

 public:
  static const bool value = sizeof(probe(from_)) == sizeof(int);
};

ARDUINOJSON_END_PRIVATE_NAMESPACE

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

#ifdef __ICCARM__
#pragma diag_default=Pa093
#endif
