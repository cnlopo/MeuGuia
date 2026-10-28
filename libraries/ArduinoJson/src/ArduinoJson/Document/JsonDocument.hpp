// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Array/ElementProxy.hpp>
#include <ArduinoJson/Memory/Allocator.hpp>
#include <ArduinoJson/Memory/ResourceManager.hpp>
#include <ArduinoJson/Object/JsonObject.hpp>
#include <ArduinoJson/Object/MemberProxy.hpp>
#include <ArduinoJson/Polyfills/utility.hpp>
#include <ArduinoJson/Variant/JsonVariantConst.hpp>
#include <ArduinoJson/Variant/VariantTo.hpp>

ARDUINOJSON_BEGIN_PUBLIC_NAMESPACE

class JsonDocument : public detail::VariantOperators<const JsonDocument&> {
  friend class detail::VariantAttorney;

 public:
  explicit JsonDocument(Allocator* alloc = detail::DefaultAllocator::instance())
      : resources_(alloc) {}

  JsonDocument(const JsonDocument& src) : JsonDocument(src.allocator()) {
    set(src);
  }

  JsonDocument(JsonDocument&& src)
      : JsonDocument(detail::DefaultAllocator::instance()) {
    swap(*this, src);
  }

  template <typename T,
            detail::enable_if_t<detail::IsVariant<T>::value ||
                                    detail::is_same<T, JsonArray>::value ||
                                    detail::is_same<T, JsonArrayConst>::value ||
                                    detail::is_same<T, JsonObject>::value ||
                                    detail::is_same<T, JsonObjectConst>::value,
                                int> = 0>
  JsonDocument(const T& src,
               Allocator* alloc = detail::DefaultAllocator::instance())
      : JsonDocument(alloc) {
    set(src);
  }

  JsonDocument& operator=(JsonDocument src) {
    swap(*this, src);
    return *this;
  }

  template <typename T>
  JsonDocument& operator=(const T& src) {
    set(src);
    return *this;
  }

  Allocator* allocator() const {
    return resources_.allocator();
  }

  void shrinkToFit() {
    resources_.shrinkToFit();
  }

  template <typename T>
  T as() {
    return getVariant().template as<T>();
  }

  template <typename T>
  T as() const {
    return getVariant().template as<T>();
  }

  void clear() {
    resources_.clear();
    data_.reset();
  }

  template <typename T>
  bool is() {
    return getVariant().template is<T>();
  }

  template <typename T>
  bool is() const {
    return getVariant().template is<T>();
  }

  bool isNull() const {
    return getVariant().isNull();
  }

  bool overflowed() const {
    return resources_.overflowed();
  }

  size_t nesting() const {
    return data_.nesting(&resources_);
  }

  size_t size() const {
    return data_.size(&resources_);
  }

  bool set(const JsonDocument& src) {
    return to<JsonVariant>().set(src.as<JsonVariantConst>());
  }

  template <
      typename T,
      detail::enable_if_t<!detail::is_base_of<JsonDocument, T>::value, int> = 0>
  bool set(const T& src) {
    return to<JsonVariant>().set(src);
  }

  template <typename TChar,
            detail::enable_if_t<!detail::is_const<TChar>::value, int> = 0>
  bool set(TChar* src) {
    return to<JsonVariant>().set(src);
  }

  template <typename T>
  typename detail::VariantTo<T>::type to() {
    clear();
    return getVariant().template to<T>();
  }

  template <typename TChar>
  ARDUINOJSON_DEPRECATED("use doc[\"key\"].is<T>() instead")
  bool containsKey(TChar* key) const {
    return data_.getMember(detail::adaptString(key), &resources_) != 0;
  }

  template <typename TString,
            detail::enable_if_t<detail::IsString<TString>::value, int> = 0>
  ARDUINOJSON_DEPRECATED("use doc[key].is<T>() instead")
  bool containsKey(const TString& key) const {
    return data_.getMember(detail::adaptString(key), &resources_) != 0;
  }

  template <typename TVariant,
            detail::enable_if_t<detail::IsVariant<TVariant>::value, int> = 0>
  ARDUINOJSON_DEPRECATED("use doc[key].is<T>() instead")
  bool containsKey(const TVariant& key) const {
    return containsKey(key.template as<const char*>());
  }

  template <typename TString,
            detail::enable_if_t<detail::IsString<TString>::value, int> = 0>
  detail::MemberProxy<JsonDocument&, detail::AdaptedString<TString>> operator[](
      const TString& key) {
    return {*this, detail::adaptString(key)};
  }

  template <typename TChar,
            detail::enable_if_t<detail::IsString<TChar*>::value &&
                                    !detail::is_const<TChar>::value,
                                int> = 0>
  detail::MemberProxy<JsonDocument&, detail::AdaptedString<TChar*>> operator[](
      TChar* key) {
    return {*this, detail::adaptString(key)};
  }

  template <typename TString,
            detail::enable_if_t<detail::IsString<TString>::value, int> = 0>
  JsonVariantConst operator[](const TString& key) const {
    return JsonVariantConst(
        data_.getMember(detail::adaptString(key), &resources_), &resources_);
  }

