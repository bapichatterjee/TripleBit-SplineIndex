#pragma once

#include <iterator>
#include <limits>
#include <vector>

#include "builder.h"
#include "radix_spline.h"
#include "serializer.h"
#include <iostream>
using namespace std;
namespace rs {

// A drop-in replacement for std::multimap. Internally creates a sorted copy of
// the data.
template <class KeyType, class ValueType>
class MultiMap {
 public:
  // Member type definitions.
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<KeyType, ValueType>;
  using size_type = std::size_t;
  using iterator = typename std::vector<value_type>::iterator;
  using const_iterator = typename std::vector<value_type>::const_iterator;

  // Constructor, creates a copy of the data.
  template <class BidirIt>
  MultiMap(BidirIt first, BidirIt last, size_t num_radix_bits = 18,
           size_t max_error = 32);

  // Lookup functions, like in std::multimap.
  const_iterator find(KeyType key) const;
  const_iterator lower_bound(KeyType key) const;

  // Iterators.
  const_iterator begin() const { return data_.begin(); }
  const_iterator end() const { return data_.end(); }

  // Size.
  std::size_t size(int cnt=0) const { return data_.size(); }

  std::size_t size_of_rs_() const { 
      // cout << "size_of_RS: "<<rs_.size_of_spline_points() << endl;
    return sizeof(size_t) * 7 + sizeof(KeyType) * (2 + rs_.spline_points_.size()) + sizeof(double) * rs_.spline_points_.size() + sizeof(uint32_t) * rs_.radix_table_.size();
  }
  std::size_t size_of_data_() const {
    return size() * sizeof(value_type) + sizeof(size_t);
  }

  std::size_t total_size() const {
    return size_of_rs_() + size_of_data_();
  }


