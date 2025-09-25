#include <algorithm>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace homework_nfa {
std::vector<int> parce_string(std::string input_string, char delimiter = ' ');

class NFA {
    int n = 0;
    int m = 0;
    std::set<int> start_state_id;
    std::set<int> final_state_id;
    std::map<std::pair<int, int>, std::set<int>> transitions;

public:
    NFA() = default;
    void read_file(const std::string &file_path);
    [[nodiscard]] bool simulate(const std::string &input) const;
    [[nodiscard]] std::string toDFA() const;
    [[nodiscard]] std::string minimizeDFA() const;
    [[nodiscard]] bool equal(const NFA &nfa1, const NFA &nfa2);
    void writeDFAtoFile(const std::string &file_path) const;
};
}  // namespace homework_nfa