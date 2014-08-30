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
    ResetArgs("-Dibz");
    BOOST_CHECK(GetBoolArg("-Dibz"));
    BOOST_CHECK(GetBoolArg("-Dibz", false));
    BOOST_CHECK(GetBoolArg("-Dibz", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-Dibzo"));
    BOOST_CHECK(!GetBoolArg("-Dibzo", false));
    BOOST_CHECK(GetBoolArg("-Dibzo", true));

    ResetArgs("-Dibz=0");
    BOOST_CHECK(!GetBoolArg("-Dibz"));
    BOOST_CHECK(!GetBoolArg("-Dibz", false));
    BOOST_CHECK(!GetBoolArg("-Dibz", true));

    ResetArgs("-Dibz=1");
    BOOST_CHECK(GetBoolArg("-Dibz"));
    BOOST_CHECK(GetBoolArg("-Dibz", false));
    BOOST_CHECK(GetBoolArg("-Dibz", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noDibz");
    BOOST_CHECK(!GetBoolArg("-Dibz"));
    BOOST_CHECK(!GetBoolArg("-Dibz", false));
    BOOST_CHECK(!GetBoolArg("-Dibz", true));

    ResetArgs("-noDibz=1");
    BOOST_CHECK(!GetBoolArg("-Dibz"));
    BOOST_CHECK(!GetBoolArg("-Dibz", false));
    BOOST_CHECK(!GetBoolArg("-Dibz", true));

    ResetArgs("-Dibz -noDibz");  // -Dibz should win
    BOOST_CHECK(GetBoolArg("-Dibz"));
    BOOST_CHECK(GetBoolArg("-Dibz", false));
    BOOST_CHECK(GetBoolArg("-Dibz", true));

    ResetArgs("-Dibz=1 -noDibz=1");  // -Dibz should win
    BOOST_CHECK(GetBoolArg("-Dibz"));
    BOOST_CHECK(GetBoolArg("-Dibz", false));
    BOOST_CHECK(GetBoolArg("-Dibz", true));

    ResetArgs("-Dibz=0 -noDibz=0");  // -Dibz should win
    BOOST_CHECK(!GetBoolArg("-Dibz"));
    BOOST_CHECK(!GetBoolArg("-Dibz", false));
    BOOST_CHECK(!GetBoolArg("-Dibz", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--Dibz=1");
    BOOST_CHECK(GetBoolArg("-Dibz"));
    BOOST_CHECK(GetBoolArg("-Dibz", false));
    BOOST_CHECK(GetBoolArg("-Dibz", true));

    ResetArgs("--noDibz=1");
    BOOST_CHECK(!GetBoolArg("-Dibz"));
    BOOST_CHECK(!GetBoolArg("-Dibz", false));
    BOOST_CHECK(!GetBoolArg("-Dibz", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-Dibz", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-Dibz", "eleven"), "eleven");

    ResetArgs("-Dibz -bar");
    BOOST_CHECK_EQUAL(GetArg("-Dibz", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-Dibz", "eleven"), "");

    ResetArgs("-Dibz=");
    BOOST_CHECK_EQUAL(GetArg("-Dibz", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-Dibz", "eleven"), "");

    ResetArgs("-Dibz=11");
    BOOST_CHECK_EQUAL(GetArg("-Dibz", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-Dibz", "eleven"), "11");

    ResetArgs("-Dibz=eleven");
    BOOST_CHECK_EQUAL(GetArg("-Dibz", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-Dibz", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-Dibz", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-Dibz", 0), 0);

    ResetArgs("-Dibz -bar");
    BOOST_CHECK_EQUAL(GetArg("-Dibz", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-Dibz=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-Dibz", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-Dibz=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-Dibz", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--Dibz");
    BOOST_CHECK_EQUAL(GetBoolArg("-Dibz"), true);

    ResetArgs("--Dibz=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-Dibz", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noDibz");
    BOOST_CHECK(!GetBoolArg("-Dibz"));
    BOOST_CHECK(!GetBoolArg("-Dibz", true));
    BOOST_CHECK(!GetBoolArg("-Dibz", false));

    ResetArgs("-noDibz=1");
    BOOST_CHECK(!GetBoolArg("-Dibz"));
    BOOST_CHECK(!GetBoolArg("-Dibz", true));
    BOOST_CHECK(!GetBoolArg("-Dibz", false));

    ResetArgs("-noDibz=0");
    BOOST_CHECK(GetBoolArg("-Dibz"));
    BOOST_CHECK(GetBoolArg("-Dibz", true));
    BOOST_CHECK(GetBoolArg("-Dibz", false));

    ResetArgs("-Dibz --noDibz");
    BOOST_CHECK(GetBoolArg("-Dibz"));

    ResetArgs("-noDibz -Dibz"); // Dibz always wins:
    BOOST_CHECK(GetBoolArg("-Dibz"));
}

BOOST_AUTO_TEST_SUITE_END()
