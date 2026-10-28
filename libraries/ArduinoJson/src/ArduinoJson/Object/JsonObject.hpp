// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Object/JsonObjectConst.hpp>
#include <ArduinoJson/Object/MemberProxy.hpp>

ARDUINOJSON_BEGIN_PUBLIC_NAMESPACE

class JsonArray;

class JsonObject : public detail::VariantOperators<JsonObject> {
  friend class detail::VariantAttorney;

 public:
  using iterator = JsonObjectIterator;

  JsonObject() : data_(0), resources_(0) {}

  JsonObject(detail::ObjectData* data, detail::ResourceManager* resource)
      : data_(data), resources_(resource) {}

  operator JsonVariant() const {
    void* data = data_;
    return JsonVariant(reinterpret_cast<detail::VariantData*>(data),
                       resources_);
  }

  operator JsonObjectConst() const {
    return JsonObjectConst(data_, resources_);
  }

  operator JsonVariantConst() const {
    return JsonVariantConst(collectionToVariant(data_), resources_);
  }

  bool isNull() const {
    return data_ == 0;
  }

  operator bool() const {
    return data_ != 0;
  }

  size_t nesting() const {
    return detail::VariantData::nesting(collectionToVariant(data_), resources_);
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

  void clear() const {
    detail::ObjectData::clear(data_, resources_);
  }

  bool set(JsonObjectConst src) {
    if (!data_ || !src.data_)
      return false;

    clear();
    for (auto kvp : src) {
      if (!operator[](kvp.key()).set(kvp.value()))
        return false;
    }

    return true;
  }

  template <typename TString,
            detail::enable_if_t<detail::IsString<TString>::value, int> = 0>
  detail::MemberProxy<JsonObject, detail::AdaptedString<TString>> operator[](
      const TString& key) const {
    return {*this, detail::adaptString(key)};
  }

  template <typename TChar,
            detail::enable_if_t<detail::IsString<TChar*>::value &&
                                    !detail::is_const<TChar>::value,
                                int> = 0>
  detail::MemberProxy<JsonObject, detail::AdaptedString<TChar*>> operator[](
      TChar* key) const {
    return {*this, detail::adaptString(key)};
  }

  template <typename TVariant,
            detail::enable_if_t<detail::IsVariant<TVariant>::value, int> = 0>
  detail::MemberProxy<JsonObject, detail::AdaptedString<JsonString>> operator[](
      const TVariant& key) const {
    return {*this, detail::adaptString(key.template as<JsonString>())};
  }

  FORCE_INLINE void remove(iterator it) const {
    detail::ObjectData::remove(data_, it.iterator_, resources_);
  }

  template <typename TString,
            detail::enable_if_t<detail::IsString<TString>::value, int> = 0>
  void remove(const TString& key) const {
    detail::ObjectData::removeMember(data_, detail::adaptString(key),
                                     resources_);
  }

  template <typename TVariant,
            detail::enable_if_t<detail::IsVariant<TVariant>::value, int> = 0>
  void remove(const TVariant& key) const {
    if (key.template is<const char*>())
      remove(key.template as<const char*>());
  }

  template <typename TChar>
  FORCE_INLINE void remove(TChar* key) const {
    detail::ObjectData::removeMember(data_, detail::adaptString(key),
                                     resources_);
  }

  template <typename TString,
            detail::enable_if_t<detail::IsString<TString>::value, int> = 0>
  ARDUINOJSON_DEPRECATED("use obj[key].is<T>() instead")
  bool containsKey(const TString& key) const {
    return detail::ObjectData::getMember(data_, detail::adaptString(key),
                                         resources_) != 0;
  }

  template <typename TChar,
            detail::enable_if_t<detail::IsString<TChar*>::value &&
                                    !detail::is_const<TChar>::value,
                                int> = 0>
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

  template <typename TChar>
  ARDUINOJSON_DEPRECATED("use obj[key].to<JsonArray>() instead")
  JsonArray createNestedArray(TChar* key) const {
    return operator[](key).template to<JsonArray>();
  }

  template <typename TString>
  ARDUINOJSON_DEPRECATED("use obj[key].to<JsonArray>() instead")
  JsonArray createNestedArray(const TString& key) const {
    return operator[](key).template to<JsonArray>();
  }

  template <typename TChar>
  ARDUINOJSON_DEPRECATED("use obj[key].to<JsonObject>() instead")
  JsonObject createNestedObject(TChar* key) {
    return operator[](key).template to<JsonObject>();
  }

  template <typename TString>
  ARDUINOJSON_DEPRECATED("use obj[key].to<JsonObject>() instead")
  JsonObject createNestedObject(const TString& key) {
    return operator[](key).template to<JsonObject>();
  }

  ARDUINOJSON_DEPRECATED("always returns zero")
  size_t memoryUsage() const {
    return 0;
  }

 private:
  detail::ResourceManager* getResourceManager() const {
    return resources_;
  }

  detail::VariantData* getData() const {
    return detail::collectionToVariant(data_);
  }

  detail::VariantData* getOrCreateData() const {
    return detail::collectionToVariant(data_);
  }

  detail::ObjectData* data_;
  detail::ResourceManager* resources_;
};

ARDUINOJSON_END_PUBLIC_NAMESPACE
