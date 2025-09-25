#include <iostream>
#include <string>
#include <vector>

namespace HomeWork2 {
class VM {
public:
    bool run(const std::string &input) {
        auto code = compile(input);
        return run_(code, input, 0, 0);
    }

private:
    enum class InstructionType { CHAR, MATCH, JMP, SPLIT };

    struct Instruction {
        InstructionType type;
        char c;
        int target;
        int x, y;
    };

    bool run_(
        const std::vector<Instruction> &code,
        const std::string &input,
        int pc,
        int pos
    ) {
        if (pc >= code.size()) {
            return false;
        }

        const Instruction &inst = code[pc];

        switch (inst.type) {
            case InstructionType::CHAR:
                if (pos >= input.size() || input[pos] != inst.c) {
                    return false;
                }
                return run_(code, input, pc + 1, pos + 1);

            case InstructionType::MATCH:
                return pos == input.size();

            case InstructionType::JMP:
                return run_(code, input, inst.target, pos);

            case InstructionType::SPLIT:
                return run_(code, input, inst.x, pos) ||
                       run_(code, input, inst.y, pos);

            default:
                return false;
        }
    }

    int compile_recursive(
        const std::string &regex,
        int pos,
        std::vector<Instruction> &code
    ) {
        int start = static_cast<int>(code.size());

        while (pos < regex.size()) {
            char c = regex[pos];

            if (c == 'a' || c == 'b') {
                code.push_back({InstructionType::CHAR, c, 0, 0, 0});
                pos++;
            } else if (c == '|') {
                pos++;

                int left_end = static_cast<int>(code.size());
                int split_placeholder = static_cast<int>(code.size());
                code.push_back({InstructionType::SPLIT, 0, 0, -1, -1});

                pos = compile_recursive(regex, pos, code);

                int exit_jump_pos = static_cast<int>(code.size());
                code.push_back({InstructionType::JMP, 0, -1, 0, 0});

                code[split_placeholder].x = split_placeholder + 1;
                code[split_placeholder].y = static_cast<int>(code.size());

                Instruction split_inst = code[split_placeholder];
                code.erase(code.begin() + split_placeholder);
                code.insert(code.begin() + start, split_inst);

                code[exit_jump_pos].target = static_cast<int>(code.size()) + 1;

                start = start;
            } else if (c == '*' || c == '+' || c == '?') {
                if (code.size() <= static_cast<size_t>(start)) {
                    pos++;
                    continue;
                }

                int atom_index = static_cast<int>(code.size()) - 1;
                Instruction atom = code[atom_index];

                if (c == '*') {
                    int split_pos = static_cast<int>(code.size());
                    code.push_back(
                        {InstructionType::SPLIT, 0, 0, split_pos + 1,
                         split_pos + 2}
                    );
                    code.push_back(atom);
                    code.push_back({InstructionType::JMP, 0, split_pos, 0, 0});
                } else if (c == '+') {
                    code.push_back(atom);
                    int split_pos = static_cast<int>(code.size());
                    code.push_back(
                        {InstructionType::SPLIT, 0, 0, atom_index,
                         split_pos + 1}
                    );
                    code.push_back({InstructionType::JMP, 0, atom_index, 0, 0});
                } else if (c == '?') {
                    int split_pos = static_cast<int>(code.size());
                    code.push_back(
                        {InstructionType::SPLIT, 0, 0, split_pos + 1,
                         split_pos + 2}
                    );
                    code.push_back(atom);
                }

                pos++;
            } else {
                pos++;
            }
        }

        return pos;
    }

    std::vector<Instruction> compile(const std::string &input) {
        std::vector<Instruction> code;
        int pos = 0;
        pos = compile_recursive(input, pos, code);
        code.push_back({InstructionType::MATCH, 0, 0, 0, 0});
        return code;
    }
};
}  // namespace HomeWork2