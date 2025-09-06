#include "../inc/NFA.hpp"

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
}

[[nodiscard]] bool NFA::simulate(const std::string &input) const {
    {
        for (auto p : transitions) {
            std::cout << "{" << p.first.first << ", " << p.first.second << "} -> {";
            for (int num : p.second) {
                std::cout << num << ", ";
            }
            std::cout << "}\n";
        }
    }

    std::set<int> current_states = start_state_id;

    {
        std::cout << "Start states: ";
        for (int i : current_states) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }
    for (const char c : input) {
        const int number = c - '0';
        std::set<int> new_states;

        for (const int current_state : current_states) {
            if (transitions.find({current_state, number}) !=
                transitions.end()) {
                const auto &next_states =
                    transitions.at({current_state, number});
                for (const int state : next_states) {
                    new_states.insert(state);
                }
            }
        }
        current_states = new_states;
        {
            std::cout << "Current states: ";
            for (int i : current_states) {
                std::cout << i << " ";
            }
            std::cout << std::endl;
        }
    }
    for (const int final_state : final_state_id) {
        if (current_states.count(final_state) != 0) {
            return true;
        }
    }
    return false;
}
}  // namespace homework_nfa
