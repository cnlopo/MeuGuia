// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <ArduinoJson/Memory/ResourceManager.hpp>
#include <ArduinoJson/Polyfills/type_traits.hpp>
#include <ArduinoJson/Strings/IsString.hpp>
#include <ArduinoJson/Strings/StringAdapters.hpp>
#include <ArduinoJson/Variant/VariantAttorney.hpp>
#include <ArduinoJson/Variant/VariantOperators.hpp>
#include <ArduinoJson/Variant/VariantTag.hpp>

ARDUINOJSON_BEGIN_PUBLIC_NAMESPACE

class JsonArray;
class JsonObject;

class JsonVariantConst : public detail::VariantTag,
                         public detail::VariantOperators<JsonVariantConst> {
  friend class detail::VariantAttorney;

  template <typename T>
  using ConversionSupported =
      detail::is_same<typename detail::function_traits<
                          decltype(&Converter<T>::fromJson)>::arg1_type,
                      JsonVariantConst>;

 public:
  JsonVariantConst() : data_(nullptr), resources_(nullptr) {}

  explicit JsonVariantConst(const detail::VariantData* data,
                            const detail::ResourceManager* resources)
      : data_(data), resources_(resources) {}

  bool isNull() const {
    return detail::VariantData::isNull(data_);
  }

  bool isUnbound() const {
    return !data_;
  }

  size_t nesting() const {
    return detail::VariantData::nesting(data_, resources_);
  }

  size_t size() const {
    return detail::VariantData::size(data_, resources_);
  }

  template <typename T,
            detail::enable_if_t<ConversionSupported<T>::value, int> = 0>
  T as() const {
    return Converter<T>::fromJson(*this);
  }

  template <typename T,
            detail::enable_if_t<!ConversionSupported<T>::value, int> = 0>
  detail::InvalidConversion<JsonVariantConst, T> as() const;

  template <typename T,
            detail::enable_if_t<ConversionSupported<T>::value, int> = 0>
  bool is() const {
    return Converter<T>::checkJson(*this);
  }

  template <typename T,
            detail::enable_if_t<!ConversionSupported<T>::value, int> = 0>
  bool is() const {
    return false;
  }

  template <typename T>
  operator T() const {
    return as<T>();
  }

  template <typename T,
            detail::enable_if_t<detail::is_integral<T>::value, int> = 0>
  JsonVariantConst operator[](T index) const {
    return JsonVariantConst(
        detail::VariantData::getElement(data_, size_t(index), resources_),
        resources_);
  }

  template <typename TString,
            detail::enable_if_t<detail::IsString<TString>::value, int> = 0>
  JsonVariantConst operator[](const TString& key) const {
    return JsonVariantConst(detail::VariantData::getMember(
                                data_, detail::adaptString(key), resources_),
                            resources_);
  }

  template <typename TChar,
            detail::enable_if_t<detail::IsString<TChar*>::value &&
                                    !detail::is_const<TChar>::value,
                                int> = 0>
  JsonVariantConst operator[](TChar* key) const {
    return JsonVariantConst(detail::VariantData::getMember(
                                data_, detail::adaptString(key), resources_),
                            resources_);
  }

  template <typename TVariant,
            detail::enable_if_t<detail::IsVariant<TVariant>::value, int> = 0>
  JsonVariantConst operator[](const TVariant& key) const {
    if (key.template is<size_t>())
      return operator[](key.template as<size_t>());
    else
      return operator[](key.template as<JsonString>());
  }

  template <typename TString,
            detail::enable_if_t<detail::IsString<TString>::value, int> = 0>
  ARDUINOJSON_DEPRECATED("use var[key].is<T>() instead")
  bool containsKey(const TString& key) const {
    return detail::VariantData::getMember(getData(), detail::adaptString(key),
                                          resources_) != 0;
  }

  template <typename TChar,
            detail::enable_if_t<detail::IsString<TChar*>::value &&
                                    !detail::is_const<TChar>::value,
                                int> = 0>
  ARDUINOJSON_DEPRECATED("use obj[\"key\"].is<T>() instead")
  bool containsKey(TChar* key) const {
    return detail::VariantData::getMember(getData(), detail::adaptString(key),
                                          resources_) != 0;
  }

  template <typename TVariant,
            detail::enable_if_t<detail::IsVariant<TVariant>::value, int> = 0>
  ARDUINOJSON_DEPRECATED("use var[key].is<T>() instead")
  bool containsKey(const TVariant& key) const {
    return containsKey(key.template as<const char*>());
  }

  ARDUINOJSON_DEPRECATED("always returns zero")
  size_t memoryUsage() const {
    return 0;
  }

 protected:
  const detail::VariantData* getData() const {
    return data_;
  }

  const detail::ResourceManager* getResourceManager() const {
    return resources_;
  }

 private:
  const detail::VariantData* data_;
  const detail::ResourceManager* resources_;
};

ARDUINOJSON_END_PUBLIC_NAMESPACE
