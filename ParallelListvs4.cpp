//Concurrent List
//Massiel Oviedo Sivincha
//EDA-lab

#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <vector>

using namespace std;
mutex mtx;
int g_num = 0;
const unsigned int kNumberLists = 3;

int get_random(int low, int high) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(low, high);
    return distribution(gen);
}

template <class T>
class nodo
{
public:
    T valor;
    bool visit = false;
    nodo* next;
    mutex node_mutex;
    nodo(T v, nodo* n = nullptr)
    {
        valor = v;
        next = n;
    }
    void locked() {
        node_mutex.lock();
    }
    void unlocked() {
        node_mutex.unlock();
    }
};

template <class T>
class LE
{
    nodo<T>* head;
    nodo<T>* tail;
    nodo<T>* head_mutex;
    nodo<T>* tail_mutex;

public:
    LE() {
        tail = new nodo<T>(99999999);
        head = new nodo<T>(-99999999);
        head->next = tail;
    }
    void print();
    //bool find(T v,nodo<T> *&pos);
    bool add(T v);
    bool del(T v);
    ~LE();
};

template <class T>
void LE<T>::print()
{
    cout << "Head->";
    for (nodo <T>* p = head; p; p = p->next)
        cout << p->valor << "->";
    cout << "NULL" << endl;
}

/*template <class T>
bool LE<T>::find(T v, nodo<T> *&pos)
{
    mtx.lock();
    pos = nullptr;
    nodo<T>* p =head;

    for(;p && p->valor<v;pos = p,p=p->next)
        ;
    if(p && p->valor == v)
        return true;
    return false;

    mtx.unlock();
}
*/
template <class T>
bool LE<T>::add(T v)
{
    bool found = true;
    
    while (found) {
        nodo<T>* pred = head;
        nodo<T>* curr = head->next;
        while (curr->valor < v) {
            pred = curr;
            curr = curr->next;

        }
        pred->locked();
        curr->locked();
        //es el valor opuesto de la variable
        //Si no esta marcado pred,curr y pred->next es igual a curr
        if (!pred->visit && !curr->visit && pred->next == curr)
        {
            if (curr->valor != v) {
                curr->unlocked();
                found = false;
            }
            else {
                nodo <T>* new_node = new nodo<T>(v);
                new_node->next = curr;
                pred->next = new_node;

                found = true;
            }
            pred->unlocked();
            curr->unlocked();
            return found;
        }
        pred->unlocked();
        curr->unlocked();
    }
    /*nodo<T>* pos_ant=head_mutex;
    pos_ant.lock();

        return true;
    }
    mtx.lock();
    nodo<T>* pos_ant;
    if (!find(v, pos_ant))
    {
        if (pos_ant)
            pos_ant->next = new nodo<int>(v, pos_ant->next);
        else
            head = new nodo<int>(v, head);

        //size_++;
        mtx.unlock();
        return true;
    }
    mtx.unlock();
    return false;*/
}

template <class T>
bool LE<T>::del(T v)
{
    bool found = true;
    
    //se repetira hasta q retorne o hasta q entre al if de visita
    while (found) {
        nodo<T>* pred = head;
        nodo<T>* curr = head->next;
        while (curr->valor < v) {
            pred = curr;
            curr = curr->next;

        }
        pred->locked();
        curr->locked();
        //es el valor opuesto de la variable
        //Si no esta marcado pred,curr y pred->next es igual a curr
        if (!pred->visit && !curr->visit && pred->next == curr)
        {
            if (curr->valor != v) {
                curr->unlocked();
                found = false;
            }
            else {
                curr->visit = true;
                pred->next = curr->next;
                curr->unlocked();
                /*nodo <T>* new_node=new nodo<T>(v);
                new_node->next=curr;
                pred->next=new_node;*/
                found = true;
            }
            pred->unlocked();
            //curr->unlocked();
            return found;
        }
        pred->unlocked();
        curr->unlocked();
    }
}

template <class T>
LE<T>::~LE()
{
    /* cout<<"Destruyendo lista..."<<endl;
     while(head)
    {
      cout<<head->valor<<" ";
       del(head->valor);
    }
    cout<<endl<<"Lista destruida";
    while(head)
        del(head->valor);*/
}

template <class T>
struct add_x {
    LE<T>* ptrm_;
    int min_, max_;

    add_x (int min, int max, LE<T>* ptrm) : min_(min), max_(max), ptrm_(ptrm) {}

    void operator()(int y) {
        for (int i = 0; i < y; i++) {
            x = get_random(min_, max_);
            cout << "number to add: " << x << endl;
            ptrm_->add(x);
        }
    };

private:
    int x;
};


template <class T>
struct del_x {
    LE<T>* ptrm_;
    int min_, max_;

    del_x (int min, int max, LE<T>* ptrm) : min_(min), max_(max), ptrm_(ptrm) {};

    void operator()(int y) {
        for (int i = 0; i < y; i++) {
            x = get_random(min_, max_);
            cout << "number to delete: " << x << endl;
            ptrm_->del(x);
        }
    };

private:
    int x;
};

/*template <class T>
struct find_x
{
    LE<T> * ptrm_;
    int min_,max_;

    find_x(int min,int max,LE<T>* ptrm) : min_(min),max_(max),ptrm_(ptrm){} ;

    void operator()(int y) {
        for (int i=0;i<y;i++){
            ptrm_->find(get_random(min_,max_));
        }
    };

private:
    int x;
};
*/
int main() {
    /*LE <int> lista;
    thread* Lists[2];
    Lists[0] = new thread(add_x<int>(0, 100, &lista), 100);
    Lists[1] = new thread(del_x<int>(0, 100, &lista), 100);
    Lists[0]->join();
    Lists[1]->join();
    
    cout<<"ah";
    lista.print();
    lista.add(5);
    lista.print();


    thread* Lists[2];

    //Lists[2] = new thread(find_x<int>(0,100,&lista),100);
    Lists[0]->join();
    Lists[1]->join();
    //Lists[2]->join();
    cout<<"ah";
    lista.print();
    lista.add(5);
    lista.print();
    lista.add(1);
    lista.print();
    */
    LE<int> list;
    vector<thread>A1;
    vector<thread>A2;

    for (int i = 0; i < 8; i++) {
        A1.push_back(thread(add_x<int>(1, 100, &list), 100));
        A2.push_back(thread(del_x<int>(1, 100, &list), 100));
    }

    for (int i = 0; i < 8; i++) {
        A1[i].join();
        A2[i].join();
    }

    return 0;
}