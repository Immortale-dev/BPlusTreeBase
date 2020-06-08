#ifndef BPLUSTREEBASEUTILS_H
#define BPLUSTREEBASEUTILS_H


#define __B_PLUS_TREE_BASE_CLASS__ BPlusTreeBase<Key, T, D, INTERNAL, LEAF, ITERATOR>
#define __B_PLUS_TREE_BASENODE_CLASS__ BPlusTreeBaseNode<Key, T, D>
#define __B_PLUS_TREE_BASEINTERNALNODE_CLASS__ BPlusTreeBaseInternalNode<Key, T, D>
#define __B_PLUS_TREE_BASELEAFNODE_CLASS__ BPlusTreeBaseLeafNode<Key, T, D>
#define __B_PLUS_TREE_BASEITERATOR_CLASS__ BPlusTreeBaseIterator<Key, T, D>

#define __B_PLUS_TREE_BASE_TEMPLATE__DEF__ template <class Key, class T, class D = void*, class INTERNAL = __B_PLUS_TREE_BASEINTERNALNODE_CLASS__, class LEAF = __B_PLUS_TREE_BASELEAFNODE_CLASS__, class ITERATOR = __B_PLUS_TREE_BASEITERATOR_CLASS__>

#define __B_PLUS_TREE_NODE_TEMPLATE__ template <class Key, class T, class D>
#define __B_PLUS_TREE_ITERATOR_TEMPLATE__ template <class Key, class T, class D>
#define __B_PLUS_TREE_BASE_TEMPLATE__ template <class Key, class T, class D, class INTERNAL, class LEAF, class ITERATOR>

__B_PLUS_TREE_NODE_TEMPLATE__ class BPlusTreeBaseInternalNode;
__B_PLUS_TREE_NODE_TEMPLATE__ class BPlusTreeBaseLeafNode;
__B_PLUS_TREE_ITERATOR_TEMPLATE__ class BPlusTreeBaseIterator;

__B_PLUS_TREE_BASE_TEMPLATE__DEF__ class BPlusTreeBase;


#endif // BPLUSTREEBASEUTILS_H
