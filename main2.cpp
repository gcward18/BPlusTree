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

        //Dynamic array of elements that are contained
        //within the B+ Tree nodes
        int*   key;

        //Variables representing the maximum number
        //of pointer  and elements able to be stored
        //in a particular node
        int    maxVals;
        int    maxPtrs;

        //Variables representing the actual number
        //of pointer  and elements that are currently
        //in the node
        int    actVals;
        int    actPtrs;

    Node(){

        //Defaulting the key for a B+ Tree of p=4
        leaf = true;
        maxVals = 3;
        pointer  = new Node*[maxVals+1];
        key = new int[maxVals];
        actVals = 0;
        maxPtrs = maxVals+1;
        actPtrs = 0;

        //Set key to default -1, nothing has been
        //inserted yet
        for(int i = 0; i < maxVals; i++)
            key[i] = -1;

        //Defaultpointer to NULL
        for(int i= 0; i < maxPtrs; i++)
           pointer[i] = NULL;
    }

    //This is a constructor for an oversized node, to be
    //used when the split function is called
    Node(int input){

        //Defaulting the key for a B+ Tree of p=num_vals
        leaf = true;
        maxVals = input;
        pointer = new Node*[maxVals+1];
        key = new int[maxVals];
        actVals = 0;
        maxPtrs = input+1;
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

        //Recusively try to find leaf node
        while(!isLeaf(node)){
            if(input < node->key[0]){
                    key = insert(node->pointer[0],input,0);
                    //if(key != -1)
                      //something
            }
            else if(input >= node->key[0] && input < node->key[1]){
                    key = insert(node->pointer[1],input,1);
                    //if(key != -1)
                      //something
            }
            else if(input >= node->key[1] && input < node->key[2]){
                    key = insert(node->pointer[2],input,2);
                    //if(key != -1)
                      //something
            }
            else if(input >= node->key[2] ){
                    key = insert(node->pointer[3],input,3);
                    //if(key != -1)
                      //something
            }
            else
                cout << "ERROR"<<endl;
                return -1;
        }

        if(alreadyInserted(node,input)){
            cout << "Value already in the tree, cannot insert "
            << input << endl;
            return -1;
        }

        //This is a cause for a split
        if(areEqual(actual, maximum)){
            split(node,input,index);
            cout << "printing node"<<endl;
            print(node);
        }
        else {
            node->key[actual]= input;
            insertion_sort(node->key,actual+1);
            cout << "value inserted " << input << " now node looks like"<<endl;
            print(node);
            node->actVals++;
        }
        return key;
    }

    void remove(int val)
    {

    }

    int split(Node* node, int input, int index=0){

        int actual  = node->actVals;
        int maximum = node->maxVals;
        int mid     = actual/2;

        Node *temp  = new Node(4);
        Node* n1    = new Node();
        Node* n2    = new Node();
        node->leaf  = false;
        temp->key[actual] = input;

        for(int i = 0; i < actual; i++)
            temp->key[i] = node->key[i];

        insertion_sort(temp->key,maximum+1);
        mid = ceil(actual/2)+1;
        print(temp);
        //n1,n2 = destination; temp = src; mid,maximum+1 = positions
        copyLowerVals(n1,temp,mid);
        copyUpperVals(n2,temp,mid,maximum+1);
        cout << "n1 NODE:" <<endl;
        print(n1);
        cout << "n2 NODE:"<<endl;
        print(n2);

        clearRemainingKeys(node,index+1);
        node->key[0] = n2->key[0];
        node->actVals =1;

        delete temp;
        node->pointer[index] = n1;
        node->pointer[index+1] = n2;
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

};

int main()
{
    BPlusTree root;
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
            root.insert(root.root,input);
        }
    }
    cout << numNodes << endl;
    return 0;
}
