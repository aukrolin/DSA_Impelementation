#include <functional>
#include <utility>
#include <stdint.h>
#include <iostream>
#include <string>
#include <chrono>



#pragma GCC optimize(3)
/**
 * @file rb_tree.cpp
 * @brief This is a template of rb_tree.
 * Use array instead of pointer.
 * Can be used for map or set, multiset, etc.
 * @author Sea of Voices, CKHS INFOR 36th.
 * @date 2023/07/29
 */
using std::pair;


int x;

template <typename _Key, typename _Mapped, typename _Cmp = std::less<_Key>>
class Rb_tree {
/*
Red-Black-Tree 
requirement 1: Note is Black or Red
requirement 2: Root is Black
requirement 3: leafs are black(Nil is Black)
requirement 4: A red node does not have a red child.
requirement 5: Every path from a given node to any of its descendant NIL nodes goes through the same number of black nodes.
*/



#define BLACK 0
#define RED 1

#define LEFT  0
#define RIGHT 1
#define left  child[LEFT]
#define right child[RIGHT]

#define NIL 0
#define Nil 0
#define nil 0

#define Dir(N) ( (N) == ((N)->parent)->right ? RIGHT : LEFT )
private:
_Cmp cmp;
    struct Node {
    public:
        _Key key;
        _Mapped value;
        int8_t color;
        struct Node *parent, *child[2];
        
