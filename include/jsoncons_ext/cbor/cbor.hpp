// Copyright 2017 Daniel Parker
// Distributed under the Boost license, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See https://github.com/danielaparker/jsoncons for latest version

#ifndef JSONCONS_CBOR_CBOR_HPP
#define JSONCONS_CBOR_CBOR_HPP

#include <string>
#include <sstream>
#include <vector>
#include <istream>
#include <cstdlib>
#include <memory>
#include <limits>
#include <cassert>
#include <iterator>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/binary/binary_utilities.hpp>
#include <jsoncons_ext/cbor/cbor_view.hpp>
#include <jsoncons_ext/cbor/cbor_encoder.hpp>

namespace jsoncons { namespace cbor {

class cbor_packed : public cbor_view
{
    std::vector<uint8_t> data_;
public:

    cbor_packed() = default;
    cbor_packed(const cbor_packed& val)
        : data_(val.data_)
    {
        this->set_data(data_.data(),data_.size());
    }
    cbor_packed(cbor_packed&& val)
        : data_(std::move(val.data_))
    {
        this->set_data(data_.data(),data_.size());
    }
    cbor_packed(const std::vector<uint8_t>& val)
        : data_(val)
    {
        this->set_data(data_.data(),data_.size());
    }
    cbor_packed(std::vector<uint8_t>&& val)
        : data_(val)
    {
        this->set_data(data_.data(),data_.size());
    }

};

struct Encode_cbor_
{
    template <typename T>
    void operator()(T val, std::vector<uint8_t>& v)
    {
        binary::to_big_endian(val,v);
    }
};

struct Calculate_size_
{
    template <typename T>
    void operator()(T, size_t& size)
    {
        size += sizeof(T);
    }
};
  
template<class Json>
struct cbor_Encoder_
{
    typedef typename Json::string_view_type string_view_type;

    static size_t calculate_size(const Json& j)
    {
        size_t n = 0;
        cbor_Encoder_<Json>::encode(j,Calculate_size_(),n);
        return n;
    }

