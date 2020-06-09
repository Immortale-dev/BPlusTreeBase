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
    typedef std::vector<child_item_type_ptr> child_type;
    typedef typename child_type::iterator childs_type_iterator;

    public:
        BPlusTreeBaseLeafNode();
        BPlusTreeBaseLeafNode(child_type* ch);
        ~BPlusTreeBaseLeafNode();
        void release_node(child_item_type_ptr node);
        void set_childs(child_type* ch);
        child_type* get_childs();
        void insert(child_item_type_ptr item);
        void insert(record_type_ptr item);
        void insert(int index, childs_type_iterator s, childs_type_iterator e);
        child_item_type_ptr erase(int index);
        void erase(childs_type_iterator s, childs_type_iterator e);
        inline bool is_leaf();
        bool exists(const Key& key);
        int get_index(const Key& key, bool to_lower = false);
        int get_index(child_item_type_ptr node);
        int size();
        int childs_size();
        child_item_type_ptr get(int index);
        child_item_type_ptr first_child();
        child_item_type_ptr last_child();
        void split(node_ptr node, node_ptr parent);
        void shift_left(node_ptr parent);
        void shift_right(node_ptr parent);
        void join_left(node_ptr parent);
        void join_right(node_ptr parent);
        const Key get_key(child_item_type_ptr item);
        virtual void set_next_leaf(node_ptr node);
        virtual void set_prev_leaf(node_ptr node);
        virtual node_ptr next_leaf();
        virtual node_ptr prev_leaf();
        childs_type_iterator childs_iterator();
        childs_type_iterator childs_iterator_end();

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
		for(int i=0;i<childs_size();i++){
			release_node((*childs)[i]);
		}
		delete childs;
	}
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASELEAFNODE_CLASS__::release_node(child_item_type_ptr node)
{
    // TODO: replace with allocator methods
    // No need to delete shared ptr
    // delete node;
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
typename __B_PLUS_TREE_BASELEAFNODE_CLASS__::child_item_type_ptr __B_PLUS_TREE_BASELEAFNODE_CLASS__::erase(int ind)
{
    if(ind < 0){
        return nullptr;
	}
	child_item_type_ptr deleted = (*childs)[ind];
    childs->erase(childs->begin()+ind);
    return deleted;
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASELEAFNODE_CLASS__::erase(childs_type_iterator s, childs_type_iterator e)
{
    childs->erase(s, e);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASELEAFNODE_CLASS__::insert(int index, childs_type_iterator s, childs_type_iterator e)
{
    childs->insert(childs->begin()+index, s, e);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASELEAFNODE_CLASS__::insert(child_item_type_ptr item)
{
    int res = get_index(get_key(item));
    childs->insert(childs->begin()+res, item);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASELEAFNODE_CLASS__::insert(record_type_ptr item)
{
	child_item_type* ch = new child_item_type();
	ch->item = item;
    insert(child_item_type_ptr(ch));
}

__B_PLUS_TREE_NODE_TEMPLATE__
bool __B_PLUS_TREE_BASELEAFNODE_CLASS__::exists(const Key& key)
{
    int index = get_index(key);
    return index < childs_size() && get_key((*childs)[index]) == key;
}

__B_PLUS_TREE_NODE_TEMPLATE__
int __B_PLUS_TREE_BASELEAFNODE_CLASS__::get_index(child_item_type_ptr node)
{
    return get_index(get_key(node));
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
int __B_PLUS_TREE_BASELEAFNODE_CLASS__::get_index(const Key& key, bool to_lower)
{
    int mn=0,mx=childs_size()-1,md;
    int res = childs_size();
    while(mn<=mx){
        md = (mn+mx)/2;
        const Key& k = get_key((*childs)[md]);
        if( (to_lower && k <= key) || (!to_lower && k < key) ){
            mn = md+1;
            if(to_lower){
				res = md;
			}
        }
        else{
            mx = md-1;
            if(!to_lower){
				res = md;
			}
        }
    }
    return res;
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASELEAFNODE_CLASS__::child_item_type_ptr __B_PLUS_TREE_BASELEAFNODE_CLASS__::get(int index)
{
    return (*childs)[index];
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASELEAFNODE_CLASS__::child_item_type_ptr __B_PLUS_TREE_BASELEAFNODE_CLASS__::first_child()
{
    return get(0);
}

__B_PLUS_TREE_NODE_TEMPLATE__
typename __B_PLUS_TREE_BASELEAFNODE_CLASS__::child_item_type_ptr __B_PLUS_TREE_BASELEAFNODE_CLASS__::last_child()
{
    return get(childs_size()-1);
}

__B_PLUS_TREE_NODE_TEMPLATE__
void __B_PLUS_TREE_BASELEAFNODE_CLASS__::split(node_ptr node, node_ptr parent)
{
	Key ret;
	childs_type_iterator c_start, c_end;
	int index = parent->get_index(this);
	bool nextIns = false;
	if(parent->first_child_node().get() == this){
		c_start = childs_iterator() + childs_size()/2;
		c_end = childs_iterator_end();
		// Add items to parent node
		parent->add_nodes(index+1, node);
		nextIns = true;
	}
	else{
		c_start = childs_iterator();
		c_end = childs_iterator() + childs_size()/2;
		// Add items to parent node
		parent->add_nodes(index, node);
	}
	
	node->insert(0, c_start, c_end);
    erase(c_start, c_end);
	
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
    insert(prev->last_child());
    prev->erase(prev->childs_size()-1);
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
    insert(next->first_child());
    next->erase(0);
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
    insert(0, prev->childs_iterator(), prev->childs_iterator_end());
    prev->erase(prev->childs_iterator(), prev->childs_iterator_end());
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
    insert(childs_size(), next->childs_iterator(), next->childs_iterator_end());
    next->erase(next->childs_iterator(), next->childs_iterator_end());
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


#endif // BPLUSTREEBASELEAFNODE_H
