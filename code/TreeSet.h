#ifndef _TREE_SET_H_
#define _TREE_SET_H_

#include "RedBlackTree.h"
#include "BiderectionalIterators.h"

template<typename Tree>
class TreeSetIterator;

namespace BTS_NAMESPACE
{
	template<typename _KeyType>
	struct TreeSetNode;

	template<typename _KeyType>
	struct TreeMultiSetNode;

	template<bool _Multi, typename _NodeType, typename _LessComp>
	class BasicTreeSet;
}


template<typename _KeyType>
struct BTS_NAMESPACE::TreeSetNode
{
	using KeyType = _KeyType;

	TreeSetNode* parent = nullptr;
	TreeSetNode* left = nullptr;
	TreeSetNode* right = nullptr;

	KeyType key;
	char color = 'R';


	template<typename KT>
	TreeSetNode(TreeSetNode* _parent, KT&& _key) :
		parent(_parent), key(std::forward<KT>(_key)) {};

	TreeSetNode(TreeSetNode* _parent, TreeSetNode* _other) : 
		parent(_parent), key(_other->key) {}; //Для insertNewNode


	void swap(TreeSetNode* _other)
	{
		std::swap(key, _other->key);
	}
	
	const KeyType& getKey() const noexcept { return key; };

	void nullifyNext() {};

	TreeSetNode* getNextEqual() noexcept { return nullptr; }

	TreeSetNode* getFirstEqual() noexcept { return nullptr; }

	TreeSetNode* getLastEqual() noexcept { return nullptr; }

	TreeSetNode** getAddressOfLastEqualNextPtr() noexcept { return nullptr; };
};

template<typename _KeyType>
struct BTS_NAMESPACE::TreeMultiSetNode
{
	using KeyType = _KeyType;

	TreeMultiSetNode* parent = nullptr;
	TreeMultiSetNode* left = nullptr;
	TreeMultiSetNode* right = nullptr;
	TreeMultiSetNode* next = nullptr;

	KeyType key;
	char color = 'R';


	template<typename KT>
	TreeMultiSetNode(TreeMultiSetNode* _parent, KT&& _key) :
		parent(_parent), key(std::forward<KT>(_key)) {};

	TreeMultiSetNode(TreeMultiSetNode* _parent, TreeMultiSetNode* _other) : 
		parent(_parent), key(_other->key) {}; //Для insertNewNode


	void swap(TreeMultiSetNode* _other) 
	{
		std::swap(key, _other->key);
	}

	const KeyType& getKey() const noexcept { return key; };

	void nullifyNext() { next = nullptr; };

	TreeMultiSetNode* getNextEqual() noexcept
	{
		return next;
	}

	TreeMultiSetNode* getFirstEqual() noexcept
	{
		TreeMultiSetNode* temp = this;

		while (temp->parent && temp == temp->parent->next)
			temp = temp->parent;

		return temp;
	}

	TreeMultiSetNode* getLastEqual() noexcept
	{
		if (!next)
			return this;

		TreeMultiSetNode* temp = next;
		while (temp->next)
			temp = temp->next;

		return temp;
	}

	TreeMultiSetNode** getAddressOfLastEqualNextPtr() noexcept
	{
		return &(getLastEqual()->next);
	}
};


template<bool _Multi, typename _NodeType, typename _LessComp>
class BTS_NAMESPACE::BasicTreeSet : public BRBT_NAMESPACE::RedBlackTree<_Multi, _NodeType, _LessComp>
{
//Public Types----------------------------------------------------------------------------------------------------------------------------------
public:

	using KeyType = typename _NodeType::KeyType;
	using LessCompType = _LessComp;
	using IteratorType = TreeSetIterator<BasicTreeSet>;

//Private Types & Usings------------------------------------------------------------------------------------------------------------------------
private:
	