    template <class Action, class Result>
    static void encode(const Json& jval, Action action, Result& v)
    {
        switch (jval.type_id())
        {
        case json_type_tag::null_t:
            {
                action(static_cast<uint8_t>(0xf6), v);
                break;
            }

        case json_type_tag::bool_t:
            {
                action(static_cast<uint8_t>(jval.as_bool() ? 0xf5 : 0xf4), v);
                break;
            }

        case json_type_tag::integer_t:
            {
                int64_t val = jval.as_integer();
                if (val >= 0)
                {
                    if (val <= 0x17)
                    {
                        action(static_cast<uint8_t>(val), v);
                    } else if (val <= (std::numeric_limits<uint8_t>::max)())
                    {
                        action(static_cast<uint8_t>(0x18), v);
                        action(static_cast<uint8_t>(val), v);
                    } else if (val <= (std::numeric_limits<uint16_t>::max)())
                    {
                        action(static_cast<uint8_t>(0x19), v);
                        action(static_cast<uint16_t>(val), v);
                    } else if (val <= (std::numeric_limits<uint32_t>::max)())
                    {
                        action(static_cast<uint8_t>(0x1a), v);
                        action(static_cast<uint32_t>(val), v);
                    } else if (val <= (std::numeric_limits<int64_t>::max)())
                    {
                        action(static_cast<uint8_t>(0x1b), v);
                        action(static_cast<int64_t>(val), v);
                    }
                } else
                {
                    const auto posnum = -1 - val;
                    if (val >= -24)
                    {
                        action(static_cast<uint8_t>(0x20 + posnum), v);
                    } else if (posnum <= (std::numeric_limits<uint8_t>::max)())
                    {
                        action(static_cast<uint8_t>(0x38), v);
                        action(static_cast<uint8_t>(posnum), v);
                    } else if (posnum <= (std::numeric_limits<uint16_t>::max)())
                    {
                        action(static_cast<uint8_t>(0x39), v);
                        action(static_cast<uint16_t>(posnum), v);
                    } else if (posnum <= (std::numeric_limits<uint32_t>::max)())
                    {
                        action(static_cast<uint8_t>(0x3a), v);
                        action(static_cast<uint32_t>(posnum), v);
                    } else if (posnum <= (std::numeric_limits<int64_t>::max)())
                    {
                        action(static_cast<uint8_t>(0x3b), v);
                        action(static_cast<int64_t>(posnum), v);
                    }
                }
                break;
            }

        case json_type_tag::uinteger_t:
            {
                uint64_t val = jval.as_uinteger();
                if (val <= 0x17)
                {
                    action(static_cast<uint8_t>(val),v);
                } else if (val <=(std::numeric_limits<uint8_t>::max)())
                {
                    action(static_cast<uint8_t>(0x18), v);
                    action(static_cast<uint8_t>(val),v);
                } else if (val <=(std::numeric_limits<uint16_t>::max)())
                {
                    action(static_cast<uint8_t>(0x19), v);
                    action(static_cast<uint16_t>(val),v);
                } else if (val <=(std::numeric_limits<uint32_t>::max)())
                {
                    action(static_cast<uint8_t>(0x1a), v);
                    action(static_cast<uint32_t>(val),v);
                } else if (val <=(std::numeric_limits<uint64_t>::max)())
                {
                    action(static_cast<uint8_t>(0x1b), v);
                    action(static_cast<uint64_t>(val),v);
                }
                break;
            }

        case json_type_tag::double_t:
            {
                action(static_cast<uint8_t>(0xfb), v);
                action(jval.as_double(),v);
                break;
            }

        case json_type_tag::byte_string_t:
            {
                encode_byte_string(jval. template as<std::vector<uint8_t>>(), action, v);
                break;
            }

        case json_type_tag::small_string_t:
        case json_type_tag::string_t:
            {
                encode_string(jval.as_string_view(), action, v);
                break;
            }

        case json_type_tag::array_t:
            {
                const auto length = jval.array_value().size();
                if (length <= 0x17)
                {
                    action(static_cast<uint8_t>(static_cast<uint8_t>(0x80 + length)), v);
                } else if (length <= 0xff)
                {
                    action(static_cast<uint8_t>(0x98), v);
                    action(static_cast<uint8_t>(static_cast<uint8_t>(length)), v);
                } else if (length <= 0xffff)
                {
                    action(static_cast<uint8_t>(0x99), v);
                    action(static_cast<uint16_t>(length),v);
                } else if (length <= 0xffffffff)
                {
                    action(static_cast<uint8_t>(0x9a), v);
                    action(static_cast<uint32_t>(length),v);
                } else if (length <= 0xffffffffffffffff)
                {
                    action(static_cast<uint8_t>(0x9b), v);
                    action(static_cast<uint64_t>(length),v);
                }

                // append each element
                for (const auto& el : jval.array_range())
                {
                    encode(el,action,v);
                }
                break;
            }

        case json_type_tag::object_t:
            {
                const auto length = jval.object_value().size();
                if (length <= 0x17)
                {
                    action(static_cast<uint8_t>(static_cast<uint8_t>(0xa0 + length)), v);
                } else if (length <= 0xff)
                {
                    action(static_cast<uint8_t>(0xb8), v);
                    action(static_cast<uint8_t>(static_cast<uint8_t>(length)), v);
                } else if (length <= 0xffff)
                {
                    action(static_cast<uint8_t>(0xb9), v);
                    action(static_cast<uint16_t>(length),v);
                } else if (length <= 0xffffffff)
                {
                    action(static_cast<uint8_t>(0xba), v);
                    action(static_cast<uint32_t>(length),v);
                } else if (length <= 0xffffffffffffffff)
                {
                    action(static_cast<uint8_t>(0xbb), v);
                    action(static_cast<uint64_t>(length),v);
                }

                // append each element
                for (const auto& kv: jval.object_range())
                {
                    encode_string(kv.key(), action, v);
                    encode(kv.value(), action, v);
                }
                break;
            }

        default:
            {
                break;
            }
        }
    }

