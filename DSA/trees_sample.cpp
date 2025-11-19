#include<iostream>
#include<queue>

struct Node
{
    int val;
    Node* left;
    Node* right;
};

void Preorder(Node* root){
    if(!root) return;
    std::cout<<root->val<<"->";
    Preorder(root->left);
    Preorder(root->right);
}
void Inorder(Node* root){
    if(!root) return;
    Inorder(root->left);
    std::cout<<root->val<<"->";
    Inorder(root->right);
}
void Postorder(Node* root){
    if(!root) return;
    Postorder(root->left);
    Postorder(root->right);
    std::cout<<root->val<<"->";
}
void Levelorder(Node* root){
    if(!root) return;
    std::queue<Node*> q;
    q.push(root);
    while(!q.empty()){
        Node* cur = q.front();
        q.pop();
        std::cout<<cur->val<<"->";
        if(cur->left) q.push(cur->left);
        if(cur->right) q.push(cur->right);
    }
}
void freeTree(Node* root){
    if(!root) return;
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

int main(){
    Node* root = new Node{10,nullptr,nullptr};
    root->left = new Node{5,nullptr,nullptr};
    root->right = new Node{15,nullptr,nullptr};
    root->right->left = new Node{12,nullptr,nullptr};
    root->right->right = new Node{20,nullptr,nullptr};
    std::cout<<"Preorder: ";
    Preorder(root);
    std::cout<<"\n";
    std::cout<<"Inorder: ";
    Inorder(root);
    std::cout<<"\n";
    std::cout<<"Postorder: ";
    Postorder(root);
    std::cout<<"\n";
    std::cout<<"Level Order: ";
    Levelorder(root);
    std::cout<<"\n";
    freeTree(root);
    return 0;
}