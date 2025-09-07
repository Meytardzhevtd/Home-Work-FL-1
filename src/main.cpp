#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../inc/NFA.hpp"
#include "../inc/doctest.h"

TEST_CASE("Task 1 Test 1") {
    homework_nfa::NFA nfa;
    nfa.read_file("../tests/Test11.txt");
    CHECK(nfa.simulate("0") == false);
    CHECK(nfa.simulate("01") == false);
    CHECK(nfa.simulate("00") == true);
    CHECK(nfa.simulate("") == false);
}

TEST_CASE("Task 1 Test 2") {
    homework_nfa::NFA nfa;
    nfa.read_file("../tests/Test12.txt");
    CHECK(nfa.simulate("1") == true);
    CHECK(nfa.simulate("01") == true);
    CHECK(nfa.simulate("001") == true);
    CHECK(nfa.simulate("000") == false);
}

TEST_CASE("Task 1 Test 3") {
    homework_nfa::NFA nfa;
    nfa.read_file("../tests/Test13.txt");
    CHECK(nfa.simulate("") == true);
    CHECK(nfa.simulate("0") == true);
    CHECK(nfa.simulate("000") == true);
    CHECK(nfa.simulate("1") == false);
}

TEST_CASE("Task 1 Test 4") {
    homework_nfa::NFA nfa;
    nfa.read_file("../tests/Test14.txt");
    CHECK(nfa.simulate("0") == false);
    CHECK(nfa.simulate("00") == true);
    CHECK(nfa.simulate("1") == true);
    CHECK(nfa.simulate("01") == true);
}

TEST_CASE("Task 1 Test 5") {
    homework_nfa::NFA nfa;
    nfa.read_file("../tests/Test15.txt");
    CHECK(nfa.simulate("03") == true);
    CHECK(nfa.simulate("214") == false);
    CHECK(nfa.simulate("52") == false);
    CHECK(nfa.simulate("2314442303211111111") == true);
    CHECK(nfa.simulate("1440222222") == false);
    CHECK(nfa.simulate("2301022222") == false);
}

TEST_CASE("NFA to DFA equivalence") {
    homework_nfa::NFA nfa;
    nfa.read_file("../tests/Test15.txt");
    nfa.writeDFAtoFile("../tests/GeneratedDFA.txt");

    homework_nfa::NFA dfa;
    dfa.read_file("../tests/GeneratedDFA.txt");

    std::vector<std::string> test_cases = {
        "",           "0",          "1",          "2",          "0321111111",
        "1021111111", "2111111111", "1440211111", "2300321111", "52",
        "000",        "111",        "444"
    };

    for (const auto &s : test_cases) {
        CAPTURE(s);
        CHECK(nfa.simulate(s) == dfa.simulate(s));
    }
}