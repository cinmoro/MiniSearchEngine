#include <gtest/gtest.h>

#include "search.h"
#include "tests/build_index.h"
#include "tests/clean_token.h"
#include "tests/find_query_matches.h"
#include "tests/gather_tokens.h"

//Tests punctuation at both ends
TEST(CleanToken, PunctAtBothEnds) {
    map<string, set<string>> tests;
    tests["word"].insert("!!word!!");
    tests["gasp"].insert("...gasp...");
    tests["hell0"].insert("/!?..hEll0?/");

    testCleanToken(tests);
}

// Tests for punctuaion in the middle, but not at the ends
TEST(CleanToken, PunctInMidNotAtEnds) {
    map<string, set<string>> tests;
    tests["scream!yell"].insert("Scream!yell");
    tests["can...you"].insert("can...YOU");
    tests["pigs?fly"].insert("pigs?fly");

    testCleanToken(tests);
}

// Tests for punctusaion in the middle and at the beginning of the word
TEST(CleanToken, PunctInMidAtStart) {
    map<string, set<string>> tests;
    tests["scream!yell"].insert("!!Scream!yell");
    tests["can...you"].insert("?/can...you");
    tests["pigs?fly"].insert("!;?/pigs?fly");

    testCleanToken(tests);
}

// Tests for punctuation in the middle and at the end of the word
TEST(CleanToken, PunctInMidAtEnd) {
    map<string, set<string>> tests;
    tests["scream!yell"].insert("Scream!yell!!");
    tests["can...you"].insert("can...you/?~?");
    tests["pigs?fly"].insert("pigs?fly/?!");

    testCleanToken(tests);
}

TEST(CleanToken, ExampleEmptyTest) {
    cout << "Hi! This shows up in the terminal." << endl;
}

// Tests for leading space in the string
TEST(GatherTokens, LeadSpace) {
    string text = "  we are th3 ~leading~ spaces!!";
    set<string> expected = {"we", "are", "th3", "leading", "spaces"};
    set<string> studentResult = gatherTokens(text);

    string testFeedback = "gatherTokens(\"" + text + "\") incorrect\n";

    EXPECT_EQ(expected, studentResult) << testFeedback;
}

// Tests for removing ending space in the string
TEST(GatherTokens, EndSpace) {
    string text = "we are th3 ~ENDING~ spaces!!     ";
    set<string> expected = {"we", "are", "th3", "ending", "spaces"};
    set<string> studentResult = gatherTokens(text);

    string testFeedback = "gatherTokens(\"" + text + "\") incorrect\n";

    EXPECT_EQ(expected, studentResult) << testFeedback;
}

// Tests for removing space between words in the string
TEST(GatherTokens, BetweenTokenSpace) {
    string text = "we  are      th3    ~  BETWEEN~    spaces!!";
    set<string> expected = {"we", "are", "th3", "between", "spaces"};
    set<string> studentResult = gatherTokens(text);

    string testFeedback = "gatherTokens(\"" + text + "\") incorrect\n";

    EXPECT_EQ(expected, studentResult) << testFeedback;
}

// Tests for index built correctly and returns correct number of urls in index
TEST(BuildIndex, TinyTxt) {
    string filename = "tiny.txt";
    map<string, set<string>> expected = {
        {"eggs", {"www.shoppinglist.com"}},
        {"milk", {"www.shoppinglist.com"}},
        {"fish", {"www.shoppinglist.com", "www.dr.seuss.net"}},
        {"bread", {"www.shoppinglist.com"}},
        {"cheese", {"www.shoppinglist.com"}},
        {"red", {"www.rainbow.org", "www.dr.seuss.net"}},
        {"green", {"www.rainbow.org"}},
        {"orange", {"www.rainbow.org"}},
        {"yellow", {"www.rainbow.org"}},
        {"blue", {"www.rainbow.org", "www.dr.seuss.net"}},
        {"indigo", {"www.rainbow.org"}},
        {"violet", {"www.rainbow.org"}},
        {"one", {"www.dr.seuss.net"}},
        {"two", {"www.dr.seuss.net"}},
        {"i'm", {"www.bigbadwolf.com"}},
        {"not", {"www.bigbadwolf.com"}},
        {"trying", {"www.bigbadwolf.com"}},
        {"to", {"www.bigbadwolf.com"}},
        {"eat", {"www.bigbadwolf.com"}},
        {"you", {"www.bigbadwolf.com"}},
    };
    map<string, set<string>> studentIndex;
    int studentNumProcesed = buildIndex(filename, studentIndex);

    string indexTestFeedback = "buildIndex(\"" + filename + "\", ...) index incorrect\n";
    EXPECT_EQ(expected, studentIndex) << indexTestFeedback;

    string retTestFeedback = "buildIndex(\"" + filename + "\", ...) return value incorrect\n";
    EXPECT_EQ(4, studentNumProcesed) << retTestFeedback;
}

// Tests for invalid file
TEST(BuildIndex, InvalidFile) {
    string filename = "absdbasdbasd.txt";
    map<string, set<string>> expected = {};
    map<string, set<string>> studentIndex;
    int studentNumProcesed = buildIndex(filename, studentIndex);

    string indexTestFeedback = "buildIndex(\"" + filename + "\", ...) index incorrect\n";
    EXPECT_EQ(expected, studentIndex) << indexTestFeedback;

    string retTestFeedback = "buildIndex(\"" + filename + "\", ...) return value incorrect\n";
    EXPECT_EQ(0, studentNumProcesed) << retTestFeedback;
}

// Tests for the first word in query not being in the index
TEST(FindQueryMatches, FirstNotInIndex) {
    set<string> expected;

    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "girl"));

    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "hi +hello"));

    expected = {"random.org"};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "cindy aviation laws"));

    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "corndog -according +hello"));
}

// Tests for a word in query not being in the index, but with a modifier +
TEST(FindQueryMatches, NotInIndexIntersection) {
    set<string> expected;

    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "known +aliens"));

    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "according +hamsters +science"));

    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "all according +unicorns"));

    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "according to all known laws of +gnomes"));
}

// Tests for a word in query not being in the index, but with a modifier -
TEST(FindQueryMatches, NotInIndexDifference) {
    set<string> expected;

    expected = {"random.org"};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "aviation -birds"));

    expected = {"uic.edu", "example.com", "random.org"};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "according to all -doctors"));

    expected = {"uic.edu", "random.org"};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "a law -crime of"));

    expected = {"example.com", "uic.edu", "random.org"};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "hello there to all -chipmunks"));
}

// Tests for a word in query not being in the index, but at a much later part of the query
TEST(FindQueryMatches, NotInIndexLater) {
    set<string> expected;

    expected = {"random.org"};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "laws +aviation +a kites"));

    expected = {"random.org"};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "to  all +of science"));

    expected = {};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "all +according -to +teachers"));

    expected = {"uic.edu", "example.com", "random.org"};
    EXPECT_EQ(expected, findQueryMatches(INDEX, "according to all known donuts laws of"));
}
