#ifndef _TREE_MAP_H_
#define _TREE_MAP_H_

#include "RedBlackTree.h"
#include "BiderectionalIterators.h"

template<typename Tree>
class TreeMapIterator;

template<typename Tree>
class ConstTreeMapIterator;


namespace BTM_NAMESPACE
{
	template<typename _KeyType, typename _ValueType>
	struct TreeMapNode;

	template<typename _KeyType, typename _ValueType>
	struct TreeMultiMapNode;

	template<bool _Multi, typename _NodeType, typename _LessComp>
	class BasicTreeMap;
}


template<typename _KeyType, typename _ValueType>
struct BTM_NAMESPACE::TreeMapNode
{
	using KeyType = _KeyType;
	using ValueType = _ValueType;
	using PairType = std::pair<KeyType, ValueType>;

	TreeMapNode* parent = nullptr;
	TreeMapNode* left = nullptr;
	TreeMapNode* right = nullptr;

	PairType pair;
	char color = 'R';


	template<typename KT, typename VT>
	TreeMapNode(TreeMapNode* _parent, KT&& _key, VT&& _value) :
		parent(_parent), pair(std::forward<KT>(_key), std::forward<VT>(_value)) {};
	
	template<typename Pair>
	TreeMapNode(TreeMapNode* _parent, Pair&& _pair) :
		parent(_parent), pair(std::forward<Pair>(_pair)) {};

	TreeMapNode(TreeMapNode* _parent, TreeMapNode* _other) :
		parent(_parent), pair(_other->pair) {};


	void swap(TreeMapNode* _other)
	{
		std::swap(pair, _other->pair);
	}

	const KeyType& getKey() const noexcept { return pair.first; }

	void nullifyNext() {};

	TreeMapNode* getNextEqual() noexcept { return nullptr; }

	TreeMapNode* getFirstEqual() noexcept { return nullptr; }

	TreeMapNode* getLastEqual() noexcept { return nullptr; }

	TreeMapNode** getAddressOfLastEqualNextPtr() noexcept { return nullptr; };
};

template<typename _KeyType, typename _ValueType>
struct BTM_NAMESPACE::TreeMultiMapNode
{
	using KeyType = _KeyType;
	using ValueType = _ValueType;
	using PairType = std::pair<KeyType, ValueType>;

	TreeMultiMapNode* parent = nullptr;
	TreeMultiMapNode* left = nullptr;
	TreeMultiMapNode* right = nullptr;
	TreeMultiMapNode* next = nullptr;

	PairType pair;
	char color = 'R';


	template<typename KT, typename VT>
	TreeMultiMapNode(TreeMultiMapNode* _parent, KT&& _key, VT&& _value) :
		parent(_parent), pair(std::forward<KT>(_key), std::forward<VT>(_value)) {};

	template<typename Pair>
	TreeMultiMapNode(TreeMultiMapNode* _parent, Pair&& _pair) :
		parent(_parent), pair(std::forward<Pair>(_pair)) {};

	TreeMultiMapNode(TreeMultiMapNode* _parent, TreeMultiMapNode* _other) :
		parent(_parent), pair(_other->pair) {};


	void swap(TreeMultiMapNode* _other)
	{
		std::swap(pair, _other->pair);
	}

	const KeyType& getKey() const noexcept { return pair.first; }

	void nullifyNext() { next = nullptr; };

	TreeMultiMapNode* getNextEqual() noexcept
	{
		return next;
	}

	TreeMultiMapNode* getFirstEqual() noexcept
	{
		TreeMultiMapNode* temp = this;

		while (temp->parent && temp == temp->parent->next)
			temp = temp->parent;

		return temp;
	}

	TreeMultiMapNode* getLastEqual() noexcept
	{
		if (!next)
			return this;

		TreeMultiMapNode* temp = next;
		while (temp->next)
			temp = temp->next;

		return temp;
	}

	TreeMultiMapNode** getAddressOfLastEqualNextPtr() noexcept
	{
		return &(getLastEqual()->next);
	}
};


template<bool _Multi, typename _NodeType, typename _LessComp>
class BTM_NAMESPACE::BasicTreeMap : public BRBT_NAMESPACE::RedBlackTree<_Multi, _NodeType, _LessComp>
{
//Public Types---------------------------------------------------------------------------------------------------------------------
public:

	using KeyType = typename _NodeType::KeyType;
	using ValueType = typename _NodeType::ValueType;
	using PairType = std::pair<KeyType, ValueType>;
	using LessCompType = _LessComp;
	using IteratorType = TreeMapIterator<BasicTreeMap>;
	using ConstIteratorType = ConstTreeMapIterator<BasicTreeMap>;

//Private Types & Usings------------------------------------------------------------------------------------------------------------------------
private:

	using NodeType = _NodeType;
	using BasicTree = BRBT_NAMESPACE::RedBlackTree<_Multi, NodeType, LessCompType>;
	using BasicTree::innerMapInsert;
	using BasicTree::innerMapPairInsert;
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

