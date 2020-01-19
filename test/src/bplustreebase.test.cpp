
#define DEBUG

#include <iostream>
#include <cassert>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <unordered_set>
#include "BPlusTreeBase.hpp"

using namespace std;


DESCRIBE("[BPlusTreeBase.hpp] Given empty tree", {
	
	srand (time(NULL));
	auto base = BPlusTreeBase<int,string>(3);
		
	DESCRIBE("w/o doing anything", {
		auto itno = base.begin();
		auto itno2 = base.end();
		
		IT("begin() should be equal end()", {
			EXPECT(itno).toBe(itno2);
		});
		IT("next to begin() should be equal end()", {
			auto nxt = itno++;
			EXPECT(nxt).toBe(itno2);
		});
		IT("prev to begin() should be equal end()", {
			auto prv = base.begin();
			EXPECT(--prv).toBe(itno2);
		});
	});
	
	DESCRIBE("Add 4 items", {
		BEFORE_ALL({
			base.insert(make_pair(1,"wtf"));
			base.insert(make_pair(5,"test"));
			base.insert(make_pair(15,"wtf"));
			base.insert(make_pair(16,"alola"));
		});
		
		DESCRIBE("Check iterators", {
			IT("third from begin() iterator should be equal to --end()", {
				auto b = base.begin();
				++b; ++b; ++b;
				EXPECT(b).toBe(--base.end());
			});
		});
		
		DESCRIBE("Insert one more unique lvalue item to the tree", {
			pair<int,string> lval = make_pair(17,"lol");
			base.insert(lval);
			
			IT("Size of tree should be equal 5", {
				EXPECT(base.size()).toBe(5);
				INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
			});
		});
		
		DESCRIBE("Insert few items with key that already exists in tree", {
			BEFORE_EACH({				
				base.insert(make_pair(16,"not"));
				base.insert(make_pair(1,"not"));
			});
			IT("Size of tree should be equal 5", {
				EXPECT(base.size()).toBe(5);
				INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
			});
		});
		
		DESCRIBE("Get base.end()", {
			
			//BPlusTreeBase<int,string>::iterator itl;
			auto itl = base.end();
			
			BEFORE_EACH({
				itl = base.end();
			});
			
			DESCRIBE("Decrement iterator", {
				BEFORE_EACH({
					--itl;
				});
				IT("Value should be equal to `lol`", {
					EXPECT(itl->second).toBe("lol");
					INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
				});
			});
			
			DESCRIBE("Increment iterator", {
				BEFORE_EACH({		
					itl++;
				});
				IT("Iterator should be equal to base.begin()", {
					EXPECT(itl).toBe(base.begin());
					INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
				});
			});
		});
		
		DESCRIBE("Add more items", {
			int cc = 5;
			
			BEFORE_ALL({
				for(int i=100;i<146;i++){
					base.insert(make_pair(i, "wtfi"+to_string(i)));
					cc++;
				}
				
				for(int i=50;i<55;i++){
					base.insert(make_pair(i, "wtfi"+to_string(i)));
					cc++;
				}
			});
			
			AFTER_ALL({
				for(int i=1000;i>20;i--){
					base.erase(i);
				}
			});
			
			IT("Expect base.size to be equal `cc`", {
				EXPECT(base.size()).toBe(cc);
				INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
				INFO_PRINT("Reserved: "+to_string(base.reserved_count));
			});

			IT("Value of item with `key` 15 to be `wtf`", {
				EXPECT(base.find(15).get_value()).toBe("wtf");
				INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
				INFO_PRINT("Reserved: "+to_string(base.reserved_count));
			});
			
			IT("Value of item with `key` 145 to be wtfi145", {
				EXPECT(base.find(145).get_value()).toBe("wtfi145");
				INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
				INFO_PRINT("Reserved: "+to_string(base.reserved_count));
			});
			
			DESCRIBE("Remove item with `key` 125", {
				BEFORE_ALL({
					base.erase(125);
					cc--;
				});
				
				IT("base.size() should be as expected", {
					EXPECT(base.size()).toBe(cc);
					INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
					INFO_PRINT("Reserved: "+to_string(base.reserved_count));
				});
			});
			
			DESCRIBE("Add 500 more items in reverse versa style (from 1000 to 501)", {
				BEFORE_ALL({
					for(int i=1000;i>500;i--){
						base.insert(make_pair(i, "lola"+to_string(i*i)));
						cc++;
					}
				});
				
				IT("size of three should be expected", {
					EXPECT(base.size()).toBe(cc);
					INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
				});
				
				DESCRIBE("Remove items in interval [600,900)", {
					BEFORE_ALL({
						for(int i=600;i<900;i++){
							base.erase(i);
							cc--;
						}
					});
					
					IT("size of three should be expected", {
						EXPECT(base.size()).toBe(cc);
						INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
					});
				});
			});
		});
		
		DESCRIBE("Remove items with keys 1,5 and 16", {
			BEFORE_ALL({
				base.erase(1);
				base.erase(5);
				base.erase(16);
			});
			IT("Size of tree should be equal to 2", {
				EXPECT(base.size()).toBe(2);
				INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
			});
			
			DESCRIBE("Remove items with keys 15 and 17", {
				
				BEFORE_ALL({
					base.erase(15);
					base.erase(17);
				});
				
				IT("Tree should be empty",{
					EXPECT(base.size()).toBe(0);
					INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
				});
			});
		});
	}); 
	
	DESCRIBE("Insert item as rvalue", {
		
		BEFORE_ALL({			
			base.insert(make_pair(21, "heh"));
		});
		
		AFTER_ALL({
			base.erase(21);
		});
		
		IT("Tree should contains 1 item", {
			EXPECT(base.size()).toBe(1);
			INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
		});
	});
	
	DESCRIBE("Insert 6 items", {
		BEFORE_ALL({
			base.insert(make_pair(1,"1"));
			base.insert(make_pair(2,"2"));
			base.insert(make_pair(3,"3"));
			base.insert(make_pair(4,"4"));
			base.insert(make_pair(5,"5"));
			base.insert(make_pair(6,"6"));
		});
		AFTER_ALL({
			//Clear tree
			for(int i=0;i<100;i++)
				base.erase(i);
		});
		
		IT("Tree structure should be equal to `{1,4,3,1,2,3,3,4,5,6}`", {
			/**
			 *       4 
			 *     /   \
			 * 1,2,3   4,5,6
			 */
			EXPECT(base.bfs_result()).toBeIterableEqual({1,4,3,1,2,3,3,4,5,6});
		});
		
		DESCRIBE("INSERT 3 more items (17,18 and 19)", {
			BEFORE_ALL({
				base.insert(make_pair(17,"17"));
				base.insert(make_pair(18,"18"));
				base.insert(make_pair(19,"19"));
			});
			IT("Then one more leaf should be created and tree structure should be equal to {2,4,17,3,1,2,3,3,4,5,6,3,17,18,19}", {
				/**
				 *          4,     17
				 *       /      |       \
				 *    1,2,3   4,5,6    17,18,19
				 */
				EXPECT(base.bfs_result()).toBeIterableEqual({2,4,17,3,1,2,3,3,4,5,6,3,17,18,19});
				INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
			});
			
			DESCRIBE("And remove item with key=18", {
				BEFORE_ALL({
					base.erase(18);
				});
				AFTER_ALL({
					base.insert(make_pair(18,"18"));
				});
				IT("Then last leaf should be joined to previous one, and structure to be equal to {1,4,3,1,2,3,5,4,5,6,17,19}", {
					/**
					 *          4     
					 *       /     \
					 *    1,2,3   4,5,6,17,19
					 */
					EXPECT(base.bfs_result()).toBeIterableEqual({1,4,3,1,2,3,5,4,5,6,17,19});
					INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
				});
			});
			
			DESCRIBE("And add item with key=10, and remove item with key=19", {
				BEFORE_ALL({
					base.insert(make_pair(10,"10"));
					base.erase(19);
				});
				IT("Then there should be 3 leafs with 3 items in each, and the structure to be {2,4,10,3,1,2,3,3,4,5,6,3,10,17,18}", {
					EXPECT(base.bfs_result()).toBeIterableEqual({2,4,10,3,1,2,3,3,4,5,6,3,10,17,18});
					INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
				});
			});
		});
	});
	
	DESCRIBE("Add items in interval [10,100)", {
		int cc = 0;
		
		BEFORE_ALL({
			for(int i=10;i<100;i++){
				base.insert(make_pair(i, "str"));
				cc++;
			}
		});
		
		IT("bese.end() should be equal to base.end()", {
			EXPECT(base.end()).toBe(base.end());
			INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
		});
		
		IT("base.begin() should not to be equal to base.end()", {
			EXPECT(base.begin()).NOT().toBe(base.end());
			INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
		});
		
		DESCRIBE("Change item value with key 35 to `lola`", {
			BPlusTreeBase<int,string>::iterator it;
			BEFORE_ALL({
				it = base.find(35);
				it->second = "lola";
			});
			
			IT("Item with key 35 value should be lola", {
				it = base.find(35);
				EXPECT(it->second).toBe("lola");
				INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
			});			
		});
		
		IT("Next item to item with key 99 to be equal to base.end()", {
			auto it = base.find(99);
			EXPECT(++it).toBe(base.end());
			INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
		});
		
		DESCRIBE("Iterate through all the items and save the count value", {
			int hh = 0;
			bool fail=false;
			BEFORE_ALL({
				for(auto &it : base){
					if(it.first != hh+10)
						fail = true;
					hh++;
				}
			});
			
			IT("counter should be equal to `cc`", {
				EXPECT(hh).toBe(cc);
				EXPECT(fail).toBe(false);
				INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
			});
		});
		
		IT("Iterator with key 99 first should be equal to 99 and second to be equal to `str`", {
			auto it2 = base.find(99);
			EXPECT((*it2).first).toBe(99);
			EXPECT((*it2).second).toBe("str");
			INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
		});
		
		IT("base[88] should equal to `str`", {
			EXPECT(base[88]).toBe("str");
			INFO_PRINT("Mutex: "+to_string(base.get_mutex_count()));
		});
	});
	
	DESCRIBE_SKIP("Add 1000000 items", {
		BEFORE_ALL({
			cout << "CHECK " << endl;// << aaa << " " << bbb << endl;
			int a;
			cin >> a;
			for(int i=0;i<1000000;i++){
				base.insert(make_pair(i, "test"));
			}
		});
		
		IT("MEMORY CHECK CHECK", {
			TEST_SUCCEED();
			int a;
			cout << "CHECK " << endl;// << aaa << " " << bbb << endl;
			cin >> a;
		});
		
		AFTER_ALL({
			for(int i=0;i<1000000;i++){
				base.erase(i);
			}
			int a;
			cout << "CHECK " << endl;// << aaa << " " << bbb << endl;
			cin >> a;
		});
	});
	
	DESCRIBE("Insert 1000 items", {
		BEFORE_ALL({
			for(int i=0;i<1000;i++){
				base.insert(make_pair(i, "test"));
			}
		});
		
		DESCRIBE("Test iterator process hooks", {
			
			DESCRIBE("Move through all of the items", {
				BEFORE_ALL({
					auto it = base.begin();
					while(it != base.end()){
						++it;
					}
				});
				
				IT("`reserved_count` should be equal to 0", {
					EXPECT(base.reserved_count).toBe(0);
					INFO_PRINT("reserved_count: "+to_string(base.reserved_count));
				});
			});
			
			DESCRIBE("Move through all the items using ++iterator", {
				
				BEFORE_ALL({
					auto it = base.begin();
					while(it != base.end())
						++it;
				});
				
				IT("`iterator_move_count` should be equal to 0", {
					EXPECT(base.iterator_move_count).toBe(0);
					INFO_PRINT("iterator_move_count: " + to_string(base.iterator_move_count));
				});
			});
			
			DESCRIBE("Move through all the items using iterator++", {
				
				BEFORE_ALL({
					auto it = base.begin();
					while(it != base.end())
						it++;
				});
				
				IT("`iterator_move_count` should be equal to 0", {
					EXPECT(base.iterator_move_count).toBe(0);
					INFO_PRINT("iterator_move_count: " + to_string(base.iterator_move_count));
				});
			});
			
			DESCRIBE("Move through all the items using --iterator", {
				
				BEFORE_ALL({
					auto it = base.end();
					while(it != base.begin()){
						--it;
					}
				});
				
				IT("`iterator_move_count` should be equal to 0", {
					EXPECT(base.iterator_move_count).toBe(0);
					INFO_PRINT("iterator_move_count: " + to_string(base.iterator_move_count));
				});
			});
			
			DESCRIBE("Move through all the items using iterator--", {
				
				BEFORE_ALL({
					auto it = base.end();
					while(it != base.begin()){
						it--;
					}
				});
				
				IT("`iterator_move_count` should be equal to 0", {
					EXPECT(base.iterator_move_count).toBe(0);
					INFO_PRINT("iterator_move_count: " + to_string(base.iterator_move_count));
				});
			});
			
			DESCRIBE("Move through the some items", {
				
				BEFORE_ALL({
					auto it = base.begin();
					int num = 278;
					while(num--)
						it++;
				});
				
				IT("`iterator_move_count` should be equal to 0", {
					EXPECT(base.iterator_move_count).toBe(0);
					INFO_PRINT("iterator_move_count: " + to_string(base.iterator_move_count));
				});
			});
		});
		
		AFTER_ALL({
			for(int i=0;i<1000;i++){
				base.erase(i);
			}
		});
	});
	
	DESCRIBE("Add items from 100 to 1000 and remove from 900 to 10", {
		BPlusTreeBase<int, int> tree(3);
		
		BEFORE_ALL({			
			for(int i=100;i<1000;i++){
				tree.insert(make_pair(i,i));
			}
			for(int i=900;i>=10;i--){
				tree.erase(i);
			}
		});
		
		IT("tree size should be 99", {
			EXPECT(tree.size()).toBe(99);
		});
	});
	
	DESCRIBE("Add item {10,10}", {
		BPlusTreeBase<int, int> tree(3);
		
		BEFORE_ALL({
			tree.insert(make_pair(10,10));
		});
		
		IT("Call insert {10,20} w/o overwriting, value should be equal to 10", {
			tree.insert(make_pair(10,20));
			EXPECT(tree.find(10)->second).toBe(10);
		});
		
		IT("Call insert {10,20} with overwriting, value should be equal to 20", {
			tree.insert(make_pair(10,20), true);
			EXPECT(tree.find(10)->second).toBe(20);
		});
	});
	
	DESCRIBE("Add 100`000 items randomly and remove 100`000 items randomly to both btree and unordered_set", {
		BPlusTreeBase<int,int> tree(50);
		unordered_set<int> tmp;
		
		BEFORE_ALL({
			for(int i=0;i<100000;i++){
				int r = rand();
				tree.insert(make_pair(r,r));
				tmp.insert(r);
			}
			
			for(int i=0;i<100000;i++){
				int r = rand();
				tree.erase(r);
				tmp.erase(r);
			}
		});
		
		IT("Every item from unordere_set should appear in tree", {
			EXPECT(tree.size()).toBe(tmp.size());
			vector<int> v1;
			vector<int> v2;
			for(auto& it : tmp){
				v1.push_back(it);
			}
			for(auto& it : tree){
				v2.push_back(it.first);
				EXPECT(it.first).toBe(it.second);
			}
			sort(v1.begin(), v1.end());
			sort(v2.begin(), v2.end());
			EXPECT(v1).toBeIterableEqual(v2);
			INFO_PRINT("Elements left: " + to_string(tree.size()));
		});
	});
});

