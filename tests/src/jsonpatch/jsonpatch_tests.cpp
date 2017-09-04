// Copyright 2013 Daniel Parker
// Distributed under Boost license

#ifdef __linux__
#define BOOST_TEST_DYN_LINK
#endif

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <sstream>
#include <vector>
#include <map>
#include <utility>
#include <ctime>
#include <new>
#include <codecvt>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpatch/jsonpatch.hpp>

using namespace jsoncons;
using namespace jsoncons::literals;

BOOST_AUTO_TEST_SUITE(jsonpatch_tests)

BOOST_AUTO_TEST_CASE(test_patch)
{
    json target = R"(
    { "foo": "bar"}
    )"_json;

    json patch = R"(
    [
      { "op": "add", "path": "/baz", "value": "qux" }
    ]
    )"_json;

    jsonpatch::patch(target,patch);
}

BOOST_AUTO_TEST_SUITE_END()



