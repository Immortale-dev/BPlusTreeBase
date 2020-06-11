

DESCRIBE("[BPlusTreeBaseInternalNode]", {
	
	//typedef BPlusTreeBaseLeafNode<int, string> LeafNode;
	typedef BPlusTreeBaseInternalNode<int, string> InternalNode;
	typedef BPlusTreeBaseNode<int,string> Node;
	typedef std::shared_ptr<Node> node_ptr;
	//typedef std::pair<const int,string> record_type;
	//typedef Node::child_item_type child_item_type;
	//typedef std::shared_ptr<record_type> record_type_ptr;
	//typedef std::shared_ptr<child_item_type> child_item_type_ptr;
	
	DESCRIBE("Create instance", {
		
		node_ptr node;
		
		BEFORE_ALL({
			node = node_ptr(new InternalNode());
		});
		
		AFTER_ALL({
			//delete node;
		});
		
		IT("is_leaf() method should return false", {
			EXPECT(node->is_leaf()).toBe(false);
		});
	});
});