DESCRIBE("Given tree with factor=50, Add 100`000 items to the tree and use clear method", {
	BPlusTreeBase<int,int> tree(50);
	BEFORE_ALL({
		for(int i=0;i<100000;i++){
			int r = rand();
			tree.insert(make_pair(r,r));
		}
		tree.clear();
	});
	
	IT("size should be equal to 0", {
		EXPECT(tree.size()).toBe(0);
	});
	
	// Because root node and stem node is still active
	IT("`active_nodes_count` debug global variable should be equal to 2", {
		EXPECT(active_nodes_count).toBe(2); 
		INFO_PRINT("Active nodes count: " + to_string(active_nodes_count));
	});
	
	IT("And debug `mutex_count` should be 0", {
		EXPECT(tree.get_mutex_count()).toBe(0);
		INFO_PRINT("Mutex: "+to_string(tree.get_mutex_count()));
	});
	
	DESCRIBE("Add 100 nodes after clear is done", {
		vector<int> check;
		BEFORE_ALL({
			for(int i=22;i<122;i++){
				tree.insert(make_pair(i*i,i*2));
				check.push_back(i*i);
			}
		});
		
		IT("Tree size should be equal to 100", {
			EXPECT(tree.size()).toBe(100);
		});
		
		IT("Tree should be equal to `check`", {
			vector<int> tree_cont;
			for(auto &it : tree){
				tree_cont.push_back(it.first);
			}
			EXPECT(tree_cont).toBeIterableEqual(check);
		});
	});
});

