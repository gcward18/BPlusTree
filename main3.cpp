#include <iostream>
#include <cmath>
#include "fstream"
#include <string>
using namespace std;


class Node{
    public:

        //Variable used for telling whether or not
        //the node you're at is a leaf
        bool   leaf;

        //Array of pointer  to used for internal nodes
        Node** pointer;

        //Pointer to next leaf node, only initialized if leaf is true
        Node* next;

        //Pointer to parent of this node
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
        next = NULL;
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

    ~Node(){
        delete[] pointer;
        delete key;
    }

};

class BPlusTree
{
public:
    int numNodes;
    Node* root;

    BPlusTree(){
        numNodes = 1;
        root = new Node();
    }

    ~BPlusTree(){
       // root->ClearTree(root);
        delete root->key;
    }

    //Prints the leaves and the root
    void printLeaves()
    {
        Node* node = root;
        //Outputs root node
        cout << "Root:" << endl;
        for(int i = 0; i < root->actVals; i++)
            cout << "Value: " << root->key[i] << endl;
        while(!node->leaf)
            node = node->pointer[0];
        while(node != NULL)
        {
            //Outputs all leaf nodes
            cout << "Node:" << endl;
            for(int i = 0; i < node->actVals; i++)
                cout << "Value " << i << ": " << node->key[i] << endl;
            if(numNodes !=1)
            cout << endl;
            node = node->next;
        }
        cout << endl;
        return;
    }
    //Gives the leaf directly to the right of the leaf parameter given
    Node* findNextLeaf(Node* node)
    {
        int value = node->key[node->actVals - 1];
        if(node->parent == NULL)
            return NULL;
        Node* temp = node->parent;
        int index;
        //Finds index of node in parent
        for(int i = 0; i < temp->actPtrs; i++)
            if(temp->pointer[i]->key[temp->pointer[i]->actVals-1] == value)
                index = i;
        //If node is right sibling, find sibling of parent
        if(index == temp->actPtrs - 1)
            return findNextLeaf(temp);
        else
        {
            temp = temp->pointer[index+1];
            while(!temp->leaf)
                temp = temp->pointer[0];

            return temp;
        }
    }

