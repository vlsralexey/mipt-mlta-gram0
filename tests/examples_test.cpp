#include "helpers.h"

#include <gtest/gtest.h>

static std::string ToBinary(int t) {
    std::vector<char> bits;
    if (t == 0) {
        bits = {'0'};
    } else {
        while (t > 0) {
            if (t % 2 == 0) {
                bits.push_back('0');
            } else {
                bits.push_back('1');
            }
            t >>= 1;
        }
    }

    std::string result;
    for (size_t i = 0; i < bits.size(); ++i) {
        result += bits[bits.size() - i - 1];
        if (i + 1 != bits.size()) {
            result += ',';
        }
    }
    return result;
}

static std::string InvertBinary(const std::string& binary) {
    std::string result;
    for (char c : binary) {
        if (c == '0') {
            result += '1';
        } else if (c == '1') {
            result += '0';
        } else {
            result += c;
        }
    }
    return result;
}

static std::string BinaryToState(const std::string& binary) {
    return ":B," + binary + ",:E";
}

TEST(Examples, PlusOne) {
    auto rules = ParseRulesFile(EXAMPLES_DIR "/plus_one.txt");
    for (int t = 0; t < (1 << 6); ++t) {
        EXPECT_EQ(RunSimple(BinaryToState(ToBinary(t)), rules), BinaryToState(ToBinary(t + 1)));
    }
}

TEST(Examples, Invert) {
    auto rules = ParseRulesFile(EXAMPLES_DIR "/invert.txt");
    for (int t = 0; t < (1 << 6); ++t) {
        std::string binary = ToBinary(t);
        std::string inverted_binary = InvertBinary(binary);
        EXPECT_EQ(RunSimple(BinaryToState(binary), rules), BinaryToState(inverted_binary));
    }
}

TEST(Examples, Duplicate) {
    auto rules = ParseRulesFile(EXAMPLES_DIR "/duplicate.txt");
    for (int t = 0; t < (1 << 6); ++t) {
        std::string binary = ToBinary(t);
        std::string duplicate_binary = binary;
        duplicate_binary += ',';
        duplicate_binary += binary;
        EXPECT_EQ(RunSimple(BinaryToState(binary), rules), BinaryToState(duplicate_binary));
    }
}
