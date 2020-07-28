


DESCRIBE("[BPlusTreeBaseLeafNode]", {
	
	//typedef BPlusTreeBaseLeafNode<int, string> LeafNode;
	//typedef BPlusTreeBaseInternalNode<int, string> InternalNode;
	//typedef BPlusTreeBaseNode<int,string> Node;
	//typedef std::shared_ptr<Node> node_ptr;
	//typedef std::pair<const int,string> record_type;
	//typedef Node::child_item_type child_item_type;
	//typedef std::shared_ptr<record_type> record_type_ptr;
	//typedef std::shared_ptr<child_item_type> child_item_type_ptr;
	
	//DESCRIBE("Create instance with default constructor", {
		
		//node_ptr node;
		
		//BEFORE_EACH({
			//node = node_ptr(new LeafNode());
		//});
		
		//AFTER_EACH({
			////delete node;
		//});
		
		//IT("is_leaf() method should return true", {
			//EXPECT(node->is_leaf()).toBe(true);
		//});
		
		//IT("get_childs() should not return nullptr", {
			//EXPECT(node->get_childs()).NOT().toBeNullPtr();
		//});
		
		//IT("I can insert `record_type_ptr` item", {
			//auto ch = record_type_ptr(make_pair(10, "test"));
			//node->insert(ch);
			//EXPECT(node->size()).toBe(1);
		//});
		
		//IT("I can insert `child_item_type_ptr` item", {
			//auto ch = child_item_type_ptr(new child_item_type());
			//ch->pos = 0;
			//ch->item = record_type_ptr(new record_type(10, "test"));
			//node->insert(ch);
			//EXPECT(node->size()).toBe(1);
		//});
		
		//DESCRIBE("Create another node with default constructor and add couple of items", {
			//node_ptr new_node;
			
			//BEFORE_EACH({
				//new_node = node_ptr(new LeafNode());
				//new_node->insert(record_type_ptr(new record_type(1, "test1")));
				//new_node->insert(record_type_ptr(new record_type(3, "test3")));
				//new_node->insert(record_type_ptr(new record_type(5, "test5")));
				//new_node->insert(record_type_ptr(new record_type(7, "test7")));
				//new_node->insert(record_type_ptr(new record_type(9, "test9")));
				//new_node->insert(record_type_ptr(new record_type(11, "test11")));
			//});
			
			//AFTER_EACH({
				////delete new_node;
			//});
			
			//IT("I could insert items using `child_item_type_ptr` iterators", {
				//node->insert(0, new_node->childs_iterator(), new_node->childs_iterator()+3);
				//EXPECT(node->childs_size()).toBe(3);
				//EXPECT(node->size()).toBe(3);
			//});
			
			//IT("`set_childs` should work", {
				//node->set_childs(new_node->get_childs());
				//EXPECT(node->get_childs()).toBe(new_node->get_childs());
				//EXPECT(node->size()).toBe(6);
				//new_node->set_childs(nullptr);
			//});
			
			//IT("I can erase childs using index", {
				//new_node->erase(2);
				//EXPECT(new_node->size()).toBe(5);
			//});
			
			//IT("I can erase childs using iterators", {
				//new_node->erase(new_node->childs_iterator(), new_node->childs_iterator()+3);
				//EXPECT(new_node->size()).toBe(3);
			//});
			
			//IT("`exists` should works", {
				//EXPECT(new_node->exists(3)).toBe(true);
				//EXPECT(new_node->exists(4)).toBe(false);
			//});
			
			//IT("`get_index` should works", {
				//EXPECT(new_node->get_index(5)).toBe(2);
				//EXPECT(new_node->get_index(6)).toBe(3);
				//EXPECT(new_node->get_index(6, true)).toBe(2);
				//EXPECT(new_node->get_index(new_node->get(4))).toBe(4);
			//});
			
			//IT("first/last child should works", {
				//EXPECT(new_node->get_key(new_node->first_child())).toBe(1);
				//EXPECT(new_node->get_key(new_node->last_child())).toBe(11);
			//});	
			
			//IT("`next/prev` leafs should work", {
				//new_node->set_next_leaf(node);
				//node->set_prev_leaf(new_node);
				//EXPECT(node->next_leaf()).toBe(nullptr);
				//EXPECT(node->prev_leaf()).toBe(new_node);
				//EXPECT(new_node->next_leaf()).toBe(node);
				//EXPECT(new_node->prev_leaf()).toBe(nullptr);
			//});
			
			//DESCRIBE_SKIP("Create InternalNode", {
				//BEFORE_EACH({
					//node_ptr int_node = node_ptr(new InternalNode());	
				//});
				
				//IT("`split` should works", {
					
				//});
				
				//IT("`join left/right` should works", {
					
				//});
				
				//IT("`shift left/right` should works", {
					
				//});
			//});
		//});
	//});
});
