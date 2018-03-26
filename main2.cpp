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
            cout << "swapped value: "<< unsorted_array[j] <<", "<<unsorted_array[j+1] <<endl;
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

        this->leaf = true;
        this->max_num_of_vals = root->max_num_of_vals;
        this->act_num_of_vals = root->act_num_of_vals;
        this->max_num_of_pointers = root->max_num_of_pointers;
        this->act_num_of_pointers = root->act_num_of_pointers;
        this->values = new int[this->max_num_of_vals];
        this->children = new Node*[this->max_num_of_pointers];

        for(int i = 0; i < root->max_num_of_vals; i++){
            this->values[i] = root->values[i];
            this->children[i] = root->children[i];
        }
    }

    ~Node(){
        for(int i= 0; i < max_num_of_pointers; i++)
            this->ClearTree(this->children[i]);
        delete[] this->children;
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

    bool areEqual(int a, int b){
        if(a==b)
            return true;
        else
            return false;
    }

    int split(Node* root, int input){

            int actual = root->act_num_of_vals;
            int maximum = root->max_num_of_vals;
            int mid = actual/2;

            Node *temp = new Node(4);
            Node* n1   = new Node();
            Node* n2   = new Node();
            root->leaf = false;
            temp->values[actual-1] = input;

            for(int i = 0; i < actual-1; i++)
                temp->values[i] = root->values[i];

            insertion_sort(temp->values,maximum+1);
            mid = ceil(actual/2);

            copyLowerVals(n1,temp,mid);
            copyUpperVals(n2,temp,mid,maximum+1);

            delete temp;

            root->children[0] = n1;
            root->children[1] = n2;
    }

    Node* insert(Node* root, int input){

        //Recusively try to find leaf node
        while(root->leaf == false){
            cout << "current Node is not a Leaf Node"<<endl;
            if(input > root->values[2])
                    insert(root->children[3],input);
            else if(input > root->values[1])
                    insert(root->children[2],input);
            else if(input > root->values[0])
                    insert(root->children[1],input);
            else
                    insert(root->children[0],input);
        }

        if(alreadyInserted(root,input)==true){
            cout << "Value already in the tree, cannot insert "
            << input << endl;
            return root;
        }
        //Variables for betting understanding of the code
        root->act_num_of_vals++;
        int actual = root->act_num_of_vals;
        int maximum = root->max_num_of_vals;

        //This is a cause for a split
        if(areEqual(actual, maximum+1)){
            split(root,input);
        }
        else {
            root->values[actual-1]= input;
            insertion_sort( root->values,actual);
        }
        return root;
    }

    void remove(int val)
    {

    }

    ~BPlusTree(){
        root->ClearTree(root);
        delete root->values;
    }

    void copyLowerVals(Node* n1, Node* root, int range){
        n1->act_num_of_vals     = range;
        n1->act_num_of_pointers = range+1;
        for(int i= 0; i < range; i++){
            n1->values[i]=root->values[i];
        }
    }

    void copyUpperVals(Node* n1, Node* root, int start, int stop){
        n1->act_num_of_vals     = stop-start;
        n1->act_num_of_pointers = stop-start+1;
        for(int i= 0; i < start; i++){
            n1->values[i]=root->values[start+i];
        }
    }

    void print(Node* n){
    cout << "print called"<<endl;
        for(int i = 0; i < n->max_num_of_vals; i++)
            cout << "i = " << i << ", root value = " << n->values[i] <<endl;
        return;
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

