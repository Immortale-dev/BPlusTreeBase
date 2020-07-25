#ifndef BPLUSTREEBASEBRANCH_H
#define BPLUSTREEBASEBRANCH_H

template<class K, class T>
class BPlusTreeRB{
	public:
		struct Node{
			K key; 
			T data;
			Node *parent; 
			Node *left;
			Node *right; 
			bool color;
		}
		
		typedef Node* NodePtr;
	
		BPlusTreeRB();
		~BPlusTreeRB();
		
		NodePtr find_root();
		NodePtr find(K k);
		NodePtr find_min(NodePtr node);
		NodePtr find_max(NodePtr node);
		NodePtr find_next(NodePtr node);
		NodePtr find_prev(NodePtr node);
		NodePtr insert(NodePtr node) 
		NodePtr insert(NodePtr node, NodePtr hint);
		NodePtr insert(K key, T val);
		NodePtr erase(K key); 
		NodePtr erase(NodePtr node); 
		NodePtr lower_bound(K key);
		NodePtr upper_bound(K key);
		int size();
		
		
	private:
		
		void leftRotate(NodePtr x);
		void rightRotate(NodePtr x);
		void fixDelete(NodePtr x);
		void fixInsert(NodePtr x);
		void rbTransplant(NodePtr u, NodePtr v);
		
		int count;
		NodePtr root;
		NodePtr TNULL;
};

template<class K, class T>
BPlusTreeRB<K, T>::BPlusTreeRB()
{
	TNULL = new Node;
	TNULL->color = 0;
	TNULL->left = nullptr;
	TNULL->right = nullptr;
	root = TNULL;
	
	size = 0;
}

template<class K, class T>
typename BPlusTreeRB<K, T>::NodePtr BPlusTreeRB<K, T>::find_root()
{
	return root;
}

template<class K, class T>
typename BPlusTreeRB<K, T>::NodePtr BPlusTreeRB<K, T>::find(K k) 
{
	NodePtr node = root;
	while(true){
		if(node == TNULL){
			return nullptr;
		}
		if(node->key == k){
			return node;
		}
		if(node->key < k){
			node = node->right;
		} else {
			node = node->left;
		}
	}
	return nullptr;
}

template<class K, class T>
typename BPlusTreeRB<K, T>::NodePtr BPlusTreeRB<K, T>::find_min(NodePtr node) 
{
	while (node->left != TNULL) {
		node = node->left;
	}
	return node;
}

template<class K, class T>
typename BPlusTreeRB<K, T>::NodePtr BPlusTreeRB<K, T>::find_max(NodePtr node) 
{
	while (node->right != TNULL) {
		node = node->right;
	}
	return node;
}

template<class K, class T>
typename BPlusTreeRB<K, T>::NodePtr BPlusTreeRB<K, T>::find_next(NodePtr x) 
{
	if (x->right != TNULL) {
		return find_min(x->right);
	}
	NodePtr y = x->parent;
	while (y != nullptr && x == y->right) {
		x = y;
		y = y->parent;
	}
	return y;
}

template<class K, class T>
typename BPlusTreeRB<K, T>::NodePtr BPlusTreeRB<K, T>::find_prev(NodePtr x) 
{
	if (x->left != TNULL) {
		return find_max(x->left);
	}
	NodePtr y = x->parent;
	while (y != nullptr && x == y->left) {
		x = y;
		y = y->parent;
	}
	return y;
}

template<class K, class T>
typename BPlusTreeRB<K, T>::NodePtr BPlusTreeRB<K, T>::insert(NodePtr node) 
{
	return insert(node, root);
}

template<class K, class T>
typename BPlusTreeRB<K, T>::NodePtr BPlusTreeRB<K, T>::insert(NodePtr node, NodePtr hint) 
{
	NodePtr x = hint;
	NodePtr y = nullptr;

	while (x != TNULL) {
		y = x;
		if (node->key < x->key) {
			x = x->left;
		} else {
			x = x->right;
		}
	}
	
	node->parent = y;
	if (y == nullptr) {
		root = node;
	} else if (node->key < y->key) {
		y->left = node;
	} else {
		y->right = node;
	}
	
	// Set basic values
	node->left = TNULL;
	node->right = TNULL;
	node->color = 1;
	
	count++;

	// if new node is a root node, simply return
	if (node->parent == nullptr){
		node->color = 0;
		return node;
	}

	// if the grandparent is null, simply return
	if (node->parent->parent == nullptr) {
		return node;
	}

	// Fix the tree
	fixInsert(node);
	return node;
}

template<class K, class T>
typename BPlusTreeRB<K, T>::NodePtr BPlusTreeRB<K, T>::insert(K key, T val) 
{
	// Ordinary Binary Search Insertion
	NodePtr node = new Node;
	node->parent = nullptr;
	node->key = key;
	node->data = val;
	node->left = TNULL;
	node->right = TNULL;
	node->color = 1; // new node must be red

	return insert(node);
}

// delete the node from the tree
template<class K, class T>
typename BPlusTreeRB<K, T>::NodePtr BPlusTreeRB<K, T>::erase(K key) {
	// find the node containing key
	NodePtr node = root;
	
	while (node != TNULL){
		if (node->key == key) {
			return erase(node);
		}
		if (node->key <= key) {
			node = node->right;
		} else {
			node = node->left;
		}
	}
	return nullptr;
}

