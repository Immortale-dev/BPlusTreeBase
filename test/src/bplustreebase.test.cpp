#include <iostream>
#include <cassert>
#include "BPlusTreeBase.hpp"

using namespace std;

int testBase(){
    BPlusTreeBase<int,string> base(5);
    base.insert(make_pair(1,"wtf"));
    assert(base.size() == 1);
    base.erase(1);
    assert(base.size() == 0);
    cout << "TESTS FOR BPlusTreeBase.hpp FINISHED SUCCESSFULLY" << endl;
    return 0;
}
