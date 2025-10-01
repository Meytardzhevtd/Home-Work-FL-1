#include "NFA.hpp"
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

[[nodiscard]] std::string NFA::minimizeDFA() const {
    std::string dfa_str = toDFA();
    if (dfa_str.empty()) {
        return "";
    }
    std::istringstream iss(dfa_str);
    std::string line;
    std::getline(iss, line);
    int n = std::stoi(line);
    std::getline(iss, line);
    int m = std::stoi(line);
    std::getline(iss, line);
    int start_state = std::stoi(line);
    std::getline(iss, line);
    std::set<int> final_states;
    if (!line.empty()) {
        for (int s : parce_string(line)) {
            final_states.insert(s);
        }
    }
    std::vector<std::vector<int>> trans(n, std::vector<int>(m, -1));
    while (std::getline(iss, line)) {
        if (line.empty()) {
            continue;
        }
        std::vector<int> parts = parce_string(line);
        if (parts.size() == 3) {
            int from = parts[0];
            int sym = parts[1];
            int to = parts[2];
            trans[from][sym] = to;
        }
    }
    std::vector<bool> reachable(n, false);
    std::queue<int> q;
    q.push(start_state);
    reachable[start_state] = true;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int c = 0; c < m; ++c) {
            int v = trans[u][c];
            if (v != -1 && !reachable[v]) {
                reachable[v] = true;
                q.push(v);
            }
        }
    }
    std::vector<int> old_to_new(n, -1);
    std::vector<int> new_to_old;
    for (int i = 0; i < n; ++i) {
        if (reachable[i]) {
            old_to_new[i] = static_cast<int>(new_to_old.size());
            new_to_old.push_back(i);
        }
    }
    int new_n = static_cast<int>(new_to_old.size());
    if (new_n == 0) {
        new_n = 1;
        new_to_old = {start_state};
        old_to_new[start_state] = 0;
    }
    int new_start = old_to_new[start_state];
    std::set<int> new_final;
    for (int s : final_states) {
        if (old_to_new[s] != -1) {
            new_final.insert(old_to_new[s]);
        }
    }
    std::vector<std::vector<int>> new_trans(new_n, std::vector<int>(m, -1));
    for (int i = 0; i < new_n; ++i) {
        int old_i = new_to_old[i];
        for (int c = 0; c < m; ++c) {
            int old_next = trans[old_i][c];
            if (old_next != -1 && old_to_new[old_next] != -1) {
                new_trans[i][c] = old_to_new[old_next];
            }
        }
    }
    std::vector<int> color(new_n);
    for (int i = 0; i < new_n; ++i) {
        color[i] = (new_final.count(i) ? 1 : 0);
    }

    bool changed = true;
    while (changed) {
        changed = false;
        std::map<std::vector<int>, std::vector<int>> groups;

        for (int i = 0; i < new_n; ++i) {
            std::vector<int> signature(m);
            for (int c = 0; c < m; ++c) {
                int next = new_trans[i][c];
                signature[c] = (next == -1 ? -1 : color[next]);
            }
            groups[signature].push_back(i);
        }

        std::map<int, int> new_color;
        int new_col = 0;
        for (const auto &[sig, states] : groups) {
            for (int s : states) {
                if (new_color.find(color[s]) == new_color.end() ||
                    new_color[color[s]] != new_col) {
                    if (color[s] != new_col) {
                        changed = true;
                    }
                }
                new_color[s] = new_col;
            }
            new_col++;
        }

        for (int i = 0; i < new_n; ++i) {
            color[i] = new_color[i];
        }
    }
    std::map<int, std::vector<int>> classes;
    for (int i = 0; i < new_n; ++i) {
        classes[color[i]].push_back(i);
    }
    std::vector<int> state_to_class(new_n);
    std::vector<int> class_rep;
    for (const auto &[cls, states] : classes) {
        int rep = *std::min_element(states.begin(), states.end());
        class_rep.push_back(rep);
        for (int s : states) {
            state_to_class[s] = static_cast<int>(class_rep.size() - 1);
        }
    }

    int min_n = static_cast<int>(classes.size());
    int min_start = state_to_class[new_start];
    std::set<int> min_final;
    for (int s : new_final) {
        min_final.insert(state_to_class[s]);
    }
    std::vector<std::tuple<int, int, int>> min_trans;
    for (int cls = 0; cls < min_n; ++cls) {
        int rep = class_rep[cls];
        for (int c = 0; c < m; ++c) {
            int next_old = new_trans[rep][c];
            if (next_old != -1) {
                int next_cls = state_to_class[next_old];
                min_trans.emplace_back(cls, c, next_cls);
            }
        }
    }
    std::string result = std::to_string(min_n) + '\n' + std::to_string(m) +
                         '\n' + std::to_string(min_start) + '\n';

    for (int f : min_final) {
        result += std::to_string(f) + ' ';
    }
    if (!min_final.empty()) {
        result.pop_back();
    }
    result += '\n';

    for (const auto &[from, sym, to] : min_trans) {
        result += std::to_string(from) + ' ' + std::to_string(sym) + ' ' +
                  std::to_string(to) + '\n';
    }
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}

[[nodiscard]] bool NFA::equal(const NFA &nfa1, const NFA &nfa2) {
    return nfa1.minimizeDFA() == nfa2.minimizeDFA();
}

[[nodiscard]] bool NFA::acceptsAllStrings(const NFA &nfa) const {
    /*
     * По идее нам просто надо минимизировать автомат и проверить, эквивалентен
     * ли он тривиальному (то есть имеет одно состояние и все переходы из этого
     * состояния в это же). Реши прокомментировать тк хз вроде бы и рабочий
     * метод а вроде бы и не уверен в нем
     */
    std::string min_dfa_str = nfa.minimizeDFA();
    if (min_dfa_str.empty()) {
        return false;
    }

    std::istringstream iss(min_dfa_str);
    std::string line;

    std::getline(iss, line);
    int n = std::stoi(line);

    std::getline(iss, line);
    int m = std::stoi(line);

    std::getline(iss, line);
    int start_state = std::stoi(line);

    std::getline(iss, line);
    std::set<int> final_states;
    if (!line.empty()) {
        for (int s : parce_string(line)) {
            final_states.insert(s);
        }
    }
    if (n != 1 || start_state != 0 || final_states.size() != 1 ||
        !final_states.contains(0)) {
        return false;
    }
    std::vector<std::vector<int>> trans(n, std::vector<int>(m, -1));
    while (std::getline(iss, line)) {
        if (line.empty()) {
            continue;
        }
        std::vector<int> parts = parce_string(line);
        if (parts.size() == 3) {
            int from = parts[0];
            int sym = parts[1];
            int to = parts[2];
            if (from != 0 || to != 0) {
                return false;
            }
            trans[from][sym] = to;
        }
    }
    for (int c = 0; c < m; ++c) {
        if (trans[0][c] != 0) {
            return false;
        }
    }

    return true;
}

}  // namespace homework_nfa
