#include "helpers.h"

#include <gtest/gtest.h>

TEST(Run, SingleStep) {
    auto rules = ParseRulesStr(":a -> b");
    EXPECT_EQ(RunSimple(":a", rules), "b");
}

TEST(Run, MultiStep) {
    auto rules = ParseRulesStr(
        ":a -> :b\n"
        ":b -> c");
    EXPECT_EQ(RunSimple(":a", rules), "c");
}

TEST(Run, UseFirstRule) {
    auto rules = ParseRulesStr(
        ":a -> x\n"
        ":a -> y");
    EXPECT_EQ(RunSimple(":a", rules), "x");
}

TEST(Run, ReplaceFirstOccurrence) {
    auto rules = ParseRulesStr(
        ":a -> :b\n"
        ":b -> c");
    EXPECT_EQ(RunSimple(":a,:a", rules, 0), ":b,:a");
}

TEST(Run, StopsWhenNoRuleApplies) {
    auto rules = ParseRulesStr(":b -> c");
    EXPECT_EQ(RunSimple(":a", rules), ":a");
}

TEST(Run, StopsAtMaxSteps) {
    auto rules = ParseRulesStr(
        ":s1 -> :s2\n"
        ":s2 -> :s3\n"
        ":s3 -> :s4\n");
    EXPECT_EQ(RunSimple(":s1", rules, 2), ":s3");
}

TEST(Run, StopsAtEndState) {
    auto rules = ParseRulesStr(":a -> b");
    EXPECT_EQ(RunSimple(":B,:a,:E", rules), ":B,b,:E");
}
