#ifndef _RED_BLACK_TREE_BASIC_H_
#define _RED_BLACK_TREE_BASIC_H_

#include <stack>
#include <utility>
#include <functional>

namespace BRBT_NAMESPACE
{
	template<bool Multi, typename NodeType, typename LessComp>
	class RedBlackTree;

	template<typename KeyType, typename Comparator>
	constexpr bool isValidComparator =
		noexcept (static_cast<bool>(std::declval<const Comparator&>() (std::declval<const KeyType&>(), std::declval<const KeyType&>())));

}

template<bool Multi, typename NodeType, typename LessComp>
class BRBT_NAMESPACE::RedBlackTree
{
	using KeyType = typename NodeType::KeyType;

//Included Structs------------------------------------------------------------------------------------------------------------------------------

	struct InsertBalancingTraits
	{
		NodeType* parent; 
		NodeType* grandParent; 
		NodeType* uncle;

		char uColor; 
		char nodeSide; 
		char parentSide;

		InsertBalancingTraits(NodeType* _node)
		{
			setNode(_node);
		}

		void setNode(NodeType* _node)
		{
			parent = _node->parent;
			grandParent = parent->parent;

			if (_node == parent->right)
				nodeSide = 'R';
			else 
				nodeSide = 'L';

			if (parent == grandParent->right)
			{
				parentSide = 'R';
				uncle = grandParent->left;
			}
			else
			{
				parentSide = 'L';
				uncle = grandParent->right;
			}

			if (uncle)
				uColor = uncle->color;
			else
				uColor = 'B';
		}
	};

	struct EraseBalancingTraits
	{
		NodeType* parent;
		NodeType* sibling;

		char nodeSide;
		char sColor;
		char lnColor;
		char rnColor;

		EraseBalancingTraits(NodeType* _parent, char _nodeSide)
		{
			setNode(_parent, _nodeSide);
		}

		void setNode(NodeType* _parent, char _nodeSide)
		{
			parent = _parent;
			nodeSide = _nodeSide;

			if (nodeSide == 'R')
				sibling = parent->left;
			else
				sibling = parent->right;

			if (sibling)
			{
				sColor = sibling->color;

				if (sibling->left)
					lnColor = sibling->left->color;
				else
					lnColor = 'B';

				if (sibling->right)
					rnColor = sibling->right->color;
				else
					rnColor = 'B';
			}
			else
			{
				sColor = lnColor = rnColor = 'B';
			}

		}
	};

//Properties(fields)----------------------------------------------------------------------------------------------------------------------------
protected:

	static constexpr bool isMulti = Multi;

	NodeType* root = nullptr;

	size_t elementCount = 0;

	LessComp comp;

	NodeType* beginPtr = nullptr;
	NodeType* backPtr = nullptr;
	NodeType* beforeBeginPtr;
	NodeType* endPtr;

//Protected Methods-----------------------------------------------------------------------------------------------------------------------------

	//Constructor And Destructor----------------------------------------------------------------------------------------------------------------

	RedBlackTree() 
	{
		if (isValidComparator<KeyType, LessComp>) {};

		beforeBeginPtr = reinterpret_cast<NodeType*>(new char[sizeof(NodeType)]);
		endPtr = reinterpret_cast<NodeType*>(new char[sizeof(NodeType)]);
	}

	~RedBlackTree() 
	{ 
		clear();

		delete[] reinterpret_cast<char*>(beforeBeginPtr);
		delete[] reinterpret_cast<char*>(endPtr);
	};

	//Specific Methods--------------------------------------------------------------------------------------------------------------------------

	void leftRotate(NodeType* _node) noexcept
	{
		NodeType* parent = _node->parent;
		NodeType* rightChild = _node->right;

		// ����� ��������� ������� �������, ���������� ������ ���������� �������� ����
		_node->right = rightChild->left;
		if (_node->right)
			_node->right->parent = _node;

		// ������� ���� ���������� ����� ���������� ������� �������
		rightChild->left = _node;
		_node->parent = rightChild;

		if (!parent)
		{
			root = rightChild;
			root->parent = nullptr;
			return;
		}

		// ������ ������� ���������� ���������� �������� �������� ����
		if (parent->right == _node)
			parent->right = rightChild;
		else
			parent->left = rightChild;

		rightChild->parent = parent;
	}

