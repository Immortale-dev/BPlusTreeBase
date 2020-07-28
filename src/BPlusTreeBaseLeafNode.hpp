#ifndef BPLUSTREEBASELEAFNODE_H
#define BPLUSTREEBASELEAFNODE_H

#include "BPlusTreeBaseNode.hpp"
#include <utility>
#include <map>
#include <memory>


__B_PLUS_TREE_NODE_TEMPLATE__
class BPlusTreeBaseLeafNode : public __B_PLUS_TREE_BASENODE_CLASS__
{	
    typedef __B_PLUS_TREE_BASENODE_CLASS__ Node;
    typedef __B_PLUS_TREE_BASELEAFNODE_CLASS__ LeafNode;
    typedef std::shared_ptr<Node> node_ptr;
    typedef std::weak_ptr<Node> node_weak;
    typedef std::pair<const Key,T> record_type;
    typedef std::shared_ptr<record_type> record_type_ptr;
    typedef typename Node::child_item_type child_item_type;
    typedef std::shared_ptr<child_item_type> child_item_type_ptr;
    typedef typename Node::child_type child_type;
    typedef typename Node::childs_type_iterator childs_type_iterator;

    public:
        BPlusTreeBaseLeafNode();
        BPlusTreeBaseLeafNode(child_type* ch);
        ~BPlusTreeBaseLeafNode();
        void release_node(childs_type_iterator node);
        void set_childs(child_type* ch);
        child_type* get_childs();
        void insert(childs_type_iterator item);
        void insert(childs_type_iterator item, childs_type_iterator hint);
        childs_type_iterator erase(const Key& item);
        childs_type_iterator erase(childs_type_iterator item);
        inline bool is_leaf();
        
        bool exists(const Key& key);
        childs_type_iterator get_iterator(const Key& key);
        
        int size();
        int childs_size();
        
        childs_type_iterator first_child();
        childs_type_iterator last_child();
        
        void split(node_ptr node, node_ptr parent);
        void shift_left(node_ptr parent);
        void shift_right(node_ptr parent);
        void join_left(node_ptr parent);
        void join_right(node_ptr parent);
        
        const Key get_key(child_item_type_ptr item);
        const Key get_key(childs_type_iterator item);
        virtual void set_next_leaf(node_ptr node);
        virtual void set_prev_leaf(node_ptr node);
        virtual node_ptr next_leaf();
        virtual node_ptr prev_leaf();
        
        childs_type_iterator childs_iterator();
        childs_type_iterator childs_iterator_end();
        childs_type_iterator move_childs_iterator(childs_type_iterator it, int count);

    protected:
        child_type* childs;
        node_weak next_leaf_node;
        node_weak prev_leaf_node;
};


__B_PLUS_TREE_NODE_TEMPLATE__
__B_PLUS_TREE_BASELEAFNODE_CLASS__::BPlusTreeBaseLeafNode()
{
	childs = new child_type();
}