	using NodeType = _NodeType;
	using BasicTree = BRBT_NAMESPACE::RedBlackTree<_Multi, NodeType, LessCompType>;
	using BasicTree::innerSetInsert;
	using BasicTree::eraseSingleNode;
	using BasicTree::innerFind;
	using BasicTree::copyConstruct;
	using BasicTree::moveConstruct;
	using BasicTree::markEqualRange;
	using BasicTree::getLowerBound;
	using BasicTree::getUpperBound;
	using BasicTree::getBegin;
	using BasicTree::getBack;
	using BasicTree::beforeBeginPtr;
	using BasicTree::endPtr;

	friend class BD_ITER_NAMESPACE::BDIterator<BasicTreeSet>;
	friend class IteratorType;

//Protected Methods-----------------------------------------------------------------------------------------------------------------------------
protected:

	//Constructors------------------------------------------------------------------------------------------------------------------------------

	BasicTreeSet() : BasicTree() {};

	BasicTreeSet(std::initializer_list<KeyType> _initList) : BasicTree()
	{
		insert(_initList);
	};

	template<typename InputIterator>
	BasicTreeSet(InputIterator _first, InputIterator _last) : BasicTree()
	{
		insert(_first, _last);
	}

	BasicTreeSet(const BasicTreeSet& _other) : BasicTree()
	{
		copyConstruct(_other);
	}

	BasicTreeSet(BasicTreeSet&& _other) noexcept : BasicTree()
	{
		moveConstruct(_other);
	}

//Public Methods--------------------------------------------------------------------------------------------------------------------------------
public:

	//Insertion---------------------------------------------------------------------------------------------------------------------------------

	IteratorType insert(const KeyType& _key)
	{		
		return IteratorType(this, innerSetInsert(_key));
	}

	IteratorType insert(KeyType&& _key)
	{		
		return IteratorType(this, innerSetInsert(std::move(_key)));
	}

	void insert(std::initializer_list<KeyType> _initList)
	{
		for (auto&& elem : _initList)
			innerSetInsert(std::move(elem));
	}

	template<typename InputIterator>
	void insert(InputIterator _first, InputIterator _last)
	{
		while (_first != _last)
		{
			innerSetInsert(*_first);
			++_first;
		}

		innerSetInsert(*_last);
	}

	//Erasing-----------------------------------------------------------------------------------------------------------------------------------

	size_t erase(const KeyType& _key)
	{
		NodeType* res = innerFind(_key);

		if (res == endPtr)
			return 0;

		return eraseSingleNode(res);
	}

	size_t erase(IteratorType _iter) 
	{
		if (!_iter.isValid())
			return 0;

		return eraseSingleNode(_iter.node);
	}

	size_t erase(IteratorType _first, IteratorType _last)
	{
		if (!_first.isValid() || !_last.isValid() || !_first.hasSameContainer(_last))
			return 0;

		size_t count = 0;
		
		//Сперва нужно сохранить список нод(ключей для multi версии), т.к при удалении итераторы могут сломаться		
		if(_Multi)
		{
			std::vector<KeyType> vec;

			while (_first != _last)
			{
				vec.push_back(_first.node->getKey());
				++_first;
			}

			vec.push_back(_last.node->getKey());

			count = vec.size();

			for (const auto& key : vec)
				erase(key);
			
		}
		else
		{
			std::vector<NodeType*> vec;

			while (_first != _last)
			{
				vec.push_back(_first.node);
				++_first;
			}

			vec.push_back(_last.node);

			count = vec.size();

			for (auto node : vec)
				eraseSingleNode(node);
		}
		
		return count;
	}

	//Search------------------------------------------------------------------------------------------------------------------------------------

	IteratorType find(const KeyType& _key) const noexcept
	{
		return IteratorType(this, innerFind(_key));
	}

	std::pair<IteratorType, IteratorType> equalRange(const KeyType& _key) const noexcept
	{
		NodeType* first = nullptr;
		NodeType* last = nullptr;

		markEqualRange(_key, first, last);

		return { IteratorType(this, first), IteratorType(this, last) };
	}

	IteratorType lowerBound(const KeyType& _key) const noexcept
	{
		return IteratorType(this, getLowerBound(_key));
	}