	void rightRotate(NodeType* _node) noexcept
	{
		NodeType* parent = _node->parent;
		NodeType* leftChild = _node->left;

		// ������ ��������� ������ ������� ���������� ����� ���������� �������� ����
		_node->left = leftChild->right;
		if (_node->left)
			_node->left->parent = _node;

		// ������� ���� ���������� ������ ���������� ������ �������
		leftChild->right = _node;
		_node->parent = leftChild;

		if (!parent)
		{
			root = leftChild;
			root->parent = nullptr;
			return;
		}

		// ����� ������� ���������� ���������� �������� �������� ����
		if (parent->right == _node)
			parent->right = leftChild;
		else
			parent->left = leftChild;

		leftChild->parent = parent;
	}

	void insertBalancing(NodeType* _node) noexcept
	{
		// ���� ����� ���� ���� ������, ������ ������������� ��� � ������
		if (_node == root)
		{
			_node->color = 'B';
			return;
		}

		if (isMulti && !comp(_node->getKey(), _node->parent->getKey()) && !comp(_node->parent->getKey(), _node->getKey()))
			return;

		// ���� �������� ������, �� ������ ������ �� ���������� � ������������ �� �����
		if (_node->parent->color == 'B')
			return;

		InsertBalancingTraits traits(_node);

		while (true)
		{
			if (traits.uColor == 'R')
			{
				// ���� �������� � ���� ����, ������ � ���� ���������� ������ ���, �.� ���� � ���� �������
				// ������������� ���� � ���� � ������
				traits.uncle->color = 'B';
				traits.parent->color = 'B';

				// ���� ��� ������, ���������� ������������ �� ���������
				if (traits.grandParent == root)
					return;

				// � ��������� ������, ������������� ���� � �������
				traits.grandParent->color = 'R';

				// ���� ���� ���� ������, ������������ ���������
				if (traits.grandParent->parent->color == 'B')
					return;

				// � ��������� ������, ��������� ������������ ��� ����
				traits.setNode(traits.grandParent);
				continue;
			}

			// ���� ������� ���� � ��� ����, ����� �� ������ �������� ������������ ����� ���������
			// ������ ��������������� ������� ������ ���� �������� ����
			if (traits.nodeSide != traits.parentSide)
			{
				if (traits.nodeSide == 'R')
					leftRotate(traits.parent);
				else
					rightRotate(traits.parent);

				// ��������� ���� � ��� �������� ���������� �������
				traits.setNode(traits.parent);
			}

			// ���� �������� ����, ������ ���� � ��� ���� ����� �� ����� ������� ������������ ����� ���������
			// ������ ������� ������ ���� �� ������� ����, ������������� ���� � ������, � ��� ����� ����� � �������			
			if (traits.parentSide == 'R')
				leftRotate(traits.grandParent);
			else
				rightRotate(traits.grandParent);

			traits.parent->color = 'B';
			traits.parent->left->color = 'R';
			traits.parent->right->color = 'R';

			return;
		}
	}

