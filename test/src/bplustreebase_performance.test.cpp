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

SCENARIO_START

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
			INFO_PRINT("Time access: " + to_string(time));
		});
	});
});


SCENARIO_END
