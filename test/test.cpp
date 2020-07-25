//#define TEST_ONLY_RULE
#include "qtest.hpp"

#define DEBUG

#include <iostream>
#include <cassert>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <chrono>
#include <unordered_set>
#include "BPlusTreeBase.hpp"


using namespace std;

#include "bplustreebaseleafnode.test.cpp"
#include "bplustreebaseinternalnode.test.cpp"
#include "bplustreebase.test.cpp"
#include "bplustreebasebranch.test.cpp"
#include "bplustreebase_multithreading.test.cpp"

#include <iostream>

int main(){ return 0; }
