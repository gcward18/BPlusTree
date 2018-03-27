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

        //Array of Pointers to used for internal nodes
        Node** children;

        //Dynamic array of elements that are contained
        //within the B+ Tree nodes
        int*   values;

        //Variables representing the maximum number
        //of pointers and elements able to be stored
        //in a particular node
        int    max_num_of_vals;
        int    max_num_of_pointers;

        //Variables representing the actual number
        //of pointers and elements that are currently
        //in the node
        int    act_num_of_vals;
        int    act_num_of_pointers;

    Node(){

        //Defaulting the values for a B+ Tree of p=4
        leaf = true;
        max_num_of_vals = 3;
        children = new Node*[max_num_of_vals+1];
        values = new int[max_num_of_vals];
        act_num_of_vals = 0;
        max_num_of_pointers = max_num_of_vals+1;
        act_num_of_pointers = 0;

        //Set values to default -1, nothing has been
        //inserted yet
        for(int i = 0; i < max_num_of_vals; i++)
            values[i] = -1;

        //Default children to NULL
        for(int i= 0; i < max_num_of_pointers; i++)
            children[i] = NULL;
    }

    //This is a constructor for an oversized node, to be
    //used when the split function is called
    Node(int input){

        //Defaulting the values for a B+ Tree of p=num_vals
        leaf = true;
        max_num_of_vals = input;
        children = new Node*[max_num_of_vals+1];
        values = new int[max_num_of_vals];
        act_num_of_vals = 0;
        max_num_of_pointers = input+1;
        act_num_of_pointers = 0;

        //Default values to -1
        for(int i = 0; i < max_num_of_vals; i++){
            values[i] = -1;
        }

        //Default children to NULL
        for(int i = 0; i < max_num_of_pointers; i++){
            children[i] = NULL;
        }
    }

    Node(Node* root){

        leaf = true;
        max_num_of_vals = root->max_num_of_vals;
        act_num_of_vals = root->act_num_of_vals;
        max_num_of_pointers = root->max_num_of_pointers;
        act_num_of_pointers = root->act_num_of_pointers;
        values = new int[this->max_num_of_vals];
        children = new Node*[this->max_num_of_pointers];

        for(int i = 0; i < root->max_num_of_vals; i++){
            values[i] = root->values[i];
            children[i] = root->children[i];
        }
    }

    ~Node(){
        for(int i= 0; i < max_num_of_pointers; i++)
            ClearTree(children[i]);
        delete[] children;
    }

    Node* ClearTree(Node* node){
        if(node == NULL)
            return NULL;

        for(int i = 0; i < max_num_of_vals; i++)
            ClearTree(node->children[i]);
        for(int i = 0; i < max_num_of_vals; i++)
            delete node->children[i];
        delete[] node->children;

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
        delete root->values;
    }

    int insert(Node* root, int input,int index=0){
        int key;
        int actual;
        int maximum;

        //Recusively try to find leaf node
        while(!isLeaf(root)){
            cout << "current Node is not a Leaf Node"<<endl;
            if(input < root->values[0]){
                    insert(root->children[0],input,0);
            }
            else if(input >= root->values[0] && input < root->values[1]){
                    insert(root->children[1],input,1);
            }
            else if(input >= root->values[1] && input < root->values[2]){
                    insert(root->children[2],input,2);
            }
            else if(input >= root->values[2] ){
                    insert(root->children[3],input,3);
            }
            else
                cout << "ERROR"<<endl;
                return -1;
        }
        if(alreadyInserted(root,input)){
            cout << "Value already in the tree, cannot insert "
            << input << endl;
            return -1;
        }
        //Variables for betting understanding of the code
        actual = root->act_num_of_vals;
        maximum = root->max_num_of_vals;

        //This is a cause for a split
        if(areEqual(actual, maximum)){
            key = split(root,input,index);
            cout << "printing root"<<endl;
            print(root);
        }
        else {
            root->values[actual-1]= input;
            insertion_sort( root->values,actual);
            root->act_num_of_vals++;
        }
        return key;
    }

    void remove(int val)
    {

    }

    int split(Node* root, int input, int index=0){

            int actual  = root->act_num_of_vals;
            int maximum = root->max_num_of_vals;
            int mid     = actual/2;

            Node *temp  = new Node(4);
            Node* n1    = new Node();
            Node* n2    = new Node();
            root->leaf  = false;
            temp->values[actual-1] = input;

            for(int i = 0; i < actual-1; i++)
                temp->values[i] = root->values[i];

            insertion_sort(temp->values,maximum+1);
            mid = ceil(actual/2);

            copyLowerVals(n1,temp,mid);
            copyUpperVals(n2,temp,mid,maximum+1);
            cout << "printing n1: "<<endl;
            print(n1);
            cout << "printing n2: "<<endl;
            print(n2);

            clearRemainingKeys(root,index-1);
            root->values[0] = n2->values[0];
            root->act_num_of_vals =1;

            delete temp;
            root->children[0] = n1;
            root->children[1] = n2;
            return n2->values[0];
    }



    ///////////////////////////////////////////////
    //             HELPER FUNCITONS              //
    ///////////////////////////////////////////////
    void copyUpperVals(Node* n1, Node* root, int start, int stop){
        n1->act_num_of_vals     = stop-start;
        n1->act_num_of_pointers = stop-start+1;
        for(int i= 0; i < start; i++){
            n1->values[i]=root->values[start+i];
        }
    }

    void print(Node* n){
        for(int i = 0; i < n->max_num_of_vals; i++){
            cout << "i = " << i << ", root value = " << n->values[i] <<endl;
        }
        return;
    }

    bool isLeaf(Node* root){
        if(root->leaf == true)
            return true;
        else
            return false;
    }

    void clearRemainingKeys(Node* root, int start){
        for(int i= start-1; i < root->max_num_of_vals; i++)
            root->values[i] = -1;
    }

    bool areEqual(int a, int b){
        if(a==b)
            return true;
        else
            return false;
    }

    bool alreadyInserted(Node* root, int input){
        for(int i = 0; i < root->max_num_of_vals; i++){
            //Value has already been inserted, cannot
            //insert again
            if(root->values[i] == input){
                return true;
            }
        }
        return false;
    }

    void copyLowerVals(Node* n1, Node* root, int range){
        n1->act_num_of_vals     = range;
        n1->act_num_of_pointers = range+1;
        for(int i= 0; i < range; i++){
            n1->values[i]=root->values[i];
        }
    }

};

int main()
{
    BPlusTree root;
    int numNodes = 0;
    int input = 0;
    cout << "Welcome to the B+ tree creator!" << endl;
    cout << "Please enter the starting values of the tree, type negative number when done:" << endl;
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