    template <class Action,class Result>
    static void encode_string(const string_view_type& sv, Action action, Result& v)
    {
        std::basic_string<uint8_t> target;
        auto result = unicons::convert(
            sv.begin(), sv.end(), std::back_inserter(target), 
            unicons::conv_flags::strict);
        if (result.ec != unicons::conv_errc())
        {
            JSONCONS_THROW(json_exception_impl<std::runtime_error>("Illegal unicode"));
        }

        const size_t length = target.length();
        if (length <= 0x17)
        {
            // fixstr stores a byte array whose length is upto 31 bytes
            action(static_cast<uint8_t>(static_cast<uint8_t>(0x60 + length)), v);
        }
        else if (length <= 0xff)
        {
            action(static_cast<uint8_t>(0x78), v);
            action(static_cast<uint8_t>(static_cast<uint8_t>(length)), v);
        }
        else if (length <= 0xffff)
        {
            action(static_cast<uint8_t>(0x79), v);
            action(static_cast<uint16_t>(length), v);
        }
        else if (length <= 0xffffffff)
        {
            action(static_cast<uint8_t>(0x7a), v);
            action(static_cast<uint32_t>(length), v);
        }
        else if (length <= 0xffffffffffffffff)
        {
            action(static_cast<uint8_t>(0x7b), v);
            action(static_cast<uint64_t>(length),v);
        }

        for (size_t i = 0; i < length; ++i)
        {
            action(static_cast<uint8_t>(target.data()[i]), v);
        }
    }

    template <class Action,class Result>
    static void encode_byte_string(const std::vector<uint8_t>& target, Action action, Result& v)
    {
        const size_t length = target.size();
        if (length <= 0x17)
        {
            // fixstr stores a byte array whose length is upto 31 bytes
            action(static_cast<uint8_t>(static_cast<uint8_t>(0x40 + length)), v);
        }
        else if (length <= 0xff)
        {
            action(static_cast<uint8_t>(0x58), v);
            action(static_cast<uint8_t>(static_cast<uint8_t>(length)), v);
        }
        else if (length <= 0xffff)
        {
            action(static_cast<uint8_t>(0x59), v);
            action(static_cast<uint16_t>(length), v);
        }
        else if (length <= 0xffffffff)
        {
            action(static_cast<uint8_t>(0x5a), v);
            action(static_cast<uint32_t>(length), v);
        }
        else if (length <= 0xffffffffffffffff)
        {
            action(static_cast<uint8_t>(0x5b), v);
            action(static_cast<uint64_t>(length),v);
        }

        for (size_t i = 0; i < length; ++i)
        {
            action(static_cast<uint8_t>(target.data()[i]), v);
        }
    }
};

// decode_cbor

template<class Json>
class Decode_cbor_
{
    const uint8_t* begin_;
    const uint8_t* end_;
    const uint8_t* it_;
public:
    typedef typename Json::char_type char_type;

    Decode_cbor_(const uint8_t* begin, const uint8_t* end)
        : begin_(begin), end_(end), it_(begin)
    {
    }