	void eraseBalancing(EraseBalancingTraits& _traits) noexcept 
	{
		// ��������� �� ������ ������������ ������� ���� ��� ������, _traits.setNode(NodeType* _parent, char _nodeSide) 
		// ���������� ��������� �� �������� � ���������� � ��������� �������� ����

		while (true)
		{
			// ���� ���� �������, ������������� ��� � ������, � �������� � �������
			// ������ ������� ������ �������� �� ������� ����� � ���������� ������������ ������������ �������� ����
			if (_traits.sColor == 'R')
			{
				_traits.sibling->color = 'B';
				_traits.parent->color = 'R';

				if (_traits.nodeSide == 'R')
					rightRotate(_traits.parent);            
				else
					leftRotate(_traits.parent); 

				// ��������� _traits 
				_traits.setNode(_traits.parent, _traits.nodeSide);

				continue;
			}

		//����� ���� ������--------------------------------------------------------------------------------------------

			// ���� ���� � ��� ��� ������� ������, ������������� ����� � �������
			// ���� �������� �������, ������������� ��� � ������ � ��������� ������������
			// � ��������� ������ ��������� ������������ ��� ��������
			if (_traits.lnColor == 'B' && _traits.rnColor == 'B')
			{
				_traits.sibling->color = 'R';

				if (_traits.parent->color == 'R')
				{
					_traits.parent->color = 'B'; 
					return;
				}

				//���� �������� ������, ��������� ������������
				if (_traits.parent == root) 
					return;

				char parentSide = (_traits.parent == _traits.parent->parent->left) ? 'L' : 'R';
				_traits.setNode(_traits.parent->parent, parentSide);

				continue;
			}

		//����� ���� ������ � ���� �� ��� ����� �������----------------------------------------------------------------
			
			// ���� ������� ��������� ����� �� ��������������� ������� �� �����
			// (�������� ���� - ������ �������, � ��� ����������� ������� ������� - �����) 
			// ������������� �������� ���������� � ������, � ����� � ������� � ������ ������� ������ ����� �� ������� �������� �������
			// ����� ����� ��������� _traits ������������ ������� ������� � �������� � ��������� ��������:

			// ���� ������� ��������� ����� �� ��� �� �������, ��� � ����
			// ������������� ����� � ���� ��������, � �������� � �������� ���������� � ������
			// ������ ������� ������ �������� �� ������� ����� � ��������� ������������

			if (_traits.nodeSide == 'L') // ���� ������
			{
				if (_traits.rnColor == 'B')
				{
					_traits.sibling->left->color = 'B';
					_traits.sibling->color = 'R';
					rightRotate(_traits.sibling); 

					// ��������� _traits 
					_traits.setNode(_traits.parent, _traits.nodeSide);
				}
				_traits.sibling->color = _traits.parent->color;
				_traits.parent->color = 'B';
				_traits.sibling->right->color = 'B';
				leftRotate(_traits.parent);
				
				return;
			}

			else // ���� �����
			{
				if (_traits.lnColor == 'B')
				{
					_traits.sibling->right->color = 'B';
					_traits.sibling->color = 'R';
					leftRotate(_traits.sibling);

					// ��������� _traits 
					_traits.setNode(_traits.parent, _traits.nodeSide);
				}
				_traits.sibling->color = _traits.parent->color;
				_traits.parent->color = 'B';
				_traits.sibling->left->color = 'B';
				rightRotate(_traits.parent); 

				return;
			}
		}
	}

	void updateBeginBack(NodeType* _node) noexcept
	{
		if (beginPtr)
		{
			if (comp(_node->getKey(), beginPtr->getKey()))
			{
				beginPtr = _node;
				return;
			}

			if (comp(backPtr->getKey(), _node->getKey()))
			{
				backPtr = _node;
				return;
			}

			if (isMulti)
			{
				if (_node == backPtr->getNextEqual()) // ���� _node ���� next �������� backPtr
					backPtr = _node;
			}			
		}
		else
		{
			beginPtr = backPtr = _node;
		}
	}

	//Construction------------------------------------------------------------------------------------------------------------------------------

	void copyConstruct(const RedBlackTree& _other)
	{
		if (!_other.root)
			return;

		std::stack<NodeType*> nodeStack; // ���� ��� �������� ����� ��� ������ ������
		NodeType* current = nullptr;

		nodeStack.push(_other.root);

		// ����� ������
		while (!nodeStack.empty())
		{
			current = nodeStack.top();
			nodeStack.pop();

			if (current->right)
				nodeStack.push(current->right);

			if (current->left)
				nodeStack.push(current->left);

			insertNewNode(current);

			if (isMulti)
			{
				NodeType* next = nullptr;
				NodeType* prev = nullptr;

				next = current->getNextEqual();

				while (next)
				{
					prev = next;
					next = next->getNextEqual();

					insertNewNode(prev);
				}
			}
		}
	}

	void moveConstruct(RedBlackTree& _other) noexcept
	{
		root = _other.root;
		elementCount = _other.elementCount;

		_other.root = nullptr;
		_other.elementCount = 0;
		_other.beginPtr = _other.backPtr = nullptr;
	}

	//Search------------------------------------------------------------------------------------------------------------------------------------

	NodeType* innerFind(const KeyType& _key) const noexcept
	{
		NodeType* seeker = root; // �������� ����� � �����

		while (seeker)
		{
			if (comp(_key, seeker->getKey()))
			{
				seeker = seeker->left;
				continue;
			}

			if (comp(seeker->getKey(), _key))
			{
				seeker = seeker->right;
				continue;
			}
			
			return seeker; // ����� seeker->getKey() == _key
		}

		return endPtr;
	}

	void markEqualRange(const KeyType& _key, NodeType*& _first, NodeType*& _last) const noexcept
	{
		_first = _last = innerFind(_key);

		if (_first == endPtr)
			return;
		
		while (_last->getNextEqual())
			_last = _last->getNextEqual();
	}

