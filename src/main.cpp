#include <bits/stdc++.h>
#include "VM.hpp"

using namespace std;
using namespace HomeWork2;

int main() {
    VM vm;
    vector<pair<string, vector<string>>> suites = {
        {"a", {"a", "b", "", "aa"}},
        {"b", {"b", "a", "bb", ""}},
        {"ab", {"ab", "a", "b", "abb", "aab"}},
        {"a*", {"", "a", "aa", "aaa", "b", "ba", "ab"}},
        {"b*", {"", "b", "bb", "bbb", "a", "ab", "ba"}},
        {"a+", {"a", "aa", "aaa", "", "b", "ba", "ab"}},
        {"b+", {"b", "bb", "bbb", "", "a", "ab", "ba"}},
        {"a?", {"", "a", "aa", "b"}},
        {"b?", {"", "b", "bb", "a"}},
        {"a+b+", {"ab", "aab", "abb", "aaabb", "a", "b", "aaaabbbb"}},
        {"a*b*", {"", "a", "aa", "aaa", "b", "bb", "abb", "aab", "aaabbb"}},
        {"a?b", {"b", "ab", "a", "bb", ""}},
        {"a|b", {"a", "b", "ab", "ba", ""}}
    };
    for (auto &s : suites) {
        cout << "=== Regex: " << s.first << " ===\n";
        for (auto &str : s.second) {
            cout << "  \"" << str << "\" -> "
                 << (vm.run(s.first, str) ? "MATCH" : "NO MATCH") << "\n";
        }
        cout << "\n";
    }
}
