#include <iostream>

using namespace std;

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
    Node(int num_vals){

        //Defaulting the values for a B+ Tree of p=num_vals
        leaf = true;
        max_num_of_vals = num_vals;
        children = new Node*[max_num_of_vals+1];
        values = new int[max_num_of_vals];
        act_num_of_vals = 0;
        max_num_of_pointers = num_vals+1;
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

    void insert(Node* root, int input){
        cout << "Insertion Called"<<endl;
        Node* currNode = new Node(root);

        //Recusively try to find leaf node
        while(currNode->leaf == false){
            cout << "current Node is not a Leaf Node"<<endl;
            if((currNode->act_num_of_vals <= 4)&&(input > currNode->values[2]))
                    insert(currNode->children[3],input);
            if((currNode->act_num_of_vals <= 3)&&(input > currNode->values[1]))
                    insert(currNode->children[2],input);
            if((currNode->act_num_of_vals <= 2)&&(input > currNode->values[0]))
                    insert(currNode->children[1],input);
            if((currNode->act_num_of_vals == 1)&&(input > currNode->values[0]))
                    insert(currNode->children[0],input);
        }
        cout << "current Node is a leaf Node"<<endl;
        //Leaf is found, now check to find where to insert
        //value that has been passed
        for(int i = 0; i < currNode->max_num_of_vals; i++){
            //Value has already been inserted, cannot
            //insert again
            if(currNode->values[i] == input){
                cout << "Value already in the tree, cannot insert "
                << input << endl;
                return;
            }
        }
        currNode->act_num_of_vals++;
        for(int i = 0; i< currNode->act_num_of_vals;i++){
            cout << currNode->values[i] <<endl;
            if(currNode->values[i] < input){
                currNode->values[i] = input;
                currNode->act_num_of_vals++;
                cout << "node at postion " << i << " updated"<<endl;
            }
        }
    }

    void remove(int val)
    {

    }

    ~BPlusTree(){
        root->ClearTree(root);
        delete root->values;
    }

    void clear(Node* root){
        while(root->leaf == false){
            if(root->act_num_of_vals == 1){
                clear(root->children[0]);
                clear(root->children[1]);
            }

            else if(root->act_num_of_vals == 2){
                clear(root->children[0]);
                clear(root->children[1]);
                clear(root->children[2]);
            }

            else if(root->act_num_of_vals == 3){
                clear(root->children[0]);
                clear(root->children[1]);
                clear(root->children[2]);
                clear(root->children[3]);
            }
        }
        for(int i= 0; i <= root->max_num_of_vals; i++){
            delete root->children[i];
        }
        delete[] root;
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

