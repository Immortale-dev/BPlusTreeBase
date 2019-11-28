#ifndef BPLUSTREEBASENODE_H
#define BPLUSTREEBASENODE_H

#include <iostream>
#include <vector>
#include <utility>
#include <mutex>
#include <memory>


template<class Key, class T>
class BPlusTreeBaseNode
{
	
	typedef BPlusTreeBaseNode Node;
	typedef std::shared_ptr<Node> node_ptr;
	typedef std::vector<Key> keys_type;
	typedef std::vector<node_ptr> nodes_type;
	typedef typename nodes_type::iterator nodes_type_iterator;
	typedef typename keys_type::iterator keys_type_iterator;
	typedef std::pair<const Key,T> child_item_type;
	typedef std::shared_ptr<child_item_type> child_item_type_ptr;
	typedef std::vector<child_item_type_ptr> child_type;
	
	public:
		typedef typename child_type::iterator childs_type_iterator;
    
        BPlusTreeBaseNode();
        virtual ~BPlusTreeBaseNode();

        virtual int size() = 0;
        virtual void shift_left(node_ptr parent) = 0;
        virtual void shift_right(node_ptr parent) = 0;
        virtual void join_left(node_ptr parent) = 0;
        virtual void join_right(node_ptr parent) = 0;
        virtual Key split(node_ptr node) = 0;
        virtual int get_index(const Key& key) = 0;
        
        virtual void lock(){ mtx.lock(); };
        virtual void unlock(){ mtx.unlock(); };
        virtual inline bool is_leaf(){ return 0; };
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
        virtual keys_type* get_keys() { return nullptr; };
        virtual nodes_type* get_nodes() { return nullptr; };
        virtual child_type* get_childs() { return nullptr; };
        virtual int get_index(Node* node) { return 0; };
        virtual int get_index(child_item_type_ptr node) { return 0; };
        virtual node_ptr first_child_node() { return nullptr; };
        virtual node_ptr last_child_node() { return nullptr; };
        virtual node_ptr find(const Key& key) { return nullptr; };
        virtual node_ptr find_next(const Key& key) { return nullptr; };
        virtual node_ptr find_prev(const Key& key) { return nullptr; };
        virtual keys_type_iterator keys_iterator() { return {}; };
        virtual nodes_type_iterator nodes_iterator() { return {}; };
        virtual void release_node(child_item_type_ptr node) {};
        virtual void insert(child_item_type_ptr item) {};
        virtual void insert(int index, childs_type_iterator s, childs_type_iterator e) {};
        virtual child_item_type_ptr erase(int index) { return nullptr; };
        virtual void erase(childs_type_iterator s, childs_type_iterator e) {};
        virtual bool exists(const Key& key) { return 0; };
        virtual child_item_type_ptr get(int index) { return nullptr; };
        virtual child_item_type_ptr first_child() { return nullptr; };
        virtual child_item_type_ptr last_child() { return nullptr; };
        virtual void set_next_leaf(node_ptr node) {};
        virtual void set_prev_leaf(node_ptr node) {};
        virtual const Key get_key(child_item_type_ptr item) { return 0; };
        virtual node_ptr next_leaf() { return nullptr; };
        virtual node_ptr prev_leaf() { return nullptr; };
        virtual childs_type_iterator childs_iterator() { return {}; };
        
        std::shared_ptr<void> data = nullptr;
        std::mutex mtx;
};

template<class Key, class T>
BPlusTreeBaseNode<Key,T>::BPlusTreeBaseNode()
{

}

template<class Key, class T>
BPlusTreeBaseNode<Key,T>::~BPlusTreeBaseNode()
{

}

#endif // BPLUSTREEBASENODE_H
