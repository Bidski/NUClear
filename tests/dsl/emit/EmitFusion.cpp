/*
 * Copyright (C) 2013-2016 Trent Houliston <trent@houliston.me>, Jake Woods <jake.f.woods@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <catch.hpp>

#include "nuclear"

// Anonymous namespace to keep everything file local
namespace {

int v1				= 0;
int v2				= 0;
int v3				= 0;
int storedA			= 0;
std::string storedB = "";
double storedC		= 0;
double storedD		= 0;

template <typename T>
struct EmitTester1 {
	static inline void emit(NUClear::PowerPlant&, std::shared_ptr<T> p, int a, std::string b) {
		v1		= *p;
		storedA = a;
		storedB = b;
	}

	static inline void emit(NUClear::PowerPlant&, std::shared_ptr<T> p, double c) {
		v2		= *p;
		storedC = c;
	}
};

template <typename T>
struct EmitTester2 {
	static inline void emit(NUClear::PowerPlant&, std::shared_ptr<T> p, double d) {
		v3		= *p;
		storedD = d;
	}
};

class TestReactor : public NUClear::Reactor {
public:
	TestReactor(std::unique_ptr<NUClear::Environment> environment) : Reactor(std::move(environment)) {

		// Make some things to emit
		auto t1 = std::make_unique<int>(8);
		auto t2 = std::make_unique<int>(10);
		auto t3 = std::make_unique<int>(52);
		auto t4 = std::make_unique<int>(100);

		// Test using the second overload
		emit<EmitTester1>(t1, 7.2);
		REQUIRE(v1 == 0);
		REQUIRE(v2 == 8);
		v2 = 0;
		REQUIRE(v3 == 0);
		REQUIRE(storedA == 0);
		REQUIRE(storedB == "");
		REQUIRE(storedC == 7.2);
		storedC = 0;
		REQUIRE(storedD == 0);

		// Test using the first overload
		emit<EmitTester1>(t2, 1337, "This is text");
		REQUIRE(v1 == 10);
		v1 = 0;
		REQUIRE(v2 == 0);
		REQUIRE(v3 == 0);
		REQUIRE(storedA == 1337);
		storedA = 0;
		REQUIRE(storedB == "This is text");
		storedB = "";
		REQUIRE(storedC == 0);
		REQUIRE(storedD == 0);

		// Test multiple functions
		emit<EmitTester1, EmitTester2>(t3, 15, 8.3);
		REQUIRE(v1 == 0);
		REQUIRE(v2 == 52);
		v2 = 0;
		REQUIRE(v3 == 52);
		v3 = 0;
		REQUIRE(storedA == 0);
		REQUIRE(storedB == "");
		REQUIRE(storedC == 15);
		storedC = 0;
		REQUIRE(storedD == 8.3);
		storedD = 0;

		// Test even more multiple functions
		emit<EmitTester1, EmitTester2, EmitTester1>(t4, 2, "Hello World", 9.2, 5);
		REQUIRE(v1 == 100);
		REQUIRE(v2 == 100);
		REQUIRE(v3 == 100);
		REQUIRE(storedA == 2);
		REQUIRE(storedB == "Hello World");
		REQUIRE(storedC == 5);
		REQUIRE(storedD == 9.2);

		on<Startup>().then([this] { powerplant.shutdown(); });
	}
};
}

TEST_CASE("Testing emit function fusion", "[api][emit][fusion]") {
	NUClear::PowerPlant::Configuration config;
	config.threadCount = 1;
	NUClear::PowerPlant plant(config);
	plant.install<TestReactor>();

	plant.start();
}