__B_PLUS_TREE_NODE_TEMPLATE__
__B_PLUS_TREE_BASELEAFNODE_CLASS__::BPlusTreeBaseLeafNode(child_type* ch)
{
	set_childs(ch);
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASELEAFNODE_CLASS__::child_type* __B_PLUS_TREE_BASELEAFNODE_CLASS__::get_childs()
{
	return childs;
}

__B_PLUS_TREE_NODE_TEMPLATE__
__B_PLUS_TREE_BASELEAFNODE_CLASS__::~BPlusTreeBaseLeafNode()
{
	if(childs){
		childs_type_iterator begin = childs->begin();
		while(begin != childs->end()){
			childs_type_iterator rm = begin;
			begin = childs->find_next(begin);
			release_node(erase(rm));
		}
		delete childs;
	}
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASELEAFNODE_CLASS__::release_node(childs_type_iterator node)
{
    // TODO: replace with allocator methods
	delete node;
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASELEAFNODE_CLASS__::set_childs(child_type* ch)
{
	childs = ch;
}

__B_PLUS_TREE_NODE_TEMPLATE__
inline bool __B_PLUS_TREE_BASELEAFNODE_CLASS__::is_leaf()
{
    return true;
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASELEAFNODE_CLASS__::childs_type_iterator __B_PLUS_TREE_BASELEAFNODE_CLASS__::erase(const Key& item)
{
	return childs->erase(item);
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASELEAFNODE_CLASS__::childs_type_iterator __B_PLUS_TREE_BASELEAFNODE_CLASS__::erase(childs_type_iterator item)
{
	return childs->erase(item);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASELEAFNODE_CLASS__::insert(childs_type_iterator item)
{
    childs->insert(item);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASELEAFNODE_CLASS__::insert(childs_type_iterator item, childs_type_iterator hint)
{
    childs->insert(item, hint);
}

__B_PLUS_TREE_NODE_TEMPLATE__
bool __B_PLUS_TREE_BASELEAFNODE_CLASS__::exists(const Key& key)
{
	return childs->find(key) != childs->end();
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASELEAFNODE_CLASS__::childs_type_iterator __B_PLUS_TREE_BASELEAFNODE_CLASS__::get_iterator(const Key& key)
{
	return childs->find(key);
}

__B_PLUS_TREE_NODE_TEMPLATE__
int __B_PLUS_TREE_BASELEAFNODE_CLASS__::size()
{
	return childs_size();
}

__B_PLUS_TREE_NODE_TEMPLATE__
int __B_PLUS_TREE_BASELEAFNODE_CLASS__::childs_size()
{
	return childs->size();
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASELEAFNODE_CLASS__::childs_type_iterator __B_PLUS_TREE_BASELEAFNODE_CLASS__::first_child()
{
    return childs->begin();
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASELEAFNODE_CLASS__::childs_type_iterator __B_PLUS_TREE_BASELEAFNODE_CLASS__::last_child()
{
    return childs->find_max(childs->find_root());
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASELEAFNODE_CLASS__::split(node_ptr node, node_ptr parent)
{
	Key ret;
	childs_type_iterator c_start, c_end;
	int index = parent->get_index(this);
	bool nextIns = false;
	
	if(parent->first_child_node().get() == this){
		c_start = move_childs_iterator(childs_iterator(), childs_size()/2);
		c_end = childs_iterator_end();
		// Add items to parent node
		parent->add_nodes(index+1, node);
		nextIns = true;
	}
	else{
		c_start = childs_iterator();
		c_end = move_childs_iterator(childs_iterator(), childs_size()/2);
		// Add items to parent node
		parent->add_nodes(index, node);
	}
	
	childs_type_iterator hnt = nullptr;
    while(c_start != c_end){
		childs_type_iterator mv = c_start;
		c_start = childs->find_next(c_start);
		
		node->insert(erase(mv), hnt);
		hnt = mv;
	}
	
	if(nextIns){
		ret = get_key(node->first_child());
	}
	else{
		ret = get_key(first_child());
	}
    
	parent->add_keys(index, ret);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASELEAFNODE_CLASS__::shift_left(node_ptr parent)
{
	int current_index = parent->get_index(this);
    // Left node index
    int index = current_index-1;
    // Get shift node
    node_ptr prev = parent->get_node(index);
    // Shift item
    insert(prev->erase(prev->last_child()));
    // Update key between shifted nodes
    *(parent->keys_iterator()+index) = get_key(first_child());
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASELEAFNODE_CLASS__::shift_right(node_ptr parent)
{
	int current_index = parent->get_index(this);
    // Left node index
    int index = current_index+1;
    // Get shift node
    node_ptr next = parent->get_node(index);
    // Shift item
    insert(next->erase(next->first_child()));
    // Update key between shifted nodes
    *(parent->keys_iterator()+(index-1)) = get_key(next->first_child());
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASELEAFNODE_CLASS__::join_left(node_ptr parent)
{
	int current_index = parent->get_index(this);
    // Get join node index
    int index = current_index-1;
    // Get join node
    node_ptr prev = parent->get_node(index);
    // Move all items from join node
	childs_type_iterator start = prev->childs_iterator(), rem, hint = nullptr;
	while(start != prev->childs_iterator_end()){
		rem = start;
		start = prev->get_childs()->find_next(start);
		// Insert with hint
		insert(prev->erase(rem), hint);
		hint = rem;
	}
    // Remove from parent
    parent->remove_keys(index);
    parent->remove_nodes(index);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASELEAFNODE_CLASS__::join_right(node_ptr parent)
{
	int current_index = parent->get_index(this);
    // Get join node index
    int index = current_index+1;
    // Get join node
    node_ptr next = parent->get_node(index);
    // Move all items from join node
    childs_type_iterator start = next->childs_iterator(), rem, hint = nullptr;
	while(start != next->childs_iterator_end()){
		rem = start;
		start = next->get_childs()->find_next(start);
		// Insert with hint
		insert(next->erase(rem), hint);
		hint = rem;
	}
    // Remove from parent
    parent->remove_keys(index-1);
    parent->remove_nodes(index);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASELEAFNODE_CLASS__::set_next_leaf(node_ptr node)
{
    next_leaf_node = node_weak(node);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASELEAFNODE_CLASS__::set_prev_leaf(node_ptr node)
{
    prev_leaf_node = node_weak(node);
}

__B_PLUS_TREE_NODE_TEMPLATE__
const Key __B_PLUS_TREE_BASELEAFNODE_CLASS__::get_key(child_item_type_ptr item)
{
    return item->item->first;
}

__B_PLUS_TREE_NODE_TEMPLATE__
const Key __B_PLUS_TREE_BASELEAFNODE_CLASS__::get_key(childs_type_iterator item)
{
    return item->key;
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASELEAFNODE_CLASS__::node_ptr __B_PLUS_TREE_BASELEAFNODE_CLASS__::next_leaf()
{
    return next_leaf_node.lock();
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASELEAFNODE_CLASS__::node_ptr __B_PLUS_TREE_BASELEAFNODE_CLASS__::prev_leaf()
{
    return prev_leaf_node.lock();
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASELEAFNODE_CLASS__::childs_type_iterator __B_PLUS_TREE_BASELEAFNODE_CLASS__::childs_iterator()
{
    return childs->begin();
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASELEAFNODE_CLASS__::childs_type_iterator __B_PLUS_TREE_BASELEAFNODE_CLASS__::childs_iterator_end()
{
    return childs->end();
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASELEAFNODE_CLASS__::childs_type_iterator __B_PLUS_TREE_BASELEAFNODE_CLASS__::move_childs_iterator(childs_type_iterator it, int count)
{
	while(count != 0){
		if(count > 0){
			it = childs->find_next(it);
			count--;
		} else {
			it = childs->find_prev(it);
			count++;
		}
	}
	return it;
}


#endif // BPLUSTREEBASELEAFNODE_H
