#include<bits/stdc++.h>
using namespace std;

#define DEBUG printf("Passing line %d in function [%s].\n", __LINE__, __FUNCTION__)

typedef vector<int> vi;

struct SharedBest {
    int best_diff = INT_MAX;
    double print_threshold = numeric_limits<double>::infinity();
    set<vi> Xbest;
    int best_owner = -1;
    int owner_stall = 0;
    bool assist_in_progress = false;
    bool shutdown = false;

    vector<int> worker_best_diff;
    vector<bool> worker_finished;
    vector<int> worker_restart_token;

    int restart_token = 0;
    int restart_seed_diff = INT_MAX;
    set<vi> restart_seed_X;

    mutex data_mutex;
    condition_variable cv;
};

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
    thread_local mt19937 gen(random_device{}());
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

void trigger_assist_locked(SharedBest &shared, const int THREADS){
    if(shared.assist_in_progress || shared.best_owner < 0) return;

    int need = max(1, THREADS / 2);
    vector<int> selected;
    selected.reserve(THREADS - 1);

    for(int i = 0; i < THREADS; ++i){
        if(i == shared.best_owner) continue;
        if(shared.worker_finished[i]) selected.push_back(i);
    }

    if((int)selected.size() < need){
        vector<pair<int, int>> active;
        active.reserve(THREADS);
        for(int i = 0; i < THREADS; ++i){
            if(i == shared.best_owner) continue;
            if(shared.worker_finished[i]) continue;
            active.push_back({shared.worker_best_diff[i], i});
        }
        sort(active.begin(), active.end(), [](const pair<int, int> &a, const pair<int, int> &b){
            return a.first > b.first;
        });
        for(auto &it : active){
            if((int)selected.size() >= need) break;
            selected.push_back(it.second);
        }
    }

    if(selected.empty()) return;

    int token = ++shared.restart_token;
    shared.restart_seed_X = shared.Xbest;
    shared.restart_seed_diff = shared.best_diff;
    for(int id : selected){
        shared.worker_restart_token[id] = token;
        shared.worker_finished[id] = false;
    }

    shared.assist_in_progress = true;
    shared.owner_stall = 0;
    shared.cv.notify_all();
}

bool try_update_global_best(const int thread_id, const int candidate_diff,
                            const set<vi> &candidate_best, SharedBest &shared){
    lock_guard<mutex> guard(shared.data_mutex);
    if(candidate_diff >= shared.best_diff) return false;

    shared.best_diff = candidate_diff;
    shared.Xbest = candidate_best;
    shared.best_owner = thread_id;
    shared.owner_stall = 0;
    shared.assist_in_progress = false;
    if(candidate_diff < shared.print_threshold){
        cout << "Best diff: " << candidate_diff << "\n";
    }
    return true;
}

void notify_owner_progress(const int thread_id, const bool improved_global,
                           const int STALL_LIMIT, const int THREADS, SharedBest &shared){
    lock_guard<mutex> guard(shared.data_mutex);
    if(shared.best_owner != thread_id) return;
    if(improved_global){
        shared.owner_stall = 0;
        return;
    }

    ++shared.owner_stall;
    if(shared.owner_stall >= STALL_LIMIT){
        trigger_assist_locked(shared, THREADS);
    }
}