    Json decode()
    {
        const uint8_t* pos = it_++;
        switch (*pos)
        {
            
        case JSONCONS_CBOR_0x00_0x17: // Integer 0x00..0x17 (0..23)
            // FALLTHRU
        case 0x18: // Unsigned integer (one-byte uint8_t follows)
            // FALLTHRU
        case 0x19: // Unsigned integer (two-byte uint16_t follows)
            // FALLTHRU
        case 0x1a: // Unsigned integer (four-byte uint32_t follows)
            // FALLTHRU
        case 0x1b: // Unsigned integer (eight-byte uint64_t follows)
            {
                const uint8_t* endp;
                uint64_t val = detail::get_uinteger(pos,end_,&endp);
                if (endp == pos)
                {
                    JSONCONS_THROW(cbor_decode_error(endp-begin_));
                }
                it_ = endp;
                return Json(val);
            }
            break;

        case JSONCONS_CBOR_0x20_0x37: // Negative integer -1-0x00..-1-0x17 (-1..-24)
            // FALLTHRU
        case 0x38: // Negative integer (one-byte uint8_t follows)
            // FALLTHRU
        case 0x39: // Negative integer -1-n (two-byte uint16_t follows)
            // FALLTHRU
        case 0x3a: // Negative integer -1-n (four-byte uint32_t follows)
            // FALLTHRU
        case 0x3b: // Negative integer -1-n (eight-byte uint64_t follows)
            {
                const uint8_t* endp;
                int64_t val = detail::get_integer(pos,end_,&endp);
                if (endp == pos)
                {
                    JSONCONS_THROW(cbor_decode_error(endp-begin_));
                }
                it_ = endp;
                return Json(val);
            }
            break;
            // byte string (0x00..0x17 bytes follow)
        case JSONCONS_CBOR_0x40_0x57:
        case 0x58:
        case 0x59:
        case 0x5a:
        case 0x5b:
        case 0x5f:
            {
                const uint8_t* endp;
                std::vector<uint8_t> v = detail::get_byte_string(pos,end_,&endp);
                if (endp == pos)
                {
                    JSONCONS_THROW(cbor_decode_error(end_-pos));
                }
                else
                {
                    it_ = endp;
                }

                return Json(v.data(),v.size());
            }

            // UTF-8 string (0x00..0x17 bytes follow)
        case JSONCONS_CBOR_0x60_0x77:
        case 0x78:
        case 0x79:
        case 0x7a:
        case 0x7b:
        case 0x7f:
            {
                const uint8_t* endp;
                std::string s = detail::get_text_string(pos,end_,&endp);
                if (endp == pos)
                {
                    JSONCONS_THROW(cbor_decode_error(end_-pos));
                }
                else
                {
                    it_ = endp;
                }
                std::basic_string<char_type> target;
                auto result = unicons::convert(s.begin(),s.end(),std::back_inserter(target),unicons::conv_flags::strict);
                if (result.ec != unicons::conv_errc())
                {
                    JSONCONS_THROW(json_exception_impl<std::runtime_error>("Illegal unicode"));
                }
                return Json(target);
            }

            // array (0x00..0x17 data items follow)
        case JSONCONS_CBOR_0x80_0x97:
            {
                return get_fixed_length_array(*pos & 0x1f);
            }

            // array (one-byte uint8_t for n follows)
        case 0x98:
            {
                const uint8_t* endp;
                const auto len = binary::from_big_endian<uint8_t>(it_,end_,&endp);
                if (endp == it_)
                {
                    JSONCONS_THROW(cbor_decode_error(end_-it_));
                }
                else
                {
                    it_ = endp;
                }
                return get_fixed_length_array(len);
            }

            // array (two-byte uint16_t for n follow)
        case 0x99:
            {
                const uint8_t* endp;
                const auto len = binary::from_big_endian<uint16_t>(it_,end_,&endp);
                if (endp == it_)
                {
                    JSONCONS_THROW(cbor_decode_error(end_-it_));
                }
                else
                {
                    it_ = endp;
                }
                return get_fixed_length_array(len);
            }

            // array (four-byte uint32_t for n follow)
        case 0x9a:
            {
                const uint8_t* endp;
                const auto len = binary::from_big_endian<int32_t>(it_,end_,&endp);
                if (endp == it_)
                {
                    JSONCONS_THROW(cbor_decode_error(end_-it_));
                }
                else
                {
                    it_ = endp;
                }
                return get_fixed_length_array(len);
            }

            // array (eight-byte uint64_t for n follow)
        case 0x9b:
            {
                const uint8_t* endp;
                const auto len = binary::from_big_endian<int64_t>(it_,end_,&endp);
                if (endp == it_)
                {
                    JSONCONS_THROW(cbor_decode_error(end_-it_));
                }
                else
                {
                    it_ = endp;
                }
                return get_fixed_length_array(len);
            }

            // array (indefinite length)
        case 0x9f:
            {
                Json result = typename Json::array();
                while (*pos != 0xff)
                {
                    result.push_back(decode());
                    pos = it_;
                }
                return result;
            }

            // map (0x00..0x17 pairs of data items follow)
        case JSONCONS_CBOR_0xa0_0xb7:
            {
                return get_fixed_length_map(*pos & 0x1f);
            }

            // map (one-byte uint8_t for n follows)
        case 0xb8:
            {
                const uint8_t* endp;
                const auto len = binary::from_big_endian<uint8_t>(it_,end_,&endp);
                if (endp == it_)
                {
                    JSONCONS_THROW(cbor_decode_error(end_-it_));
                }
                else
                {
                    it_ = endp;
                }
                return get_fixed_length_map(len);
            }

            // map (two-byte uint16_t for n follow)
        case 0xb9:
            {
                const uint8_t* endp;
                const auto len = binary::from_big_endian<uint16_t>(it_,end_,&endp);
                if (endp == it_)
                {
                    JSONCONS_THROW(cbor_decode_error(end_-it_));
                }
                else
                {
                    it_ = endp;
                }
                return get_fixed_length_map(len);
            }

            // map (four-byte uint32_t for n follow)
        case 0xba:
            {
                const uint8_t* endp;
                const auto len = binary::from_big_endian<uint32_t>(it_,end_,&endp);
                if (endp == it_)
                {
                    JSONCONS_THROW(cbor_decode_error(end_-it_));
                }
                else
                {
                    it_ = endp;
                }
                return get_fixed_length_map(len);
            }

            // map (eight-byte uint64_t for n follow)
        case 0xbb:
            {
                const uint8_t* endp;
                const auto len = binary::from_big_endian<uint64_t>(it_,end_,&endp);
                if (endp == it_)
                {
                    JSONCONS_THROW(cbor_decode_error(end_-it_));
                }
                else
                {
                    it_ = endp;
                }
                return get_fixed_length_map(len);
            }

            // map (indefinite length)
        case 0xbf:
            {
                Json result = typename Json::object();
                while (*pos != 0xff)
                {
                    auto j = decode();
                    result.set(j.as_string_view(),decode());
                    pos = it_;
                }
                return result;
            }

            // False
        case 0xf4:
            {
                return Json(false);
            }

            // True
        case 0xf5:
            {
                return Json(true);
            }

            // Null
        case 0xf6:
            {
                return Json::null();
            }

            
        case 0xf9: // Half-Precision Float (two-byte IEEE 754)
            // FALLTHRU
        case 0xfa: // Single-Precision Float (four-byte IEEE 754)
            // FALLTHRU
        case 0xfb: // Double-Precision Float (eight-byte IEEE 754)
            {
                const uint8_t* endp;
                double val = detail::get_double(pos,end_,&endp);
                if (endp == pos)
                {
                    JSONCONS_THROW(cbor_decode_error(endp-begin_));
                }
                it_ = endp;
                return Json(val);
            }

        default:
            {
                JSONCONS_THROW(cbor_decode_error(end_-pos));
            }
        }
    }

