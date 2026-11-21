// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2026, Benoit BLANCHON
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

#include <stdlib.h>
#include <string>

#include "Allocators.hpp"
#include "Literals.hpp"

TEST_CASE("JsonDocument::clear()") {
  SpyingAllocator spy;
  JsonDocument doc(&spy);

  SECTION("null") {
    doc.clear();

    REQUIRE(doc.isNull());
    REQUIRE(spy.log() == AllocatorLog{});
  }

  SECTION("releases resources") {
    doc["hello"_s] = "world"_s;
    spy.clearLog();

    doc.clear();

    REQUIRE(doc.isNull());
    REQUIRE(spy.log() == AllocatorLog{
                             Deallocate(sizeofPool()),
                             Deallocate(sizeofString("hello")),
                             Deallocate(sizeofString("world")),
                         });
  }

  SECTION("clear free list") {
    JsonObject obj = doc.to<JsonObject>();
    obj["a"] = 1;
    obj.clear();

    doc.clear();

    doc["b"] = 2;
  }
}
