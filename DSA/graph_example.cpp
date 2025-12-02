#include<iostream>
#include<vector>
#include<queue>

using namespace std;

class Graph{
    int n;
    vector<vector<int>> adj;
    bool directed;
    void dfsUtil(int u,vector<bool>& visited,vector<int>order) const{
        visited[u]= true;
        order.push_back(u);
        if(!visited[u]){
            visited[u]=true;
        }
        dfsUtil(u,visited,order);
    }

    public:
        Graph(int nodes,bool directed):n(nodes),directed(directed),adj(nodes){}
        bool isvalidvertex(int v) const{
            return v>=0 && v<n;
        }
        bool isdirected()const{
            return directed;
        }
        void addEdge(int u,int v){
            if(!isvalidvertex(u)||!isvalidvertex(v)){
                cout<<"Invalid Graph\n";
                return;
            }
            adj[u].push_back(v);
            if(!directed){
                adj[v].push_back(u);
            }
        }
        bool hasedges(int u,int v){
            if(!isvalidvertex(u) || !isvalidvertex(v)){
                return false;
            }
            for(int x : adj[u]){
                if(x == v){
                    return true;
                }
            }
            return false;
        }
        vector<int> neighbors(int n){
            if(!isvalidvertex(n)){
                static vector<int> empty;
                return empty;
            }
            return adj[n];
        }
        vector<int> bfs(int start)const{
            vector<int> order;

            if(!isvalidvertex(start)){
                cout<<"bfs: Invalid Start\n";
                return order;
            }
            vector<bool> visited(n,false);
            queue<int> q;
            visited[start] = true;
            q.push(start);
            while(!q.empty()){
                int u = q.front();
                q.pop();
                order.push_back(u);
                for(int v: adj[u]){
                    if(!visited[v]){
                        visited[v] = true;
                        q.push(v);
                    }
                }
            }
            return order;
        }

        vector<int> dfs(int start)const{
            std::vector<int>order;
            if(!isvalidvertex(start)){
                cout<<"DFS: Invalid Start\n";
                return order;
            }
            vector<bool> visited(n,false);
            dfsUtil(start,visited,order);
            return order;
        }
        void print(){
            for (int i = 0; i < n; i++)
            {
                cout<<i<<" : ";
                for(int v: adj[i])
                {
                    cout<< v<<" ";
                }
                cout<<"\n";
            }
            
        }
        
};
int main(){
    Graph g(5,false);
    g.addEdge(0,1);
    g.addEdge(0,2);
    g.addEdge(1,3);
    g.addEdge(3,4);
    g.print();
    cout<<"\n";
    auto res = g.neighbors(1);
    for(int x: res){
        cout<< x <<" ";
    }
    cout<<"\n";
    cout<<g.hasedges(1,3)<<"\n";
    cout<<g.hasedges(1,4)<<"\n";

    std::vector<int> bfsorder = g.bfs(0);
    std::cout<<"BFS from 0: ";
    for(int x:bfsorder){
        std::cout<< x <<" ";
    }
    return 0;
}