void worker_search(const int thread_id, const int N, const int S, const int MAXCNT,
                   const int STALL_LIMIT, const int THREADS, SharedBest &shared){
    set<vi> X;
    init(N, S, X);
    int cnt = 0, best_diff = evaluate_diff(N, S, X);
    set<vi> Xbest = X;

    int seen_token = 0;
    {
        lock_guard<mutex> guard(shared.data_mutex);
        shared.worker_best_diff[thread_id] = best_diff;
        shared.worker_finished[thread_id] = false;
    }
    try_update_global_best(thread_id, best_diff, Xbest, shared);

    while(true){
        {
            unique_lock<mutex> lock(shared.data_mutex);
            if(shared.shutdown) break;

            if(shared.worker_restart_token[thread_id] > seen_token){
                seen_token = shared.worker_restart_token[thread_id];
                X = shared.restart_seed_X;
                Xbest = X;
                best_diff = shared.restart_seed_diff;
                shared.worker_best_diff[thread_id] = best_diff;
                shared.worker_finished[thread_id] = false;
                cnt = 0;
            }else if(cnt >= MAXCNT){
                shared.worker_finished[thread_id] = true;
                shared.cv.notify_all();
                shared.cv.wait(lock, [&](){
                    return shared.shutdown || shared.worker_restart_token[thread_id] > seen_token;
                });
                continue;
            }
        }

        vi vec = random_vector(N, S);
        int cur_diff = update_diff(N, S, X, vec);
        bool improved_local = false;
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
                Xbest = X;
                improved_local = true;
                cnt = 0;
            }
        }else ++cnt;

        if(improved_local){
            bool improved_global = try_update_global_best(thread_id, best_diff, Xbest, shared);
            {
                lock_guard<mutex> guard(shared.data_mutex);
                shared.worker_best_diff[thread_id] = best_diff;
                shared.worker_finished[thread_id] = false;
            }
            notify_owner_progress(thread_id, improved_global, STALL_LIMIT, THREADS, shared);
        }else{
            notify_owner_progress(thread_id, false, STALL_LIMIT, THREADS, shared);
        }
    }
}

void save_result(const int &N, const int &S, const int best_diff, const set<vi> &Xbest){
    string filename = "results/" + to_string(N) + "_" + to_string(S) + "_" + to_string(best_diff) + ".txt";
    ofstream out(filename);
    for(const auto &vec : Xbest){
        for(int i = 0; i < N; ++i){
            out << vec[i] << " \n"[i == N - 1];
        }
    }
    out.close();
}

int main(){
    int N, S, MAXCNT, THREADS;
    unsigned int max_threads = thread::hardware_concurrency();
    if(max_threads == 0){
        puts("Maximum concurrent threads available: unknown (hardware_concurrency returned 0)");
    }else{
        printf("Maximum concurrent threads available: %u\n", max_threads);
    }

    while(true){
        puts("Enter N, S, MAXCNT(*10000), THREADS (or enter 0 to exit):");
        cin >> N;
        if(N == 0) break;
        cin >> S >> MAXCNT >> THREADS;
        if(max_threads != 0 && THREADS > (int)max_threads){
            printf("Warning: requested THREADS = %d exceeds suggested max = %u\n", THREADS, max_threads);
        }
        THREADS = max(1, THREADS);
        MAXCNT *= 10000;
        int STALL_LIMIT = max(1, MAXCNT / 2);

        SharedBest shared;
        double lower_bound = N + (double)N * (N - 1) * S / 6.0;
        shared.print_threshold = 1.3 * lower_bound;
        shared.worker_best_diff.assign(THREADS, INT_MAX);
        shared.worker_finished.assign(THREADS, false);
        shared.worker_restart_token.assign(THREADS, 0);

        vector<thread> workers;
        workers.reserve(THREADS);

        for(int i = 0; i < THREADS; ++i){
            workers.emplace_back([&, i]() {
                worker_search(i, N, S, MAXCNT, STALL_LIMIT, THREADS, shared);
            });
        }

        {
            unique_lock<mutex> lock(shared.data_mutex);
            shared.cv.wait(lock, [&](){
                for(bool done : shared.worker_finished){
                    if(!done) return false;
                }
                return true;
            });
            shared.shutdown = true;
            shared.cv.notify_all();
        }

        for(auto &worker : workers){
            worker.join();
        }

        int final_best;
        set<vi> best_snapshot;
        {
            lock_guard<mutex> guard(shared.data_mutex);
            final_best = shared.best_diff;
            best_snapshot = shared.Xbest;
        }
        if(final_best != INT_MAX){
            int verified = evaluate_diff(N, S, best_snapshot);
            if(verified != final_best){
                cerr << "Verification failed! Expected: " << final_best << ", Got: " << verified << "\n";
                return 1;
            }
            save_result(N, S, final_best, best_snapshot);
        }
    }
    return 0;
}