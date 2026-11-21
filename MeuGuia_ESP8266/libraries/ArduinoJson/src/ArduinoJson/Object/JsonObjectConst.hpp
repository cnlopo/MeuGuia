// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Object/JsonObjectIterator.hpp>
#include <ArduinoJson/Variant/VariantOperators.hpp>

ARDUINOJSON_BEGIN_PUBLIC_NAMESPACE

class JsonObjectConst : public detail::VariantOperators<JsonObjectConst> {
  friend class JsonObject;
  friend class detail::VariantAttorney;

 public:
  using iterator = JsonObjectConstIterator;

  JsonObjectConst() : data_(0), resources_(0) {}

  JsonObjectConst(const detail::ObjectData* data,
                  const detail::ResourceManager* resources)
      : data_(data), resources_(resources) {}

  operator JsonVariantConst() const {
    return JsonVariantConst(getData(), resources_);
  }

  bool isNull() const {
    return data_ == 0;
  }

  operator bool() const {
    return data_ != 0;
  }

  size_t nesting() const {
    return detail::VariantData::nesting(getData(), resources_);
  }

  size_t size() const {
    return data_ ? data_->size(resources_) : 0;
  }

  iterator begin() const {
    if (!data_)
      return iterator();
    return iterator(data_->createIterator(resources_), resources_);
  }

  iterator end() const {
    return iterator();
  }

  template <typename TString,
            detail::enable_if_t<detail::IsString<TString>::value, int> = 0>
  ARDUINOJSON_DEPRECATED("use obj[key].is<T>() instead")
  bool containsKey(const TString& key) const {
    return detail::ObjectData::getMember(data_, detail::adaptString(key),
                                         resources_) != 0;
  }

  template <typename TChar>
  ARDUINOJSON_DEPRECATED("use obj[\"key\"].is<T>() instead")
  bool containsKey(TChar* key) const {
    return detail::ObjectData::getMember(data_, detail::adaptString(key),
                                         resources_) != 0;
  }

  template <typename TVariant,
            detail::enable_if_t<detail::IsVariant<TVariant>::value, int> = 0>
  ARDUINOJSON_DEPRECATED("use obj[key].is<T>() instead")
  bool containsKey(const TVariant& key) const {
    return containsKey(key.template as<const char*>());
  }

  template <typename TString,
            detail::enable_if_t<detail::IsString<TString>::value, int> = 0>
  JsonVariantConst operator[](const TString& key) const {
    return JsonVariantConst(detail::ObjectData::getMember(
                                data_, detail::adaptString(key), resources_),
                            resources_);
  }

  template <typename TChar,
            detail::enable_if_t<detail::IsString<TChar*>::value &&
                                    !detail::is_const<TChar>::value,
                                int> = 0>
  JsonVariantConst operator[](TChar* key) const {
    return JsonVariantConst(detail::ObjectData::getMember(
                                data_, detail::adaptString(key), resources_),
                            resources_);
  }

  template <typename TVariant,
            detail::enable_if_t<detail::IsVariant<TVariant>::value, int> = 0>
  JsonVariantConst operator[](const TVariant& key) const {
    if (key.template is<JsonString>())
      return operator[](key.template as<JsonString>());
    else
      return JsonVariantConst();
  }

  ARDUINOJSON_DEPRECATED("always returns zero")
  size_t memoryUsage() const {
    return 0;
  }

 private:
  const detail::VariantData* getData() const {
    return collectionToVariant(data_);
  }

  const detail::ObjectData* data_;
  const detail::ResourceManager* resources_;
};

inline bool operator==(JsonObjectConst lhs, JsonObjectConst rhs) {
  if (!lhs && !rhs)
    return true;

  if (!lhs || !rhs)
    return false;

  size_t count = 0;
  for (auto kvp : lhs) {
    auto rhsValue = rhs[kvp.key()];
    if (rhsValue.isUnbound())
      return false;
    if (kvp.value() != rhsValue)
      return false;
    count++;
  }
  return count == rhs.size();
}

ARDUINOJSON_END_PUBLIC_NAMESPACE