  template <typename TChar,
            detail::enable_if_t<detail::IsString<TChar*>::value &&
                                    !detail::is_const<TChar>::value,
                                int> = 0>
  JsonVariantConst operator[](TChar* key) const {
    return JsonVariantConst(
        data_.getMember(detail::adaptString(key), &resources_), &resources_);
  }

  template <typename T,
            detail::enable_if_t<detail::is_integral<T>::value, int> = 0>
  detail::ElementProxy<JsonDocument&> operator[](T index) {
    return {*this, size_t(index)};
  }

  JsonVariantConst operator[](size_t index) const {
    return JsonVariantConst(data_.getElement(index, &resources_), &resources_);
  }

  template <typename TVariant,
            detail::enable_if_t<detail::IsVariant<TVariant>::value, int> = 0>
  JsonVariantConst operator[](const TVariant& key) const {
    if (key.template is<JsonString>())
      return operator[](key.template as<JsonString>());
    if (key.template is<size_t>())
      return operator[](key.template as<size_t>());
    return {};
  }

  template <typename T, detail::enable_if_t<
                            !detail::is_same<T, JsonVariant>::value, int> = 0>
  T add() {
    return add<JsonVariant>().to<T>();
  }

  template <typename T, detail::enable_if_t<
                            detail::is_same<T, JsonVariant>::value, int> = 0>
  JsonVariant add() {
    return JsonVariant(data_.addElement(&resources_), &resources_);
  }

  template <typename TValue>
  bool add(const TValue& value) {
    return data_.addValue(value, &resources_);
  }

  template <typename TChar,
            detail::enable_if_t<!detail::is_const<TChar>::value, int> = 0>
  bool add(TChar* value) {
    return data_.addValue(value, &resources_);
  }

  template <typename T,
            detail::enable_if_t<detail::is_integral<T>::value, int> = 0>
  void remove(T index) {
    detail::VariantData::removeElement(getData(), size_t(index),
                                       getResourceManager());
  }

  template <typename TChar,
            detail::enable_if_t<detail::IsString<TChar*>::value &&
                                    !detail::is_const<TChar>::value,
                                int> = 0>
  void remove(TChar* key) {
    detail::VariantData::removeMember(getData(), detail::adaptString(key),
                                      getResourceManager());
  }

  template <typename TString,
            detail::enable_if_t<detail::IsString<TString>::value, int> = 0>
  void remove(const TString& key) {
    detail::VariantData::removeMember(getData(), detail::adaptString(key),
                                      getResourceManager());
  }

  template <typename TVariant,
            detail::enable_if_t<detail::IsVariant<TVariant>::value, int> = 0>
  void remove(const TVariant& key) {
    if (key.template is<const char*>())
      remove(key.template as<const char*>());
    if (key.template is<size_t>())
      remove(key.template as<size_t>());
  }

  operator JsonVariant() {
    return getVariant();
  }

  operator JsonVariantConst() const {
    return getVariant();
  }

  friend void swap(JsonDocument& a, JsonDocument& b) {
    swap(a.resources_, b.resources_);
    swap_(a.data_, b.data_);
  }

  ARDUINOJSON_DEPRECATED("use add<JsonVariant>() instead")
  JsonVariant add() {
    return add<JsonVariant>();
  }

  ARDUINOJSON_DEPRECATED("use add<JsonArray>() instead")
  JsonArray createNestedArray() {
    return add<JsonArray>();
  }

  template <typename TChar>
  ARDUINOJSON_DEPRECATED("use doc[key].to<JsonArray>() instead")
  JsonArray createNestedArray(TChar* key) {
    return operator[](key).template to<JsonArray>();
  }

  template <typename TString>
  ARDUINOJSON_DEPRECATED("use doc[key].to<JsonArray>() instead")
  JsonArray createNestedArray(const TString& key) {
    return operator[](key).template to<JsonArray>();
  }

  ARDUINOJSON_DEPRECATED("use add<JsonObject>() instead")
  JsonObject createNestedObject() {
    return add<JsonObject>();
  }

  template <typename TChar>
  ARDUINOJSON_DEPRECATED("use doc[key].to<JsonObject>() instead")
  JsonObject createNestedObject(TChar* key) {
    return operator[](key).template to<JsonObject>();
  }

  template <typename TString>
  ARDUINOJSON_DEPRECATED("use doc[key].to<JsonObject>() instead")
  JsonObject createNestedObject(const TString& key) {
    return operator[](key).template to<JsonObject>();
  }

  ARDUINOJSON_DEPRECATED("always returns zero")
  size_t memoryUsage() const {
    return 0;
  }

 private:
  JsonVariant getVariant() {
    return JsonVariant(&data_, &resources_);
  }

  JsonVariantConst getVariant() const {
    return JsonVariantConst(&data_, &resources_);
  }

  detail::ResourceManager* getResourceManager() {
    return &resources_;
  }

  detail::VariantData* getData() {
    return &data_;
  }

  const detail::VariantData* getData() const {
    return &data_;
  }

  detail::VariantData* getOrCreateData() {
    return &data_;
  }

  detail::ResourceManager resources_;
  detail::VariantData data_;
};

inline void convertToJson(const JsonDocument& src, JsonVariant dst) {
  dst.set(src.as<JsonVariantConst>());
}

ARDUINOJSON_END_PUBLIC_NAMESPACE
