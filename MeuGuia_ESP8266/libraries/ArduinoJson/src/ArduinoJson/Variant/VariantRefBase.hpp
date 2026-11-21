// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Configuration.hpp>
#include <ArduinoJson/Variant/Converter.hpp>
#include <ArduinoJson/Variant/JsonVariantConst.hpp>
#include <ArduinoJson/Variant/VariantOperators.hpp>
#include <ArduinoJson/Variant/VariantTo.hpp>

ARDUINOJSON_BEGIN_PUBLIC_NAMESPACE
class JsonVariant;
ARDUINOJSON_END_PUBLIC_NAMESPACE

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE
template <typename>
class ElementProxy;

template <typename, typename>
class MemberProxy;

template <typename TDerived>
class VariantRefBase : public VariantTag {
  friend class VariantAttorney;

 public:
  void clear() const {
    VariantData::clear(getOrCreateData(), getResourceManager());
  }

  bool isNull() const {
    return VariantData::isNull(getData());
  }

  bool isUnbound() const {
    return !getData();
  }

  template <typename T>
  T as() const;

  template <typename T, enable_if_t<!is_same<T, TDerived>::value, int> = 0>
  operator T() const {
    return as<T>();
  }

  template <typename T, enable_if_t<is_same<T, JsonArray>::value, int> = 0>
  JsonArray to() const;

  template <typename T, enable_if_t<is_same<T, JsonObject>::value, int> = 0>
  JsonObject to() const;

  template <typename T, enable_if_t<is_same<T, JsonVariant>::value, int> = 0>
  JsonVariant to() const;

  template <typename T>
  FORCE_INLINE bool is() const;

  template <typename T>
  bool set(const T& value) const {
    using TypeForConverter = conditional_t<IsStringLiteral<T>::value, T,
                                           remove_cv_t<remove_reference_t<T>>>;
    return doSet<Converter<TypeForConverter>>(value);
  }

  template <typename T,
            detail::enable_if_t<!detail::is_const<T>::value, int> = 0>
  bool set(T* value) const {
    return doSet<Converter<T*>>(value);
  }

  size_t size() const {
    return VariantData::size(getData(), getResourceManager());
  }

  size_t nesting() const {
    return VariantData::nesting(getData(), getResourceManager());
  }

  template <typename T, enable_if_t<!is_same<T, JsonVariant>::value, int> = 0>
  T add() const {
    return add<JsonVariant>().template to<T>();
  }

  template <typename T, enable_if_t<is_same<T, JsonVariant>::value, int> = 0>
  T add() const;

  template <typename T>
  bool add(const T& value) const {
    return detail::VariantData::addValue(getOrCreateData(), value,
                                         getResourceManager());
  }

  template <typename T, enable_if_t<!is_const<T>::value, int> = 0>
  bool add(T* value) const {
    return detail::VariantData::addValue(getOrCreateData(), value,
                                         getResourceManager());
  }

  void remove(size_t index) const {
    VariantData::removeElement(getData(), index, getResourceManager());
  }

  template <typename TChar, enable_if_t<IsString<TChar*>::value, int> = 0>
  void remove(TChar* key) const {
    VariantData::removeMember(getData(), adaptString(key),
                              getResourceManager());
  }

  template <typename TString, enable_if_t<IsString<TString>::value, int> = 0>
  void remove(const TString& key) const {
    VariantData::removeMember(getData(), adaptString(key),
                              getResourceManager());
  }

  template <typename TVariant, enable_if_t<IsVariant<TVariant>::value, int> = 0>
  void remove(const TVariant& key) const {
    if (key.template is<size_t>())
      remove(key.template as<size_t>());
    else
      remove(key.template as<const char*>());
  }

  ElementProxy<TDerived> operator[](size_t index) const;

  template <typename TString, enable_if_t<IsString<TString>::value, int> = 0>
  ARDUINOJSON_DEPRECATED("use obj[key].is<T>() instead")
  bool containsKey(const TString& key) const;

