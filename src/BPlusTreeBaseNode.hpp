#ifndef BPLUSTREEBASENODE_H
#define BPLUSTREEBASENODE_H

#include <iostream>
#include <vector>
#include <utility>


template<class Key, class T>
class BPlusTreeBaseNode
{
    typedef BPlusTreeBaseNode Node;
    typedef std::vector<Key> keys_type;
    typedef std::vector<Node*> nodes_type;
    typedef typename nodes_type::iterator nodes_type_iterator;
    typedef typename keys_type::iterator keys_type_iterator;
    typedef std::pair<Key,T> child_item_type;
    typedef std::vector<child_item_type*> child_type;
    typedef typename child_type::iterator childs_type_iterator;

    public:
        BPlusTreeBaseNode();
        virtual ~BPlusTreeBaseNode();

        virtual int size() = 0;
        virtual void shift_left(Node* parent) = 0;
        virtual void shift_right(Node* parent) = 0;
        virtual void join_left(Node* parent) = 0;
        virtual void join_right(Node* parent) = 0;
        virtual Key split(Node* node) = 0;
        virtual int get_index(Key key) = 0;

        virtual inline bool is_leaf(){ return 0; };
        virtual Node* get_node(int index) { return nullptr; };
        virtual void release_node(Node* node) {};
        virtual void add_keys(int ind, Key key) {};
        virtual void add_keys(int ind, keys_type_iterator s, keys_type_iterator e) {};
        virtual void add_nodes(int ind, Node* node) {};
        virtual void add_nodes(int ind, nodes_type_iterator s, nodes_type_iterator e) {};
        virtual void remove_keys(int ind) {};
        virtual void remove_keys(keys_type_iterator s, keys_type_iterator e) {};
        virtual void remove_nodes(int ind) {};
        virtual void remove_nodes(nodes_type_iterator s, nodes_type_iterator e) {};
        virtual int get_index(Node* node) { return 0; };
        virtual int get_index(child_item_type* node) { return 0; };
        virtual Node* first_child_node() { return nullptr; };
        virtual Node* last_child_node() { return nullptr; };
        virtual Node* find(Key key) { return nullptr; };
        virtual Node* find_next(Key key) { return nullptr; };
        virtual Node* find_prev(Key key) { return nullptr; };
        virtual keys_type_iterator keys_iterator() { return {}; };
        virtual nodes_type_iterator nodes_iterator() { return {}; };
        virtual void release_node(child_item_type* node) {};
        virtual void insert(child_item_type* item) {};
        virtual void insert(int index, childs_type_iterator s, childs_type_iterator e) {};
        virtual child_item_type* erase(int index) { return nullptr; };
        virtual void erase(childs_type_iterator s, childs_type_iterator e) {};
        virtual bool exists(Key key) { return 0; };
        virtual child_item_type* get(int index) { return nullptr; };
        virtual child_item_type* first_child() { return nullptr; };
        virtual child_item_type* last_child() { return nullptr; };
        virtual void set_next_leaf(Node* node) {};
        virtual void set_prev_leaf(Node* node) {};
        virtual Key get_key(child_item_type* item) { return 0; };
        virtual Node* next_leaf() { return nullptr; };
        virtual Node* prev_leaf() { return nullptr; };
        virtual childs_type_iterator childs_iterator() { return {}; };
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
