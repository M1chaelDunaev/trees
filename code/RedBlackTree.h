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

		// Левое поддерево правого ребенка, становится правым поддеревом текущего узла
		_node->right = rightChild->left;
		if (_node->right)
			_node->right->parent = _node;

		// Текущий узел становится левым поддеревом правого ребенка
		rightChild->left = _node;
		_node->parent = rightChild;

		if (!parent)
		{
			root = rightChild;
			root->parent = nullptr;
			return;
		}

		// Правый ребенок становится поддеревом родителя текущего узла
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

		// Правое поддерево левого ребенка становится левым поддеревом текущего узла
		_node->left = leftChild->right;
		if (_node->left)
			_node->left->parent = _node;

		// Текущий узел становится правым поддеревом левого ребенка
		leftChild->right = _node;
		_node->parent = leftChild;

		if (!parent)
		{
			root = leftChild;
			root->parent = nullptr;
			return;
		}

		// Левый ребенок становится поддеревом родителя текущего узла
		if (parent->right == _node)
			parent->right = leftChild;
		else
			parent->left = leftChild;

		leftChild->parent = parent;
	}

	void insertBalancing(NodeType* _node) noexcept
	{
		// Если новый узел стал корнем, просто перекрашиваем его в черный
		if (_node == root)
		{
			_node->color = 'B';
			return;
		}

		if (isMulti && !comp(_node->getKey(), _node->parent->getKey()) && !comp(_node->parent->getKey(), _node->getKey()))
			return;

		// Если родитель черный, то черная высота не изменилась и балансировка не нужна
		if (_node->parent->color == 'B')
			return;

		InsertBalancingTraits traits(_node);

		while (true)
		{
			if (traits.uColor == 'R')
			{
				// Если попадаем в этот кейс, значит у узла существует черный дед, т.к отец и дядя красные
				// Перекрашиваем отца и дядю в черный
				traits.uncle->color = 'B';
				traits.parent->color = 'B';

				// Если дед корень, дальнейшая балансировка не требуется
				if (traits.grandParent == root)
					return;

				// В противном случае, перекрашиваем деда в красный
				traits.grandParent->color = 'R';

				// Если отец деда черный, балансировка завершена
				if (traits.grandParent->parent->color == 'B')
					return;

				// В противном случае, запускаем балансировку для деда
				traits.setNode(traits.grandParent);
				continue;
			}

			// Если текущий узел и его отец, лежат на разных сторонах отностиельно своих родителей
			// Делаем соответствующий поворот вокруг отца текущего узла
			if (traits.nodeSide != traits.parentSide)
			{
				if (traits.nodeSide == 'R')
					leftRotate(traits.parent);
				else
					rightRotate(traits.parent);

				// Поскольку узел и его родитель поменялись местами
				traits.setNode(traits.parent);
			}

			// Если попадаем сюда, значит узел и его отец лежат на одной стороне относительно своих родителей
			// Делаем поворот вокруг деда со стороны отца, перекрашиваем отца в черный, а его новых детей в красный			
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
		// Поскольку на начало балансировки текущий узел уже удален, _traits.setNode(NodeType* _parent, char _nodeSide) 
		// Использует указатель на родителя и информацию о положении текущего узла

		while (true)
		{
			// Если брат красный, перекрашиваем его в черный, а родителя в красный
			// Делаем поворот вокруг родителя со стороны брата и продолжаем балансировку относительно текущего узла
			if (_traits.sColor == 'R')
			{
				_traits.sibling->color = 'B';
				_traits.parent->color = 'R';

				if (_traits.nodeSide == 'R')
					rightRotate(_traits.parent);            
				else
					leftRotate(_traits.parent); 

				// Обновляем _traits 
				_traits.setNode(_traits.parent, _traits.nodeSide);

				continue;
			}

		//Здесь брат черный--------------------------------------------------------------------------------------------

			// Если брат и оба его ребенка черные, перекрашиваем брата в красный
			// Если родитель красный, перекрашиваем его в черный и завершаем балансировку
			// В противном случае запускаем балансирвоку для родителя
			if (_traits.lnColor == 'B' && _traits.rnColor == 'B')
			{
				_traits.sibling->color = 'R';

				if (_traits.parent->color == 'R')
				{
					_traits.parent->color = 'B'; 
					return;
				}

				//Если родитель корень, завершаем балансировку
				if (_traits.parent == root) 
					return;

				char parentSide = (_traits.parent == _traits.parent->parent->left) ? 'L' : 'R';
				_traits.setNode(_traits.parent->parent, parentSide);

				continue;
			}

		//Здесь брат черный и один из его детей красный----------------------------------------------------------------
			
			// Если красный племянник лежит на противоположной стороне от брата
			// (например брат - правый ребенок, а его собственный красный ребенок - левый) 
			// Перекрашиваем красного племянника в черный, а брата в красный и делаем поворот вокруг брата со стороны красного ребенка
			// После этого обновляем _traits относительно текущей позиции и приходим к следующей ситуации:

			// Если красный племянник лежит на той же стороне, что и брат
			// Перекрашиваем брата в цвет родителя, а родителя и красного племянника в черный
			// Делаем поворот вокруг родителя со стороны брата и завершаем балансировку

			if (_traits.nodeSide == 'L') // Брат правый
			{
				if (_traits.rnColor == 'B')
				{
					_traits.sibling->left->color = 'B';
					_traits.sibling->color = 'R';
					rightRotate(_traits.sibling); 

					// Обновляем _traits 
					_traits.setNode(_traits.parent, _traits.nodeSide);
				}
				_traits.sibling->color = _traits.parent->color;
				_traits.parent->color = 'B';
				_traits.sibling->right->color = 'B';
				leftRotate(_traits.parent);
				
				return;
			}

			else // Брат левый
			{
				if (_traits.lnColor == 'B')
				{
					_traits.sibling->right->color = 'B';
					_traits.sibling->color = 'R';
					leftRotate(_traits.sibling);

					// Обновляем _traits 
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
				if (_node == backPtr->getNextEqual()) // Если _node стал next текущего backPtr
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

		std::stack<NodeType*> nodeStack; // Стек для хранения узлов при обходе дерева
		NodeType* current = nullptr;

		nodeStack.push(_other.root);

		// Обход дерева
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
		NodeType* seeker = root; // Начинаем поиск с корня

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
			
			return seeker; // Здесь seeker->getKey() == _key
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

		//Ищем первый элемент с ключом не меньше _key
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

			return seeker; // Здесь seeker->getKey == _key
		}
	}

	NodeType* getUpperBound(const KeyType& _key) const noexcept
	{		
		NodeType* res = getLowerBound(_key);

		if (res == endPtr)
			return endPtr;

		//Ищем первый элемент с ключом больше _key
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

			//Здесь seeker->getKey() == _key
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
			// Если у узла есть equal-ы, свапаем его с последним, он будет фактически удален
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

		// Если у удаляемого узла 2 потомка, ищем самый правый узел левого поддерева
		// он будет фактически удален
		if (_node->left && _node->right)
		{
			NodeType* erasingNode = _node->left;
			while (erasingNode->right)
				erasingNode = erasingNode->right;

			_node->swap(erasingNode);

			_node = erasingNode;
		}

	// Здесь _node фактически удаляемый узел------------------------------------------------

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

		// Обновляем beginPtr и backPtr
		// Проверка на end/beforeBegin не нужна т.к в дереве минимум 2 элемента
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


		// Если удаляемый узел красный, балансировка не нужна
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

		// Здесь _node черный узел------------------------------------------------------------------------

		if (_node->left)
		{
			// При удаление черного узла с красным ребенком-заменой нужно просто перекрасить ребенка
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
			// При удаление черного узла с красным ребенком-заменой нужно просто перекрасить ребенка
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

		// Здесь _node черный узел без детей

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
			_current = getBegin(); // Контейнер может быть пустым
			return;
		}

		if (isMulti)
		{
			if (_current->getNextEqual())
			{
				_current = _current->getNextEqual();
				return;
			}

			// Если нет следующего узла с equal ключом, возращаемся к первому
			_current = _current->getFirstEqual();
		}

		// Ищем самый левый узел правого поддерева
		if (_current->right)
		{
			_current = _current->right;
			
			while (_current->left)
				_current = _current->left;

			return;
		}
		
		// Идем вверх по родителям
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
			_current = getBack(); // Контейнер может быть пустым
			return;
		}

		if (isMulti)
		{
			// Если у родителя такой же ключ, он предыдущий узел
			if (!comp(_current->getKey(), _current->parent->getKey()) && !comp(_current->parent->getKey(), _current->getKey()))
			{
				_current = _current->parent;
				return;
			}
		}

		//Ищем самый правый узел левого поддерева
		if (_current->left)
		{
			_current = _current->left;

			while (_current->right)
				_current = _current->right;

			return;
		}

		//Идем вверх по родителям
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

