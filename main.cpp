#include <chrono>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <thread>
#include <vector>

const int DEFAULT_MAX_STEPS = 1'000'000;
const int DEFAULT_DELAY_MS = 1'000;

struct Rule {
    std::string a;
    std::string b;

    Rule(std::string a, std::string b)
        : a(std::move(a)),
          b(std::move(b)) {
    }
};

const char* rule_file_name;
const char* input_file_name;
const char* output_file_name;
const char* trace_file_name = nullptr;

bool interactive = false;
int delay_ms = DEFAULT_DELAY_MS;
int max_steps = DEFAULT_MAX_STEPS;

std::regex rule_regex(
    R"(((?::?[0-9a-zA-Z_.']+)(?:,:?[0-9a-zA-Z_.']+)*)\s*->\s*((?::?[0-9a-zA-Z_.']+)(?:,:?[0-9a-zA-Z_.']+)*))");

std::regex stop_regex(R"((,:B)?[^:]*(:E,)?)");

std::string state;
std::vector<Rule> rules;

std::string Crop(const std::string& s) {
    return s.substr(1, s.size() - 2);
}

void ExitWithError(const std::string& error_message) {
    std::cerr << "\033[1;31mError:\033[0m " << error_message << "\n";
    exit(1);
}

void ParseArguments(int argc, char* argv[]) {
    if (argc < 4) {
        ExitWithError(
            "Usage: <rule file> <input file> <output file> [-t <trace file>] [-m <max steps>] [-i] "
            "[-d <ms>]");
    }
    rule_file_name = argv[1];
    input_file_name = argv[2];
    output_file_name = argv[3];

    for (int i = 4; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-t") {
            if (i + 1 >= argc) {
                ExitWithError("-t requires a file argument");
            }

            trace_file_name = argv[++i];
        } else if (arg == "-i") {
            interactive = true;
        } else if (arg == "-d") {
            if (i + 1 >= argc) {
                ExitWithError("-d requires a number argument");
            }

            try {
                delay_ms = std::stoi(argv[++i]);
            } catch (...) {
                ExitWithError("-d requires a valid integer");
            }
        } else if (arg == "-m") {
            if (i + 1 >= argc) {
                ExitWithError("-m requires a number argument");
            }

            try {
                max_steps = std::stoi(argv[++i]);
            } catch (...) {
                ExitWithError("-m requires a valid integer");
            }
        } else {
            ExitWithError("Unknown flag: " + arg);
        }
    }
}

void ParseRules() {
    std::ifstream rule_file(rule_file_name);
    if (!rule_file) {
        ExitWithError("Cannot open rule file");
    }

    std::string line;
    int line_num = 0;
    while (std::getline(rule_file, line)) {
        ++line_num;
        if (line.empty()) {
            continue;
        }

        std::smatch matches;
        if (!std::regex_match(line, matches, rule_regex)) {
            ExitWithError("Syntax error on line number " + std::to_string(line_num));
        }
        rules.emplace_back(',' + std::string(matches[1]) + ',',
                           ',' + std::string(matches[2]) + ',');
    }
}

void GetInput() {
    std::ifstream input_file(input_file_name);
    if (!input_file) {
        ExitWithError("Cannot open input file");
    }
    std::getline(input_file, state);
    state = ',' + state + ',';
}

bool TryToApplyRule(const Rule& rule) {
    size_t pos = state.find(rule.a);
    if (pos != std::string::npos) {
        state.replace(pos, rule.a.size(), rule.b);
        return true;
    }
    return false;
}

void Run() {
    std::ofstream trace_file;
    if (trace_file_name) {
        trace_file.open(trace_file_name);
        if (!trace_file) {
            ExitWithError("Cannot open trace file");
        }
    }

    int step = 1;
    do {
        bool replaced = false;
        for (const auto& rule : rules) {
            if (TryToApplyRule(rule)) {
                if (interactive) {
                    std::cout << Crop(state) << "\n";
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
                }
                if (trace_file_name) {
                    trace_file << "Step " << step << "\n";
                    trace_file << "Rule: " << Crop(rule.a) << " -> " << Crop(rule.b) << "\n";
                    trace_file << "State: " << Crop(state) << "\n\n";
                }

                replaced = true;
                break;
            }
        }
        if (!replaced) {
            break;
        }
        ++step;
    } while (step <= max_steps && !std::regex_match(state, stop_regex));
}

void WriteOutput() {
    std::ofstream output_file(output_file_name);
    if (!output_file) {
        ExitWithError("Cannot open output file");
    }
    output_file << state << "\n";
}

int main(int argc, char* argv[]) {
    ParseArguments(argc, argv);
    ParseRules();
    GetInput();
    Run();
    WriteOutput();
}
