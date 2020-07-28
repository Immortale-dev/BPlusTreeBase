#include "BPlusTreeBaseBranch.hpp"

DESCRIBE("BPlusTreeBaseRB", {
	
	
	DESCRIBE("Initialize with <int,string>", {
		
		BPlusTreeRB<int, string>* tree;
		
		BEFORE_EACH({
			tree = new BPlusTreeRB<int, string>();
		});
		
		AFTER_EACH({
			delete tree;
		});
		
		IT("Should initialize", {
			EXPECT(tree->size()).toBe(0);
		});
		
		DESCRIBE("Add 10 items with keys from 1 to 10", {
			BEFORE_ALL({
				for(int i=1;i<=10;i++){
					tree->insert(i,"test_"+std::to_string(i));
				}
			});
			
			IT("Items should be inserted", {
				EXPECT(tree->size()).toBe(10);
				for(int i=1;i<=10;i++){
					EXPECT(tree->find(i)->data).toBe("test_"+std::to_string(i));
				}
				std::vector<std::pair<int,int> > keys_order;
				tree->dfs(tree->find_root(), [&keys_order](typename BPlusTreeRB<int, string>::NodePtr n){
					
					int par = 0;
					if(n->parent){
						par = n->parent->key;
					}
					keys_order.push_back(make_pair(n->key, par));
					//INFO_PRINT(std::to_string(n->key) + " -> " + std::to_string(par));
				});
				
				EXPECT(keys_order).toBeIterableEqual(std::vector<pair<int,int>> {{1,2}, {3,2}, {2,4}, {5,6}, {7,8}, {10,9}, {9,8}, {8,6}, {6,4}, {4,0}});
			});
		});
		
		DESCRIBE("Add 10 items in next order: {6,1,8,2,4,5,10,9,7,3}", {
			BEFORE_ALL({
				vector<int> v{6,1,8,2,4,5,10,9,7,3};
				for(auto it : v){
					tree->insert(it,"test_"+std::to_string(it));
				}
			});
			
			IT("Items should be inserted", {
				EXPECT(tree->size()).toBe(10);
				for(int i=1;i<=10;i++){
					EXPECT(tree->find(i)->data).toBe("test_"+std::to_string(i));
				}
				std::vector<std::pair<int,int> > keys_order;
				tree->dfs(tree->find_root(), [&keys_order](typename BPlusTreeRB<int, string>::NodePtr n){
					
					int par = 0;
					if(n->parent){
						par = n->parent->key;
					}
					keys_order.push_back(make_pair(n->key, par));
					//INFO_PRINT(std::to_string(n->key) + " -> " + std::to_string(par));
				});
				
				EXPECT(keys_order).toBeIterableEqual(std::vector<pair<int,int>> { {1,2}, {3,4}, {5,4}, {4,2}, {2,6}, {7,8}, {8,9}, {10,9}, {9,6}, {6,0} });
			});
		});
		
		DESCRIBE("Add 30 items from 1 to 30", {
			BEFORE_EACH({
				for(int i=1;i<=30;i++){
					tree->insert(i,"tst");
				}
			});
			
			IT("Items should exists", {
				EXPECT(tree->size()).toBe(30);
				for(int i=1;i<=30;i++){
					EXPECT(tree->find(i)->data).toBe("tst");
				}
			});
			
			IT("find_min/find_max should works fine", {
				auto* node_min = tree->find_min(tree->find_root());
				EXPECT(node_min->key).toBe(1);
				
				auto* node_max = tree->find_max(tree->find_root());
				EXPECT(node_max->key).toBe(30);
			});
			
			IT("find_next/find_prev shuld works fine", {
				int ind = 0;
				auto* node = tree->find_min(tree->find_root());
				while(node != tree->end()){
					assert((bool)node);
					EXPECT(node->key).toBe(++ind);
					node = tree->find_next(node);
				}
				EXPECT(ind).toBe(30);
			});
			
			DESCRIBE("Delete items from 5 to 25", {
				BEFORE_EACH({
					for(int i=5;i<25;i++){
						delete tree->erase(i);
					}
				});
				
				IT("Items should be expected", {
					
					EXPECT(tree->size()).toBe(10);
					
					std::vector<std::pair<int,int> > keys_order;
					tree->dfs(tree->find_root(), [&keys_order](typename BPlusTreeRB<int, string>::NodePtr n){
						
						int par = 0;
						if(n->parent){
							par = n->parent->key;
						}
						keys_order.push_back(make_pair(n->key, par));
						//INFO_PRINT(std::to_string(n->key) + " -> " + std::to_string(par));
					});
					
					EXPECT(keys_order).toBeIterableEqual(std::vector<pair<int,int>> { {1,2}, {3,4}, {4,2}, {2,25}, {27,26}, {26,28}, {30,29}, {29,28}, {28,25}, {25,0} });
				});
				
				IT("lower_bound of `2` should be 2", {
					EXPECT(tree->lower_bound(2)->key).toBe(2);
				});
				
				IT("upper_bound of `2` should be 3", {
					EXPECT(tree->upper_bound(2)->key).toBe(3);
				});
				
				IT("lower_bound of `7` should be 4", {
					EXPECT(tree->lower_bound(7)->key).toBe(4);
					INFO_PRINT(std::to_string(tree->lower_bound(7)->key));
				});
				
				IT("upper_bound of `7` should be 25", {
					EXPECT(tree->upper_bound(7)->key).toBe(25);
					INFO_PRINT(std::to_string(tree->upper_bound(7)->key));
				});
				
				IT("lower_bound of `0` should be 1", {
					EXPECT(tree->lower_bound(0)->key).toBe(1);
					INFO_PRINT(std::to_string(tree->lower_bound(0)->key));
				});
				
				IT("upper_bound of `0` should be 1", {
					EXPECT(tree->upper_bound(0)->key).toBe(1);
					INFO_PRINT(std::to_string(tree->upper_bound(0)->key));
				});
				
				IT("lower_bound of `35` should be 30", {
					EXPECT(tree->lower_bound(35)->key).toBe(30);
				});
				
				IT("upper_bound of `35` should be end()", {
					EXPECT(tree->upper_bound(35)).toBe(tree->end());
				});
			});
		});
		
	});
	
	DESCRIBE("After tree destruct", {
		IT("Nodes count should be `0`", {
			EXPECT(branch_nodes_count).toBe(0);
			INFO_PRINT("`branch_nodes_count` = " + std::to_string(branch_nodes_count));
		});
	});
});