	NodeType* getLowerBound(const KeyType& _key) const noexcept
	{
		if (!root)
			return endPtr;

		NodeType* seeker = root;

		//���� ������ ������� � ������ �� ������ _key
		while (true)
		{
			if (comp(_key, seeker->getKey()))
			{
				if (!seeker->left)
					return seeker;

				seeker = seeker->left;
				continue;
			}

			if (comp(seeker->getKey(), _key))
			{
				if (!seeker->right)
					return endPtr;

				seeker = seeker->right;
				continue;
			}

			return seeker; // ����� seeker->getKey == _key
		}
	}

	NodeType* getUpperBound(const KeyType& _key) const noexcept
	{		
		NodeType* res = getLowerBound(_key);

		if (res == endPtr)
			return endPtr;

		//���� ������ ������� � ������ ������ _key
		while (res != endPtr && !comp(_key, res->getKey()))
			toNext(res);

		return res;
	}

	//Insertion---------------------------------------------------------------------------------------------------------------------------------

	std::pair<NodeType*, NodeType**> findPosToInsert(const KeyType& _key) noexcept
	{
		if (!root)
			return { nullptr, &root };

		NodeType* seeker = root;
		
		while (true)
		{
			if (comp(_key, seeker->getKey()))
			{
				if (!seeker->left)
					return { seeker, &seeker->left };

				seeker = seeker->left;
				continue;
			}

			if (comp(seeker->getKey(), _key))
			{
				if (!seeker->right)
					return { seeker, &seeker->right };

				seeker = seeker->right;
				continue;
			}

			//����� seeker->getKey() == _key
			if (isMulti)
				return { seeker->getLastEqual(), seeker->getAddressOfLastEqualNextPtr()};
			else
				return { nullptr, nullptr };
		}
	}

	template<typename KT>
	NodeType* innerSetInsert(KT&& _key) 
	{
		std::pair<NodeType*, NodeType**> pos = findPosToInsert(_key);
		
		if (pos.second)
		{
			*pos.second = new NodeType(pos.first, std::forward<KT>(_key));

			updateBeginBack(*pos.second);
			
			++elementCount;
			insertBalancing(*pos.second);

			return *pos.second;
		}

		return endPtr;
	}
	
	template<typename KT, typename VT>
	NodeType* innerMapInsert(KT&& _key, VT&& _value)
	{
		std::pair<NodeType*, NodeType**> pos = findPosToInsert(_key);

		if (pos.second)
		{
			*pos.second = new NodeType(pos.first, std::forward<KT>(_key), std::forward<VT>(_value));

			updateBeginBack(*pos.second);

			++elementCount;
			insertBalancing(*pos.second);

			return *pos.second;
		}

		return endPtr;
	}

	template<typename Pair>
	NodeType* innerMapPairInsert(Pair&& _pair)
	{
		std::pair<NodeType*, NodeType**> pos = findPosToInsert(_pair.first);

		if (pos.second)
		{
			*pos.second = new NodeType(pos.first, std::forward<Pair>(_pair));

			updateBeginBack(*pos.second);

			++elementCount;
			insertBalancing(*pos.second);

			return *pos.second;
		}

		return endPtr;
	}

	void insertExistNode(NodeType* _node) noexcept
	{
		std::pair<NodeType*, NodeType**> pos = findPosToInsert(_node->getKey());

		if (pos.second)
		{
			if (isMulti)
				_node->nullifyNext();

			_node->left = _node->right = nullptr;
			_node->color = 'R';

			*pos.second = _node;
			_node->parent = pos.first;
			
			updateBeginBack(_node);

			++elementCount;
			insertBalancing(_node);
		}
	}

	void insertNewNode(NodeType* _node)
	{
		std::pair<NodeType*, NodeType**> pos = findPosToInsert(_node->getKey());

		if(pos.second)
		{
			*pos.second = new NodeType(pos.first, _node);

			updateBeginBack(*pos.second);

			++elementCount;
			insertBalancing(*pos.second);
		}
	}
	
	//Erasing-----------------------------------------------------------------------------------------------------------------------------------

