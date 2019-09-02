
#define DEBUG

#include <iostream>
#include <cassert>
#include "BPlusTreeBase.hpp"



using namespace std;


void testBase(){
    BPlusTreeBase<int,string> base(10);
    base.insert(make_pair(1,"wtf"));
    base.insert(make_pair(5,"test"));
    base.insert(make_pair(15,"wtf"));
    base.insert(make_pair(16,"alola"));
    base.insert(make_pair(16,"not"));
    base.insert(make_pair(1,"not"));
    
    int cc = 4;

	for(int i=100;i<146;i++){
		base.insert(make_pair(i, "wtfi"+to_string(i)));
		cc++;
	}
	
	for(int i=50;i<55;i++){
		base.insert(make_pair(i, "wtfi"+to_string(i)));
		cc++;
	}
	
	
	assert(base.size() == cc);
	
	base.print_tree();

    base.erase(125);
    cc--;
    
    assert(base.size() == cc);
    
    base.print_tree();
    
    assert(base.find(15).get_value() == "wtf");
    assert(base.find(145).get_value() == "wtfi145");
    
    for(int i=1000;i>500;i--){
		base.insert(make_pair(i, "lola"+to_string(i*i)));
		cc++;
	}
	
	assert(base.size() == cc);
	
	base.print_tree();
	
	for(int i=600;i<900;i++){
		base.erase(i);
		cc--;
	}
	
	assert(base.size() == cc);
	
	base.print_tree();
	
	for(int i=1000;i>20;i--){
		base.erase(i);
	}
	
	cc = 4;
	
	assert(base.size() == cc);
	
	base.print_tree();
	
	base.erase(1);
	base.erase(5);
	base.erase(16);

	assert(base.size() == 1);
	
	base.erase(15);
	
	assert(base.size() == 0);
	
	base.insert(make_pair(21, "heh"));
	
	assert(base.size() == 1);
	
	base.print_tree();

	base.erase(21);
	
	
	cc = 0;
	for(int i=10;i<100;i++){
		base.insert(make_pair(i, "str"));
		cc++;
	}
	
	assert(base.end() == base.end());
	assert(base.begin() != base.end());
	
	BPlusTreeBase<int,string>::iterator it = base.find(35);
	
	//cout << *it << endl;
	
	cout << it->first << " " << it->second << endl;
	it->second = "lola";
	
	it = base.find(35);
	assert(it->second == "lola");
	
	
	it = base.find(99);
	assert(++it == base.end());
	
	
	
	int hh = 0;
	for(auto &it : base){
		assert(it.first == hh+10);
		//cout << it.first << " " << it.second << endl;
		hh++;
	}
	assert(hh == cc);
	
	
	auto it2 = base.find(99);
	assert((*it2).first == 99 && (*it2).second == "str");
	
	
	assert(base[88] == "str");
	
    cout << "=================================================" << endl << "TESTS FOR BPlusTreeBase.hpp FINISHED SUCCESSFULLY" << endl << "=================================================" << endl;
    return;
}