	IteratorType upperBound(const KeyType& _key) const noexcept
	{
		return IteratorType(this, getUpperBound(_key));
	}

	//Iterator--------------------------------------------------------------------------------------------------------------------

	IteratorType beforeBegin() const noexcept
	{
		return IteratorType(this, beforeBeginPtr);
	}

	IteratorType begin() const noexcept
	{
		return IteratorType(this, getBegin());
	}

	IteratorType back() const noexcept
	{
		return IteratorType(this, getBack());
	}

	IteratorType end() const noexcept
	{
		return IteratorType(this, endPtr);
	}
};


template<typename KeyType, typename LessComp = std::less<KeyType>>
class TreeSet : public BTS_NAMESPACE::BasicTreeSet<false, BTS_NAMESPACE::TreeSetNode<KeyType>, LessComp>
{
	using BasicSet = BTS_NAMESPACE::BasicTreeSet<false, BTS_NAMESPACE::TreeSetNode<KeyType>, LessComp>;

//Public Methods--------------------------------------------------------------------------------------------------------------------------------
public:

	//Constructors------------------------------------------------------------------------------------------------------------------------------

	TreeSet() : BasicSet() {};

	TreeSet(std::initializer_list<KeyType>& _initList) : BasicSet(_initList) {};

	template<typename InputIterator>
	TreeSet(InputIterator _first, InputIterator _last) : BasicSet(_first, _last) {};

	TreeSet(const TreeSet& _other) : BasicSet(_other) {};

	TreeSet(TreeSet&& _other) noexcept : BasicSet(std::move(_other)) {};
};


template<typename KeyType, typename LessComp = std::less<KeyType>>
class TreeMultiSet : public BTS_NAMESPACE::BasicTreeSet<true, BTS_NAMESPACE::TreeMultiSetNode<KeyType>, LessComp>
{
	using BasicSet = BTS_NAMESPACE::BasicTreeSet<true, BTS_NAMESPACE::TreeMultiSetNode<KeyType>, LessComp>;

//Public Methods--------------------------------------------------------------------------------------------------------------------------------
public:

	//Constructors------------------------------------------------------------------------------------------------------------------------------

	TreeMultiSet() : BasicSet() {};

	TreeMultiSet(std::initializer_list<KeyType>& _initList) : BasicSet(_initList) {};

	template<typename InputIterator>
	TreeMultiSet(InputIterator _first, InputIterator _last) : BasicSet(_first, _last) {};

	TreeMultiSet(const TreeMultiSet& _other) : BasicSet(_other) {};

	TreeMultiSet(TreeMultiSet&& _other) noexcept : BasicSet(std::move(_other)) {};
};


template<typename Tree>
class TreeSetIterator : public BD_ITER_NAMESPACE::SetIterator<Tree>
{
	using NodeType = typename Tree::NodeType;

	using BasicIter = BD_ITER_NAMESPACE::SetIterator<Tree>;
	using BasicIter::cont;
	using BasicIter::node;
	
	friend class BTS_NAMESPACE::BasicTreeSet<false, NodeType, typename Tree::LessCompType>; //Для TreeSet
	friend class BTS_NAMESPACE::BasicTreeSet<true, NodeType, typename Tree::LessCompType>; //Для TreeMultiSet

public:

	TreeSetIterator(const Tree* _tree, NodeType* _node) :
		BasicIter(_tree, _node) {};

	
	TreeSetIterator& operator++() noexcept
	{
		cont->toNext(node);

		return *this;
	}

	TreeSetIterator operator++(int) noexcept
	{
		TreeSetIterator temp = *this;

		cont->toNext(node);

		return temp;
	}

	TreeSetIterator& operator--() noexcept
	{
		cont->toPrev(node);

		return *this;
	}

	TreeSetIterator operator--(int) noexcept
	{
		TreeSetIterator temp = *this;

		cont->toPrev(node);

		return temp;
	}
};


#endif // !_TREE_SET_H_