	size_t eraseSingleNode(NodeType* _node)
	{
		if (isMulti)
		{
			// ���� � ���� ���� equal-�, ������� ��� � ���������, �� ����� ���������� ������
			if(_node->getNextEqual())
			{
				NodeType* erasingNode = _node->getLastEqual();

				_node->swap(erasingNode);

				if (erasingNode == backPtr)
					backPtr = erasingNode->parent;

				erasingNode->parent->nullifyNext();

				delete erasingNode;
				--elementCount;

				return 1;
			}
		}

		// ���� � ���������� ���� 2 �������, ���� ����� ������ ���� ������ ���������
		// �� ����� ���������� ������
		if (_node->left && _node->right)
		{
			NodeType* erasingNode = _node->left;
			while (erasingNode->right)
				erasingNode = erasingNode->right;

			_node->swap(erasingNode);

			_node = erasingNode;
		}

	// ����� _node ���������� ��������� ����------------------------------------------------

		if (_node == root)
		{
			if (_node->right)
			{
				root = _node->right;
				root->color = 'B';
				beginPtr = backPtr = _node->right;
			}
			else if (_node->left)
			{
				root = _node->left;
				root->color = 'B';
				beginPtr = backPtr = _node->left;
			}
			else
			{
				root = nullptr;
				beginPtr = backPtr = nullptr;
			}

			delete _node;
			--elementCount;

			return 1;
		}

		NodeType** parentPointerToNode = nullptr;
		char nodeSide;

		// ��������� beginPtr � backPtr
		// �������� �� end/beforeBegin �� ����� �.� � ������ ������� 2 ��������
		if (_node == beginPtr)
			toNext(beginPtr);

		else if (_node == backPtr)
			toPrev(backPtr);


		if (_node == _node->parent->right)
		{
			parentPointerToNode = &_node->parent->right;
			nodeSide = 'R';
		}
		else
		{
			parentPointerToNode = &_node->parent->left;
			nodeSide = 'L';
		}


		// ���� ��������� ���� �������, ������������ �� �����
		if (_node->color == 'R')
		{
			if (_node->left)
			{
				_node->left->parent = _node->parent;
				*parentPointerToNode = _node->left;
			}
			else if (_node->right)
			{
				_node->right->parent = _node->parent;
				*parentPointerToNode = _node->right;
			}
			else
			{
				*parentPointerToNode = nullptr;
			}

			delete _node;
			--elementCount;

			return 1;
		}

		// ����� _node ������ ����------------------------------------------------------------------------

		if (_node->left)
		{
			// ��� �������� ������� ���� � ������� ��������-������� ����� ������ ����������� �������
			if (_node->left->color = 'R')
			{
				_node->left->parent = _node->parent;
				*parentPointerToNode = _node->left;

				_node->left->color = 'B';

				delete _node;
				--elementCount;

				return 1;
			}

			EraseBalancingTraits traits(_node->parent, nodeSide);
			_node->left->parent = _node->parent;
			*parentPointerToNode = _node->left;

			delete _node;
			--elementCount;
			eraseBalancing(traits);

			return 1;
		}

		if (_node->right)
		{
			// ��� �������� ������� ���� � ������� ��������-������� ����� ������ ����������� �������
			if (_node->right->color = 'R')
			{
				_node->right->parent = _node->parent;
				*parentPointerToNode = _node->right;

				_node->right->color = 'B';

				delete _node;
				--elementCount;

				return 1;
			}

			EraseBalancingTraits traits(_node->parent, nodeSide);
			_node->right->parent = _node->parent;
			*parentPointerToNode = _node->right;

			delete _node;
			--elementCount;
			eraseBalancing(traits);

			return 1;
		}

		// ����� _node ������ ���� ��� �����

		EraseBalancingTraits traits(_node->parent, nodeSide);
		*parentPointerToNode = nullptr;

		delete _node;
		--elementCount;
		eraseBalancing(traits);

		return 1;
	}

	//Iterator functional-----------------------------------------------------------------------------------------------------------------------

	NodeType* getBegin() const noexcept
	{
		if (!root)
			return endPtr;

		return beginPtr;
	}

	NodeType* getBack() const noexcept
	{
		if (!root)
			return endPtr;

		return backPtr;
	}

	void toNext(NodeType*& _current) const noexcept
	{
		if (_current == endPtr)
			return;

		if (_current == backPtr)
		{
			_current = endPtr;
			return;
		}

		if (_current == beforeBeginPtr)
		{
			_current = getBegin(); // ��������� ����� ���� ������
			return;
		}

		if (isMulti)
		{
			if (_current->getNextEqual())
			{
				_current = _current->getNextEqual();
				return;
			}

			// ���� ��� ���������� ���� � equal ������, ����������� � �������
			_current = _current->getFirstEqual();
		}

		// ���� ����� ����� ���� ������� ���������
		if (_current->right)
		{
			_current = _current->right;
			
			while (_current->left)
				_current = _current->left;

			return;
		}
		
		// ���� ����� �� ���������
		NodeType* seeker = _current->parent;

		while (comp(seeker->getKey(), _current->getKey()))
			seeker = seeker->parent;

		_current = seeker;
	}

