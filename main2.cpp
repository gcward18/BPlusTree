#include <iostream>
#include <cmath>
using namespace std;


template <typename T>
void swap_val(T& val1,T& val2)
{
    T temp = val1;
    val1 = val2;
    val2 = temp;

    return;
}

template <typename T>
void insertion_sort(T unsorted_array[], int size_of_array)
{
    int i = 1;
    int j = 0;
    while(i < size_of_array)
    {
        j = i -1;
        while((unsorted_array[j] > unsorted_array[j+1]) && (j >= 0))
        {
            swap_val(unsorted_array[j], unsorted_array[j+1]);
            j--;
        }
        i++;
    }
}

class Node{
    public:

        //Variable used for telling whether or not
        //the node you're at is a leaf
        bool   leaf;

        //Array of pointer  to used for internal nodes
        Node** pointer;

        Node* parent;

        //Dynamic array of elements that are contained
        //within the B+ Tree nodes
        int*   key;

        //Variables representing the maximum number
        //of pointers and elements able to be stored
        //in a particular node
        int    maxVals;
        int    maxPtrs;

        //Variables representing the actual number
        //of pointer  and elements that are currently
        //in the node
        int    actVals;
        int    actPtrs;

    //This is a constructor for both a standard
    //node of order 3 and an oversized node, to
    //be used when the split function is called
    Node(int input = 3){

        //Defaulting the key for a B+ Tree of p=num_vals
        leaf = true;
        maxVals = input;
        maxPtrs = maxVals+1;
        key = new int[maxVals];
        pointer = new Node*[maxVals+1];
        parent = NULL;
        actVals = 0;
        actPtrs = 0;

        //Default key to -1
        for(int i = 0; i < maxVals; i++){
            key[i] = -1;
        }

        //Default pointer  to NULL
        for(int i = 0; i < maxPtrs; i++){
            pointer [i] = NULL;
        }
    }

    Node(Node* node){

        leaf = true;
        maxVals = node->maxVals;
        actVals = node->actVals;
        maxPtrs = node->maxPtrs;
        actPtrs = node->actPtrs;
        key = new int[this->maxVals];
        pointer  = new Node*[this->maxPtrs];
        parent = NULL;

        for(int i = 0; i < node->maxVals; i++){
            key[i] = node->key[i];
            pointer [i] = node->pointer[i];
        }
    }

    ~Node(){
        for(int i= 0; i < maxPtrs; i++)
            ClearTree(pointer[i]);
        delete[] pointer ;
    }

    Node* ClearTree(Node* node){
        if(node == NULL)
            return NULL;

        for(int i = 0; i < maxVals; i++)
            ClearTree(node->pointer[i]);
        for(int i = 0; i < maxVals; i++)
            delete node->pointer[i];
        delete[] node->pointer ;

        return NULL;

    }
};



class BPlusTree
{
public:
    int numNodes;
    Node* root;

    BPlusTree(){
        numNodes = 0;
        root = new Node();
    }

    ~BPlusTree(){
        root->ClearTree(root);
        delete root->key;
    }

    int insert(Node* node, int input,int index=0){

        int key;
        //Variables for betting understanding of the code
        int actual = node->actVals;
        int maximum = node->maxVals;
        Node* temp = node;

        //Recusively try to find leaf node
        while(!isLeaf(temp)){
            temp = findLeafNode(node,input);
        }
        if(alreadyInserted(temp,input)){
            cout << "Value already in the tree, cannot insert "
            << input << endl;
            return -1;
        }

        //This is a cause for a split
        if(areEqual(actual, maximum)){
            split(temp,input,index);
cout << "NODE"<<endl;
            print(temp);
        }
        else {
            temp->key[actual]= input;
            insertion_sort(temp->key,actual+1);
            temp->actVals++;
            cout << "Printing Node that is being inserted into"<<endl;
            print(temp);

        }
        node = temp;
        return key;
    }

    void remove(int val)
    {

    }

