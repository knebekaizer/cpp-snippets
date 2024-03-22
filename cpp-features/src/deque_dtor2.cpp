    #include <iostream>
    #include <deque>

    int actual_elements_destroyed = 0;
    class TestClass {
    public:
        TestClass(int id) : m_id(id) {

            ++s_numConstructsCalled;
        }
        TestClass() {

            ++s_numConstructsCalled;
        }
        ~TestClass() {

            ++s_numDestructorsCalled;
        }

        int m_id;
        static int s_numDestructorsCalled;
        static int s_numConstructsCalled;
    };

    int TestClass::s_numDestructorsCalled = 0;
    int TestClass::s_numConstructsCalled = 0;

    template<typename T>
    class custom_allocator {
    public:
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        template<typename V>
        friend class custom_allocator;
        custom_allocator() : m_numAllocations(0) {}

        template<typename U>
        custom_allocator(const custom_allocator<U>& other) : m_numAllocations(other.m_numAllocations) {}

        T* allocate(std::size_t n) {
            pointer p = static_cast<pointer>(::operator new(n * sizeof(T)));
            ++m_numAllocations;
            return p;
        }

        void deallocate(T* p, std::size_t n) {
                ::operator delete(p);
                --m_numAllocations;
        }

        template<typename U, typename... Args>
        void construct(U* p, Args&&... args) {
            new(p) U(std::forward<Args>(args)...);
        }

        template<typename U>
        void destroy(U* p) {
                ++actual_elements_destroyed;
                p->~U(); 
        }

        template<typename U>
        bool operator==(const custom_allocator<U>& other) const {
            return m_numAllocations == other.m_numAllocations;
        }

        template<typename U>
        bool operator!=(const custom_allocator<U>& other) const {
            return !(*this == other);
        }

        std::size_t m_numAllocations;
        pointer m_start;
        pointer m_end;
    };


    int main() {
        int deque_size = 0;
        {
            using std_deque = std::deque<TestClass, custom_allocator<TestClass>>;
            std_deque myDeque(custom_allocator<TestClass>{});
            myDeque.push_back(1);
            myDeque.push_back(2);
            myDeque.push_back(3);
            deque_size = myDeque.size();
        }
        int numConstructsCalled = 0;
        numConstructsCalled = TestClass::s_numConstructsCalled;
        std::cout << "Number of elements within deque with size function: " << deque_size << std::endl;
        std::cout << "Number of elements constructed (counted from inner type): " << numConstructsCalled << std::endl;
        std::cout << "Number of elements destroyed (count from destroy of custom_allocator): " << actual_elements_destroyed << std::endl;

        return 0;
    }
    