// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Array/JsonArrayIterator.hpp>
#include <ArduinoJson/Variant/VariantAttorney.hpp>
#include <ArduinoJson/Variant/VariantData.hpp>

ARDUINOJSON_BEGIN_PUBLIC_NAMESPACE

class JsonObject;

class JsonArrayConst : public detail::VariantOperators<JsonArrayConst> {
  friend class JsonArray;
  friend class detail::VariantAttorney;

 public:
  using iterator = JsonArrayConstIterator;

  iterator begin() const {
    if (!data_)
      return iterator();
    return iterator(data_->createIterator(resources_), resources_);
  }

  iterator end() const {
    return iterator();
  }

  JsonArrayConst() : data_(0), resources_(0) {}

  JsonArrayConst(const detail::ArrayData* data,
                 const detail::ResourceManager* resources)
      : data_(data), resources_(resources) {}

  template <typename T,
            detail::enable_if_t<detail::is_integral<T>::value, int> = 0>
  JsonVariantConst operator[](T index) const {
    return JsonVariantConst(
        detail::ArrayData::getElement(data_, size_t(index), resources_),
        resources_);
  }

  template <typename TVariant,
            detail::enable_if_t<detail::IsVariant<TVariant>::value, int> = 0>
  JsonVariantConst operator[](const TVariant& variant) const {
    if (variant.template is<size_t>())
      return operator[](variant.template as<size_t>());
    else
      return JsonVariantConst();
  }

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

  ARDUINOJSON_DEPRECATED("always returns zero")
  size_t memoryUsage() const {
    return 0;
  }

 private:
  const detail::VariantData* getData() const {
    return collectionToVariant(data_);
  }

  const detail::ArrayData* data_;
  const detail::ResourceManager* resources_;
};

inline bool operator==(JsonArrayConst lhs, JsonArrayConst rhs) {
  if (!lhs && !rhs)
    return true;
  if (!lhs || !rhs)
    return false;

  auto a = lhs.begin();
  auto b = rhs.begin();

  for (;;) {
    if (a == b)
      return true;
    if (a == lhs.end() || b == rhs.end())
      return false;
    if (*a != *b)
      return false;
    ++a;
    ++b;
  }
}

ARDUINOJSON_END_PUBLIC_NAMESPACE