	friend class BD_ITER_NAMESPACE::BDIterator<BasicTreeMap>;
	friend class IteratorType;
	friend class ConstIteratorType;

//Protected Methods----------------------------------------------------------------------------------------------------------------
protected:

	//Constuctors------------------------------------------------------------------------------------------------------------------

	BasicTreeMap() : BasicTree() {};

	BasicTreeMap(std::initializer_list<PairType> _initList) : BasicTree()
	{
		insert(_initList);
	};

	template<typename InputIterator>
	BasicTreeMap(InputIterator _first, InputIterator _last) : BasicTree()
	{
		insert(_first, _last);
	}

	BasicTreeMap(const BasicTreeMap& _other) : BasicTree()
	{
		copyConstruct(_other);
	}

	BasicTreeMap(BasicTreeMap&& _other) noexcept : BasicTree()
	{
		moveConstruct(_other);
	}

//Public Methods-----------------------------------------------------------------------------------------------------------------
public:

	//Insertion------------------------------------------------------------------------------------------------------------------

	IteratorType insert(const KeyType& _key, const ValueType& _value)
	{
		return IteratorType(this, innerMapInsert(_key, _value));
	}

	IteratorType insert(KeyType&& _key, const ValueType& _value)
	{
		return IteratorType(this, innerMapInsert(std::move(_key), _value));
	}

	IteratorType insert(const KeyType& _key, ValueType&& _value)
	{
		return IteratorType(this, innerMapInsert(_key, std::move(_value)));
	}

	IteratorType insert(KeyType&& _key, ValueType&& _value)
	{
		return IteratorType(this, innerMapInsert(std::move(_key), std::move(_value)));
	}

	IteratorType insert(const PairType& _pair)
	{
		return IteratorType(this, innerMapPairInsert(_pair));
	}

	IteratorType insert(PairType&& _pair)
	{
		return IteratorType(this, innerMapPairInsert(std::move(_pair)));
	}

	void insert(std::initializer_list<PairType> _initList)
	{
		for (auto&& elem : _initList)
			innerMapPairInsert(std::move(elem));
	}

	template<typename InputIterator>
	void insert(InputIterator _first, InputIterator _last)
	{
		while (_first != _last)
		{
			innerMapPairInsert(*_first);
			++_first;
		}

		innerMapPairInsert(*_last);
	}

	//Erasing---------------------------------------------------------------------------------------------------------------------

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

		if (_Multi)
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

	//Search----------------------------------------------------------------------------------------------------------------------

	IteratorType find(const KeyType& _key) noexcept
	{
		return IteratorType(this, innerFind(_key));
	}

	ConstIteratorType find(const KeyType& _key) const noexcept
	{
		return ConstIteratorType(this, innerFind(_key));
	}

	std::pair<IteratorType, IteratorType> equalRange(const KeyType& _key) noexcept
	{
		NodeType* first = nullptr;
		NodeType* last = nullptr;

		markEqualRange(_key, first, last);

		return { IteratorType(this, first), IteratorType(this, last) };
	}

	std::pair<ConstIteratorType, ConstIteratorType> equalRange(const KeyType& _key) const noexcept
	{
		NodeType* first = nullptr;
		NodeType* last = nullptr;

		markEqualRange(_key, first, last);

		return { ConstIteratorType(this, first), ConstIteratorType(this, last) };
	}

	IteratorType lowerBound(const KeyType& _key) noexcept
	{
		return IteratorType(this, getLowerBound(_key));
	}

	ConstIteratorType lowerBound(const KeyType& _key) const noexcept
	{
		return ConstIteratorType(this, getLowerBound(_key));
	}

	IteratorType upperBound(const KeyType& _key) noexcept
	{
		return IteratorType(this, getUpperBound(_key));
	}

	ConstIteratorType upperBound(const KeyType& _key) const noexcept
	{
		return ConstIteratorType(this, getUpperBound(_key));
	}

	//Iterator-------------------------------------------------------------------------------------------------------------------

	IteratorType begin() noexcept
	{
		return IteratorType(this, getBegin());
	}

	ConstIteratorType begin() const noexcept
	{
		return ConstIteratorType(this, getBegin());
	}

	IteratorType beforeBegin() noexcept
	{
		return IteratorType(this, beforeBeginPtr);
	}

	ConstIteratorType beforeBegin() const noexcept
	{
		return ConstIteratorType(this, beforeBeginPtr);
	}

	IteratorType back() noexcept
	{
		return IteratorType(this, getBack());
	}

	ConstIteratorType back() const noexcept
	{
		return ConstIteratorType(this, getBack());
	}

	IteratorType end() noexcept
	{
		return IteratorType(this, endPtr);
	}

	ConstIteratorType end() const noexcept
	{
		return ConstIteratorType(this, endPtr);
	}

	ConstIteratorType cbegin() const noexcept
	{
		return ConstIteratorType(this, getBegin());
	}

	ConstIteratorType cbeforeBegin() const noexcept
	{
		return ConstIteratorType(this, beforeBeginPtr);
	}

