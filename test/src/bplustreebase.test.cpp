
#define DEBUG

#include <iostream>
#include <cassert>
#include "BPlusTreeBase.hpp"
#include "qtest.hpp"



using namespace std;



DESCRIBE("[BPlusTreeBase.hpp] Given empty tree", {
	
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
	});
	
	DESCRIBE("Add 4 items", {
		
		BEFORE_ALL({
			base.insert(make_pair(1,"wtf"));
			base.insert(make_pair(5,"test"));
			base.insert(make_pair(15,"wtf"));
			base.insert(make_pair(16,"alola"));
		});
		
		DESCRIBE("Insert one more unique lvalue item to the tree", {
			pair<int,string> lval = make_pair(17,"lol");
			base.insert(lval);
			
			IT("Size of tree should be equal 5", {
				EXPECT(base.size()).toBe(5);
				INFO_PRINT("Mutex: "+to_string(base.mutex_count));
			});
		});
		
		DESCRIBE("Insert few items with key that already exists in tree", {
			BEFORE_EACH({				
				base.insert(make_pair(16,"not"));
				base.insert(make_pair(1,"not"));
			});
			IT("Size of tree should be equal 5", {
				EXPECT(base.size()).toBe(5);
				INFO_PRINT("Mutex: "+to_string(base.mutex_count));
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
					INFO_PRINT("Mutex: "+to_string(base.mutex_count));
				});
			});
			
			DESCRIBE("Increment iterator", {
				BEFORE_EACH({		
					itl++;
				});
				IT("Iterator should be equal to base.begin()", {
					EXPECT(itl).toBe(base.begin());
					INFO_PRINT("Mutex: "+to_string(base.mutex_count));
				});
			});
		});
		
		DESCRIBE_ONLY("Add more items", {
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
				INFO_PRINT("Mutex: "+to_string(base.mutex_count));
				INFO_PRINT("Reserved: "+to_string(base.reserved_count));
			});

			IT("Value of item with `key` 15 to be `wtf`", {
				EXPECT(base.find(15).get_value()).toBe("wtf");
				INFO_PRINT("Mutex: "+to_string(base.mutex_count));
				INFO_PRINT("Reserved: "+to_string(base.reserved_count));
			});
			
			IT("Value of item with `key` 145 to be wtfi145", {
				EXPECT(base.find(145).get_value()).toBe("wtfi145");
				INFO_PRINT("Mutex: "+to_string(base.mutex_count));
				INFO_PRINT("Reserved: "+to_string(base.reserved_count));
			});
			
			DESCRIBE("Remove item with `key` 125", {
				cout << "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB: " << base.reserved_count << endl;
				BEFORE_ALL({
					base.erase(125);
					cc--;
				});
				
				IT("base.size() should be as expected", {
					EXPECT(base.size()).toBe(cc);
					INFO_PRINT("Mutex: "+to_string(base.mutex_count));
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
					INFO_PRINT("Mutex: "+to_string(base.mutex_count));
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
						INFO_PRINT("Mutex: "+to_string(base.mutex_count));
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
				INFO_PRINT("Mutex: "+to_string(base.mutex_count));
			});
			
			DESCRIBE("Remove items with keys 15 and 17", {
				
				BEFORE_ALL({
					base.erase(15);
					base.erase(17);
				});
				
				IT("Tree should be empty",{
					EXPECT(base.size()).toBe(0);
					INFO_PRINT("Mutex: "+to_string(base.mutex_count));
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
			INFO_PRINT("Mutex: "+to_string(base.mutex_count));
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
				INFO_PRINT("Mutex: "+to_string(base.mutex_count));
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
					INFO_PRINT("Mutex: "+to_string(base.mutex_count));
				});
			});
			
			DESCRIBE("And add item with key=10, and remove item with key=19", {
				BEFORE_ALL({
					base.insert(make_pair(10,"10"));
					base.erase(19);
				});
				IT("Then there should be 3 leafs with 3 items in each, and the structure to be {2,4,10,3,1,2,3,3,4,5,6,3,10,17,18}", {
					EXPECT(base.bfs_result()).toBeIterableEqual({2,4,10,3,1,2,3,3,4,5,6,3,10,17,18});
					INFO_PRINT("Mutex: "+to_string(base.mutex_count));
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
			INFO_PRINT("Mutex: "+to_string(base.mutex_count));
		});
		
		IT("base.begin() should not to be equal to base.end()", {
			EXPECT(base.begin()).NOT().toBe(base.end());
			INFO_PRINT("Mutex: "+to_string(base.mutex_count));
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
				INFO_PRINT("Mutex: "+to_string(base.mutex_count));
			});			
		});
		
		IT("Next item to item with key 99 to be equal to base.end()", {
			auto it = base.find(99);
			EXPECT(++it).toBe(base.end());
			INFO_PRINT("Mutex: "+to_string(base.mutex_count));
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
				INFO_PRINT("Mutex: "+to_string(base.mutex_count));
			});
		});
		
		IT("Iterator with key 99 first should be equal to 99 and second to be equal to `str`", {
			auto it2 = base.find(99);
			EXPECT((*it2).first).toBe(99);
			EXPECT((*it2).second).toBe("str");
			INFO_PRINT("Mutex: "+to_string(base.mutex_count));
		});
		
		IT("base[88] should equal to `str`", {
			EXPECT(base[88]).toBe("str");
			INFO_PRINT("Mutex: "+to_string(base.mutex_count));
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
	
	DESCRIBE("Reserved/released nodes", {
		BEFORE_ALL({
			for(int i=0;i<1000;i++){
				base.insert(make_pair(i, "test"));
			}
		});
		
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
		
		AFTER_ALL({
			for(int i=0;i<1000;i++){
				base.erase(i);
			}
		});
	});
});
