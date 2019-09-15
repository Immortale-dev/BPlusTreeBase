
#define DEBUG

#include <iostream>
#include <cassert>
#include "BPlusTreeBase.hpp"



using namespace std;


void testBase(){
	
	
	
    BPlusTreeBase<int,string> base(3);
    
    auto itno = base.begin();
    auto itno2 = base.end();
    
    assert(itno++ == itno2);
    assert(itno == itno2);
    
    pair<int,string> lval = make_pair(17,"lol");
    base.insert(lval);
    
    base.insert(make_pair(1,"wtf"));
    base.insert(make_pair(5,"test"));
    base.insert(make_pair(15,"wtf"));
    base.insert(make_pair(16,"alola"));
    base.insert(make_pair(16,"not"));
    base.insert(make_pair(1,"not"));
    
    auto itl = base.end();
    itl--;
    assert(itl->second == "lol");
    
    auto itf = base.end();
    ++itf;
    assert(itf == base.begin());
    
    
    int cc = 5;

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
	
	cc = 5;
	
	assert(base.size() == cc);
	
	base.print_tree();
	
	base.erase(1);
	base.erase(5);
	base.erase(16);

	assert(base.size() == 2);
	
	base.erase(15);
	base.erase(17);
	
	assert(base.size() == 0);
	
	base.insert(make_pair(21, "heh"));
	
	assert(base.size() == 1);
	
	base.print_tree();

	base.erase(21);
	
	
	cc = 0;
	
	
	// Test structure
	vector<int> result;
	vector<int> compare;
	
	// T1
	base.insert(make_pair(1,"1"));
	base.insert(make_pair(2,"2"));
	base.insert(make_pair(3,"3"));
	base.insert(make_pair(4,"4"));
	base.insert(make_pair(5,"5"));
	base.insert(make_pair(6,"6"));
	
	result = base.bfs_result();
	compare = {1,4,3,1,2,3,3,4,5,6};

	assert(result.size() == compare.size());
	for(size_t i=0;i<result.size();i++){
		assert(result[i] == compare[i]);
	}

	//T2
	base.insert(make_pair(17,"17"));
	base.insert(make_pair(18,"18"));
	base.insert(make_pair(19,"19"));
	
	result = base.bfs_result();
	compare = {2,4,17,3,1,2,3,3,4,5,6,3,17,18,19};
	
	assert(result.size() == compare.size());
	for(size_t i=0;i<result.size();i++){
		assert(result[i] == compare[i]);
	}
	
	//T3
	base.erase(18);
	
	result = base.bfs_result();
	compare = {1,4,3,1,2,3,5,4,5,6,17,19};
	
	assert(result.size() == compare.size());
	for(size_t i=0;i<result.size();i++){
		assert(result[i] == compare[i]);
	}
	
	//T4
	base.insert(make_pair(18,"18"));
	base.insert(make_pair(10,"10"));
	
	base.erase(19);
	
	result = base.bfs_result();
	compare = {2,4,10,3,1,2,3,3,4,5,6,3,10,17,18};
	
	assert(result.size() == compare.size());
	for(size_t i=0;i<result.size();i++){
		assert(result[i] == compare[i]);
	}
	

	

	// Clear
	for(int i=0;i<100;i++)
		base.erase(i);
	//////////////
	
	
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
