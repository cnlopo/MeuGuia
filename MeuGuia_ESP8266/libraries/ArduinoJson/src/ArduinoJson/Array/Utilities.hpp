// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Array/JsonArray.hpp>
#include <ArduinoJson/Document/JsonDocument.hpp>

ARDUINOJSON_BEGIN_PUBLIC_NAMESPACE

template <typename T, detail::enable_if_t<!detail::is_array<T>::value, int> = 0>
inline bool copyArray(const T& src, JsonVariant dst) {
  return dst.set(src);
}

template <typename T, size_t N, typename TDestination,
          detail::enable_if_t<
              !detail::is_base_of<JsonDocument, TDestination>::value, int> = 0>
inline bool copyArray(T (&src)[N], const TDestination& dst) {
  return copyArray(src, N, dst);
}

template <typename T, typename TDestination,
          detail::enable_if_t<
              !detail::is_base_of<JsonDocument, TDestination>::value, int> = 0>
inline bool copyArray(const T* src, size_t len, const TDestination& dst) {
  bool ok = true;
  for (size_t i = 0; i < len; i++) {
    ok &= copyArray(src[i], dst.template add<JsonVariant>());
  }
  return ok;
}

template <typename TDestination>
inline bool copyArray(const char* src, size_t, const TDestination& dst) {
  return dst.set(src);
}

template <typename T>
inline bool copyArray(const T& src, JsonDocument& dst) {
  return copyArray(src, dst.to<JsonArray>());
}

template <typename T>
inline bool copyArray(const T* src, size_t len, JsonDocument& dst) {
  return copyArray(src, len, dst.to<JsonArray>());
}

template <typename T, detail::enable_if_t<!detail::is_array<T>::value, int> = 0>
inline size_t copyArray(JsonVariantConst src, T& dst) {
  dst = src.as<T>();
  return 1;
}

template <typename T, size_t N>
inline size_t copyArray(JsonArrayConst src, T (&dst)[N]) {
  return copyArray(src, dst, N);
}

template <typename T>
inline size_t copyArray(JsonArrayConst src, T* dst, size_t len) {
  size_t i = 0;
  for (JsonArrayConst::iterator it = src.begin(); it != src.end() && i < len;
       ++it)
    copyArray(*it, dst[i++]);
  return i;
}

template <size_t N>
inline size_t copyArray(JsonVariantConst src, char (&dst)[N]) {
  JsonString s = src;
  size_t len = N - 1;
  if (len > s.size())
    len = s.size();
  memcpy(dst, s.c_str(), len);
  dst[len] = 0;
  return 1;
}

template <
    typename TSource, typename T,
    detail::enable_if_t<detail::is_array<T>::value &&
                            detail::is_base_of<JsonDocument, TSource>::value,
                        int> = 0>
inline size_t copyArray(const TSource& src, T& dst) {
  return copyArray(src.template as<JsonArrayConst>(), dst);
}

ARDUINOJSON_END_PUBLIC_NAMESPACE
