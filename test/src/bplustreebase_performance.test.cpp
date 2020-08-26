list<string>::iterator move_index(list<string>::iterator it, int pos){
	while(pos--){
		++it;
	}
	return it;
}

class A{
	public:
		A(bool a, string b, string c, string d) : a(a), b(b), c(c), d(d){};
		bool a;
		string b,c,d;
};

void do_something(){ int a=10; a++; --a; }

#include <unordered_map>
#include <fstream>

DESCRIBE("[performance_tests]", {

	DESCRIBE("INSERT 100000 items", {
		auto base = BPlusTreeBase<string,shared_ptr<A>>(200);
		int time_free = 0;
		chrono::system_clock::time_point p1,p2;
		BEFORE_ALL({
			p1 = chrono::system_clock::now();	
											
			for(int i=0;i<100000;i++){
				base.insert(make_pair("t"+to_string(i),shared_ptr<A>(new A(false, "test", "value", "another value with stra ta ta"))));
			}
			
			p2 = chrono::system_clock::now();
			time_free = chrono::duration_cast<chrono::milliseconds>(p2-p1).count();
		});
		
		IT("insert all elements time", {
			TEST_SUCCEED();
			INFO_PRINT("Time to insert: " + to_string(time_free));
		});
		
		IT("access all elements time", {
			p1 = chrono::system_clock::now();
			for(int i=0;i<100000;i++){
				base.find("t"+to_string(i));
			}
			p2 = chrono::system_clock::now();
			int time = chrono::duration_cast<chrono::milliseconds>(p2-p1).count();
			TEST_SUCCEED();
			INFO_PRINT(to_string(time));
		});
		
		IT_ONLY("hashmap", {
			std::unordered_map<string, int> map;
			int* map_arr = new int[2000];
			
			p1 = chrono::system_clock::now();
			for(int i=0;i<100000;i++){
				string s = "";
				for(int i=0;i<16;i++){
					s.push_back('a'+rand()%26);
				}
				//cout<< s << endl;
				map[s]++;
			}
			p2 = chrono::system_clock::now();
			INFO_PRINT("UM: " + to_string(chrono::duration_cast<chrono::milliseconds>(p2-p1).count()));
			
			p1 = chrono::system_clock::now();
			for(int i=0;i<100000;i++){
				string s = "";
				for(int i=0;i<16;i++){
					s.push_back('a'+rand()%26);
				}
				string t = s;
				//cout<< s << endl;
				int hs = 0;
				for(int i=0;i<(int)s.size();i++){
					hs = (hs + s[i]*137) % 1997;
				}
				map_arr[hs]++;
			}
			p2 = chrono::system_clock::now();
			INFO_PRINT("ARR: " + to_string(chrono::duration_cast<chrono::milliseconds>(p2-p1).count()));
		});
		
		IT_ONLY("function call", {
			std::function<void()> fn;
			fn = [](){ do_something(); };
			
			p1 = chrono::system_clock::now();
			for(int i=0;i<1000000;i++){
				fn();
			}
			p2 = chrono::system_clock::now();
			INFO_PRINT( "Time to call with std::function " + std::to_string(chrono::duration_cast<chrono::milliseconds>(p2-p1).count()) );
			
			
			p1 = chrono::system_clock::now();
			for(int i=0;i<1000000;i++){
				do_something();
			}
			p2 = chrono::system_clock::now();
			INFO_PRINT( "Time to call directly " + std::to_string(chrono::duration_cast<chrono::milliseconds>(p2-p1).count()) );
			
			TEST_SUCCEED();
		});
		
		IT("file read", {
			{
				for(int i=0;i<200;i++){
					ofstream fs("tfiles/f"+std::to_string(i)+"");
					for(int i=0;i<1000;i++){
						fs << "test_key_name_" + std::to_string(i) + " " + to_string(i+123) + " ";
					}
				}
			}
			
			p1 = chrono::system_clock::now();
			for(int i=0;i<200;i++){
				ifstream fs("tfiles/f"+std::to_string(i)+"");
				fs.seekg(0);
				for(int i=0;i<500;i++){
					string key;
					int ind;
					fs >> key;
					fs >> ind;
				}
			}
			p2 = chrono::system_clock::now();
			INFO_PRINT( "Read with << " + std::to_string(chrono::duration_cast<chrono::milliseconds>(p2-p1).count()) );
			
			/*
			p1 = chrono::system_clock::now();
			for(int i=0;i<200;i++){
				char arr[20];
				ifstream fs("test_file.txt");
				fs.seekg(0);
				for(int i=0;i<500;i++){
					fs.read(arr,15);
				}
			}
			p2 = chrono::system_clock::now();
			INFO_PRINT( "Read with read " + std::to_string(chrono::duration_cast<chrono::milliseconds>(p2-p1).count()) );
			*/
		});
		
		IT_ONLY("More read tests",{
			{
				ofstream fs("test_file.txt");
				for(int i=0;i<10000;i++){
					fs << "test_line_to_test ";
				}
			}
			
			{
				ifstream fs("test_file.txt");
				std::cout << "Bytes available: " << fs.rdbuf()->in_avail() << std::endl;
				
				char* arr = new char[1024];
				fs.read(arr, 1024);
				std::cout << "Bytes available: " << fs.rdbuf()->in_avail() << std::endl;
				
				fs.read(arr, 10);
				std::cout << "Bytes available: " << fs.rdbuf()->in_avail() << std::endl;
			}
		});
		
		
		//IT_SKIP("big structure create time", {
		//	struct node_addition_t{
		//		struct{
		//			std::mutex m,g;
		//			int c = 0;
		//			bool wlock = false;
		//		} travel_locks;
		//		struct{
		//			std::mutex m;
		//			int c = 0;
		//		} owner_locks;
		//		struct{
		//			std::mutex m,g,p;
		//			bool wlock = false;
		//			int c = 0;
		//			bool promote = false;
		//			std::condition_variable cond, p_cond;
		//			bool shared_lock = false;
		//		} change_locks;
		//		std::shared_ptr<void> drive_data;
		//		//std::shared_ptr<DBFS::File> f;
		//	};
		//	
		//	p1 = chrono::system_clock::now();
		//	
		//	for(int i=0;i<100000;i++){
		//		node_addition_t tt;
		//	}
		//	
		//	p2 = chrono::system_clock::now();
		//	int time = chrono::duration_cast<chrono::milliseconds>(p2-p1).count();
		//	
		//	TEST_SUCCEED();
		//	INFO_PRINT(std::to_string(time));
		//});
		//
		//IT_SKIP("test vector vs list", {
		//	vector<string> v;
		//	list<string> l;
		//	
		//	p1 = chrono::system_clock::now();			
		//	for(int i=0;i<100000000;i++){
		//		v.push_back("test value");
		//	}
		//	p2 = chrono::system_clock::now();
		//	int vector_time = chrono::duration_cast<chrono::milliseconds>(p2-p1).count();
		//	
		//	///////////////////////////////////////////////////
		//	
		//	p1 = chrono::system_clock::now();			
		//	for(int i=0;i<1000000;i++){
		//		l.insert(move_index(l.begin(),min((int)l.size()/2,100)), "test value");
		//	}
		//	p2 = chrono::system_clock::now();
		//	int list_time = chrono::duration_cast<chrono::milliseconds>(p2-p1).count();
		//	
		//	TEST_SUCCEED();
		//	
		//	INFO_PRINT(to_string(vector_time) + " " + to_string(list_time));
		//	
		//});
	});
});
