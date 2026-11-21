// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Array/ElementProxy.hpp>
#include <ArduinoJson/Array/JsonArrayConst.hpp>

ARDUINOJSON_BEGIN_PUBLIC_NAMESPACE

class JsonObject;

class JsonArray : public detail::VariantOperators<JsonArray> {
  friend class detail::VariantAttorney;

 public:
  using iterator = JsonArrayIterator;

  JsonArray() : data_(0), resources_(0) {}

  JsonArray(detail::ArrayData* data, detail::ResourceManager* resources)
      : data_(data), resources_(resources) {}

  operator JsonVariant() {
    void* data = data_;
    return JsonVariant(reinterpret_cast<detail::VariantData*>(data),
                       resources_);
  }

  operator JsonArrayConst() const {
    return JsonArrayConst(data_, resources_);
  }

  template <typename T, detail::enable_if_t<
                            !detail::is_same<T, JsonVariant>::value, int> = 0>
  T add() const {
    return add<JsonVariant>().to<T>();
  }

  template <typename T, detail::enable_if_t<
                            detail::is_same<T, JsonVariant>::value, int> = 0>
  JsonVariant add() const {
    return JsonVariant(detail::ArrayData::addElement(data_, resources_),
                       resources_);
  }

  template <typename T>
  bool add(const T& value) const {
    return detail::ArrayData::addValue(data_, value, resources_);
  }

  template <typename T,
            detail::enable_if_t<!detail::is_const<T>::value, int> = 0>
  bool add(T* value) const {
    return detail::ArrayData::addValue(data_, value, resources_);
  }

  iterator begin() const {
    if (!data_)
      return iterator();
    return iterator(data_->createIterator(resources_), resources_);
  }

  iterator end() const {
    return iterator();
  }

  bool set(JsonArrayConst src) const {
    if (!data_)
      return false;

    clear();
    for (auto element : src) {
      if (!add(element))
        return false;
    }

    return true;
  }

  void remove(iterator it) const {
    detail::ArrayData::remove(data_, it.iterator_, resources_);
  }

  void remove(size_t index) const {
    detail::ArrayData::removeElement(data_, index, resources_);
  }

  template <typename TVariant,
            detail::enable_if_t<detail::IsVariant<TVariant>::value, int> = 0>
  void remove(const TVariant& variant) const {
    if (variant.template is<size_t>())
      remove(variant.template as<size_t>());
  }

  void clear() const {
    detail::ArrayData::clear(data_, resources_);
  }

  template <typename T,
            detail::enable_if_t<detail::is_integral<T>::value, int> = 0>
  detail::ElementProxy<JsonArray> operator[](T index) const {
    return {*this, size_t(index)};
  }

  template <typename TVariant,
            detail::enable_if_t<detail::IsVariant<TVariant>::value, int> = 0>
  detail::ElementProxy<JsonArray> operator[](const TVariant& variant) const {
    if (variant.template is<size_t>())
      return {*this, variant.template as<size_t>()};
    else
      return {*this, size_t(-1)};
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

  ARDUINOJSON_DEPRECATED("use add<JsonVariant>() instead")
  JsonVariant add() const {
    return add<JsonVariant>();
  }

  ARDUINOJSON_DEPRECATED("use add<JsonArray>() instead")
  JsonArray createNestedArray() const {
    return add<JsonArray>();
  }

  ARDUINOJSON_DEPRECATED("use add<JsonObject>() instead")
  JsonObject createNestedObject() const;

  ARDUINOJSON_DEPRECATED("always returns zero")
  size_t memoryUsage() const {
    return 0;
  }

 private:
  detail::ResourceManager* getResourceManager() const {
    return resources_;
  }

  detail::VariantData* getData() const {
    return collectionToVariant(data_);
  }

  detail::VariantData* getOrCreateData() const {
    return collectionToVariant(data_);
  }

  detail::ArrayData* data_;
  detail::ResourceManager* resources_;
};

ARDUINOJSON_END_PUBLIC_NAMESPACE
