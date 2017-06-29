// MyMap.h

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.

#ifndef MYMAP_INCLUDED
#define MYMAP_INCLUDED

//#include <iostream>
//using namespace std;

template<typename KeyType, typename ValueType>
class MyMap
{
public:
    MyMap();            //constructor
    ~MyMap();           //destructor, deletes all of the trees nodes
    void clear();       //deletes all of the trees nodes producing an empty tree
    int size() const;   //returns size
    void associate(const KeyType& key, const ValueType& value);
    
    // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;
    
    // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
    }
    
    // C++11 syntax for preventing copying and assignment
    MyMap(const MyMap&) = delete;
    MyMap& operator=(const MyMap&) = delete;
    
private:
    struct Node
    {
        Node()
        {
            m_left = nullptr;
            m_right = nullptr;
        }
        KeyType m_key;
        ValueType m_value;
        Node* m_left;
        Node* m_right;
    };
    Node* m_root;       //empty tree when m_root == nullptr
    int m_size;
    void clearHelper(Node* t);
    ValueType* findHelper(Node* t, const KeyType& key) const;
    void associateHelper(Node* t, const KeyType& key, const ValueType& value);
};

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::MyMap()
{
    m_size = 0;
    m_root = nullptr;
}

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::~MyMap()
{
    clear();
}


template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clear()
{
    clearHelper(m_root);
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clearHelper(Node* t)
{
    if (t != nullptr)
    {
        clearHelper(t->m_left);         //clears left tree recursively
        clearHelper(t->m_right);        //clears right tree recursively
        delete t;
    }
}

template<typename KeyType, typename ValueType>
int MyMap<KeyType, ValueType>::size() const
{
    return m_size;
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    if (m_size == 0)
    {                                   //inserts root node
        Node* newGuy = new Node;
        newGuy->m_key = key;
        newGuy->m_value = value;
        m_root = newGuy;
        m_size++;
        return;
    }
    associateHelper(m_root, key, value);

}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::associateHelper(Node* t, const KeyType& key, const ValueType& value)
{
    //searches recurisvely for spot
    if (key == t->m_key)
    {
        t->m_value = value;             //if the value is there already, replace value
    }
    if (key < t->m_key)
    {
        if (t->m_left == nullptr)       //if the key is not there, add a node
        {
            Node* newGuy = new Node;
            newGuy->m_key = key;
            newGuy->m_value = value;
            t->m_left = newGuy;
            m_size++;
        }
        else
        {                               //check left node recursively
            associateHelper(t->m_left, key, value);
        }
    }
    else //(key > t->m_key)
    {
        if (t->m_right == nullptr)          //if the key isn ot there, add a node
        {
            Node* newGuy = new Node;
            newGuy->m_key = key;
            newGuy->m_value = value;
            t->m_right = newGuy;
            m_size++;
        }
        else
        {                               //recursively check right node
            associateHelper(t->m_right, key, value);
        }
    }
    
}

template<typename KeyType, typename ValueType>
const ValueType* MyMap<KeyType, ValueType>::find(const KeyType& key) const
{
    if (m_root != nullptr)
        return findHelper(m_root, key);
    return nullptr;
}

template<typename KeyType, typename ValueType>
ValueType* MyMap<KeyType, ValueType>::findHelper(Node* t, const KeyType& key) const
{
    if (key == t->m_key)
        return &(t->m_value);
    if (key < t->m_key)
    {
        if (t->m_left == nullptr)
        {
            return nullptr;                 //if the value is not there, return nullptr
        }
        else
        {
            return findHelper(t->m_left, key);      //check recursively left side
        }
    }
    else
    {
        if (t->m_right == nullptr)
        {
                return nullptr;
        }
        else
        {
                return findHelper(t->m_right, key);     //check right side recursively
        }
    }
}


#endif // MYMAP_INCLUDED


