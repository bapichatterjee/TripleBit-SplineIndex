#pragma once

#include <sstream>
#include <iostream>
#include "radix_spline.h"
#include "string.h"
using namespace std;

namespace rs {

template <class KeyType>
class Serializer {
 public:
  // Serializes the `rs` model and appends it to `bytes`.
  // static void ToBytes(const RadixSpline<KeyType>& rs, std::string* bytes) {
  //   std::stringstream buffer;

  //   // Scalar members.
  //   buffer.write(reinterpret_cast<const char*>(&rs.min_key_), sizeof(KeyType));
  //   buffer.write(reinterpret_cast<const char*>(&rs.max_key_), sizeof(KeyType));
  //   buffer.write(reinterpret_cast<const char*>(&rs.num_keys_), sizeof(size_t));
  //   buffer.write(reinterpret_cast<const char*>(&rs.num_radix_bits_),
  //                sizeof(size_t));
  //   buffer.write(reinterpret_cast<const char*>(&rs.num_shift_bits_),
  //                sizeof(size_t));
  //   buffer.write(reinterpret_cast<const char*>(&rs.max_error_), sizeof(size_t));

  //   // Radix table.
  //   const size_t radix_table_size = rs.radix_table_.size();
  //   buffer.write(reinterpret_cast<const char*>(&radix_table_size),
  //                sizeof(size_t));
  //   for (size_t i = 0; i < rs.radix_table_.size(); ++i) {
  //     buffer.write(reinterpret_cast<const char*>(&rs.radix_table_[i]),
  //                  sizeof(uint32_t));
  //   }

  //   // Spline points.
  //   const size_t spline_points_size = rs.spline_points_.size();
  //   buffer.write(reinterpret_cast<const char*>(&spline_points_size),
  //                sizeof(size_t));
  //   for (size_t i = 0; i < rs.spline_points_.size(); ++i) {
  //     buffer.write(reinterpret_cast<const char*>(&rs.spline_points_[i].x),
  //                  sizeof(KeyType));
  //     buffer.write(reinterpret_cast<const char*>(&rs.spline_points_[i].y),
  //                  sizeof(double));
  //   }

  //   bytes->append(buffer.str());
  // }

  static void ToBytes(const RadixSpline<KeyType>& rs,char*& bytes) {
    std::ostringstream buffer;

    // Scalar members.
    size_t sz = 0;
    size_t size_to_serialise = sizeof(size_t) * 6 + sizeof(KeyType) * (2 + rs.spline_points_.size()) + sizeof(double) * rs.spline_points_.size() + sizeof(uint32_t) * rs.radix_table_.size();

    // cout<<"Size to serialise (ToBytes): "<<size_to_serialise<<endl;
    // cout<<"Address of bytes (ToBytes):"<<(void *)bytes<<endl; 

    buffer.write(reinterpret_cast<const char*>(&size_to_serialise),
                 sizeof(size_t)); sz += sizeof(size_t);
    buffer.write(reinterpret_cast<const char*>(&rs.min_key_), sizeof(KeyType)); sz += sizeof(KeyType);
    buffer.write(reinterpret_cast<const char*>(&rs.max_key_), sizeof(KeyType)); sz += sizeof(KeyType);
    buffer.write(reinterpret_cast<const char*>(&rs.num_keys_), sizeof(size_t)); sz += sizeof(size_t);
    buffer.write(reinterpret_cast<const char*>(&rs.num_radix_bits_),
                 sizeof(size_t)); sz += sizeof(size_t);
    buffer.write(reinterpret_cast<const char*>(&rs.num_shift_bits_),
                 sizeof(size_t)); sz += sizeof(size_t);
    buffer.write(reinterpret_cast<const char*>(&rs.max_error_), sizeof(size_t)); sz += sizeof(size_t);

    // Radix table.
    const size_t radix_table_size = rs.radix_table_.size();
    buffer.write(reinterpret_cast<const char*>(&radix_table_size),
                 sizeof(size_t)); sz += sizeof(size_t);

    for (size_t i = 0; i < rs.radix_table_.size(); ++i) {
      buffer.write(reinterpret_cast<const char*>(&rs.radix_table_[i]),
                   sizeof(uint32_t)); sz += sizeof(uint32_t);
    }

    // Spline points.
        const size_t spline_points_size = rs.spline_points_.size();
    buffer.write(reinterpret_cast<const char*>(&spline_points_size),
                 sizeof(size_t)); sz += sizeof(size_t);
    for (size_t i = 0; i < rs.spline_points_.size(); ++i) {
      buffer.write(reinterpret_cast<const char*>(&rs.spline_points_[i].x),
                   sizeof(KeyType)); sz += sizeof(KeyType);
      buffer.write(reinterpret_cast<const char*>(&rs.spline_points_[i].y),
                   sizeof(double)); sz += sizeof(double);
    }
    memcpy(bytes, buffer.str().c_str(), sz);
    // comment this line if testing toBytes function individually for ease of testing
    bytes += sz;
    // cout<<"Address of bytes after (ToBytes):"<<(void *)bytes<<endl; 



  }