  char* serialize(char*& buffer) const;
  char* deserialize(char*& buffer) ;
 private:
  RadixSpline<KeyType> rs_;
  public:
  std::vector<value_type> data_;
  friend class SplineIndex;
};

template <class KeyType, class ValueType>
template <class BidirIt>
MultiMap<KeyType, ValueType>::MultiMap(BidirIt first, BidirIt last,
                                       size_t num_radix_bits,
                                       size_t max_error) {
  // Empty spline.
  if (first == last) {
    // cout<<"empty splines"<<endl;
    rs::Builder<KeyType> rsb(std::numeric_limits<KeyType>::min(),
                             std::numeric_limits<KeyType>::max(),
                             num_radix_bits, max_error);
    rs_ = rsb.Finalize();
    return;
  }

  // Copy data and check if sorted.
  bool is_sorted = true;
  KeyType previous_key = first->first;
  for (auto current = first; current != last; ++current) {
    is_sorted &= current->first >= previous_key;
    previous_key = current->first;
    data_.push_back(*current);
  }

  // Sort if necessary.
  if (!is_sorted) {
    std::sort(data_.begin(), data_.end(),
              [](const value_type& lhs, const value_type& rhs) {
                return lhs.first < rhs.first;
              });
  }

  // Create spline builder.
  const auto min_key = data_.front().first;
  const auto max_key = data_.back().first;
  rs::Builder<KeyType> rsb(min_key, max_key, num_radix_bits, max_error);

  // Build the radix spline.
  for (const auto& iter : data_) {
    rsb.AddKey(iter.first);
  }
  rs_ = rsb.Finalize();
  // cout << "CONSTRUCTOR multimap "<<(void*)&rs_ << endl;
}

template <class KeyType, class ValueType>
typename MultiMap<KeyType, ValueType>::const_iterator
MultiMap<KeyType, ValueType>::lower_bound(KeyType key) const {
  SearchBound bound = rs_.GetSearchBound(key);
  return std::lower_bound(data_.begin() + bound.begin,
                          data_.begin() + bound.end, key,
                          [](const value_type& lhs, const KeyType& rhs) {
                            return lhs.first < rhs;
                          });
}

template <class KeyType, class ValueType>
typename MultiMap<KeyType, ValueType>::const_iterator
MultiMap<KeyType, ValueType>::find(KeyType key) const {
  auto iter = lower_bound(key);
  return iter != data_.end() && iter->first == key ? iter : data_.end();
}

template<typename POD>
char* serializeVector(char *&buffer, std::vector<POD> const& v)
{
    // this only works on built in data types (PODs)
    // static_assert(std::is_trivial<POD>::value && std::is_standard_layout<POD>::value,
    //     "Can only serialize POD types with this function");

    // std::ostream os;
    char *original = buffer;
    // decltype(v.size()) size;
    size_t size = v.size();
    // size_t sz = 0;
    // os.write(reinterpret_cast<char const*>(&size), sizeof(size)); sz += sizeof(size);
    // os.write(reinterpret_cast<char const*>(v.data()), v.size() * sizeof(POD)); sz += v.size() * sizeof(POD);
    memcpy(buffer, reinterpret_cast<char const*>(&size), sizeof(size)); buffer += sizeof(size);
    memcpy(buffer, reinterpret_cast<char const*>(v.data()), v.size() * sizeof(POD)); buffer += v.size() * sizeof(POD);
    // std::stringstream ss;
    // ss << os.rdbuf();

    // memcpy(buffer, ss.str().c_str(), sz);
    // buffer += sz;
    return original;
}

template<typename POD>
char* deserializeVector(char *&buffer, std::vector<POD>& v, bool p=0)
{
    // static_assert(std::is_trivial<POD>::value && std::is_standard_layout<POD>::value,
    //     "Can only deserialize POD types with this function");

    decltype(v.size()) size;
    char *original = buffer;
    // is.read(reinterpret_cast<char*>(&size), sizeof(size));
    // v.resize(size);
    // is.read(reinterpret_cast<char*>(v.data()), v.size() * sizeof(POD));
    memcpy(reinterpret_cast<char*>(&size), buffer, sizeof(size)); 
    
    // if(p) {
    //   std::cout << "HELLOOO" << std::endl;
    // }
    buffer += sizeof(size);
    // std::cout << size<<" YO"<<std::endl;
    v.resize(size);
    memcpy(reinterpret_cast<char*>(v.data()), buffer, v.size() * sizeof(POD)); buffer += v.size() * sizeof(POD);
    return original;
}

template <class KeyType, class ValueType>
char* MultiMap<KeyType, ValueType>::serialize(char*& buffer) const
{
  // static char *prev = nullptr;
  
  char* orig = buffer;
  Serializer<KeyType>::ToBytes(rs_, buffer);
  // std::cout<<"saving vector at: "<<(void*)buffer<<std::endl;
  // std::cout << "CHECKING DATA before serialize at: "<<(void*)buffer << std::endl;
  // for(auto p : data_) {
  //   std::cout << p.first<<','<<p.second<<' ';
  // } std::cout << std::endl;
  
  // char *sub2 = buffer;
  // char *sub3 = sub2;
  serializeVector(buffer, data_);

  // {
  //   std::cout << "CHECKING DATA after serialize at: "<<(void*)sub2 << std::endl;
  //   vector<std::pair<KeyType, ValueType> > sub;
  //   deserializeVector(sub2, sub);
  //   for(auto p : sub) {
  //     std::cout << p.first<<','<<p.second<<' ';
  //   } std::cout << std::endl;
  // }

  // if(prev != nullptr){
  //   std::cout << "CHECKING DATA of previous serialize at: "<<(void*)prev << std::endl;
  //   // vector<std::pair<KeyType, ValueType> > sub;
  //   // deserializeVector(prev, sub, 1);
  //   // for(auto p : sub) {
  //   //   std::cout << p.first<<','<<p.second<<' ';
  //   // } std::cout << std::endl;
  // }
  // prev = sub3;

  return orig;
}

template <class KeyType, class ValueType>
char* MultiMap<KeyType, ValueType>::deserialize(char*& buffer) {
  // std::cout << (void*)buffer<<std::endl;
  char *orig = buffer;
  // cout << (void*)&rs_ << endl;
  rs_ = RadixSpline<KeyType> ();
  // auto rs2 = RadixSpline<KeyType> ();
  // cout << "idhar tak2" << endl;
  // std::cout << "MULTI: "<<(void*)buffer << std::endl;
  rs_ = Serializer<KeyType>::FromBytes(buffer);
  // std::cout << "MULTI2: "<<(void*)buffer << std::endl;
  // // std::cout << (void*)buffer<<std::endl;
  deserializeVector(buffer, data_);
  // std::cout << "CHECKING DATA after deserialize at: "<<(void*)buffer<<"\n";
  // for(auto p : data_) {
  //   std::cout << p.first<<','<<p.second<<' ';
  // } std::cout << std::endl;
  return orig;
}
// auto hi = RadixSpline<unsigned>();
}  // namespace rs