    //Splits leaf into 2 nodes
    void splitLeaf(Node* node, int input)
    {
        Node* parent = node->parent;
        int childPosition;
        Node* newNode    = new Node();
        newNode->parent = node->parent;
        node->next = newNode;

        //Splits the values between the 2 nodes
        if(input < node->key[0])
        {
            newNode->key[1] = node->key[2];
            newNode->key[0] = node->key[1];
            node->key[1] = node->key[0];
            node->key[0] = input;
        }
        else if(input < node->key[1])
        {
            newNode->key[1] = node->key[2];
            newNode->key[0] = node->key[1];
            node->key[1] = input;
        }
        else if(input < node->key[2])
        {
            newNode->key[1] = node->key[2];
            newNode->key[0] = input;
        }
        else
        {
            newNode->key[1] = input;
            newNode->key[0] = node->key[2];
        }
        node->key[2] =-1;
        node->actVals = 2;
        newNode->actVals = 2;

        //Makes new level if no parent
        if(node->parent == NULL)
        {
            Node* parent = new Node();
            parent->key[0] = node->key[1];
            parent->pointer[0] = node;
            parent->pointer[1] = newNode;
            parent->actVals = 1;
            parent->actPtrs = 2;
            parent->leaf = false;
            node->parent = parent;
            newNode->parent = parent;
            numNodes++;
            root = parent;
        }
        else
        {
            for(int i = 0; i < parent->actPtrs; i++)
                if(node->parent->pointer[i] == node)
                    childPosition = i;

            //Adding mid val to parent, splitting if parent is full
            if(parent->actVals == parent->maxVals)
            {
                splitInternalNode(parent, node->key[1], newNode, childPosition + 1);
            }
            else
            {
                for(int i = parent->maxVals - 1; i > childPosition; i--)
                    parent->key[i] = parent->key[i-1];
                parent->key[childPosition] = node->key[1];
                for(int i = parent->maxPtrs - 1; i > childPosition + 1; i--)
                    parent->pointer[i] = parent->pointer[i-1];
                parent->pointer[childPosition + 1] = newNode;
                parent->actVals++;
                parent->actPtrs++;
            }
        }

        //Get index of original node in parent
        //cout << node->parent->key[0] << endl;
        numNodes++;
        newNode->next = findNextLeaf(newNode);
        return;
    }
    //Splits internal node into 2 nodes
    void splitInternalNode(Node* node, int input, Node* childNode, int index)
    {
        Node* parent = node->parent;
        int childPosition;
        int mid;
        Node* newNode    = new Node();
        newNode->leaf = false;
        newNode->parent = node->parent;

        //Splits the values and pointers between the 2 nodes,
        //including new node from last split
        if(input < node->key[0])
        {
            newNode->key[1] = node->key[2];
            newNode->key[0] = node->key[1];
            node->key[1] = node->key[0];
            node->key[0] = input;
            newNode->pointer[2] = node->pointer[3];
            newNode->pointer[1] = node->pointer[2];
            newNode->pointer[0] = node->pointer[1];
            node->pointer[1] = childNode;
        }
        else if(input < node->key[1])
        {
            newNode->key[1] = node->key[2];
            newNode->key[0] = node->key[1];
            node->key[1] = input;
            newNode->pointer[2] = node->pointer[3];
            newNode->pointer[1] = node->pointer[2];
            newNode->pointer[0] = childNode;
        }
        else if(input < node->key[2])
        {
            newNode->key[1] = node->key[2];
            newNode->key[0] = input;
            newNode->pointer[2] = node->pointer[3];
            newNode->pointer[0] = node->pointer[2];
            newNode->pointer[1] = childNode;
        }
        else
        {
            newNode->key[1] = input;
            newNode->key[0] = node->key[2];
            newNode->pointer[2] = childNode;
            newNode->pointer[1] = node->pointer[3];
            newNode->pointer[0] = node->pointer[2];
        }
        mid = node->key[1];
        node->key[1] = -1;
        node->key[2] =-1;
        node->pointer[3] = NULL;
        node->pointer[2] = NULL;
        node->actVals = 1;
        node->actPtrs = 2;
        newNode->actPtrs = 3;
        newNode->actVals = 2;
        newNode->pointer[0]->parent = newNode;
        newNode->pointer[1]->parent = newNode;
        newNode->pointer[2]->parent = newNode;
        //Makes new level if no parent
        if(parent == NULL)
        {
            Node* parent = new Node();
            parent->key[0] = mid;
            parent->pointer[0] = node;
            parent->pointer[1] = newNode;
            parent->actVals = 1;
            parent->actPtrs = 2;
            parent->leaf = false;
            node->parent = parent;
            newNode->parent = parent;
            numNodes++;
            root = parent;
        }
        //Inserts mid value and pointer to new node to parent if not full
        else
        {
            //Get index of original node in parent
            for(int i = 0; i < node->parent->actPtrs; i++)
                if(node->parent->pointer[i] == node)
                    childPosition = i;

            //Adding mid val to parent, splitting if parent is full
            if(parent->actVals == parent->maxVals)
            {
                splitInternalNode(parent, mid, newNode, childPosition + 1);
            }
            else
            {
                for(int i = parent->maxVals - 1; i > childPosition; i--)
                    parent->key[i] = parent->key[i-1];
                parent->key[childPosition] = mid;
                for(int i = parent->maxPtrs - 1; i > childPosition + 1; i--)
                    parent->pointer[i] = parent->pointer[i-1];
                parent->pointer[childPosition + 1] = newNode;
                parent->actVals++;
                parent->actPtrs++;
            }
        }

        numNodes++;
        return;
    }
    void insert(int input)
    {
        Node* node = root;
        bool found = false;
        Node* temp = node;
        //Find leaf node
        while(!temp->leaf) {
            if(input <= temp->key[0])
                temp = temp->pointer[0];
            else if(input <= temp->key[1] || temp->key[1] == -1)
                temp = temp->pointer[1];
            else if(input <= temp->key[2] || temp->key[2] == -1)
                temp = temp->pointer[2];
            else
                temp = temp->pointer[3];
        }
        if(input == 12 && temp->parent!=NULL)
            cout << temp->parent->key[0] << endl;
        //Looks for input in node, if not found ends function
        for(int i = 0; i < temp->actVals; i++)
            if(temp->key[i] == input)
                found = true;
        if(found)
        {
            cout << "Value already in the tree, cannot insert " << input << endl;
            return;
        }

        //Splits if node is full, else simply inserts value
        if(temp->maxVals == temp->actVals){
            splitLeaf(temp, input);
        }
        else {
            int position = temp->actVals;
            for(int i = temp->actVals - 1; i >= 0; i--)
            {
                if(input < temp->key[i])
                    position = i;
            }
            for(int i = temp->actVals; i > position; i--)
                temp->key[i] = temp->key[i-1];
            temp->key[position] = input;
            temp->actVals++;
        }
        printLeaves();

        return;
    }

