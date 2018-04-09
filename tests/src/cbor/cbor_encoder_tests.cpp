// Copyright 2016 Daniel Parker
// Distributed under Boost license

#ifdef __linux__
#define BOOST_TEST_DYN_LINK
#endif

#include <boost/test/unit_test.hpp>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/cbor/cbor.hpp>
#include <sstream>
#include <vector>
#include <utility>
#include <ctime>
#include <limits>

using namespace jsoncons;
using namespace jsoncons::cbor;

BOOST_AUTO_TEST_SUITE(cbor_encoder_tests)

BOOST_AUTO_TEST_CASE(encode_cbor)
{
    std::vector<uint8_t> v;

    const uint8_t bs[] = {'H','e','l','l','o'};
    json j(bs,sizeof(bs));

    cbor_encoder encoder(v);
    j.dump(encoder);

} 

BOOST_AUTO_TEST_SUITE_END()