  // static RadixSpline<KeyType> FromBytes(const std::string& bytes) {
  //   std::istringstream in(bytes);

  //   RadixSpline<KeyType> rs;

  //   // Scalar members.
  //   in.read(reinterpret_cast<char*>(&rs.min_key_), sizeof(KeyType));
  //   in.read(reinterpret_cast<char*>(&rs.max_key_), sizeof(KeyType));
  //   in.read(reinterpret_cast<char*>(&rs.num_keys_), sizeof(size_t));
  //   in.read(reinterpret_cast<char*>(&rs.num_radix_bits_), sizeof(size_t));
  //   in.read(reinterpret_cast<char*>(&rs.num_shift_bits_), sizeof(size_t));
  //   in.read(reinterpret_cast<char*>(&rs.max_error_), sizeof(size_t));

  //   // Radix table.
  //   size_t radix_table_size;
  //   in.read(reinterpret_cast<char*>(&radix_table_size), sizeof(size_t));
  //   rs.radix_table_.resize(radix_table_size);
  //   for (int i = 0; i < rs.radix_table_.size(); ++i) {
  //     in.read(reinterpret_cast<char*>(&rs.radix_table_[i]), sizeof(uint32_t));
  //   }

  //   // Spline points.
  //   size_t spline_points_size;
  //   in.read(reinterpret_cast<char*>(&spline_points_size), sizeof(size_t));
  //   rs.spline_points_.resize(spline_points_size);
  //   for (int i = 0; i < rs.spline_points_.size(); ++i) {
  //     in.read(reinterpret_cast<char*>(&rs.spline_points_[i].x),
  //             sizeof(KeyType));
  //     in.read(reinterpret_cast<char*>(&rs.spline_points_[i].y), sizeof(double));
  //   }

  //   return rs;
  // }

  static RadixSpline<KeyType> FromBytes(char*& bytes) {

    size_t size_to_serialise = 0;
    RadixSpline<KeyType> rs;
    // rs = RadixSpline<KeyType>();
    memcpy(reinterpret_cast<char*>(&size_to_serialise), bytes, sizeof(size_t)); bytes += sizeof(size_t);
    // cout<<"Address of bytes "<<&bytes<<endl; 
    // cout<<"Size to serialise (FromBytes): "<<size_to_serialise<<endl;
    // cout<<"Address of bytes (FromBytes):"<<(void *)bytes<<endl; 


    //TODO: check size
    char* buffer = (char*) malloc(size_to_serialise);
    
    memcpy(reinterpret_cast<char*>(buffer), bytes, size_to_serialise); 
    memcpy(reinterpret_cast<char*>(&rs.min_key_), bytes,  sizeof(KeyType)); bytes+=sizeof(KeyType);
    // cout << "ITTHE AA GYA" << endl;
    memcpy(reinterpret_cast<char*>(&rs.max_key_), bytes,  sizeof(KeyType)); bytes+=sizeof(KeyType);
    memcpy(reinterpret_cast<char*>(&rs.num_keys_), bytes,  sizeof(size_t)); bytes+=sizeof(size_t);
    memcpy(reinterpret_cast<char*>(&rs.num_radix_bits_), bytes,  sizeof(size_t)); bytes+=sizeof(size_t);
    memcpy(reinterpret_cast<char*>(&rs.num_shift_bits_), bytes,  sizeof(size_t)); bytes+=sizeof(size_t);
    memcpy(reinterpret_cast<char*>(&rs.max_error_), bytes,  sizeof(size_t)); bytes+=sizeof(size_t);
    
    // std::istringstream in(buffer);
  // Scalar members.

// cout<<"pls "<<endl;
    // Radix table.
    size_t radix_table_size;
    memcpy(reinterpret_cast<char*>(&radix_table_size), bytes,  sizeof(size_t)); bytes+=sizeof(size_t);

    rs.radix_table_.resize(radix_table_size);

    for (int i = 0; i < rs.radix_table_.size(); ++i) {
      memcpy(reinterpret_cast<char*>(&rs.radix_table_[i]), bytes,  sizeof(uint32_t)); bytes+=sizeof(uint32_t);
    }

    // Spline points.
    size_t spline_points_size;
    memcpy(reinterpret_cast<char*>(&spline_points_size), bytes,  sizeof(size_t)); bytes+=sizeof(size_t);

    rs.spline_points_.resize(spline_points_size);
    for (int i = 0; i < rs.spline_points_.size(); ++i) {

      memcpy(reinterpret_cast<char*>(&rs.spline_points_[i].x), bytes,  sizeof(KeyType)); bytes+=sizeof(KeyType);
      memcpy(reinterpret_cast<char*>(&rs.spline_points_[i].y), bytes,  sizeof(double)); bytes+=sizeof(double);
    }

// cout<<"bhai yaar "<<(void*)&rs<<endl;
    // cout<<"Address of bytes after (FromBytes):"<<(void *)bytes<<endl;  

    return rs;
  }
};

}  // namespace rs