    //Turns 2 leaf nodes into 1 node
    void leafMerge(Node* left, Node* right, int input, int indexToNode)
    {
        Node* sibling;
        Node* temp;
        bool siblingFound = false;
        bool toTheRight;
        int nodeGap = 0;

        //Finds which node contains input
        if(left->key[0] == input)
            temp = left;
        else
            temp = right;

        //Checks all right siblings to see if one has an extra value
        for(int i = indexToNode + 1; i < temp->parent->actPtrs; i++)
        {
            sibling = temp->parent->pointer[i];
            if(sibling->actVals > 1)
            {
                siblingFound = true;
                toTheRight = true;
                nodeGap = i - indexToNode;
                break;
            }
        }
        sibling = temp->parent->pointer[indexToNode + nodeGap];

        //If right sibling not found, checks left
        if(!siblingFound)
        {
            for(int i = indexToNode - 1; i >= 0; i--)
            {
                sibling = temp->parent->pointer[i];
                if(sibling->actVals > 1)
                {
                    siblingFound = true;
                    toTheRight = false;
                    nodeGap = indexToNode - i;
                    break;
                }
            }
            sibling = temp->parent->pointer[indexToNode-nodeGap];
        }

        //Moves the extra sibling value down the siblings to the node
        if(siblingFound && toTheRight)
        {
            Node* leftSibling;
            for(int i = 0; i < nodeGap; i++)
            {
                leftSibling = temp;
                while(leftSibling->next != sibling)
                    leftSibling = leftSibling->next;
                leftSibling->key[1] = sibling->key[0];
                leftSibling->actVals++;
                for(int j = 0; j < sibling->actVals - 1; j++)
                    sibling->key[j] = sibling->key[j+1];
                sibling->key[sibling->actVals-1] = -1;
                sibling->actVals--;
                temp->parent->key[indexToNode + nodeGap - i - 1] = leftSibling->key[1];
                sibling = leftSibling;
            }
            temp->key[0] = temp->key[1];
            temp->key[1] = -1;
            temp->actVals--;
        }
        else if(siblingFound && !toTheRight)
        {
            Node* rightSibling;
            for(int i = 0; i < nodeGap; i++)
            {
                rightSibling = sibling->next;
                rightSibling->key[1] = rightSibling->key[0];
                rightSibling->key[0] = sibling->key[sibling->actVals-1];
                rightSibling->actVals++;
                sibling->key[sibling->actVals-1] = -1;
                sibling->actVals--;
                temp->parent->key[i + indexToNode - nodeGap] = sibling->key[sibling->actVals-1];
                sibling = rightSibling;
            }
            temp->key[1] = -1;
            temp->actVals--;
            cout << temp->actVals << endl;
        }

        //If no node found, merge sibling
        else
        {
            if(left->key[0] == input)
                left->key[0] = right->key[0];
            for(int i = indexToNode; i < left->parent->actVals-1; i++)
                left->parent->key[i] = left->parent->key[i+1];
            left->parent->key[left->parent->actVals-1] = -1;
            for(int i = indexToNode + 1; i < left->parent->actPtrs-1; i++)
                left->parent->pointer[i] = left->parent->pointer[i+1];
            left->parent->pointer[left->parent->actPtrs-1] = NULL;
            left->parent->actVals--;
            left->parent->actPtrs--;
            left->next = right->next;

            //If value removed was last value of parent, merge parent with sibling
            if(left->parent->actVals == 0)
            {
                internalMerge(left->parent);
            }
            delete right;
            numNodes--;
            return;
        }
    }

