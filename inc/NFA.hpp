#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace homework_nfa {
std::vector<int> parce_string(std::string input_string, char delimiter = ' ');

class NFA {
    int n = 0;
    int m = 0;
    std::set<int> start_state_id;
    std::set<int> final_state_id;

    // {from_state, condition} -> {all available states}
    std::map<std::pair<int, int>, std::set<int>> transitions;

public:
    NFA() = default;

    void read_file(const std::string &file_path);

    [[nodiscard]] bool simulate(const std::string &input) const;
};
}  // namespace homework_nfa