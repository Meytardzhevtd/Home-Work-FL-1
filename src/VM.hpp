#ifndef VM_HPP
#define VM_HPP

#include <bits/stdc++.h>
using namespace std;

namespace HomeWork2 {

class VM {
public:
    bool run(const string &regex, const string &input) {
        code.clear();
        auto frag = compile(regex);
        if (frag.start < 0) {
            return false;
        }
        int match_ip = emit_match();
        patch(frag.outs, match_ip);
        return execute(frag.start, input);
    }

private:
    enum Type { CHAR, MATCH, JMP, SPLIT };

    struct Inst {
        Type type;
        char c;
        int target;
        int x, y;
    };

    struct OutRef {
        int ip;
        int field;
    };

    struct Frag {
        int start;
        vector<OutRef> outs;
    };

    vector<Inst> code;

    int emit_inst(const Inst &i) {
        code.push_back(i);
        return (int)code.size() - 1;
    }

    int emit_char(char c) {
        Inst i{CHAR, c, -1, -1, -1};
        return emit_inst(i);
    }

    int emit_jmp(int target) {
        Inst i{JMP, 0, target, -1, -1};
        return emit_inst(i);
    }

    int emit_split(int x, int y) {
        Inst i{SPLIT, 0, -1, x, y};
        return emit_inst(i);
    }

    int emit_match() {
        Inst i{MATCH, 0, -1, -1, -1};
        return emit_inst(i);
    }

    void patch(const vector<OutRef> &outs, int target) {
        for (auto &r : outs) {
            if (r.field == 0) {
                code[r.ip].target = target;
            } else if (r.field == 1) {
                code[r.ip].x = target;
            } else if (r.field == 2) {
                code[r.ip].y = target;
            }
        }
    }

    vector<OutRef> append(const vector<OutRef> &a, const vector<OutRef> &b) {
        vector<OutRef> res = a;
        res.insert(res.end(), b.begin(), b.end());
        return res;
    }

    Frag make_char_frag(char c) {
        const int ip = emit_char(c);
        vector<OutRef> outs{{ip, 0}};
        return {ip, outs};
    }

    Frag concat_frag(const Frag &a, const Frag &b) {
        patch(a.outs, b.start);
        return {a.start, b.outs};
    }

    Frag alt_frag(const Frag &a, const Frag &b) {
        int ip = emit_split(a.start, b.start);
        vector<OutRef> outs = append(a.outs, b.outs);
        return {ip, outs};
    }

    Frag star_frag(const Frag &a) {
        int split_ip = emit_split(a.start, -1);
        patch(a.outs, split_ip);
        vector<OutRef> outs{{split_ip, 2}};
        return {split_ip, outs};
    }

    Frag plus_frag(const Frag &a) {
        const int split_ip = emit_split(a.start, -1);
        patch(a.outs, split_ip);
        return {a.start, vector<OutRef>{{split_ip, 2}}};
    }

    Frag ques_frag(const Frag &a) {
        int split_ip = emit_split(a.start, -1);
        vector<OutRef> outs = a.outs;
        outs.push_back({split_ip, 2});
        return {split_ip, outs};
    }

    vector<string> tokenize_with_concat(const string &s) {
        vector<string> toks;
        string prev = "";
        for (size_t i = 0; i < s.size(); i++) {
            string cur(1, s[i]);
            if (!prev.empty()) {
                bool prev_is_sym =
                    (prev == "a" || prev == "b" || prev == ")" || prev == "*" ||
                     prev == "+" || prev == "?");
                bool cur_is_sym = (cur == "a" || cur == "b" || cur == "(");
                if (prev_is_sym && cur_is_sym) {
                    toks.push_back(".");
                }
            }
            toks.push_back(cur);
            prev = cur;
        }
        return toks;
    }

    int prec(const string &op) {
        if (op == "*" || op == "+" || op == "?") {
            return 3;
        }
        if (op == ".") {
            return 2;
        }
        if (op == "|") {
            return 1;
        }
        return 0;
    }

    vector<string> to_postfix(const string &s) {
        vector<string> toks = tokenize_with_concat(s);
        vector<string> out, ops;
        for (auto &t : toks) {
            if (t == "a" || t == "b") {
                out.push_back(t);
            } else if (t == "(") {
                ops.push_back(t);
            } else if (t == ")") {
                while (!ops.empty() && ops.back() != "(") {
                    out.push_back(ops.back());
                    ops.pop_back();
                }
                if (!ops.empty() && ops.back() == "(") {
                    ops.pop_back();
                }
            } else {
                while (!ops.empty() && ops.back() != "(" &&
                       ((prec(ops.back()) > prec(t)) ||
                        (prec(ops.back()) == prec(t) && t != "*" && t != "+" &&
                         t != "?"))) {
                    out.push_back(ops.back());
                    ops.pop_back();
                }
                ops.push_back(t);
            }
        }
        while (!ops.empty()) {
            out.push_back(ops.back());
            ops.pop_back();
        }
        return out;
    }

    Frag compile(const string &regex) {
        vector<string> pf = to_postfix(regex);
        vector<Frag> st;
        for (auto &tok : pf) {
            if (tok == "a" || tok == "b") {
                st.push_back(make_char_frag(tok[0]));
            } else if (tok == ".") {
                auto b = st.back();
                st.pop_back();
                auto a = st.back();
                st.pop_back();
                st.push_back(concat_frag(a, b));
            } else if (tok == "|") {
                auto b = st.back();
                st.pop_back();
                auto a = st.back();
                st.pop_back();
                st.push_back(alt_frag(a, b));
            } else if (tok == "*") {
                auto a = st.back();
                st.pop_back();
                st.push_back(star_frag(a));
            } else if (tok == "+") {
                auto a = st.back();
                st.pop_back();
                st.push_back(plus_frag(a));
            } else if (tok == "?") {
                auto a = st.back();
                st.pop_back();
                st.push_back(ques_frag(a));
            }
        }
        if (st.empty()) {
            return {-1, {}};
        }
        return st.back();
    }

    bool execute(int start_pc, const string &input) {
        struct State {
            int pc, pos;
        };

        unordered_set<unsigned long long> seen;
        function<bool(int, int)> dfs = [&](int pc, int pos) {
            unsigned long long key = ((unsigned long long)pc << 32) | pos;
            if (seen.count(key)) {
                return false;
            }
            seen.insert(key);
            if (pc < 0 || pc >= (int)code.size()) {
                return false;
            }
            auto &inst = code[pc];
            if (inst.type == CHAR) {
                if (pos >= (int)input.size() || input[pos] != inst.c) {
                    return false;
                }
                int next = (inst.target == -1 ? pc + 1 : inst.target);
                return dfs(next, pos + 1);
            } else if (inst.type == MATCH) {
                return pos == (int)input.size();
            } else if (inst.type == JMP) {
                return dfs(inst.target, pos);
            } else if (inst.type == SPLIT) {
                int a = (inst.x == -1 ? pc + 1 : inst.x);
                int b = (inst.y == -1 ? pc + 1 : inst.y);
                return dfs(a, pos) || dfs(b, pos);
            }
            return false;
        };
        return dfs(start_pc, 0);
    }
};
}  // namespace HomeWork2

#endif
