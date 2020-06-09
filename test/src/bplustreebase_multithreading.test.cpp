
template<class Key, class T>
class MyInternal : public BPlusTreeBaseInternalNode<Key, T> {
	public: 
		std::mutex my_mutex;
};
template<class Key, class T>
class MyLeaf : public BPlusTreeBaseLeafNode<Key, T> {
	public: 
		std::mutex my_mutex;
};
template<class Key, class T>
class MyIterator : public BPlusTreeBaseIterator<Key, T> {
	using BPlusTreeBaseIterator<Key, T>::BPlusTreeBaseIterator;
};
template<class Key, class T>
class MyBPT : public BPlusTreeBase<Key, T, MyInternal<Key, T>, MyLeaf<Key, T>, MyIterator<Key, T> >{
	public:
		typedef typename BPlusTreeBase<Key,T>::node_ptr node_ptr;
		typedef typename BPlusTreeBase<Key,T>::PROCESS_TYPE PROCESS_TYPE;
		MyBPT(int c) : BPlusTreeBase<Key, T, MyInternal<Key, T>, MyLeaf<Key, T>, MyIterator<Key, T> >(c){};
		
	protected:
		void processSearchNodeStart(node_ptr node, PROCESS_TYPE type){ 
			if(node->is_leaf()){
				auto rnode = std::static_pointer_cast<MyLeaf<Key, T> >(node);
				rnode->my_mutex.lock();
			} else {
				auto rnode = std::static_pointer_cast<MyInternal<Key, T> >(node);
				rnode->my_mutex.lock();
			}
		};
		void processSearchNodeEnd(node_ptr node, PROCESS_TYPE type){ 
			if(node->is_leaf()){
				auto rnode = std::static_pointer_cast<MyLeaf<Key, T> >(node);
				rnode->my_mutex.unlock();
			} else {
				auto rnode = std::static_pointer_cast<MyInternal<Key, T> >(node);
				rnode->my_mutex.unlock();
			}
		};
};

DESCRIBE("Multithreading insert", {
	MyBPT<int,int> tree(10);
	int tc = 100;
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

// Test speed improvements

template<typename Key, typename T>
class bpt_test : public MyBPT<Key,T> {
	typedef typename MyBPT<Key,T>::node_ptr node_ptr;
	typedef typename MyBPT<Key,T>::PROCESS_TYPE PROCESS_TYPE;
	public:
		bpt_test(int factor) : MyBPT<Key,T>(factor)
		{
			
		}
	protected:
		void processInsertNode(node_ptr node)
		{
			this_thread::sleep_for(chrono::milliseconds(1));
		}
		void processSearchNodeStart(node_ptr node, PROCESS_TYPE pt)
		{
			MyBPT<Key,T>::processSearchNodeStart(node, pt);
			this_thread::sleep_for(chrono::milliseconds(1));
		}
};

DESCRIBE("Speed Improvement test", {
	
	int time_lock_ins, time_lock_del, time_wo_lock_ins, time_wo_lock_del;
	int tc = 10;
	int cnt = 100;
	
	DESCRIBE("Time native", {
		DESCRIBE("Multithreading insert", {
			bpt_test<int,int> tree(10);
			
			BEFORE_ALL({
				auto t1 = chrono::system_clock::now();
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
				auto t2 = chrono::system_clock::now();
				auto mils = chrono::duration_cast<chrono::milliseconds>(t2-t1);
				time_wo_lock_ins = mils.count();
			});
			IT("tree size should be " + to_string(cnt), {
				EXPECT(tree.size()).toBe(cnt);
			});
			IT("mutext_count should be 0", {
				EXPECT(tree.get_mutex_count()).toBe(0);
				INFO_PRINT("Mutex count: " + to_string(tree.get_mutex_count()));
				INFO_PRINT("Time ins: " + to_string(time_wo_lock_ins) + "ms");
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
					auto t1 = chrono::system_clock::now();
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
					auto t2 = chrono::system_clock::now();
					auto mils = chrono::duration_cast<chrono::milliseconds>(t2-t1);
					time_wo_lock_del = mils.count();
				});
				IT("tree size should be 0", {
					EXPECT(tree.size()).toBe(0);
				});
				IT("mutext_count should be 0", {
					EXPECT(tree.get_mutex_count()).toBe(0);
					INFO_PRINT("Mutex count: " + to_string(tree.get_mutex_count()));
					INFO_PRINT("Time del: " + to_string(time_wo_lock_del) + "ms");
				});
			});
		});
	});
	
	DESCRIBE("Test with lock_guard", {
		DESCRIBE("Multithreading insert", {
			bpt_test<int,int> tree(10);
			mutex mtx;
			
			BEFORE_ALL({
				auto t1 = chrono::system_clock::now();
				vector<thread> trds;
				for(int i=0;i<tc;i++){
					thread t([&cnt,&tree,&tc,&mtx](int i){
						while(i<cnt){
							mtx.lock();
							tree.insert(make_pair(i,i));
							i+=tc;
							mtx.unlock();
						}
					}, i);
					trds.push_back(move(t));
				}
				for(auto &t : trds){
					t.join();
				}
				auto t2 = chrono::system_clock::now();
				auto mils = chrono::duration_cast<chrono::milliseconds>(t2-t1);
				// Calc lock ins
				time_lock_ins = mils.count();
			});
			IT("tree size should be " + to_string(cnt), {
				EXPECT(tree.size()).toBe(cnt);
			});
			IT("mutext_count should be 0", {
				EXPECT(tree.get_mutex_count()).toBe(0);
				INFO_PRINT("Mutex count: " + to_string(tree.get_mutex_count()));
				INFO_PRINT("Time ins: " + to_string(time_lock_ins) + "ms");
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
					auto t1 = chrono::system_clock::now();
					for(int i=0;i<tc;i++){
						thread t([&cnt,&tree,&tc,&mtx](int i){
							while(i<cnt){
								mtx.lock();
								tree.erase(i);
								i+=tc;
								mtx.unlock();
							}
						}, i);
						trds.push_back(move(t));
					}
					for(auto &t : trds){
						t.join();
					}
					auto t2 = chrono::system_clock::now();
					auto mils = chrono::duration_cast<chrono::milliseconds>(t2-t1);
					time_lock_del = mils.count();
				});
				IT("tree size should be 0", {
					EXPECT(tree.size()).toBe(0);
				});
				IT("mutext_count should be 0", {
					EXPECT(tree.get_mutex_count()).toBe(0);
					INFO_PRINT("Mutex count: " + to_string(tree.get_mutex_count()));
					INFO_PRINT("Time del: " + to_string(time_lock_del) + "ms");
				});
			});
		});
	});
	
	
	DESCRIBE("Time comparison", {
		IT("Improved version insert should be faster than locked", {
			EXPECT(time_lock_ins).toBeGreaterThan(time_wo_lock_ins);
		});
		IT("Improved version erase should be faster than locked", {
			EXPECT(time_lock_del).toBeGreaterThan(time_wo_lock_del);
		});
	});
});
