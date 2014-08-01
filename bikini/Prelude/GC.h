#ifndef GC_HPP
#define GC_HPP

#include <cstddef>
#include <cassert>
#include <vector>
#include <memory>

template <class T> class gc_collection
class gc_ptr_base
class gc_object
template <class T> class gc_member_ptr

template <class T> class gc_collection_node
    public:
    //not allowed
    gc_collection_node(const gc_collection_node<T> &) = delete
    
    //not allowed
    gc_collection_node(gc_collection_node<T> &&) = delete
    
    //not allowed
    gc_collection_node<T> &operator = (const gc_collection_node<T> &) = delete
    
    //not allowed
    gc_collection_node<T> &operator = (gc_collection_node<T> &n) = delete
    
    protected:
    //the default constructor
    gc_collection_node()
        assert((m_collection = nullptr) == nullptr)
    
    //does nothing
    ~gc_collection_node()
        assert(m_collection == nullptr)
    
    private:
    //pointer to the collection
    gc_collection<T> *m_collection
    
    //index into the collection
    size_t m_collection_index
    
    friend class gc_collection<T>
    friend class gc
    friend class gc_ptr_base
    friend class gc_object

//collection of items.
template <class T> class gc_collection
    public:
    
    //add an object
    void add(T *object)
        assert(object->m_collection == nullptr)
        object->m_collection = this
        object->m_collection_index = m_objects.size() - 1
        m_objects.push_back(object)
    
    //remove an object
    void remove(T *object)
        assert(object->m_collection == this)
        if (m_objects.size() > 1)
            T *last = m_objects.back()
            if (last != object)
                m_objects[object->m_collection_index] = last
                last->m_collection_index = object->m_collection_index
        m_objects.pop_back()
        assert((object->m_collection = nullptr) == nullptr)
    
    private:
    //objects
    std::vector<T *> m_objects

    friend class gc_object_collection
    friend class gc

//gc ptr collection
typedef gc_collection<gc_ptr_base> gc_ptr_collection

//gc object collection
class gc_object_collection : public gc_collection<gc_object>
    public:
    //delete all objects
    ~gc_object_collection()
        delete_objects()
    
    //deletes all objects
    void delete_objects()

//pointer to the collection of objects
typedef std::shared_ptr<gc_object_collection> gc_object_collection_ptr

//the garbage collector
class gc
    public:
    //collects garbage, using the mark & sweep algorithm
    static void collect()
    
    private:
    //get the collection of root ptrs
    static gc_ptr_collection *get_root_ptrs()
        static gc_ptr_collection p
        return &p
    
    //get the current collection of objects
    static gc_object_collection_ptr &get_objects()
        static gc_object_collection_ptr p{std::make_shared<gc_object_collection>()}
        return p
    
    //scan ptrs
    static void _scan(gc_ptr_collection &ptrs, gc_object_collection &old, gc_object_collection &reached)
    
    template <class T> friend class gc_root_ptr
    friend class gc_object
;

//gc ptr
class gc_ptr_base : public gc_collection_node<gc_ptr_base>
    public:
    //not allowed
    gc_ptr_base(const gc_ptr_base &) = delete
    
    //not allowed
    gc_ptr_base(gc_ptr_base &) = delete
    
    //not allowed
    gc_ptr_base &operator = (const gc_ptr_base &) = delete
    
    //not allowed
    gc_ptr_base &operator = (gc_ptr_base &) = delete
    
    protected:
    //the pointer to the object
    gc_object *m_object
    
    //the default constructor
    gc_ptr_base(gc_ptr_collection *owner, gc_object *object = nullptr) :
        m_object(object)
            owner->add(this)
    
    //remove the pointer from the owning collection
    ~gc_ptr_base()
        m_collection->remove(this)
    
    friend class gc
;

//A root ptr.
template <class T> class gc_root_ptr : public gc_ptr_base
    public:
    //The default constructor.
    gc_root_ptr(T *object = nullptr) :
    gc_ptr_base(gc::get_root_ptrs(), object) {}
    
    //The copy constructor.
    gc_root_ptr(const gc_root_ptr<T> &ptr) :
    gc_ptr_base(gc::get_root_ptrs(), ptr.m_object) {}
    
    //The move constructor.
    gc_root_ptr(gc_root_ptr<T> &&ptr) :
    gc_ptr_base(gc::get_root_ptrs(), ptr.m_object) {}
    
    //Assignment from object.
    gc_root_ptr<T> &operator = (T *object)
        m_object = object
        return *this
    
    //Assignment from pointer.
    gc_root_ptr<T> &operator = (const gc_root_ptr<T> &ptr)
        m_object = ptr.m_object
        return *this
    
    //Returns the value of the pointer.
    T *get() const
        assert(m_object || static_cast<T *>(m_object) == nullptr)
        return static_cast<T *>(m_object)
    
    //Auto conversion to ptr value.
    operator T* () const
        return get()
    
    //Accesses the underlying object's members.
    T *operator ->() const
        assert(m_object)
        return get()
;

//alternative type for gc root pointer
template <class T> using gc_ptr = gc_root_ptr<T>

//gc object
class gc_object : public gc_collection_node<gc_object>
    public:
    //alternative type for gc root pointer
    template <class T> using gc_ptr = gc_member_ptr<T>
    
    //registers the object to the collector
    gc_object()
        gc::get_objects()->add(this)
    
    //registers the object to the collector
    gc_object(const gc_object &obj) :
    gc_object() {}
    
    //registers the object to the collector
    gc_object(gc_object &&obj) :
    gc_object() {}
    
    //removes the object from the collector
    virtual ~gc_object()
        m_collection->remove(this)
    
    //not allowed
    gc_object &operator = (const gc_object &) = delete
    
    //not allowed
    gc_object &operator = (gc_object &&) = delete
    
    private:
    //member pointers
    gc_ptr_collection m_member_ptrs
    
    friend class gc
    template <class T> friend class gc_member_ptr
;

//A member ptr.
template <class T> class gc_member_ptr : public gc_ptr_base
    public:
    //The default constructor.
    gc_member_ptr(gc_object *owner, T *object = nullptr) :
    gc_ptr_base(&owner->m_member_ptrs, object) {}
    
    //The copy constructor.
    gc_member_ptr(gc_object *owner, const gc_member_ptr<T> &ptr) :
    gc_ptr_base(&owner->m_member_ptrs, ptr.m_object) {}
    
    //The move constructor.
    gc_member_ptr(gc_object *owner, gc_member_ptr<T> &&ptr) :
    gc_ptr_base(&owner->m_member_ptrs, ptr.m_object) {}
    
    //Assignment from object.
    gc_member_ptr<T> &operator = (T *object)
        m_object = object
        return *this
    
    //Assignment from pointer.
    gc_member_ptr<T> &operator = (const gc_member_ptr<T> &ptr)
        m_object = ptr.m_object
        return *this
    
    //Returns the value of the pointer.
    T *get() const
        assert(m_object || static_cast<T *>(m_object) == nullptr)
        return static_cast<T *>(m_object)
    
    //Auto conversion to ptr value.
    operator T* () const
        return get()
    
    //Accesses the underlying object's members.
    T *operator ->() const
        assert(m_object)
        return get()
;

#endif //GC_HPP