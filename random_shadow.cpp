#include<bits/stdc++.h>
using namespace std;

#define DEBUG printf("Passing line %d in function [%s].\n", __LINE__, __FUNCTION__)

typedef vector<int> vi;

int N, S, MAXCNT;

bool operator<(const vi &a, const vi &b)  {
    for (int i = 0; i < N; ++i) {
        if (a[i] != b[i]) return a[i] < b[i];
    }
    return false;
}

bool operator>(const vi &a, const vi &b)  {
    return b < a;
}

// vector<vi> X, Y;
set<vi> X, Y;

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

void print(const int &n, const vi &vec){
    for(int i = 0; i < n; ++i) printf("%d%c", vec[i], " \n"[i == n - 1]);
    // puts("");
}

void init(){
    vi vec0(N/2, 0), vec1(N - N/2, 0);
    for(int i = 0; i <= S; i += 2){
        vec0 = initial_state(N/2, i);
        do{
            vec1 = initial_state(N - N/2, S - i);
            do{
                vi combined = vec0;
                combined.insert(combined.end(), vec1.begin(), vec1.end());
                X.insert(combined);
                // print(N, combined);
            }while(next_state(N - N/2, S - i, vec1));
        }while(next_state(N/2, i, vec0));
    }
    puts("INIT() COMPLETE!");
}

vi random_vector(){
    if (N == 1) return {S};                     // 只有一种情况

    // 生成 0 .. S+N-2 的序列
    vi pool(S + N - 1);
    for (int i = 0; i < S + N - 1; ++i) pool[i] = i;

    // 均匀选取 N-1 个不同的位置
    vi chosen(N - 1);
    random_device rd;
    mt19937 gen(rd());
    sample(pool.begin(), pool.end(), chosen.begin(), N - 1, gen);
    sort(chosen.begin(), chosen.end());

    // 由间隔计算各分量
    vi vec(N);
    vec[0] = chosen[0];
    for (int i = 1; i < N - 1; ++i) {
        vec[i] = chosen[i] - chosen[i - 1] - 1;
    }
    vec[N - 1] = (S + N - 2) - chosen.back();

    return vec;
}

vector<vi> upper(const int &n, const vi &vec){
    vector<vi> ret;
    vi newVec = vec;
    for (int i = 0; i < n; ++i) {
        ++newVec[i];
        ret.push_back(newVec);
        --newVec[i];
    }
    return ret;
}

int evaluate_diff(){
    int ret = 0;
    vi vec1 = initial_state(N, S + 1);
    do{
        bool lowerinX = false, lowerinY = false;
        for(int i = 0; i < N; ++i){
            if(vec1[i] == 0) continue;
            --vec1[i]; // Now vec1 is in (N, S).
            auto itX = X.find(vec1);
            if(itX != X.end() && *itX == vec1){
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
    srand(time(NULL));
    cin >> N >> S >> MAXCNT;
    init();
    int cnt = 0, best_diff = evaluate_diff();
    printf("Initial diff: %d\n", best_diff);
    set<vi> Xbest = X;
    // getchar();getchar();
    while(cnt < MAXCNT){
        vi vec = random_vector();
        auto it = X.find(vec);
        bool isinX = (it != X.end() && *it == vec);
        if(isinX){
            X.erase(it);
        }else{
            X.insert(vec);
        }
        int cur_diff = evaluate_diff();
        if(cur_diff <= best_diff){
            if(cur_diff < best_diff){
                best_diff = cur_diff;
                printf("%d, ", best_diff);
                Xbest = X;
                cnt = 0;
            }
        }else{
            if(isinX){
                X.insert(vec);
            }else{
                auto it2 = X.find(vec);
                assert(it2 != X.end() && *it2 == vec);
                X.erase(it2);
            }
            ++cnt;
            // if(cnt % 100 == 0){
            //     printf("Current diff: %d, cnt: %d\n", cur_diff, cnt);
            // }
        }
    }
    printf("\nN = %d, S = %d, MAXCNT = %d: [%d]\n", N, S, MAXCNT, best_diff);
    for (const auto &vec : Xbest) {
        print(N, vec);
    }
    return 0;
}