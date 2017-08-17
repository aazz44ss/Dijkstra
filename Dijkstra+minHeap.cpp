#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <cstring>
#include <queue>
#include <list>
#include <set>
#include <stack>
#include <map>
#include <math.h>

using namespace std;

int visit =1;
int unvisit =0;

class Edge{
public:
    int from,to,weight;
    Edge(){};
    Edge(int f,int t,int w){
        from=f;
        to=t;
        weight=w;
    }
    bool operator == (Edge &e1){
        if(e1.from == this->from && e1.to == this->to && e1.weight == this->weight){
            return true;
        }else{
            return false;
        }
    }
};

bool operator< (const Edge &e1, const Edge &e2){
    if(e1.from == e2.from){
        return e1.to < e2.to;
    }else{
        return e1.from < e2.from;
    }
}

class Graph{
public:
    int vertexNumber;
    int edgeNumber;
    int *Mark;
    vector< vector<Edge> > edge;
    vector<string> Vertex;
    Graph(int _vertexNumber){
        vertexNumber=_vertexNumber;
        Mark = new int[vertexNumber];
    }
    void insertV(string _vertex){
        Vertex.push_back(_vertex);
        edge.push_back(vector<Edge>());
    }
    void insertE(string location1, string location2, int weight){
        int f,t;
        f=VertexIndex(location1);
        t=VertexIndex(location2);
        edge[f].push_back(Edge(f,t,weight));
        edge[t].push_back(Edge(t,f,weight));
        edgeNumber+=2;
    }
    int ToVertex(Edge e){
        return e.to;
    }
    int VertexIndex(string location){
        for(int i=0;i<vertexNumber;i++){
            if(Vertex[i]==location){
                return i;
            }
        }
        return -1;
    }
    bool isEdge(Edge e){
        if(e.from==-1 && e.to==-1){
            return false;
        }else{
            return true;
        }
    }
    Edge firstEdge(int v){
        if(edge[v].empty()){
            return Edge(-1,-1,-1);
        }else{
            return edge[v][0];
        }
    }
    Edge nextEdge(Edge e){
        int v;
        v = e.from;
        for(int i=0;i<edge[v].size();i++){
            if( edge[v][i]==e && i<edge[v].size()-1 ){
                return edge[v][i+1];
            }
        }
        return Edge(-1,-1,-1);
    }
};


//頂點
class Dist{
public:
    int index;
    int length;
    int pre;
};

bool operator< (Dist &d1, Dist &d2){
    return d1.length<d2.length;
}
bool operator> (Dist &d1, Dist &d2){
    return d1.length>d2.length;
}


//最小堆
template <class T>
class minHeap {
    T **heapArray;
    int currentSize=0;
    int maxSize;
    void BuildHeap();
    
public:
    minHeap(int n);
    T &RemoveMin();
    int Parent(int position);
    bool Insert(T *newNode);
    bool Remove(int position, T **node);
    void SiftUp(int position);
    void SiftDown(int position);
    bool isEmpty();
};

template <class T>
minHeap<T>::minHeap(int n){
    heapArray = new T*[n];
    maxSize = n;
}

template <class T>
int minHeap<T>::Parent(int position){
    return (position-1)/2;
}

template <class T>
void minHeap<T>::SiftDown(int position){
    int i = position;
    int j = 2*i+1;
    T *temp = heapArray[i];
    while(j<currentSize){
        if((j+1)<currentSize && ( *(heapArray[j]) > *(heapArray[j+1]) )){
            j++;  //找出較小的child
        }
        //parent不能比任一child大
        if(*temp > *(heapArray[j]) ){
            heapArray[i] = heapArray[j];
            i=j;
            j=2*i+1;
        }else{
            break;
        }
    }
    heapArray[i] = temp;
}

template <class T>
void minHeap<T>::SiftUp(int position){
    int tempPos = position;
    T *temp = heapArray[position];
    while(tempPos>0){
        if(*temp < *(heapArray[Parent(tempPos)]) ){
            heapArray[tempPos] = heapArray[Parent(tempPos)];
            tempPos = Parent(tempPos);
        }else{
            break;
        }
    }
    heapArray[tempPos] = temp;
}

template<class T>
void minHeap<T>::BuildHeap(){
    for(int i=currentSize/2-1;i>=0;i--){
        SiftDown(i);
    }
}