        Node() : parent(Nil), color(0) {
            child[0] = child[1] = Nil;
            color = RED;
        }
    };
    void  _Dir_Rotate_at_root(Node * P ,/*root of target subtree;*/ int8_t dir ) {
        Node* G = P->parent;
        Node* Q = P->child[dir^1];
        // if(Q==Nil) {std::cout << "Rotation No Q ERROR";}//Debug

        Node* B = Q->child[dir];

        // std::cout << (dir?"RIGHT":"LEFT");
        

        P->child[dir^1] = B; if(B) B->parent = P;
        Q->child[dir ] =P; P->parent =Q;
        Q->parent=G;        
        if (G) G->child[P == G->right ? RIGHT : LEFT] =Q; 
        else this->root = Q;//No GrandParent when P is Root 
        // return Q;

        return;
    }
    void _RBT_insert_fix(Node *N) {
    Insert_Case_1:
        Node * P = N->parent;
        // violating requirement 2 "Root is Black"

        if (P == Nil) {
            N->color = BLACK; 
            return;
        } 
        // std::cout << P->key<< ' ' << (P->color?"RED":"BLACK") << std::endl;

    Insert_Case_2:
        // Won't violate any requirement
        if (P->color == BLACK) {

            return;
        } 
    Insert_Case_3:
        //Now P muse be Red, so P Can't be Root

        /*
            For Case 3 P==U==RED , So G Can only be BLACK
            We Try draw Gen Parent Black (maintaining requirement 4 ) and draw G Red to Maintain requirement 5 which we're now violate after draw Gen Parent Black

            Now the Question is transform to fix (G) as a new node
        */

        int8_t Pdir = Dir(P);
        Node *G = P->parent;
        Node *U = G->child[Pdir ^ 1];
        if ( U!=Nil && U->color == RED) {

            P->color = BLACK;
            U->color = BLACK;
            G->color = RED;

            N = G;
            goto Insert_Case_1;
        }
    Insert_Case_4:
        /*
            For 
                - U is Black 
                - Dir(P) != Dir(N)
            we do Pdir-rotation on P to make Dir(P)==Dir(N) (Dir(P) == LEFT do left rotation on P)
            After Dir(P)==Dir(N) we can do Other process now
        */
        if (Dir(N) != Pdir) {

            _Dir_Rotate_at_root(P, Pdir); //Ndir ^ 1
            N=P;//after Rotation the Node we process become P
            P = G->child[Pdir]; //P = N; N has modified , for reduce declaring a var Tmp we just 
        }
        // U and G are not modified
    Insert_Case_5:
        /*
            Now Rotate and draw to compelte insertion
        */

        
        _Dir_Rotate_at_root(G, Pdir ^ 1);

        P->color = BLACK;
        G->color = RED;

        return;
    }
    void _RBT_delete_fix(Node *N) {
        int8_t Ndir;          // side of P on which N is located (∈ { LEFT, RIGHT })
        struct Node* P = N->parent;  // -> parent node of N
        struct Node* S;  // -> sibling of N
        struct Node* C;  // -> close   nephew
        struct Node* D;  // -> distant nephew

        Ndir = Dir(N);
        delete N;
    Delete_Case_1:
        // Case P is root
        if (P == NIL) {
            return;
        }
        Ndir = Dir(N);
        P->child[Ndir] = Nil;// delete N

    Delete_Case_2:
        S = P->child[Ndir^1];
        // if (S==Nil) {
        //     std::cout << "Delete Case 2 No S ERROR\n";
        // } Debug 
        D = S->child[Ndir^1];
        C = S->child[Ndir];

        if (S->color == RED) {  
            // S red && P+C+D black:
            // which requirement 4 prove it
            _Dir_Rotate_at_root(P,Ndir);
            S->color = BLACK;
            P->color = RED; 
            S = C;
            D = S->child[Ndir^1];
            C = S->child[Ndir];
        }

    Delete_Case_3:
    // from now on S is BLACK
        // std::cout << S->key;



        if (D == NIL || D->color == BLACK){
            if (C!=Nil && C->color == RED) {
    Delete_Case_4:
                // C red && S+D black:

                _Dir_Rotate_at_root(S,Ndir^1);
                S->color = RED;
                C->color = BLACK;
                D=S;
                S=C;
                //goto Delete_Case_6
            }
            else if(P->color == RED){
                //These else means S&C&D are BLACK
    Delete_Case_5:
                S->color = RED;
                P->color = BLACK;
                //NDir-side subtree get one black(P) when the other side didn't change
                return;
            }
            else {
                //ALL BLACk

                S->color = RED;
                N=P;
                P=N->parent;
                if  (P!=NIL){ 
                    Ndir = Dir(N);
                    goto Delete_Case_2;
                }
                else return;
            }
        }

    Delete_Case_6:

        _Dir_Rotate_at_root(P, Ndir);
        S->color = P->color;
        P->color =BLACK;
        D->color = BLACK;

        return;

    }
    _Mapped _val_with_key(_Key _k){
        return _find(_k)->value;
    }
    void _delete (Node *N){
        if (N->left && N->right) {

            Node* T = N->left;
            while (T&&T->right){
                T=T->right;
            }
            N->key = T->key;
            N->value = T->value;
            _delete(T);
            //two child case;
        } else if (N->left){

            N->left->parent = N->parent;
            N->left->color = BLACK;
            N->parent->child[Dir(N)] = N->left;//N
            delete N;
        } else if(N->right){
            N->right->parent = N->parent;
            N->right->color = BLACK;
            N->parent->child[Dir(N)] = N->right;//N
            delete N;
        }else{

            if (N->parent == Nil) {
                root = Nil;
                delete N;
                return;
            }
            if (N->color == RED){
                N->parent->child[Dir(N)] = NIL;
                delete N;
                return;
            }
            _RBT_delete_fix(N);
        }
    }

    inline Node* _find(_Key _k){
        if(!root) {
            return Nil;
        }
        Node *N = root;
        while (N) {
            if (_k == N->key) {

                return N;
            }
            else if (cmp(_k, N->key)) {
                if (N->left != Nil)N = N->left;
                else break;

            }
            else {
                if (N->right != Nil) N = N->right;
                else break;
            }

        }
        return Nil;

    }

