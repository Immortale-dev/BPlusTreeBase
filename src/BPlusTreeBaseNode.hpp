#ifndef BPLUSTREEBASENODE_H
#define BPLUSTREEBASENODE_H

#include <vector>
#include <utility>
#include <memory>
#include "BPlusTreeBaseBranch.hpp"

#ifdef DEBUG
static int active_nodes_count = 0;
#endif

__B_PLUS_TREE_NODE_TEMPLATE__
class BPlusTreeBaseNode
{	
	public:
		struct child_item_type;
		
		typedef BPlusTreeBaseNode Node;
		typedef std::shared_ptr<Node> node_ptr;
		typedef std::weak_ptr<Node> node_weak;
		typedef std::vector<Key> keys_type;
		typedef std::vector<node_ptr> nodes_type;
		typedef std::pair<const Key&,T> record_type;
		typedef std::shared_ptr<record_type> record_type_ptr;
		typedef typename nodes_type::iterator nodes_type_iterator;
		typedef typename keys_type::iterator keys_type_iterator;
		typedef std::shared_ptr<child_item_type> child_item_type_ptr;
		typedef BPlusTreeRB<Key,child_item_type_ptr> child_type;
		
		struct child_item_type{
			node_weak node;
			record_type_ptr item;
		};
	
	public:
		typedef typename child_type::Node* childs_type_iterator;
    
        BPlusTreeBaseNode();
        virtual ~BPlusTreeBaseNode();

        virtual int size() = 0;
        virtual void shift_left(node_ptr parent) = 0;
        virtual void shift_right(node_ptr parent) = 0;
        virtual void join_left(node_ptr parent) = 0;
        virtual void join_right(node_ptr parent) = 0;
        virtual void split(node_ptr node, node_ptr parent) = 0;
        
        virtual int get_index(const Key& key, bool to_lower = false){ return 0; };
        virtual bool is_leaf(){ return 0; };
        virtual node_ptr get_node(int index) { return nullptr; };
        virtual void release_node(node_ptr node) {};
        virtual void add_keys(int ind, Key key) {};
        virtual void add_keys(int ind, keys_type_iterator s, keys_type_iterator e) {};
        virtual void add_nodes(int ind, node_ptr node) {};
        virtual void add_nodes(int ind, nodes_type_iterator s, nodes_type_iterator e) {};
        virtual void remove_keys(int ind) {};
        virtual void remove_keys(keys_type_iterator s, keys_type_iterator e) {};
        virtual void remove_nodes(int ind) {};
        virtual void remove_nodes(nodes_type_iterator s, nodes_type_iterator e) {};
        virtual void set_keys(keys_type* keys) {};
        virtual void set_nodes(nodes_type* nodes) {};
        virtual void set_childs(child_type* ch) {};
        virtual int keys_size() { return 0; };
        virtual int nodes_size() { return 0; };
        virtual int childs_size() { return 0; };
        virtual keys_type* get_keys() { return nullptr; };
        virtual nodes_type* get_nodes() { return nullptr; };
        virtual child_type* get_childs() { return nullptr; };
        virtual int get_index(Node* node) { return 0; };
        virtual int get_index(child_item_type_ptr node) { return 0; };
        virtual childs_type_iterator get_iterator(const Key& key){ return{}; };
        virtual node_ptr first_child_node() { return nullptr; };
        virtual node_ptr last_child_node() { return nullptr; };
        virtual node_ptr find(const Key& key) { return nullptr; };
        virtual node_ptr find_next(const Key& key) { return nullptr; };
        virtual node_ptr find_prev(const Key& key) { return nullptr; };
        virtual keys_type_iterator keys_iterator() { return {}; };
        virtual nodes_type_iterator nodes_iterator() { return {}; };
        virtual childs_type_iterator childs_iterator() { return {}; };
        virtual keys_type_iterator keys_iterator_end() { return {}; };
        virtual nodes_type_iterator nodes_iterator_end() { return {}; };
        virtual childs_type_iterator childs_iterator_end() { return {}; };
        virtual childs_type_iterator move_childs_iterator(childs_type_iterator it, int ind) { return {}; };
        virtual void release_node(childs_type_iterator node) {};
        virtual void insert(childs_type_iterator item) {};
        virtual void insert(childs_type_iterator item, childs_type_iterator hint) {};
        virtual childs_type_iterator erase(const Key& item){ return nullptr; };
        virtual childs_type_iterator erase(childs_type_iterator item){ return nullptr; };
        virtual bool exists(const Key& key) { return 0; };
        virtual childs_type_iterator first_child() { return nullptr; };
        virtual childs_type_iterator last_child() { return nullptr; };
        virtual void set_next_leaf(node_ptr node) {};
        virtual void set_prev_leaf(node_ptr node) {};
        virtual const Key get_key(child_item_type_ptr item) { return 0; };
        virtual const Key get_key(childs_type_iterator item) { return 0; };
        virtual node_ptr next_leaf() { return nullptr; };
        virtual node_ptr prev_leaf() { return nullptr; };
};

__B_PLUS_TREE_NODE_TEMPLATE__
__B_PLUS_TREE_BASENODE_CLASS__::BPlusTreeBaseNode()
{
	#ifdef DEBUG
	active_nodes_count++;
	#endif
}

__B_PLUS_TREE_NODE_TEMPLATE__
__B_PLUS_TREE_BASENODE_CLASS__::~BPlusTreeBaseNode()
{
	#ifdef DEBUG
	active_nodes_count--;
	#endif
}

#endif // BPLUSTREEBASENODE_H