template<class T>
bool minHeap<T>::Insert(T *newNode){
    if(currentSize==maxSize){
        return false;
    }
    heapArray[currentSize] = newNode;
    SiftUp(currentSize);
    currentSize++;
    return true;
}

template<class T>
bool minHeap<T>::Remove(int position,T **Node){
    if(position<0 || position>(currentSize-1)){
        return false;
    }
    *Node = heapArray[position];
    //將最後一個元素覆蓋要刪除的元素，然後判斷他的新位置
    heapArray[position] = heapArray[--currentSize];
    if(*(heapArray[position]) < *(heapArray[Parent(position)]) ){
        SiftUp(position);
    }else{
        SiftDown(position);
    }
    return true;
}

template<class T>
bool minHeap<T>::isEmpty(){
    return currentSize==0;
}


//求出每個點到Ｓ的最短距離
//從Ｓ點開始更新每個點到達Ｓ的最短距離
//從到達Ｓ路徑的最小堆中掏出最短距離的點Ｘ
//並更新點Ｘ包含的路徑終點Ｙ到達Ｓ的距離，若是(Ｙ->S) > (Y->X) + (X->S)則更新Ｙ的距離，並將Ｙ放入最小堆
void Dijkstra(Graph &G,int s,Dist ** &D){
    D = new Dist*[G.vertexNumber];
    for(int i=0;i<G.vertexNumber;i++){
        G.Mark[i]=unvisit;
        D[i] = new Dist;
        D[i]->index = i;
        D[i]->length = 0xffff;  //先將每個點到Ｓ的距離設成無限
        D[i]->pre = s;
    }
    D[s]->length=0; //Ｓ點到Ｓ點距離為0
    minHeap<Dist> H(G.edgeNumber);
    H.Insert(D[s]);
    
    for(int i=0;i<G.vertexNumber;i++){
        bool found;
        found=false;
        Dist *d=NULL;
        while(!H.isEmpty()){
            H.Remove(0, &d);
            if(G.Mark[d->index]==unvisit){
                found=true;
                break;
            }
        }
        if(!found){
            break;
        }
        int v;
        v=d->index;
        G.Mark[v]=visit;
        for(Edge e=G.firstEdge(v);G.isEdge(e);e=G.nextEdge(e)){
            if(D[G.ToVertex(e)]->length > (D[v]->length + e.weight) ){
                D[G.ToVertex(e)]->length = (D[v]->length + e.weight);
                D[G.ToVertex(e)]->pre = v;
                H.Insert(D[G.ToVertex(e)]);
            }
        }
    }
}

void printPath(Graph &G, Dist **D,int s,int current){
    if(current==s){
        cout << G.Vertex[current] ;
        return;
    }
    printPath(G, D, s, D[current]->pre);
    cout << "->" << '(' << D[current]->length-D[D[current]->pre]->length << ')' << "->" << G.Vertex[current] ;
}

int main(){
    
    int n;
    string temp;
    string location1,location2;
    int weight;
    
    cin>>n;
    Graph G(n);
    for(int i=0;i<n;i++){
        cin >> temp;
        G.insertV(temp);
    }
    
    cin>>n;
    for(int i=0;i<n;i++){
        cin>>location1>>location2>>weight;
        if(location1!=location2){
            G.insertE(location1, location2, weight);
        }
    }
    
    cin>>n;
    int s1,s2;
    Dist **D;
    while(n--){
        cin>>location1>>location2;
        s1=G.VertexIndex(location1);
        s2=G.VertexIndex(location2);
        Dijkstra(G, s1, D);
        
        printPath(G, D, s1, s2);
        
        for(int i=0;i<G.vertexNumber;i++){
            delete D[i];
        }
        delete [] D;
        cout << endl;
    }
    
    return 0;
    
}
//20個點，編號0~19
//21個無向邊，頂點，終點，路徑長
//1個要求的路徑，求出從0->11最短路徑
//
/*
 20
 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
 21
 0 1 30
 0 4 50
 0 9 20
 2 5 30
 4 2 50
 5 1 60
 17 3 80
 18 2 70
 15 19 20
 14 8 15
 13 11 25
 7 8 40
 19 6 60
 19 10 50
 10 1 55
 15 4 45
 18 11 25
 18 18 5
 0 0 10
 16 13 20
 12 3 25
 1
 0 11
 */
