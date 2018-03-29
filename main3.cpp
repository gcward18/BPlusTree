#include <iostream>
#include <cmath>
using namespace std;


template <typename T>
void swap_val(T& val1,T& val2){
    T temp = val1;
    val1 = val2;
    val2 = temp;

    return;
}

template <typename T>
void insertion_sort(T unsorted_array[], int size_of_array){
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

    Node(Node* src){

        this->leaf = true;
        this->maxVals = src->maxVals;
        this->actVals = src->actVals;
        this->maxPtrs = src->maxPtrs;
        this->actPtrs = src->actPtrs;
        this->key = new int[this->maxVals];
        this->pointer  = new Node*[this->maxPtrs];
        this->parent = NULL;

        for(int i = 0; i < src->maxVals; i++){
            this->key[i] = src->key[i];
            this->pointer [i] = src->pointer[i];
        }
        this->pointer[src->maxVals] = src->pointer[src->maxVals];

    }

    ~Node(){
        for(int i = 0; i < maxPtrs; i++)
            if(pointer[i] != NULL)
                delete pointer[i];
        delete[] pointer ;
        delete parent;
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
       // root->ClearTree(root);
        delete root->key;
    }

    Node* insert(Node* node, int input,int index=0){
        //Variables for betting understanding of the code
        int actual;
      //  int maximum;
        Node* temp = node;

        //Recursively try to find leaf node
        while(!isLeaf(temp)){
            temp = findLeafNode(node,input);
        }
        if(alreadyInserted(temp,input)){
            cout << "Value already in the tree, cannot insert "
            << input << endl;
            return node;
        }
        actual = temp->actVals;
        //This is a cause for a split
        if(isFull(temp)){
            Node* n1 = new Node();
            if(temp->parent != NULL){
                copyNodes(n1,temp->parent);
                for(int i= 0; i< n1->maxPtrs;i++){
                    if(n1->pointer[i] != NULL)
                    n1->pointer[i] = findLeafNode(n1->pointer[i],n1->key[i]);
                }
                temp = splitLeaf(temp,input);
                temp->leaf = false;
                temp = merge(temp,n1);
            }
            else{
                temp = splitLeaf(temp,input);
                temp->leaf = false;
            }
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
       // node = temp;
       // delete temp;
        return temp;
    }

    void remove(int val){

    }

    Node* splitLeaf(Node* node, int input){

        int actual  = node->actVals;
        int maximum = node->maxVals;
        int mid;
        Node *temp  = new Node(4);
        Node* n1    = new Node();
        Node* n2    = new Node();

        for(int i = 0; i < actual; i++)
            temp->key[i] = node->key[i];

        temp->key[actual] = input;
        insertion_sort(temp->key,maximum+1);
        mid = 2;

        //n1,n2 = destination; temp = src; mid,maximum+1 = positions
        copyLowerVals(n1,temp,mid);
        copyUpperVals(n2,temp,mid,maximum+1);
        clearRemainingKeys(node,0);

        //key at non leaf after split is last value in n1
        node->key[0]=n1->key[1];

        node->pointer[0] = n1;
        node->pointer[1] = n2;
        node->actVals    = 1;

        n1->leaf    = n2->leaf    = true;
        n1->parent  = n2->parent  = node;
        n1->actVals = n2->actVals =2;

        delete temp;
        return node;
    }

    Node* splitInternalNode(Node* trg){
        Node* temp = new Node();
        Node* n1   = new Node();
        Node* n2   = new Node();
        int mid = ceil(trg->actVals/2);
        int k = 0;
        n1->leaf=false;
        n2->leaf=false;

        //Copy lower keys and pointers
        for(int i = 0; i < mid; i++){
            n1->key[i] = trg->key[i];
            n1->pointer[i] = trg->pointer[i];
        }
        n1->pointer[mid] = trg->pointer[mid];

        //Copy upper keys and pointers
        for(int i= mid; i < trg->maxVals;i++){
            n2->key[k] = trg->key[i];
            n2->pointer[k] = trg->pointer[i+1];
            k++;
        }
        n2->pointer[k+1] = trg->pointer[trg->maxPtrs];

        n1->actVals = n2->actVals = 2;
        n1->parent = n2->parent = temp;

        temp->pointer[0] = n1;
        temp->pointer[1] = n2;
        temp->actVals    = 1;
        return temp;
    }

    Node* merge(Node* trg, Node* src){
        Node* temp = new Node(4);
        if(isFull(trg)){
            temp = makeOversizedNode(trg,src);
            trg = splitInternalNode(temp);
        }
        else{
            trg = cpyInternalNodes(trg,src);
        }

        trg->leaf=false;
        delete temp;

        return trg;
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

    void copyNodes(Node* trg, Node* src){
        for(int i = 0; i < trg->maxVals; i++){
            trg->key[i] = src->key[i];
            trg->pointer[i] = src->pointer[i];
            if(src->pointer[i]!= NULL){
                src->pointer[i] = findLeafNode(src->pointer[i],src->key[i-1]);
            }
        }
        trg->pointer[trg->maxPtrs] = src->pointer[trg->maxPtrs];
    }

    Node* cpyInternalNodes(Node* trg, Node* src){
        Node* temp = new Node();
        int k = 0;
        int j = 0;
        int srcInsertedPosition=0;

        for(int i = 0; i< temp->maxVals; i++){
            if(trg->key[k] < src->key[0] || src->key[i] == -1){
                temp->key[i] = trg->key[k];
                temp->pointer[i] = trg->pointer[k];
                k++;
            }

            else if(trg->key[k] > src->key[j] && src->key[j] != -1){
                temp->key[i] = src->key[j];
                temp->pointer[i] = src->pointer[j];
                srcInsertedPosition = k;
                if(srcInsertedPosition !=3)
                    trg->pointer[k] = src->pointer[1];
                j++;
            }
                            //MODIFIED WATCH THIS
            if(srcInsertedPosition==3)
                temp->pointer[i] = src->pointer[1];
        }
        return temp;
    }

    Node* makeOversizedNode(Node* trg, Node* src){
        Node* temp = new Node(4);
        int k =0;
        int srcInsertedPosition=0;

        for(int i = 0; i< temp->maxVals; i++){
            if(trg->key[k]<src->key[0]){
                temp->key[i] = trg->key[k];
                temp->pointer[i] = trg->pointer[k];
                k++;
            }
            else if(trg->key[k] > src->key[0]){
                temp->key[i] = src->key[0];
                temp->pointer[i] = src->pointer[0];
                srcInsertedPosition = k;
                if(srcInsertedPosition !=3)
                    trg->pointer[k] = src->pointer[1];
            }
        }

        if(srcInsertedPosition == 3)
            temp->pointer[temp->maxPtrs] = src->pointer[1];
        else
            temp->pointer[temp->maxPtrs] = trg->pointer[trg->maxPtrs];
        temp->actVals = temp->maxVals;
        return temp;
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

    bool isFull(Node* trg){
        if(trg->actVals == trg->maxVals)
            return true;
        else
            return false;
    }


void prettyPrint(Node* node, int pad=0)
{
    string s(pad, ' ');
    if (node == NULL)
    cout << endl;
    else
    {
    if(node->key[0]!= NULL){
        prettyPrint(node->pointer[0], pad+4);
    }
    if(node->key[1]!= NULL){
        cout << s << node->key[0] << endl;
        prettyPrint(node->pointer[1], pad+4);
    }
    if(node->key[2]!= NULL){
        cout << s << node->key[1] << endl;
        prettyPrint(node->pointer[2], pad+4);
    }
    if(node->key[3]!= NULL){
        cout << s << node->key[2] << endl;
        prettyPrint(node->pointer[3], pad+4);
    }

    }
    return;
}

  /*  void sortInternalNodes(Node* src){
        for(int i = 0; i < )
    }*/
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
            tree.insert(tree.root,input);
        }
       // tree.prettyPrint(tree.root,0);
    }
    cout << numNodes << endl;
    return 0;
}