    //Merges 2 internal nodes into 1
    void internalMerge(Node* node)
    {
        Node* sibling;
        bool siblingFound = false;
        bool toTheRight;
        int nodeGap = 0;
        int indexToNode;
        int siblingIndex;

        //If node has no parent, reduce level by 1
        if(node->parent == NULL)
        {
            root = node->pointer[0];
            delete node;
            numNodes--;
            root->parent = NULL;
            cout << "Test" << endl;
            return;
        }
        //Find index of node in parent
        for(int i = 0; i < node->parent->actPtrs; i++)
            if(node->parent->pointer[i] == node)
                indexToNode = i;
        //Look for appropriate right sibling
        for(int i = indexToNode + 1; i < node->parent->actPtrs; i++)
        {
            sibling = node->parent->pointer[i];
            if(sibling->actVals > 1)
            {
                siblingFound = true;
                toTheRight = true;
                siblingIndex = i;
                nodeGap = i - indexToNode;
                break;
            }
        }
        sibling = node->parent->pointer[indexToNode + nodeGap];
        //If no right sibling found, look for appropriate left sibling
        if(!siblingFound)
        {
            for(int i = indexToNode - 1; i >= 0; i--)
            {
                sibling = node->parent->pointer[i];
                if(sibling->actVals > 1)
                {
                    siblingFound = true;
                    toTheRight = false;
                    nodeGap = indexToNode - i;
                    siblingIndex = i;
                    break;
                }
            }
            sibling = node->parent->pointer[indexToNode-nodeGap];
        }
        //Move extra value and pointer from sibling to next node until reaching node
        if(siblingFound && toTheRight)
        {
            Node* leftSibling;
            for(int i = 0; i < nodeGap; i++)
            {
                leftSibling = node->parent->pointer[siblingIndex-1];
                leftSibling->key[1] = sibling->parent->key[siblingIndex-1];
                sibling->parent->key[siblingIndex-1] = sibling->key[0];
                leftSibling->pointer[2] = sibling->pointer[0];
                sibling->pointer[0]->parent = leftSibling;
                leftSibling->actVals++;
                leftSibling->actPtrs++;
                for(int j = 0; j < sibling->actVals - 1; j++)
                    sibling->key[j] = sibling->key[j+1];
                for(int j = 0; j < sibling->actPtrs - 1; j++)
                    sibling->pointer[j] = sibling->pointer[j+1];
                sibling->key[sibling->actVals-1] = -1;
                sibling->actVals--;
                sibling->pointer[sibling->actPtrs-1] = NULL;
                sibling->actPtrs--;
                sibling = leftSibling;
                siblingIndex--;
            }
            node->key[0] = node->key[1];
            node->key[1] = -1;
            node->pointer[1] = node->pointer[2];
            node->pointer[2] = NULL;
        }
        else if(siblingFound && !toTheRight)
        {
            Node* rightSibling;
            for(int i = 0; i < nodeGap; i++)
            {
                rightSibling = node->parent->pointer[siblingIndex+1];
                rightSibling->key[1] = rightSibling->key[0];
                rightSibling->key[0] = node->parent->key[siblingIndex];
                node->parent->key[siblingIndex] = sibling->key[sibling->actVals-1];
                rightSibling->actVals++;
                rightSibling->pointer[2] = rightSibling->pointer[1];
                rightSibling->pointer[1] = rightSibling->pointer[0];
                rightSibling->pointer[0] = sibling->pointer[sibling->actPtrs-1];
                sibling->pointer[sibling->actPtrs-1]->parent = rightSibling;
                rightSibling->actPtrs++;
                sibling->key[sibling->actVals-1] = -1;
                sibling->actVals--;
                sibling->actPtrs--;
                sibling = rightSibling;
                siblingIndex++;
            }
            node->key[node->actVals-1] = -1;
            node->pointer[2] = NULL;
        }
        //If no appropriate sibling, merge nodes
        else
        {
            Node* left;
            Node* right;
            //Merge right node if available, else left
            if(indexToNode == node->parent->actPtrs-1)
            {
                left = node->parent->pointer[indexToNode-1];
                right = node;
                left->key[1] = left->parent->key[indexToNode-1];
                left->pointer[2] = right->pointer[0];
                right->pointer[0]->parent = left;
                left->actVals = 2;
                left->actPtrs = 3;
                indexToNode--;
            }
            else
            {
                left = node;
                right = node->parent->pointer[indexToNode+1];
                left->key[0] = left->parent->key[indexToNode];
                left->key[1] = right->key[0];
                left->pointer[1] = right->pointer[0];
                right->pointer[0]->parent = left;
                left->pointer[2] = right->pointer[1];
                right->pointer[1]->parent = left;
                left->actVals = 2;
                left->actPtrs = 3;
            }

            for(int i = indexToNode; i < left->parent->actVals-1; i++)
            {
                left->parent->key[i] = left->parent->key[i+1];
                left->parent->pointer[i+1] = left->parent->pointer[i+2];
            }
            left->parent->key[left->parent->actVals-1] = -1;
            left->parent->pointer[left->parent->actPtrs-1] = NULL;
            left->parent->actVals--;
            left->parent->actPtrs--;
            //If parent's last value was removed, merge it with sibling
            if(left->parent->actVals == 0)
                internalMerge(left->parent);
            delete right;
            numNodes--;
        }
    }

