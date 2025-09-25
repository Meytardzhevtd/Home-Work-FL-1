#include <iostream>
#include <string>
#include <vector>
#include "VM.hpp"  // ← твой класс VM из HomeWork2

int main() {
    using namespace HomeWork2;

    struct TestCase {
        std::string regex;
        std::string input;
        bool expected;
    };

    std::vector<TestCase> tests = {
        // Простые символы
        {"a", "a", true},
        {"a", "b", false},
        {"a", "aa", false},  // только один 'a', не больше
        {"b", "b", true},
        {"b", "a", false},

        // Конкатенация
        {"ab", "ab", true},
        {"ab", "a", false},
        {"ab", "aba", false},
        {"ba", "ba", true},
        {"ba", "ab", false},

        // Альтернатива
        {"a|b", "a", true},
        {"a|b", "b", true},
        {"a|b", "c", false},
        {"a|b", "", false},

        // Звёздочка (ноль или более)
        {"a*", "", true},
        {"a*", "a", true},
        {"a*", "aa", true},
        {"a*", "aaa", true},
        {"a*", "aab", false},  // после 'aa' идёт 'b' — не соответствует a*

        // Плюс (один или более)
        {"a+", "a", true},
        {"a+", "aa", true},
        {"a+", "", false},
        {"a+", "b", false},

        // Вопросительный (ноль или один)
        {"a?", "", true},
        {"a?", "a", true},
        {"a?", "aa", false},  // больше одного — не подходит
        {"a?", "b", false},
    };

    std::cout << "=== Тестирование виртуальной машины для регулярных выражений "
                 "===\n\n";

    int passed = 0, total = 0;

    for (const auto &test : tests) {
        VM vm;  // Создаём VM — внутри компилирует test.regex
        bool result = vm.run(test.input);  // Запускаем НА СТРОКЕ test.input

        bool is_correct = (result == test.expected);
        if (is_correct) {
            passed++;
        }

        std::cout << "Регулярка: \"" << test.regex << "\"\n"
                  << "   Строка: \"" << test.input << "\"\n"
                  << "   Результат: " << (result ? "✅" : "❌")
                  << " (ожидалось: " << (test.expected ? "✅" : "❌") << ") — "
                  << (is_correct ? "ПРОЙДЕН" : "ПРОВАЛЕН") << "\n\n";

        total++;
    }

    std::cout << "=== ИТОГ: " << passed << " / " << total
              << " тестов пройдено ===\n";

    if (passed == total) {
        std::cout << "🎉 ПОЗДРАВЛЯЕМ! ВСЁ РАБОТАЕТ КОРРЕКТНО.\n";
    } else {
        std::cout << "🔧 Нужно доработать некоторые случаи.\n";
    }

    return 0;
}