  template <typename TChar, enable_if_t<IsString<TChar*>::value, int> = 0>
  ARDUINOJSON_DEPRECATED("use obj[\"key\"].is<T>() instead")
  bool containsKey(TChar* key) const;

  template <typename TVariant, enable_if_t<IsVariant<TVariant>::value, int> = 0>
  ARDUINOJSON_DEPRECATED("use obj[key].is<T>() instead")
  bool containsKey(const TVariant& key) const;

  template <typename TString, enable_if_t<IsString<TString>::value, int> = 0>
  FORCE_INLINE MemberProxy<TDerived, AdaptedString<TString>> operator[](
      const TString& key) const;

  template <
      typename TChar,
      enable_if_t<IsString<TChar*>::value && !is_const<TChar>::value, int> = 0>
  FORCE_INLINE MemberProxy<TDerived, AdaptedString<TChar*>> operator[](
      TChar* key) const;

  template <typename TVariant, enable_if_t<IsVariant<TVariant>::value, int> = 0>
  JsonVariantConst operator[](const TVariant& key) const {
    if (key.template is<size_t>())
      return operator[](key.template as<size_t>());
    else
      return operator[](key.template as<JsonString>());
  }

  ARDUINOJSON_DEPRECATED("use add<JsonVariant>() instead")
  JsonVariant add() const;

  ARDUINOJSON_DEPRECATED("use add<JsonArray>() instead")
  JsonArray createNestedArray() const;

  template <typename TChar>
  ARDUINOJSON_DEPRECATED("use var[key].to<JsonArray>() instead")
  JsonArray createNestedArray(TChar* key) const;

  template <typename TString>
  ARDUINOJSON_DEPRECATED("use var[key].to<JsonArray>() instead")
  JsonArray createNestedArray(const TString& key) const;

  ARDUINOJSON_DEPRECATED("use add<JsonObject>() instead")
  JsonObject createNestedObject() const;

  template <typename TChar>
  ARDUINOJSON_DEPRECATED("use var[key].to<JsonObject>() instead")
  JsonObject createNestedObject(TChar* key) const;

  template <typename TString>
  ARDUINOJSON_DEPRECATED("use var[key].to<JsonObject>() instead")
  JsonObject createNestedObject(const TString& key) const;

  ARDUINOJSON_DEPRECATED("always returns zero")
  size_t memoryUsage() const {
    return 0;
  }

  ARDUINOJSON_DEPRECATED("performs a deep copy")
  void shallowCopy(JsonVariantConst src) const {
    set(src);
  }

 private:
  TDerived& derived() {
    return static_cast<TDerived&>(*this);
  }

  const TDerived& derived() const {
    return static_cast<const TDerived&>(*this);
  }

  ResourceManager* getResourceManager() const {
    return VariantAttorney::getResourceManager(derived());
  }

  VariantData* getData() const {
    return VariantAttorney::getData(derived());
  }

  VariantData* getOrCreateData() const {
    return VariantAttorney::getOrCreateData(derived());
  }

  FORCE_INLINE ArduinoJson::JsonVariant getVariant() const;

  FORCE_INLINE ArduinoJson::JsonVariantConst getVariantConst() const {
    return ArduinoJson::JsonVariantConst(getData(), getResourceManager());
  }

  template <typename T>
  FORCE_INLINE enable_if_t<is_same<T, JsonVariantConst>::value, T> getVariant()
      const {
    return getVariantConst();
  }

  template <typename T>
  FORCE_INLINE enable_if_t<is_same<T, JsonVariant>::value, T> getVariant()
      const {
    return getVariant();
  }

  template <typename TConverter, typename T>
  bool doSet(const T& value) const {
    return doSet<TConverter>(
        value, is_same<typename function_traits<
                           decltype(&TConverter::toJson)>::return_type,
                       bool>{});
  }

  template <typename TConverter, typename T>
  bool doSet(const T& value, false_type) const;

  template <typename TConverter, typename T>
  bool doSet(const T& value, true_type) const;

  ArduinoJson::JsonVariant getOrCreateVariant() const;
};

ARDUINOJSON_END_PRIVATE_NAMESPACE