DESCRIBE("Multithreading insert", {
	BPlusTreeBase<int,int> tree(10);
	int tc = 10;
	int cnt = 10000;
    
	BEFORE_ALL({
		vector<thread> trds;
		for(int i=0;i<tc;i++){
			thread t([&cnt,&tree,&tc](int i){
				while(i<cnt){
					tree.insert(make_pair(i,i));
					i+=tc;
				}
			}, i);
			trds.push_back(move(t));
		}
		for(auto &t : trds){
			t.join();
		}
	});
	IT("tree size should be " + to_string(cnt), {
		EXPECT(tree.size()).toBe(cnt);
	});
	IT("mutext_count should be 0", {
		EXPECT(tree.get_mutex_count()).toBe(0);
		INFO_PRINT("Mutex count: " + to_string(tree.get_mutex_count()));
	});
	IT("tree should contain items from 0 to " + to_string(cnt-1), {
		vector<int> v1,v2;
		for(int i=0;i<cnt;i++){
			v1.push_back(i);
		}
		int ind = 0;
		for(auto& it : tree){
			v2.push_back(it.first);
			ind++;
		}
		EXPECT(v1).toBeIterableEqual(v2);
	});
	
	DESCRIBE("Then multithreading erase", {
		BEFORE_ALL({
			vector<thread> trds;
			for(int i=0;i<tc;i++){
				thread t([&cnt,&tree,&tc](int i){
					while(i<cnt){
						tree.erase(i);
						i+=tc;
					}
				}, i);
				trds.push_back(move(t));
			}
			for(auto &t : trds){
				t.join();
			}
		});
		IT("tree size should be 0", {
			EXPECT(tree.size()).toBe(0);
		});
		IT("mutext_count should be 0", {
			EXPECT(tree.get_mutex_count()).toBe(0);
			INFO_PRINT("Mutex count: " + to_string(tree.get_mutex_count()));
		});
	});
});
