#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-VRC");
    BOOST_CHECK(GetBoolArg("-VRC"));
    BOOST_CHECK(GetBoolArg("-VRC", false));
    BOOST_CHECK(GetBoolArg("-VRC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-VRCo"));
    BOOST_CHECK(!GetBoolArg("-VRCo", false));
    BOOST_CHECK(GetBoolArg("-VRCo", true));

    ResetArgs("-VRC=0");
    BOOST_CHECK(!GetBoolArg("-VRC"));
    BOOST_CHECK(!GetBoolArg("-VRC", false));
    BOOST_CHECK(!GetBoolArg("-VRC", true));

    ResetArgs("-VRC=1");
    BOOST_CHECK(GetBoolArg("-VRC"));
    BOOST_CHECK(GetBoolArg("-VRC", false));
    BOOST_CHECK(GetBoolArg("-VRC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noVRC");
    BOOST_CHECK(!GetBoolArg("-VRC"));
    BOOST_CHECK(!GetBoolArg("-VRC", false));
    BOOST_CHECK(!GetBoolArg("-VRC", true));

    ResetArgs("-noVRC=1");
    BOOST_CHECK(!GetBoolArg("-VRC"));
    BOOST_CHECK(!GetBoolArg("-VRC", false));
    BOOST_CHECK(!GetBoolArg("-VRC", true));

    ResetArgs("-VRC -noVRC");  // -VRC should win
    BOOST_CHECK(GetBoolArg("-VRC"));
    BOOST_CHECK(GetBoolArg("-VRC", false));
    BOOST_CHECK(GetBoolArg("-VRC", true));

    ResetArgs("-VRC=1 -noVRC=1");  // -VRC should win
    BOOST_CHECK(GetBoolArg("-VRC"));
    BOOST_CHECK(GetBoolArg("-VRC", false));
    BOOST_CHECK(GetBoolArg("-VRC", true));

    ResetArgs("-VRC=0 -noVRC=0");  // -VRC should win
    BOOST_CHECK(!GetBoolArg("-VRC"));
    BOOST_CHECK(!GetBoolArg("-VRC", false));
    BOOST_CHECK(!GetBoolArg("-VRC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--VRC=1");
    BOOST_CHECK(GetBoolArg("-VRC"));
    BOOST_CHECK(GetBoolArg("-VRC", false));
    BOOST_CHECK(GetBoolArg("-VRC", true));

    ResetArgs("--noVRC=1");
    BOOST_CHECK(!GetBoolArg("-VRC"));
    BOOST_CHECK(!GetBoolArg("-VRC", false));
    BOOST_CHECK(!GetBoolArg("-VRC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-VRC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-VRC", "eleven"), "eleven");

    ResetArgs("-VRC -vrc");
    BOOST_CHECK_EQUAL(GetArg("-VRC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-VRC", "eleven"), "");

    ResetArgs("-VRC=");
    BOOST_CHECK_EQUAL(GetArg("-VRC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-VRC", "eleven"), "");

    ResetArgs("-VRC=11");
    BOOST_CHECK_EQUAL(GetArg("-VRC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-VRC", "eleven"), "11");

    ResetArgs("-VRC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-VRC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-VRC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-VRC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-VRC", 0), 0);

    ResetArgs("-VRC -vrc");
    BOOST_CHECK_EQUAL(GetArg("-VRC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-vrc", 11), 0);

    ResetArgs("-VRC=11 -vrc=12");
    BOOST_CHECK_EQUAL(GetArg("-VRC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-vrc", 11), 12);

    ResetArgs("-VRC=NaN -vrc=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-VRC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-vrc", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--VRC");
    BOOST_CHECK_EQUAL(GetBoolArg("-VRC"), true);

    ResetArgs("--VRC=verbose --vrc=1");
    BOOST_CHECK_EQUAL(GetArg("-VRC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-vrc", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noVRC");
    BOOST_CHECK(!GetBoolArg("-VRC"));
    BOOST_CHECK(!GetBoolArg("-VRC", true));
    BOOST_CHECK(!GetBoolArg("-VRC", false));

    ResetArgs("-noVRC=1");
    BOOST_CHECK(!GetBoolArg("-VRC"));
    BOOST_CHECK(!GetBoolArg("-VRC", true));
    BOOST_CHECK(!GetBoolArg("-VRC", false));

    ResetArgs("-noVRC=0");
    BOOST_CHECK(GetBoolArg("-VRC"));
    BOOST_CHECK(GetBoolArg("-VRC", true));
    BOOST_CHECK(GetBoolArg("-VRC", false));

    ResetArgs("-VRC --noVRC");
    BOOST_CHECK(GetBoolArg("-VRC"));

    ResetArgs("-noVRC -VRC"); // VRC always wins:
    BOOST_CHECK(GetBoolArg("-VRC"));
}

BOOST_AUTO_TEST_SUITE_END()
