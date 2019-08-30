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

        virtual inline bool is_leaf() = 0;
        virtual int size() = 0;
        virtual void shift_left(Node* parent) = 0;
        virtual void shift_right(Node* parent) = 0;
        virtual void join_left(Node* parent) = 0;
        virtual void join_right(Node* parent) = 0;
        virtual Key split(Node* node) = 0;
        virtual int get_index(Key key) = 0;

        virtual Node* get_node(int index) {};
        virtual void release_node(Node* node) {};
        virtual void add_keys(int ind, Key key) {};
        virtual void add_keys(int ind, keys_type_iterator s, keys_type_iterator e) {};
        virtual void add_nodes(int ind, Node* node) {};
        virtual void add_nodes(int ind, nodes_type_iterator s, nodes_type_iterator e) {};
        virtual void remove_keys(int ind) {};
        virtual void remove_keys(keys_type_iterator s, keys_type_iterator e) {};
        virtual void remove_nodes(int ind) {};
        virtual void remove_nodes(nodes_type_iterator s, nodes_type_iterator e) {};
        virtual int get_index(Node* node) {};
        virtual int get_index(child_item_type* node) {};
        virtual Node* first_child_node() {};
        virtual Node* last_child_node() {};
        virtual Node* find(Key key) {};
        virtual Node* find_next(Key key) {};
        virtual Node* find_prev(Key key) {};
        virtual keys_type_iterator keys_iterator() {};
        virtual nodes_type_iterator nodes_iterator() {};
        virtual void release_node(child_item_type* node) {};
        virtual void insert(child_item_type* item) {};
        virtual void insert(int index, childs_type_iterator s, childs_type_iterator e) {};
        virtual void erase(int index) {};
        virtual void erase(childs_type_iterator s, childs_type_iterator e) {};
        virtual bool exists(Key key) {};
        virtual child_item_type* get(int index) {};
        virtual child_item_type* first_child() {};
        virtual child_item_type* last_child() {};
        virtual void set_next_leaf(Node* node) {};
        virtual void set_prev_leaf(Node* node) {};
        virtual Key get_key(child_item_type* item) {};
        virtual Node* next_leaf() {};
        virtual Node* prev_leaf() {};
        virtual childs_type_iterator childs_iterator() {};
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