	ConstIteratorType cback() const noexcept
	{
		return ConstIteratorType(this, getBack());
	}

	ConstIteratorType cend() const noexcept
	{
		return ConstIteratorType(this, endPtr);
	}
};


template<typename KeyType, typename ValueType, typename LessComp = std::less<KeyType>>
class TreeMap : public BTM_NAMESPACE::BasicTreeMap<false, BTM_NAMESPACE::TreeMapNode<KeyType, ValueType>, LessComp>
{
	using BasicMap = BTM_NAMESPACE::BasicTreeMap<false, BTM_NAMESPACE::TreeMapNode<KeyType, ValueType>, LessComp>;

//Public Methods--------------------------------------------------------------------------------------------------------------------------------
public:

	using typename BasicMap::PairType;

	//Constructors------------------------------------------------------------------------------------------------------------------------------

	TreeMap() : BasicMap() {};

	TreeMap(std::initializer_list<PairType>& _initList) : BasicMap(_initList) {};

	template<typename InputIterator>
	TreeMap(InputIterator _first, InputIterator _last) : BasicMap(_first, _last) {};

	TreeMap(const TreeMap& _other) : BasicMap(_other) {};

	TreeMap(TreeMap&& _other) noexcept : BasicMap(std::move(_other)) {};
};

template<typename KeyType, typename ValueType, typename LessComp = std::less<KeyType>>
class TreeMultiMap : public BTM_NAMESPACE::BasicTreeMap<true, BTM_NAMESPACE::TreeMultiMapNode<KeyType, ValueType>, LessComp>
{
	using BasicMap = BTM_NAMESPACE::BasicTreeMap<true, BTM_NAMESPACE::TreeMultiMapNode<KeyType, ValueType>, LessComp>;

//Public Methods--------------------------------------------------------------------------------------------------------------------------------
public:

	using typename BasicMap::PairType;

	//Constructors------------------------------------------------------------------------------------------------------------------------------

	TreeMultiMap() : BasicMap() {};

	TreeMultiMap(std::initializer_list<PairType>& _initList) : BasicMap(_initList) {};

	template<typename InputIterator>
	TreeMultiMap(InputIterator _first, InputIterator _last) : BasicMap(_first, _last) {};

	TreeMultiMap(const TreeMultiMap& _other) : BasicMap(_other) {};

	TreeMultiMap(TreeMultiMap&& _other) noexcept : BasicMap(std::move(_other)) {};
};


template<typename Tree>
class TreeMapIterator : public BD_ITER_NAMESPACE::MapIterator<Tree>
{
	using NodeType = typename Tree::NodeType;

	using BasicIter = BD_ITER_NAMESPACE::MapIterator<Tree>;
	using BasicIter::cont;
	using BasicIter::node;

	friend class BTM_NAMESPACE::BasicTreeMap<false, NodeType, typename Tree::LessCompType>; //Для TreeMap 
	friend class BTM_NAMESPACE::BasicTreeMap<true, NodeType, typename Tree::LessCompType>; //Для TreeMultiMap

public:

	TreeMapIterator(Tree* _tree, NodeType* _node) :
		BasicIter(_tree, _node) {};


	TreeMapIterator& operator++() noexcept
	{
		cont->toNext(node);

		return *this;
	}

	TreeMapIterator operator++(int) noexcept
	{
		TreeMapIterator temp = *this;

		cont->toNext(node);

		return temp;
	}

	TreeMapIterator& operator--() noexcept
	{
		cont->toPrev(node);

		return *this;
	}

	TreeMapIterator operator--(int) noexcept
	{
		TreeMapIterator temp = *this;

		cont->toPrev(node);

		return temp;
	}
};

template<typename Tree>
class ConstTreeMapIterator : public BD_ITER_NAMESPACE::ConstMapIterator<Tree>
{
	using NodeType = typename Tree::NodeType;

	using BasicIter = BD_ITER_NAMESPACE::ConstMapIterator<Tree>;
	using BasicIter::cont;
	using BasicIter::node;

	friend class BTM_NAMESPACE::BasicTreeMap<false, NodeType, typename Tree::LessCompType>; //Для TreeMap 
	friend class BTM_NAMESPACE::BasicTreeMap<true, NodeType, typename Tree::LessCompType>; //Для TreeMultiMap

public:

	ConstTreeMapIterator(const Tree* _tree, NodeType* _node) :
		BasicIter(_tree, _node) {};

	
	ConstTreeMapIterator& operator++() noexcept
	{
		cont->toNext(node);

		return *this;
	}

	ConstTreeMapIterator operator++(int) noexcept
	{
		ConstTreeMapIterator temp = *this;

		cont->toNext(node);

		return temp;
	}

	ConstTreeMapIterator& operator--() noexcept
	{
		cont->toPrev(node);

		return *this;
	}

	ConstTreeMapIterator operator--(int) noexcept
	{
		ConstTreeMapIterator temp = *this;

		cont->toPrev(node);

		return temp;
	}
};


#endif // !_TREE_MAP_H_