	void toPrev(NodeType*& _current) const noexcept
	{
		if (_current == beforeBeginPtr)
			return;

		if (_current == beginPtr)
		{
			_current = beforeBeginPtr;
			return;
		}

		if (_current == endPtr)
		{
			_current = getBack(); // ��������� ����� ���� ������
			return;
		}

		if (isMulti)
		{
			// ���� � �������� ����� �� ����, �� ���������� ����
			if (!comp(_current->getKey(), _current->parent->getKey()) && !comp(_current->parent->getKey(), _current->getKey()))
			{
				_current = _current->parent;
				return;
			}
		}

		//���� ����� ������ ���� ������ ���������
		if (_current->left)
		{
			_current = _current->left;

			while (_current->right)
				_current = _current->right;

			return;
		}

		//���� ����� �� ���������
		NodeType* seeker = _current->parent;

		while (comp(_current->getKey(), seeker->getKey()))
			seeker = seeker->parent;

		_current = seeker;
	};

//Public Methods--------------------------------------------------------------------------------------------------------------------------------
public:

	//Getting Properties------------------------------------------------------------------------------------------------------------------------

	size_t size() const noexcept
	{
		return elementCount;
	}

	bool isEmpty() const noexcept 
	{
		return (elementCount == 0) ? true : false;
	}

	//Modifying---------------------------------------------------------------------------------------------------------------------------------

	void clear()
	{
		if (!root)
			return;

		std::stack<NodeType*> nodeStack;
		NodeType* current = nullptr;

		nodeStack.push(root);

		while (!nodeStack.empty())
		{
			current = nodeStack.top();
			nodeStack.pop();

			if (current->right)
				nodeStack.push(current->right);

			if (current->left)
				nodeStack.push(current->left);

			if (isMulti)
			{
				NodeType* next = nullptr;
				NodeType* prev = nullptr;

				next = current->getNextEqual();

				while (next)
				{
					prev = next;
					next = next->getNextEqual();
				
					delete prev;
				}
			}
			
			delete current;
		}

		elementCount = 0;
		root = nullptr;
		beginPtr = backPtr = nullptr;
	}

	void swap(RedBlackTree& _other) noexcept
	{
		NodeType* tempRoot = root;
		NodeType* tempBeginPtr = beginPtr;
		NodeType* tempBackPtr = backPtr;
		size_t tempElementCount = elementCount;

		root = _other.root;
		beginPtr = _other.beginPtr;
		backPtr = _other.backPtr;
		elementCount = _other.elementCount;

		_other.root = tempRoot;
		_other.beginPtr = tempBeginPtr;
		_other.backPtr = tempBackPtr;
		_other.elementCount = tempElementCount;
	}

	void merge(RedBlackTree& _source) noexcept
	{
		if (!_source.root)
			return;

		std::stack<NodeType*> nodeStack;
		NodeType* current = nullptr;

		nodeStack.push(_source.root);

		while (!nodeStack.empty())
		{
			current = nodeStack.top();
			nodeStack.pop();

			if (current->right)
				nodeStack.push(current->right);

			if (current->left)
				nodeStack.push(current->left);

			if (isMulti)
			{
				NodeType* next = nullptr;
				NodeType* prev = nullptr;

				next = current->getNextEqual();

				while (next)
				{
					prev = next;
					next = next->getNextEqual();

					insertExistNode(prev);
				}
			}

			insertExistNode(current);
		}

		_source.elementCount = 0;
		_source.root = nullptr;
		_source.beginPtr = _source.backPtr = nullptr;
	}

	//Search------------------------------------------------------------------------------------------------------------------------------------

	bool contains(const KeyType& _key) const noexcept
	{
		return innerFind(_key) != endPtr;
	}

	size_t countByKey(const KeyType& _key) const noexcept
	{
		NodeType* current = innerFind(_key);
		size_t count = 0;

		if (current == endPtr)
			return 0;

		while (current)
		{
			++count;
			current = current->getNextEqual();
		}

		return count;
	}
};


#endif // !_RED_BLACK_TREE_BASIC_H_