template<class K, class T>
typename BPlusTreeRB<K, T>::NodePtr BPlusTreeRB<K, T>::erase(NodePtr z) {
	NodePtr x, y;
	y = z;
	
	bool y_original_color = y->color;
	if (z->left == TNULL) {
		x = z->right;
		rbTransplant(z, z->right);
	} else if (z->right == TNULL) {
		x = z->left;
		rbTransplant(z, z->left);
	} else {
		y = find_min(z->right);
		y_original_color = y->color;
		x = y->right;
		if (y->parent == z) {
			x->parent = y;
		} else {
			rbTransplant(y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}

		rbTransplant(z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}
	//delete z;
	if (y_original_color == 0){
		fixDelete(x);
	}
	
	count--;
	
	return z;
}

template<class K, class T>
typename BPlusTreeRB<K, T>::NodePtr BPlusTreeRB<K, T>::lower_bound(K key)
{
	NodePtr node = root;
	NodePtr ret = nullptr;
	while(true){
		if(node == TNULL){
			return ret;
		}
		if(node->key <= key){
			node = node->right;
			ret = node;
		} else {
			node = node->left;
		}
	}
	return nullptr;
}

template<class K, class T>
typename BPlusTreeRB<K, T>::NodePtr BPlusTreeRB<K, T>::upper_bound(K key)
{
	NodePtr node = lower_bound(key);
	if(!node || node->key > key){
		return node;
	}
	return find_next(node);
}

template<class K, class T>
int BPlusTreeRB<K, T>::size()
{
	return count;
}






/* PRIVATE */

// rotate left at node x
template<class K, class T>
void BPlusTreeRB<K, T>::leftRotate(NodePtr x) {
	NodePtr y = x->right;
	x->right = y->left;
	if (y->left != TNULL) {
		y->left->parent = x;
	}
	y->parent = x->parent;
	if (x->parent == nullptr) {
		this->root = y;
	} else if (x == x->parent->left) {
		x->parent->left = y;
	} else {
		x->parent->right = y;
	}
	y->left = x;
	x->parent = y;
}

// rotate right at node x
template<class K, class T>
void BPlusTreeRB<K, T>::rightRotate(NodePtr x) {
	NodePtr y = x->left;
	x->left = y->right;
	if (y->right != TNULL) {
		y->right->parent = x;
	}
	y->parent = x->parent;
	if (x->parent == nullptr) {
		this->root = y;
	} else if (x == x->parent->right) {
		x->parent->right = y;
	} else {
		x->parent->left = y;
	}
	y->right = x;
	x->parent = y;
}


// fix the rb tree modified by the delete operation
template<class K, class T>
void BPlusTreeRB<K, T>::fixDelete(NodePtr x) {
	NodePtr s;
	while (x != root && x->color == 0) {
		if (x == x->parent->left) {
			s = x->parent->right;
			if (s->color == 1) {
				// case 3.1
				s->color = 0;
				x->parent->color = 1;
				leftRotate(x->parent);
				s = x->parent->right;
			}

			if (s->left->color == 0 && s->right->color == 0) {
				// case 3.2
				s->color = 1;
				x = x->parent;
			} else {
				if (s->right->color == 0) {
					// case 3.3
					s->left->color = 0;
					s->color = 1;
					rightRotate(s);
					s = x->parent->right;
				} 

				// case 3.4
				s->color = x->parent->color;
				x->parent->color = 0;
				s->right->color = 0;
				leftRotate(x->parent);
				x = root;
			}
		} else {
			s = x->parent->left;
			if (s->color == 1) {
				// case 3.1
				s->color = 0;
				x->parent->color = 1;
				rightRotate(x->parent);
				s = x->parent->left;
			}

			if (s->right->color == 0 && s->right->color == 0) {
				// case 3.2
				s->color = 1;
				x = x->parent;
			} else {
				if (s->left->color == 0) {
					// case 3.3
					s->right->color = 0;
					s->color = 1;
					leftRotate(s);
					s = x->parent->left;
				} 

				// case 3.4
				s->color = x->parent->color;
				x->parent->color = 0;
				s->left->color = 0;
				rightRotate(x->parent);
				x = root;
			}
		} 
	}
	x->color = 0;
}


template<class K, class T>
void BPlusTreeRB<K, T>::rbTransplant(NodePtr u, NodePtr v){
	if (u->parent == nullptr) {
		root = v;
	} else if (u == u->parent->left){
		u->parent->left = v;
	} else {
		u->parent->right = v;
	}
	v->parent = u->parent;
}

// fix the red-black tree
template<class K, class T>
void BPlusTreeRB<K, T>::fixInsert(NodePtr k){
	NodePtr u;
	while (k->parent->color == 1) {
		if (k->parent == k->parent->parent->right) {
			u = k->parent->parent->left; // uncle
			if (u->color == 1) {
				// case 3.1
				u->color = 0;
				k->parent->color = 0;
				k->parent->parent->color = 1;
				k = k->parent->parent;
			} else {
				if (k == k->parent->left) {
					// case 3.2.2
					k = k->parent;
					rightRotate(k);
				}
				// case 3.2.1
				k->parent->color = 0;
				k->parent->parent->color = 1;
				leftRotate(k->parent->parent);
			}
		} else {
			u = k->parent->parent->right; // uncle

			if (u->color == 1) {
				// mirror case 3.1
				u->color = 0;
				k->parent->color = 0;
				k->parent->parent->color = 1;
				k = k->parent->parent;	
			} else {
				if (k == k->parent->right) {
					// mirror case 3.2.2
					k = k->parent;
					leftRotate(k);
				}
				// mirror case 3.2.1
				k->parent->color = 0;
				k->parent->parent->color = 1;
				rightRotate(k->parent->parent);
			}
		}
		if (k == root) {
			break;
		}
	}
	root->color = 0;
}

#endif // BPLUSTREEBASEBRANCH_H