    template<typename T>
    Json get_fixed_length_array(const T len)
    {
        Json result = typename Json::array();
        result.reserve(len);
        for (T i = 0; i < len; ++i)
        {
            result.push_back(decode());
        }
        return result;
    }

    template<typename T>
    Json get_fixed_length_map(const T len)
    {
        Json result = typename Json::object();
        result.reserve(len);
        for (T i = 0; i < len; ++i)
        {
            auto j = decode();
            result.set(j.as_string_view(),decode());
        }
        return result;
    }
};

template<class Json>
void encode_cbor(const Json& j, std::vector<uint8_t>& v)
{
    size_t n = 0;
    cbor_Encoder_<Json>::encode(j,Calculate_size_(),n);

    v.reserve(n);
    cbor_Encoder_<Json>::encode(j,Encode_cbor_(),v);
}

template<class Json>
Json decode_cbor(const cbor_view& v)
{
    Decode_cbor_<Json> decoder(v.buffer(),v.buffer()+v.buflen());
    return decoder.decode();
}
  
#if !defined(JSONCONS_NO_DEPRECATED)
template<class Json>
std::vector<uint8_t> encode_cbor(const Json& j)
{
    std::vector<uint8_t> v;
    encode_cbor(j, v);
    return v;
}
#endif

}}

#endif