    int split(Node* node, int input){
        Node* parent = node->parent;
        int actual  = node->actVals;
        int maximum = node->maxVals;
        int mid;
        int *temp  = new int(4);
        Node* n1    = new Node();
        Node* n2    = new Node();

        node->leaf  = false;
        for(int i = 0, i < actual, i++)
            temp[i] = node->key[i];
        temp[actual] = input;

        insertion_sort(temp,maximum+1);
        mid = ceil(actual/2);
        print(temp);
        //n1,n2 = destination; temp = src; mid,maximum+1 = positions
        node->pointer[0] = n1;
        node->pointer[1] = n2;
        for(int i = 0; i < mid; i++)
            n1->values[i] = node->values[i];
        for(int i = mid; i < maximum; i++)
            n2->values[i-mid] = node->values[i];
        node->values[0] = node->values[mid];
        for(int i = 1; i < maximum; i++)
            node->values[i] = -1;

        if(parent != NULL)
        {
            if(parent->maxVals == parent -> actVals)
                split(parent, node->values[0])
            parent->values[parent->actVals] = node->values[0];
            parent->pointers[parent->actVals] = n1;
            parent->pointers[parent->actVals + 1] = n2;

        }
        //copyLowerVals(n1,temp,mid);
        //copyUpperVals(n2,temp,mid,maximum+1);
        //clearRemainingKeys(node,index+1);
        cout << "CHILD 1"<<endl;
        print(n1);
        cout << "CHILD 2"<<endl;
        print(n2);
        node->key[0] = n1->key[n1->actVals-1];
        node->actVals =1;

        delete temp;
        return n2->key[0];
    }



    ///////////////////////////////////////////////
    //             HELPER FUNCITONS              //
    ///////////////////////////////////////////////
    void copyUpperVals(Node* trg, Node* src, int start, int stop){
        trg->actVals = stop-start;
        trg->actPtrs = stop-start+1;
        for(int i= 0; i < start; i++){
            trg->key[i]=src->key[start+i];
        }
    }

    void copyLowerVals(Node* trg, Node* src, int range){
        trg->actVals = range;
        trg->actPtrs = range+1;
        for(int i= 0; i < range; i++){
            trg->key[i] = src->key[i];
        }
    }

    void print(Node* src){
        for(int i = 0; i < src->maxVals; i++){
            cout << "i = " << i << ", node value = " << src->key[i] <<endl;
        }
        return;
    }

    bool isLeaf(Node* src){
        if(src->leaf == true)
            return true;
        else
            return false;
    }

    void clearRemainingKeys(Node* src, int start){
        for(int i= start-1; i < src->maxVals; i++)
            src->key[i] = -1;
    }

    bool areEqual(int a, int b){
        if(a==b)
            return true;
        else
            return false;
    }

    bool alreadyInserted(Node* src, int input){
        for(int i = 0; i < src->maxVals; i++){
            if(src->key[i] == input){
                return true;
            }
        }
        return false;
    }

    Node* findLeafNode(Node* src,int input){
      cout << "is leaf? "<< endl;
      if(!isLeaf(src)){
          if(input <= src->key[0]){
              src = findLeafNode(src->pointer[0],input);
          }
          else if(input <= src->key[1] || src->actVals == 1){
              src = findLeafNode(src->pointer[1],input);
          }
          else if(input <= src->key[2] || src->actVals == 2){
              src = findLeafNode(src->pointer[2],input);
          }
          else{
              src = findLeafNode(src->pointer[3],input);
          }

        }

        return src;

    }

    int findIndex(Node* src,int input,int index=0){
      int holder = index;
      if(!isLeaf(src)){
          if(input <= src->key[0]){
              holder = findIndex(src->pointer[0],input,0);
          }
          else if(input <= src->key[1] || src->actVals == 1){
              holder = findIndex(src->pointer[1],input,1);
          }
          else if(input <= src->key[2] || src->actVals == 2){
              holder = findIndex(src->pointer[2],input,2);
          }
          else{
              holder = findIndex(src->pointer[3],input,3);
          }

        }
      return holder;
    }
};

int main()
{
    BPlusTree tree;
    int numNodes = 0;
    int input = 0;
    cout << "Welcome to the B+ tree creator!" << endl;
    cout << "Please enter the starting key of the tree, type negative number when done:" << endl;
    while(input >= 0)
    {
        cin >> input;
        if(input > 999)
            cout << "Input must be 3 digits or less" << endl;
        else if(input >= 0)
        {
            root.insert(tree.root,input);
        }
    }
    cout << numNodes << endl;
    return 0;
}
