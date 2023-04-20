#include <iostream>
#include <vector>
#include "../include/rs/multi_map.h"
#include "../include/rs/serializer.h"

using namespace std;

int main() {

    // vector<pair<int,int> > v{{3, 3}, {100, 1}, {4, 23}, {5, 1}};
    // char *b1 = (char*)malloc(1000);
    // // cout << (void*)b1 << endl;
    // char* ori = b1;
    // rs::serializeVector(b1, v);
    // // cout << (void*)ori<<' '<<(void*)b1 << endl;
    // vector<pair<int,int> > v2;
    // rs::deserializeVector(ori, v2);
    // for(auto i : v2) {
    //     cout << i.first<<' '<< i.second<< endl;
    // }
    vector<pair<uint64_t, char>> data = {{1ull, 'a'},
                                    {12ull, 'b'},
                                    {7ull, 'c'},  // Unsorted.
                                    {42ull, 'd'}};
    rs::MultiMap<uint64_t, char> map(begin(data), end(data));

    cout << "find(7): '" << map.find(7)->second << "'" << endl;
    cout << "lower_bound(3): '" << map.lower_bound(3)->second << "'" << endl;

    // for(auto i : map.data_) {
    //     cout << i.first<<' '<<i.second << '\n';
    // } cout << endl;

    char *buf = (char*)malloc(1000);
    char* orig = buf;
    // rs::Serializer<uint64_t>::ToBytes(map.rs_, buf);
    map.serialize(buf);
    vector<pair<uint64_t, char>> dummy_data = {{0ull, 'a'}};
    rs::MultiMap<uint64_t, char> new_map(dummy_data.begin(),dummy_data.end());
    // auto RS = rs::Serializer<uint64_t>::FromBytes(orig);
    // for(auto i : RS.radix_table_) {
    //     cout << i << ' ';
    // } cout << endl;
    buf = orig;
    // vector<pair<uint64_t, char>> dummy_data = {{0ull, 'a'}};
    // rs::MultiMap<uint64_t, char> new_map(dummy_data.begin(),dummy_data.end());
    new_map.deserialize(buf);
    cout << "find(7): '" << new_map.find(7)->second << "'" << endl;
    cout << "lower_bound(3): '" << new_map.lower_bound(3)->second << "'" << endl;

    // for(auto i : new_map.data_) {
    //     cout << i.first<<' '<<i.second << '\n';
    // } cout << endl;
    return 0;
}

