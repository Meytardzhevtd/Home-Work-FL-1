#include "../inc/NFA.hpp"
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace homework_nfa {
std::vector<int> parce_string(std::string input_string, char delimiter) {
    for (char &current_char : input_string) {
        if (current_char == delimiter) {
            current_char = ' ';
        }
    }
    std::stringstream string_stream(input_string);
    std::vector<int> states_list;
    int current_state = 0;
    while (string_stream >> current_state) {
        states_list.push_back(current_state);
    }
    return states_list;
}

void NFA::read_file(const std::string &file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cout << "Error: File is not open" << '\n';
        return;
    }

    std::string input;

    std::getline(file, input);
    n = std::stoi(input);

    std::getline(file, input);
    m = std::stoi(input);

    std::getline(file, input);
    for (const int num : parce_string(input)) {
        start_state_id.insert(num);
    }

    std::getline(file, input);
    for (const int num : parce_string(input)) {
        final_state_id.insert(num);
    }

    while (std::getline(file, input)) {
        if (!input.empty()) {
            std::vector<int> transition = parce_string(input);
            const int from_state = transition[0];
            const int conditon = transition[1];
            const int available_state = transition[2];
            transitions[{from_state, conditon}].insert(available_state);
        }
    }

    file.close();
}

[[nodiscard]] bool NFA::simulate(const std::string &input) const {
    std::set<int> current_states = start_state_id;

    for (const char c : input) {
        const int number = c - '0';
        std::set<int> new_states;
        for (const int current_state : current_states) {
            if (transitions.contains({current_state, number})) {
                const auto &next_states =
                    transitions.at({current_state, number});
                for (const int state : next_states) {
                    new_states.insert(state);
                }
            }
        }
        current_states = new_states;
    }

    for (const int final_state : final_state_id) {
        if (current_states.contains(final_state)) {
            return true;
        }
    }
    return false;
}

[[nodiscard]] std::string NFA::toDFA() const {
    std::map<std::set<int>, int> state_to_id;
    state_to_id[start_state_id] = 0;
    int id = 1;
    std::queue<std::set<int>> q;
    std::vector<std::tuple<int, int, int>> dfa_transitions_list;
    std::set<int> dfa_final_states;
    q.push(start_state_id);
    while (!q.empty()) {
        auto current_state = q.front();
        for (const int state : current_state) {
            if (final_state_id.contains(state)) {
                dfa_final_states.insert(state_to_id[current_state]);
            }
        }
        q.pop();
        for (int i = 0; i < m; ++i) {
            std::set<int> T;
            for (const int state : current_state) {
                if (transitions.contains({state, i})) {
                    auto it = transitions.find({state, i});
                    if (it != transitions.end()) {
                        for (const int cur_state : it->second) {
                            T.insert(cur_state);
                        }
                    }
                }
            }
            if (T.empty()) {
                continue;
            }
            if (!state_to_id.contains(T)) {
                state_to_id[T] = id++;
                q.push(T);
            }
            dfa_transitions_list.emplace_back(
                std::make_tuple(state_to_id[current_state], i, state_to_id[T])
            );
        }
    }
    std::string result = std::to_string(state_to_id.size()) + '\n' +
                         std::to_string(m) + '\n' +
                         std::to_string(state_to_id[start_state_id]) + '\n';

    for (const int state : dfa_final_states) {
        result += std::to_string(state) + ' ';
    }
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    result += '\n';
    for (const auto &transition : dfa_transitions_list) {
        result += std::to_string(std::get<0>(transition)) + ' ' +
                  std::to_string(std::get<1>(transition)) + ' ' +
                  std::to_string(std::get<2>(transition)) + '\n';
    }
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

void NFA::writeDFAtoFile(const std::string &file_path) const {
    std::ofstream file(file_path);
    if (!file.is_open()) {
        std::cout << "Error: File is not open" << '\n';
        return;
    }
    file << toDFA();
    file.close();
}
}  // namespace homework_nfa
