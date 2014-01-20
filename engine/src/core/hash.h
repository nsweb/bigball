#ifndef BB_COREHASH_H
#define BB_COREHASH_H

namespace bigball
{

template<typename T> class Hash;

template<> class Hash<int8_t>   { public: uint32_t operator()(int8_t) const; };
template<> class Hash<uint8_t>  { public: uint32_t operator()(uint8_t) const; };
template<> class Hash<int16_t>  { public: uint32_t operator()(int16_t) const; };
template<> class Hash<uint16_t> { public: uint32_t operator()(uint16_t) const; };
template<> class Hash<int32_t>  { public: uint32_t operator()(int32_t) const; };
template<> class Hash<uint32_t> { public: uint32_t operator()(uint32_t) const; };
template<> class Hash<int64_t>  { public: uint32_t operator()(int64_t) const; };
template<> class Hash<uint64_t> { public: uint32_t operator()(uint64_t) const; };

//template<> class Hash<half>     { public: uint32_t operator()(half) const; };
template<> class Hash<float>    { public: uint32_t operator()(float) const; };
template<> class Hash<double>   { public: uint32_t operator()(double) const; };
//template<> class Hash<ldouble>  { public: uint32_t operator()(ldouble) const; };

template<> class Hash<char const *>
{
public:
    BIGBALL_API uint32_t operator()(char const *x) const;
    BIGBALL_API uint32_t operator()(String const &s) const;
};

template<> class Hash<String>
{
public:
    BIGBALL_API uint32_t operator()(String const &s) const;
    BIGBALL_API uint32_t operator()(char const *x) const;
};

} /* namespace bigball */

#endif // BB_COREHASH_H