    Node* remove(int input){
        Node* node = root;
        int actual;
        int internalIndex = -1;
        int leafIndex = -1;
        int indexToNode;
        Node* nodeWithValue;
        Node* temp = node;
        //Find leaf node
        while(!temp->leaf) {
            for(int i = 0; i < temp->actVals; i++)
                if(input == temp->key[i])
                {
                    nodeWithValue = temp;
                    internalIndex = i;
                }
            cout << temp->key[1] << " " << temp->leaf << endl;
            if(input <= temp->key[0])
                indexToNode = 0;
            else if(input <= temp->key[1] || temp->key[1] == -1)
                indexToNode = 1;
            else if(input <= temp->key[2] || temp->key[2] == -1)
                indexToNode = 2;
            else
                indexToNode = 3;
            cout << "Index: " << indexToNode << endl;
            temp = temp->pointer[indexToNode];
        }
        //Find value in tree, if not existent, cannot delete, return
        for(int i = 0; i < temp->actVals; i++)
            if(temp->key[i] == input)
                leafIndex = i;
        if(leafIndex == -1)
        {
            cout << "Value not in the tree, cannot delete " << input << endl;
            return node;
        }

        //If not last value, remove value
        if(temp->actVals > 1){
            for(int i = leafIndex; i < temp->actVals - 1; i++)
                temp->key[i] = temp->key[i+1];
            temp->key[temp->actVals - 1] = -1;
            temp->actVals--;
            if(internalIndex != -1)
                nodeWithValue->key[internalIndex] = temp->key[temp->actVals - 1];
        }
        //If last value, merge nodes unless there is only 1
        else
        {
            if(temp->parent == NULL)
            {
                temp->key[0] = -1;
                temp->actVals = 0;
            }
            else if(temp->next != NULL)
                leafMerge(temp, temp->next, temp->key[0], indexToNode);
            else
            {
                leafMerge(temp->parent->pointer[indexToNode-1], temp, temp->key[0], indexToNode);
            }
        }
        printLeaves();
        return temp;
    }

