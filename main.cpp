#include <iostream>

using namespace std;

class Node
{
    public:
        bool leaf;
        int nodes;
        Node** children;
        int values[3];
    Node(int leftVal = -1, int midVal = -1, int rightVal = -1)
    {
        nodes = 0;
        children = new Node*[4];
        leaf = true;
        values[0] = leftVal;
        if(leftVal != -1)
            nodes++;
        values[1] = midVal;
        if(midVal != -1)
            nodes++;
        values[2] = rightVal;
        if(rightVal != -1)
            nodes++;
    }
};

class BPlusTree
{
    int numNodes;
    Node* root;
    BPlusTree()
    {
        numNodes = 0;
        root = new Node();
    }
    void insert(int val)
    {
        Node* currNode = root;
        if(currNode->leaf)
        {
            if(currNode->nodes == 0)
            {
                currNode->values[0] = val;
                currNode->nodes++;
            }
            else if(currNode->nodes == 1)
            {
                currNode->values[1] = val;
                currNode->nodes++;
            }
            else if(currNode->nodes == 2)
            {
                 currNode->values[2] = val;
                 currNode->nodes++;
            }
            else
            {
                //Split
            }
        }
        else
        {
            if(val > currNode->values[2])
                currNode = currNode->children[3];
            else if(val > currNode->values[1])
                currNode = currNode->children[2];
            else if(val > currNode->values[0])
                currNode = currNode->children[1];
            else
                currNode = currNode->children[0];
        }
    }
    void remove(int val)
    {

    }
    void split(Node* node)
    {

    }
};

int main()
{
    int* values = new int();
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
           values[numNodes] = input;
            numNodes++;
        }
    }
    cout << numNodes << endl;
    return 0;
}
