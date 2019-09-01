
#define DEBUG

#include <iostream>
#include <cassert>
#include "BPlusTreeBase.hpp"



using namespace std;


void testBase(){
    BPlusTreeBase<int,string> base(50);
    base.insert(make_pair(1,"wtf"));
    base.insert(make_pair(5,"test"));
    base.insert(make_pair(15,"wtf"));
    base.insert(make_pair(16,"alola"));
    base.insert(make_pair(16,"not"));
    base.insert(make_pair(1,"not"));
    
    int cc = 4;
    
    int a;
    cout << "PUT INT" << endl;
    cin >> a;
    

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
	
	//////////////////////////////////////
	
	for(int i=0;i<1000000;i++){
		base.insert(make_pair(i, "str"+to_string(i)));
	}
	
	assert(base.size() == 1000000);
	
	cout << "PUT INT" << endl;
	cin >> a;
	
	for(int i=0;i<1000000;i++){
		base.erase(i);
	}
	
	cout << "PUT INT" << endl;
	cin >> a;
    
    cout << " TESTS FOR BPlusTreeBase.hpp FINISHED SUCCESSFULLY" << endl;
    return;
}
