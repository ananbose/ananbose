//Implement firstfit, bestfit , nextfit and seqlist algos using malloc and free
using namespace std;
#include<iostream>
#include<map>
bool cmp(int a, int b) {
    return a < b;
}
//global variable to store the value of the final buffer
int *buffer = (int*)malloc(100*sizeof(int));
//tracking the main blob
int * freep = buffer;
map <int*, int, decltype(&cmp)>  freelist(&cmp); 
//first -fit : In this case the first available free memory is returned
void *mymalloc_firstfit(int len) {
    //firstfit- heap memory, allocate from that heap , keep a track of the last free pointerand assign it to the user
    int *arr;
    if (!(freelist.empty())){
        for(auto i = freelist.begin() ; i!=freelist.end(); i++){
            if (i->second >=len) {
                arr = i->first;
                freelist.erase(i);
                return arr;
            }
        }
    }
    if (freep+ len < buffer + 100) {
        arr = freep;
        freep = freep+len;
        return arr;
    }
    else { cout << "cannot allocate memory of that size!"<<endl; return nullptr;}
}
// here we will assign the best fit 
void *mymalloc_bestfit(int len) {
    // arrange free list in ascending order , and do a search on length of the free blocks and return
    //the best fitting one.(can replace with binary search too)
    for (auto it = freelist.begin(); it!= freelist.end(); it++) {
        if (it->second >= len) {
            int * arr = it->first;
            freelist.erase(it);
            return arr;
        }
    }
    cout<< " number of bytes exceeds mem"<<endl;
    return nullptr;
}

void myfree(int *p, int len) {
    int *temp = p;
    freelist.insert({p, len});
    cout<<"freelist"<<endl;
    for(auto i = freelist.begin() ; i!=freelist.end(); i++){
        cout<<i->first<<i->second<<endl;
            
    }
}
int main(){
    int bytes;

    cout <<"Bytes to be malloced"<<bytes<<endl;
    cout<<"Enter the memory req"<<endl;
    cin>>bytes;
    int *p = (int *)mymalloc_firstfit(bytes);
    if (p==nullptr) {cout<< "did not return since it exceeds"<<endl; return 0;}
        for (int i =0 ; i<bytes ;i++) {
        p[i] = i+1;
        cout<<"element"<<p[i]<<endl;
    }
    cout<<"Enter the memory req"<<endl;
    cin>>bytes;
    int *q = (int *)mymalloc(bytes);
    if (q==nullptr) {cout<< "did not return since it exceeds"<<endl; return 0;}
        for (int i =10 ; i<10+bytes ;i++) {
        q[i] = i+1;
        cout<<"element"<<q[i]<<endl;
    }

    cout<<"Freeing the memory now"<<endl;
    myfree(p,bytes);
    myfree(q,bytes);

    return 0;
}