    void prettyPrint(ofstream& graphfile, Node* node, int position=0,int level=0)
    {
        if (node == NULL)
            cout <<"";
        else
        {
            prettyPrint(graphfile,node->pointer[0], position+1);
            prettyPrint(graphfile,node->pointer[1], position+2);
            prettyPrint(graphfile,node->pointer[2], position+3);
            prettyPrint(graphfile,node->pointer[3], position+4);
            if(node->pointer[0] == NULL){
                 graphfile << "node" << position << "[label = \" <f0> | &nbsp;" << node->key[0] << "&nbsp; |";
                if(node->key[1] == -1){
                    graphfile << "  &nbsp;"<<" * "<<"&nbsp; |" ;
                }
                else
                    graphfile << "  &nbsp;"<<node->key[1]<<"&nbsp; |" ;
                if(node->key[2] == -1){
                    graphfile <<" &nbsp;"<< " * "<<"&nbsp; | <f3> &bull;"<<" \" ];\n";
                }
                else
                    graphfile <<" &nbsp;"<< node->key[2]<<"&nbsp; | <f3> &bull;"<<" \" ];\n";
            }
            else{
                graphfile <<"node"<<position<<"[label = \" <f0> &bull;  | &nbsp;" << node->key[0]<< "&nbsp; |";
                if(node->key[1] == -1){
                    graphfile << " <f1> &bull; | &nbsp;"<<" * "<<"&nbsp; |" ;
                }
                else
                    graphfile << " <f1> &bull; | &nbsp;"<<node->key[1]<<"&nbsp; |" ;

                if(node->key[2] == -1){
                    graphfile <<"<f2> &bull; | &nbsp;"<< " * "<<"&nbsp; | <f3> &bull;"<<" \" ];\n";
                }
                else
                    graphfile <<"<f2> &bull; | &nbsp;"<< node->key[2]<<"&nbsp; | <f3> &bull;"<<" \" ];\n";

            }


        }
        return;
    }

    void dotGraphFile(Node* node){
        ofstream graphfile;
        int numLevels=0;

        graphfile.open("graph.dot");
        graphfile << "digraph {\n" <<"graph [margin=0, splines=line];\n"<<"edge [penwidth=2];\n"
                    <<"node [shape = record, margin=0.03,1.2, penwidth=2, style=filled, fillcolor=white];\n"<<endl;
        prettyPrint(graphfile,node);

        //Need to figure out how to recursively add all the pointers to sub trees
        for(int i = 0; i < 1; i++){
            for(int j= 0; j < 4; j++){
                graphfile <<"node"<<i<<":f"<<j<<"->node"<<j+1 <<";"<<endl;
                if(j+1 !=4)
                    graphfile <<"node"<<j+1<<":f3->node"<<j+2<<":f0;\n";
            }
        }
        graphfile << "}"<<endl;
        graphfile.close();
    }
};

int isNum(string input)
{
    int result = 0;
    int count = 0;
    while(count < input.length())
    {
        if(input[count] < 48 or input[count] > 57 || count >= 3)
            return -1;
        result = result * 10 + input[count] - '0';
        count++;
    }
    return result;
}

int main()
{
    BPlusTree tree;
    bool inserting = true;
    int numNodes = 0;
    string input = "";
    int num;

    cout << "Welcome to the B+ tree creator!" << endl;
    cout << "Please enter value you want to insert. To switch to delete, type delete" << endl;
    while(input != "quit")
    {
        cin >> input;
        num = isNum(input);
        if(input == "delete")
        {
            cout << "Now in delete mode: To switch to insert, type insert" << endl;
            inserting = false;
        }
        else if(input == "insert")
        {
            cout << "Now in insert mode: To switch to delete, type delete" << endl;
            inserting = true;
        }
        else if(num < 0)
            cout << "Input must be an integer of 3 digits or less" << endl;
        else if(inserting)
        {
            tree.insert(num);
        }
        else
        {
            tree.remove(num);
        }
        //tree.dotGraphFile(tree.root);
    }
    cout << numNodes << endl;
    return 0;
}