    void print_tree() {
        /*
            Developer Debug
        */
        std::string s ;
        std::cout<<root->key;
        do{
            std::cin>>s;

            Node * N = _find(s);
            std::cout << N->key;
            std::cout << (N->color == RED ? "RED " : "BLACK ");

            if (N->left) {
                std::cout << N->left->key;    
                std::cout << (N->left->color == RED ? "RED " : "BLACK "); 
            }
            if (N->right){
                std::cout << N->right->key;
                std::cout << (N->right->color == RED ? "RED " : "BLACK ");
            }
        }while (s[0] != 'e');
            

    }


    bool _check_black_height(Node* node, int& black_count) {
        if (node == Nil) {
            // 如果是葉節點 (Nil)，則返回黑色節點數量 1
            black_count = 1;
            return true;
        }

        int left_black_count = 0, right_black_count = 0;

        // 檢查左子樹的黑色節點數量
        bool left_valid = _check_black_height(node->child[0], left_black_count);
        // 檢查右子樹的黑色節點數量
        bool right_valid = _check_black_height(node->child[1], right_black_count);

        if (!left_valid || !right_valid) {
            return false;  // 如果任何一個子樹不合法，直接返回 false
        }

        // 確保左右子樹的黑色節點數量相同
        if (left_black_count != right_black_count) {
            // std::cout << "Different black heights between left and right subtree!" << std::endl;
            return false;
        }

        // 根據當前節點的顏色，調整黑色節點的數量
        if (node->color == BLACK) {
            black_count = left_black_count + 1;  // 如果是黑色，則黑色節點數量 + 1
        } else { 
            if ((node->left != Nil && node->left->color == RED) || (node->right != Nil && node->right->color == RED)) return false;
            black_count = left_black_count;  // 紅色節點不增加黑色節點數量
        }

        return true;
    }


public:
    Node * root = Nil;
    int Size = 0;
    class Iterator {

        //actually st.top is cur node
        private: 
            Node* _Ptr;
            Node* _root;

        public :
        inline Iterator(Node* N, Node* root) {
            if(!N) _Ptr=Nil;
            else _Ptr = find_left(N);
            _root = root;
        }
        inline Node* find_left(Node* N){
            while (N->left) N=N->left;
            return N;
        }
        inline Node* find_right(Node* N){
            while (N->right) N=N->right;
            return N;
        }

        inline std::pair<_Key, _Mapped> operator*() {
            return {_Ptr->key, _Ptr->value};
        }
        inline std::pair<_Key, _Mapped>* operator->() {
            return new std::pair<_Key, _Mapped>(_Ptr->key, _Ptr->value);
        }

        Iterator& operator++() {//++it
            // printf("%d\n", _Ptr->right);

            if (!(_Ptr->right)) { // climb looking for right subtree
                Node* _Pnode;
                // printf("%d ",_Ptr->key);
                
                while ((_Pnode = _Ptr->parent) /*Only if Max*/&& _Ptr == _Pnode->right) {
                    
                    _Ptr = _Pnode; // ==> parent while right subtree
                }
                _Ptr = _Pnode; // ==> parent (head if end())
            } else {
                _Ptr = find_left(_Ptr->right); // ==> smallest of right subtree
            }
            return *this;
        }
        Iterator operator++(int) noexcept { //it++
            Iterator _Tmp = *this;
            ++*this;
            return _Tmp;
        }

        Iterator& operator--() {//--it
            if(!_Ptr){
                _Ptr = find_right(_root);//end() case
            }else if(!(_Ptr->left)){
                Node* _Pnode;
                while ((_Pnode = _Ptr->parent) && _Ptr == _Pnode->left) {
                    _Ptr = _Pnode; // ==> parent while right subtree
                }            
                _Ptr = _Pnode; // ==> parent if not head
            }else {
                _Ptr = find_right(_Ptr->left); // ==> largest of left subtree
            }
            return *this;
        }
        Iterator operator--(int) noexcept { //it--
            Iterator _Tmp = *this;
            --*this;
            return _Tmp;
        }
        Iterator& operator=(const Iterator& _it){
            _Ptr = _it._Ptr;
            return *this;
        }
        bool operator==(const Iterator& other) const noexcept{
            // printf("%d %d\n", _Ptr, other._Ptr);
            return _Ptr == other._Ptr;
        }
        bool operator!=(const Iterator& other) const noexcept{
            return !(*this == other);
        }
        inline void _setHere(Node *n){
            _Ptr=n;
        }    

    };

