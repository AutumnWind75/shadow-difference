#include<bits/stdc++.h>
using namespace std;

#define DEBUG printf("Passing line %d in function [%s].\n", __LINE__, __FUNCTION__)

typedef vector<int> vi;

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

void init(const int &N, const int &S, set<vi> &X){
    X.clear();
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
    // puts("INIT() COMPLETE!");

    //////////////////////////////////////////////////////////

    // vi vec0 = initial_state(N, S);
    // do{
    //     if(rand() % 2)
    //         X.insert(vec0);
    // }while(next_state(N, S, vec0));
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
            auto itX = X.find(vec1);
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

int update_diff(const int &N, const int &S, const set<vi> &X, vi vec){ // Time complexity: O(N^2)
    auto it = X.find(vec);
    bool isinX = (it != X.end() && *it == vec);
    int ret = 0;
    for(int i = 0; i < N; ++i){
        ++vec[i];
        bool flag1 = true, flag2 = true;
        for(int j = 0; j < N; ++j){
            if(vec[j] == 0 || j == i) continue;
            --vec[j];
            auto it2 = X.find(vec);
            if(it2 != X.end()){ // This code is indeed correct. Just verify by yourself.
                if(flag1){
                    flag1 = false;
                    ret += isinX ? -1 : 1;
                    if(!flag2){
                        ++vec[j];
                        break;
                    }
                }
            }else{
                if(flag2){
                    flag2 = false;
                    ret += isinX ? 1 : -1;
                    if(!flag1){
                        ++vec[j];
                        break;
                    }
                }
            }
            ++vec[j];
        }
        --vec[i];
    }
    return ret;
}

set<vi> max_diff(const int &N, const int &S, const int MAXCNT = 100000){
    set<vi> X;
    init(N, S, X);
    int cnt = 0, best_diff = evaluate_diff(N, S, X);
    printf("Initial diff: %d\n", best_diff);
    set<vi> Xbest = X;
    while(cnt < MAXCNT){
        vi vec = random_vector(N, S);
        int cur_diff = update_diff(N, S, X, vec);
        if(cur_diff <= 0){
            auto it = X.find(vec);
            bool isinX = (it != X.end());
            if(isinX){
                X.erase(it);
            }else{
                X.insert(vec);
            }
            if(cur_diff < 0){
                best_diff += cur_diff;
                printf("New best diff: %d\n", best_diff);
                Xbest = X;
                cnt = 0;
            }
        }else ++cnt;
    }
    printf("N = %d, S = %d, MAXCNT = %d: [%d]\n", N, S, MAXCNT, best_diff);
    puts("Verifying...");
    int tmp_diff = evaluate_diff(N, S, Xbest);
    if(tmp_diff != best_diff){
        cerr << "Verification failed! Expected: " << best_diff << ", Got: " << tmp_diff << "\n";
        exit(1);
    }else{
        printf("Verification passed! Diff: %d\n", best_diff);
    }
    string filename = "results/" + to_string(N) + "_" + to_string(S) + "_" + to_string(best_diff) + ".txt";
    ofstream out(filename);
    for(const auto &vec : Xbest){
        for(int i = 0; i < N; ++i){
            out << vec[i] << " \n"[i == N - 1];
        }
    }
    cout << "Output saved to " << filename << "\n===============================\n";
    out.close();
    return X;
}

int main(){
    srand(time(0));
    int N, S, MAXCNT, EPOCHS;
    while(true){
        puts("Enter N, S, MAXCNT(*10000), EPOCHS (or enter 0 to exit):");
        cin >> N;
        if(N == 0) break;
        cin >> S >> MAXCNT >> EPOCHS;
        MAXCNT *= 10000;
        while(EPOCHS--){
            auto start = chrono::high_resolution_clock::now();
            max_diff(N, S, MAXCNT);
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
            cout << "Time spent: " << duration.count() << " ms\n";
        }
    }
    return 0;
}