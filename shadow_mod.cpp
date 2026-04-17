#include<bits/stdc++.h>
using namespace std;

#define DEBUG printf("Passing line %d in function [%s].\n", __LINE__, __FUNCTION__)

typedef vector<int> vi;
int MOD;

inline bool valid(const int &n, const int &s, const vi &a){
    int sum = 0;
    for(int i : a) sum += i;
    return sum == s;
}

vector<vi> partial(const int &n, const vi &vec){
    vector<vi> result;
    for (int i = 0; i < n; ++i) {
        vi newVec = vec;
        newVec[i] += 1;
        result.push_back(newVec);
    }
    return result;
}

vi initial_state(const int &n, const int &s){
    vi state(n, 0);
    state[n - 1] = s;
    return state;
}

bool next_state(const int &n, const int &s, vi &current){
    // 从右向左找第一个非零位置（忽略最左边）
    for (int i = n - 1; i > 0; --i) {
        if (current[i] > 0) {
            int x = current[i];
            current[i] = 0;
            current[i - 1] += 1;
            current[n - 1] += (x - 1);
            return true;
        }
    }
    return false;
}

vi mod(const vi &vec){
    if(MOD <= 0) return vec;
    vi ret = vec;
    for(int &x : ret){
        x %= MOD;
    }
    return ret;
}

void print(const int &n, const vi &vec){
    for(int i = 0; i < n; ++i) printf("%d%c", vec[i], " \n"[i == n - 1]);
    // puts("");
}

void init(const int &N, const int &S, set<vi> &X){
    // X.clear();
    // vi vec0(N/2, 0), vec1(N - N/2, 0);
    // for(int i = 0; i <= S; i += 2){
    //     vec0 = initial_state(N/2, i);
    //     do{
    //         vec1 = initial_state(N - N/2, S - i);
    //         do{
    //             vi combined = vec0;
    //             combined.insert(combined.end(), vec1.begin(), vec1.end());
    //             X.insert(combined);
    //             // print(N, combined);
    //         }while(next_state(N - N/2, S - i, vec1));
    //     }while(next_state(N/2, i, vec0));
    // }
    // // puts("INIT() COMPLETE!");

    //////////////////////////////////////////////////////////

    vi vec0 = initial_state(N, S);
    if(MOD > 0){
        set<vi> tmp;
        do{
            tmp.insert(mod(vec0));
        }while(next_state(N, S, vec0));
        for(const auto &v : tmp){
            if(rand() % 2)
                X.insert(v);
        }
    }else{
        do{
            if(rand() % 2)
                X.insert(vec0);
        }while(next_state(N, S, vec0));
    }
}

vi random_vector(const int &N, const int &S){
    if (N == 1) return {S};

    vi pool(S + N - 1);
    for (int i = 0; i < S + N - 1; ++i) pool[i] = i;

    vi chosen(N - 1);
    random_device rd;
    mt19937 gen(rd());
    sample(pool.begin(), pool.end(), chosen.begin(), N - 1, gen);
    sort(chosen.begin(), chosen.end());

    vi vec(N);
    vec[0] = chosen[0];
    for (int i = 1; i < N - 1; ++i) {
        vec[i] = chosen[i] - chosen[i - 1] - 1;
    }
    vec[N - 1] = (S + N - 2) - chosen.back();

    return vec;
}

int evaluate_diff(const int &N, const int &S, const set<vi> &X){ // Time complexity: O(N * C(S + N, N - 1))
    int ret = 0;
    vi vec1 = initial_state(N, S + 1);
    do{
        bool lowerinX = false, lowerinY = false;
        for(int i = 0; i < N; ++i){
            if(vec1[i] == 0) continue;
            --vec1[i]; // Now vec1 is in (N, S).
            auto itX = X.find(mod(vec1));
            if(itX != X.end()){
                lowerinX = true;
                if(lowerinY){++vec1[i];break;}
            }else{
                lowerinY = true;
                if(lowerinX){++vec1[i];break;}
            }
            ++vec1[i];
        }
        if(((!lowerinX) && lowerinY) || (lowerinX && !lowerinY)){
            // printf("%c, %c: ", "NY"[lowerinX], "NY"[lowerinY]);
            // print(N, vec1);
            ++ret;
        }
    }while(next_state(N, S + 1, vec1));
    return ret;
}

int main(){
    srand(time(0));
    int N, S;
    set<vi> X;
    while(true){
        puts("Enter N, S, MOD(or enter 0 to exit):");
        cin >> N;
        if(N == 0) break;
        cin >> S >> MOD;
        if(MOD <= 0){
            puts("Assuming that you are not using MOD...");
        }
        auto start = chrono::high_resolution_clock::now();
        init(N, S, X);
        printf("Size of X: %d\n", (int)X.size());
        int ans = evaluate_diff(N, S, X);
        printf("Diff: %d\n", ans);
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "Time spent: " << duration.count() << " ms\n";
    }
    return 0;
}