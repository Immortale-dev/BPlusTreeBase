#ifndef BPLUSTREEBASEUTILS_H
#define BPLUSTREEBASEUTILS_H

#include <memory>

#define __B_PLUS_TREE_BASE_CLASS__ BPlusTreeBase<Key, T, INTERNAL, LEAF, ITERATOR>
#define __B_PLUS_TREE_BASENODE_CLASS__ BPlusTreeBaseNode<Key, T>
#define __B_PLUS_TREE_BASEINTERNALNODE_CLASS__ BPlusTreeBaseInternalNode<Key, T>
#define __B_PLUS_TREE_BASELEAFNODE_CLASS__ BPlusTreeBaseLeafNode<Key, T>
#define __B_PLUS_TREE_BASEITERATOR_CLASS__ BPlusTreeBaseIterator<Key, T>

#define __B_PLUS_TREE_BASE_TEMPLATE__DEF__ template <class Key, class T, class INTERNAL = __B_PLUS_TREE_BASEINTERNALNODE_CLASS__, class LEAF = __B_PLUS_TREE_BASELEAFNODE_CLASS__, class ITERATOR = __B_PLUS_TREE_BASEITERATOR_CLASS__>

#define __B_PLUS_TREE_NODE_TEMPLATE__ template <class Key, class T>
#define __B_PLUS_TREE_ITERATOR_TEMPLATE__ template <class Key, class T>
#define __B_PLUS_TREE_BASE_TEMPLATE__ template <class Key, class T, class INTERNAL, class LEAF, class ITERATOR>

__B_PLUS_TREE_NODE_TEMPLATE__ class BPlusTreeBaseNode;
__B_PLUS_TREE_NODE_TEMPLATE__ class BPlusTreeBaseInternalNode;
__B_PLUS_TREE_NODE_TEMPLATE__ class BPlusTreeBaseLeafNode;
__B_PLUS_TREE_ITERATOR_TEMPLATE__ class BPlusTreeBaseIterator;


__B_PLUS_TREE_NODE_TEMPLATE__
class BPlusTreeBase__Interface{
	public:
		enum class PROCESS_TYPE{ READ, WRITE };
		enum class LEAF_REF{ PREV, NEXT };
		
		typedef BPlusTreeBaseNode<Key, T> Node;
		typedef std::shared_ptr<Node> node_ptr;
		typedef typename Node::child_item_type_ptr childs_item_ptr;
		
	public:
		virtual void processSearchNodeStart(node_ptr node, PROCESS_TYPE type) = 0;
		virtual void processSearchNodeEnd(node_ptr node, PROCESS_TYPE type) = 0;
		virtual void processInsertNode(node_ptr node) = 0;
		virtual void processDeleteNode(node_ptr node) = 0;
		virtual void processIteratorNodeReserved(node_ptr node) = 0;
		virtual void processIteratorNodeReleased(node_ptr node) = 0;
		virtual void processIteratorMoveStart(childs_item_ptr item, int step) = 0;
		virtual void processIteratorMoveEnd(childs_item_ptr item, int step) = 0;
		virtual void processItemReserve(childs_item_ptr item, PROCESS_TYPE type) = 0;
		virtual void processItemRelease(childs_item_ptr item, PROCESS_TYPE type) = 0;
		virtual void processItemMove(node_ptr node, bool release) = 0;
		virtual void processLeafReserve(node_ptr node, PROCESS_TYPE type) = 0;
		virtual void processLeafRelease(node_ptr node, PROCESS_TYPE type) = 0;
		virtual void processLeafInsertItem(node_ptr node, childs_item_ptr item) = 0;
		virtual void processLeafDeleteItem(node_ptr node, childs_item_ptr item) = 0;
		virtual void processLeafSplit(node_ptr node, node_ptr new_node, node_ptr link_node) = 0;
		virtual void processLeafJoin(node_ptr node, node_ptr join_node, node_ptr link_node) = 0;
		virtual void processLeafShift(node_ptr node, node_ptr shift_node) = 0;
		virtual void processLeafFree(node_ptr node) = 0;
		virtual void processLeafRef(node_ptr node, node_ptr ref_node, LEAF_REF ref) = 0;
		virtual node_ptr min_node() = 0;
		virtual node_ptr max_node() = 0;
};

__B_PLUS_TREE_BASE_TEMPLATE__DEF__ class BPlusTreeBase;


#endif // BPLUSTREEBASEUTILS_H
