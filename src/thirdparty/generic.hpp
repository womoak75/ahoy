#ifndef __GENERIC_HPP__
#define __GENERIC_HPP__

#include <stdio.h>
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>


template<typename T>
struct EntityIds;

class Entity
{
public:
    Entity(int tpid) : type_id(tpid) {}
    // need some virtual for polymorphism :/
    virtual void x() {}
    int type_id;
};

class IdEntity
{
public:
    IdEntity(int tpid) : type_id(tpid) {}
    virtual int getId() { return 0; }
    int type_id;
};

template <typename T, unsigned int tpid>
class Data : public Entity
{
public:
    Data(T &v) : Entity(tpid) , value(v) {}
    Data(T &&v) : Entity(tpid) , value(move(v)) {}
    Data(const T &v) : Entity(tpid) , value(v) {}
    T value;
};

template <typename T, int tpid>
class IdData : public Data<T,tpid> , public IdEntity
{
public:
    IdData(int id, T &v) : Data<T,tpid>(v) , IdEntity(tpid), _id(id){}
    IdData(int id, T &&v) : Data<T,tpid>((v)),  IdEntity(tpid), _id(id) {}
    IdData(const int id, const T &v) : Data<T,tpid>(v),  IdEntity(tpid), _id(id) {}
    int getId() { return _id; }
    int _id;

};

template <typename BASE>
class ContainerVector 
{

public:
 
    template <class U>
    void add(U &&v)
    {
        entities.push_back(std::make_shared<U>((v)));
    }

    template <typename T>
    void add(T &v)
    {
        entities.push_back(std::make_shared<T>((v)));
    }

    template <class T>
    void addList(T t)
    {
        add(t);
    }
    template <class U, class... T>
    void addList(U u, T... args)
    {
        add(u);
        addList(args...);
    }

    template <typename U>
    bool isType(int index)
    {
        return (EntityIds<U>::type_id == entities[index].get()->type_id);
    }

    BASE &get(int index)
    {
        auto v = (entities[index]);
        return *v.get();
    }

    template <typename U>
    U &getAs(int index)
    {
        auto v = std::static_pointer_cast<U>(entities[index]);
        return *v.get();
    }

    size_t getEntryCount() {
        return entities.size();
    }
    
    std::vector<std::shared_ptr<BASE>> entities;
};

template <typename KEY, typename BASE>
class ContainerMap 
{
public:
 
    template <class K, class U>
    void add(K &&key, U &&v)
    {
        map.insert({key,std::make_shared<U>((v))});
    }

    template <typename K, typename T>
    void add(K &key, T &v)
    {
        map.insert({key,std::make_shared<T>((v))});
    }

    template <typename T,typename K>
    bool isValueType(K key)
    {
        return (EntityIds<T>::type_id == map[key].get()->type_id);
    }
    template <typename K>
    bool hasKey(K key)
    {
        return (map.find(key) != map.end());
    }

    template <typename U,typename K>
    U &getValueAs(K key)
    {
        auto v = std::static_pointer_cast<U>(map[key]);
        return *v.get();
    }
    
    std::unordered_map<KEY,std::shared_ptr<BASE>> map;
};

#endif // __GENERIC_HPP__