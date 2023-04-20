// #include <iostream>

// #include "include/rs/multi_map.h"

// using namespace std;

// void RadixSplineExample() {
//   // Create random keys.
//   vector<uint64_t> keys(1e6);
//   generate(keys.begin(), keys.end(), rand);
//   keys.push_back(8128);
//   sort(keys.begin(), keys.end());

//   // Build RadixSpline.
//   uint64_t min = keys.front();
//   uint64_t max = keys.back();
//   rs::Builder<uint64_t> rsb(min, max);
//   for (const auto& key : keys) rsb.AddKey(key);
//   rs::RadixSpline<uint64_t> rs = rsb.Finalize();

//   // Search using RadixSpline.
//   rs::SearchBound bound = rs.GetSearchBound(8128);
//   cout << "The search key is in the range: [" << bound.begin << ", "
//        << bound.end << ")" << endl;
//   auto start = begin(keys) + bound.begin, last = begin(keys) + bound.end;
//   cout << "The key is at position: "
//        << std::lower_bound(start, last, 8128) - begin(keys) << endl;
// }

// void MultiMapExample() {
//   vector<pair<uint64_t, char>> data = {{1ull, 'a'},
//                                        {12ull, 'b'},
//                                        {7ull, 'c'},  // Unsorted.
//                                        {42ull, 'd'}};
//   rs::MultiMap<uint64_t, char> map(begin(data), end(data));

//   cout << "find(7): '" << map.find(7)->second << "'" << endl;
//   cout << "lower_bound(3): '" << map.lower_bound(3)->second << "'" << endl;
// }

// int main(int argc, char** argv) {
//   RadixSplineExample();
//   MultiMapExample();

//   return 0;
// }

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
    char* buffer = (char*) malloc(1024 * 1024 * sizeof(char));
  // Create random keys.
    vector<uint64_t> keys(1e1);
    generate(keys.begin(), keys.end(), rand);
    keys.push_back(8128);
    keys.push_back(2016788921);
    cout<<"Keys---------"<<endl;
    sort(keys.begin(), keys.end());
    for(int i=0;i<keys.size();i++){
      cout<<keys[i]<<endl;
    }
    // Build RadixSpline.
    uint64_t min = keys.front();
    uint64_t max = keys.back();
    cout<<"Min: "<<min<<" Max: "<<max<<endl;
    rs::Builder<uint64_t> rsb(min, max);
    for (const auto& key : keys) rsb.AddKey(key);
    rs::RadixSpline<uint64_t> rs_ = rsb.Finalize();
      // Search using RadixSpline.
  rs::SearchBound bound = rs_.GetSearchBound(2016788921);

  cout << "The search key is in the range: [" << bound.begin << ", "
       << bound.end << ")" << endl;
  auto start = begin(keys) + bound.begin, last = begin(keys) + bound.end;
  cout << "The key is at position: "
       << std::lower_bound(start, last, 2016788921) - begin(keys) << endl;

    // cout<<"RadixSpline: "<<rs_<<endl;

    rs::Serializer<uint64_t>::ToBytes(rs_, buffer);
    rs::RadixSpline<uint64_t> rs_deserialised = rs::RadixSpline<uint64_t>();
    rs_deserialised = rs::Serializer<uint64_t>::FromBytes(buffer);

      // Search using RadixSpline.
  rs::SearchBound bound2 = rs_deserialised.GetSearchBound(2016788921);
  cout << "The search key is in the range: [" << bound2.begin << ", "
       << bound2.end << ")" << endl;
  auto start2 = begin(keys) + bound2.begin, last2 = begin(keys) + bound2.end;
  cout << "The key is at position: "
       << std::lower_bound(start2, last2, 2016788921) - begin(keys) << endl;
}

int main(int argc, char** argv) {
  testSerialise();
  return 0;
}
