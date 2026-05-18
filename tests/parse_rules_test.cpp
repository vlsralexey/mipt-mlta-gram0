#include "helpers.h"

#include <gtest/gtest.h>

TEST(ParseRules, SingleRule) {
    auto rules = ParseRulesStr(":a -> b");
    ASSERT_EQ(rules.size(), 1);
    EXPECT_EQ(rules[0].a, ",:a,");
    EXPECT_EQ(rules[0].b, ",b,");
}

TEST(ParseRules, SkipEmptyLines) {
    auto rules = ParseRulesStr(
        "\n"
        ":a -> b\n\n"
        ":b -> c\n");
    EXPECT_EQ(rules.size(), 2);
}

TEST(ParseRules, SeveralSymbols) {
    auto rules = ParseRulesStr(":a,:b -> c,d");
    ASSERT_EQ(rules.size(), 1);
    EXPECT_EQ(rules[0].a, ",:a,:b,");
    EXPECT_EQ(rules[0].b, ",c,d,");
}

TEST(ParseRules, InvalidRuleThrow) {
    EXPECT_THROW(ParseRulesStr("rule"), std::runtime_error);
    EXPECT_THROW(ParseRulesStr("a,b - c"), std::runtime_error);
    EXPECT_THROW(ParseRulesStr("::a,b -> c"), std::runtime_error);
    EXPECT_THROW(ParseRulesStr(":a:b,c -> d"), std::runtime_error);
}

TEST(ParseRules, AvailableSymbols) {
    auto rules = ParseRulesStr("s1,s1',s1.new,s1.new -> s2,s2',s2.new,s2.new");
    ASSERT_EQ(rules.size(), 1);
    EXPECT_EQ(rules[0].a, ",s1,s1',s1.new,s1.new,");
    EXPECT_EQ(rules[0].b, ",s2,s2',s2.new,s2.new,");
}
