#include <random>
#include <unordered_set>
#include "include/rs/builder.h"
#include "include/rs/serializer.h"
#include <iterator>
#include <limits>
#include <vector>
#include <iostream>
#include "include/rs/multi_map.h"

using namespace std;

// template <class KeyType>
void testSerialise(){
    char* buffer = (char*) malloc(1024* sizeof(char));
  // Create random keys.
    vector<uint64_t> keys(1e1);
    generate(keys.begin(), keys.end(), rand);
    keys.push_back(8128);
    sort(keys.begin(), keys.end());

    // Build RadixSpline.
    uint64_t min = keys.front();
    uint64_t max = keys.back();
    cout<<"Min: "<<min<<" Max: "<<max<<endl;
    rs::Builder<uint64_t> rsb(min, max);
    for (const auto& key : keys) rsb.AddKey(key);
    rs::RadixSpline<uint64_t> rs_ = rsb.Finalize();
    // cout<<"RadixSpline: "<<rs_<<endl;
    rs::Serializer<uint64_t>::ToBytes(rs_, buffer);

    // Print the serialized string
    cout << "Serialized string: " << buffer << std::endl;
}

int main(int argc, char** argv) {
  testSerialise();

  return 0;
}