    inline Iterator begin() {
        return Iterator(root,root);  // 返回一个指向树根的迭代器
    }

    inline Iterator end() {
        return Iterator(Nil,root);  // 结束时，栈为空
    }

    Iterator lower_bound(_Key _k){
        Iterator it = begin();
        if(root == Nil) {
            it._setHere(Nil);

            return it;
        }
        if ( cmp(_k, it->first)){
            it._setHere(Nil);
            // std::cout << "Lower Bound find value less than begin ERROR";//DEBUG
            return it;
        }
        it._setHere(root);
        Node *N = root, *result = N;     
        while (N != Nil) {  
            if (_k == N->key) {
                return N; 
            } else if (cmp(_k, N->key)) {  
                N = N->left;
            } else {  
                N = N->right;
            }
        }

        return it;

    }
    int size(){
        return Size;
    }
    bool _is_valid_red_black_tree() {
        if (root == Nil) return true;  // 空樹是合法的

        // 檢查根節點是否是黑色
        if (root->color != BLACK) {
            // std::cout << "Root is not black!\n" ;
            return false;
        }


        // 檢查整棵樹的黑色高度是否一致
        int black_count = 0;
        return _check_black_height(root, black_count);
    }
    
    _Mapped& insert(_Key _k, _Mapped _val) {
        Node* T = new Node();
        T->key = _k, T->value = _val;
        T->color = RED;

        if(!root) {
            root= T;
            Size ++;
            _RBT_insert_fix(T);
            return T->value;
        }

        Node *N = root;
        while (N) {
            if (_k == N->key) {
                // printf("%d Already exist\n", _k);
                return N->value;
            }
            else if (cmp(_k, N->key)) {
                if (N->left != Nil)N = N->left;
                else break;

            }
            else {
                if (N->right != Nil) N = N->right;
                else break;
            }

        }
        
        if( cmp(_k, N->key) ) N->left = T;
        else N->right = T;
        T->parent = N;
        Size ++;

        _RBT_insert_fix(T);
        return T->value;
    }

    void erase(_Key _k){
        Node* N = _find(_k);
        if (N){
            _delete(N);
            Size--;
        } 
        // else std::cout << "No such Node" << std::endl; //Debug
    }
    inline  Iterator find(const _Key& key){
        Node * N = _find(key);
        Iterator it = begin();
        it._setHere(N);
        return it;
    }
    _Mapped& operator[](const _Key& key) {
        return insert(key, _Mapped());
    }

#undef BLACK
#undef RED
#undef LEFT
#undef RIGHT
#undef left
#undef right
#undef NIL
#undef Nil
#undef nil
#undef Dir

};
#include <map>

int main(){
    int n = (int)1e6;

    Rb_tree<int,int> m;
    
    int t=  time(0);
    srand(t*8/7);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i =0; i< n; i++){
        m[rand()%n] = rand()%n;
    }
    // if(!m._is_valid_red_black_tree()) std::cout << "Tree Broken" << std::endl; // debug
    // else std::cout << 1;
    for (int i =0 ;i < n; i+=2){
        m.erase(rand()%n);
    }
    // for (auto i : m) {`
    // }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    printf("Spent: %.2fms\n", duration.count() * 1000.0);


    // std::cout <<  Rb.size();
} 



// if(!_is_valid_red_black_tree()) std::cout << "Tree Broken" << std::endl; // debug