// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Strings/JsonString.hpp>
#include <ArduinoJson/Variant/JsonVariant.hpp>
#include <ArduinoJson/Variant/JsonVariantConst.hpp>

ARDUINOJSON_BEGIN_PUBLIC_NAMESPACE

class JsonPair {
 public:
  JsonPair(detail::ObjectData::iterator iterator,
           detail::ResourceManager* resources) {
    if (!iterator.done()) {
      key_ = iterator->asString();
      iterator.next(resources);
      value_ = JsonVariant(iterator.data(), resources);
    }
  }

  JsonString key() const {
    return key_;
  }

  JsonVariant value() {
    return value_;
  }

 private:
  JsonString key_;
  JsonVariant value_;
};

class JsonPairConst {
 public:
  JsonPairConst(detail::ObjectData::iterator iterator,
                const detail::ResourceManager* resources) {
    if (!iterator.done()) {
      key_ = iterator->asString();
      iterator.next(resources);
      value_ = JsonVariantConst(iterator.data(), resources);
    }
  }

  JsonString key() const {
    return key_;
  }

  JsonVariantConst value() const {
    return value_;
  }

 private:
  JsonString key_;
  JsonVariantConst value_;
};

ARDUINOJSON_END_PUBLIC_NAMESPACE
