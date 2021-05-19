#include <bits/stdc++.h>
using namespace std;
float inf = numeric_limits<float>::infinity();
using ll = long long;

//システム容量
int K_MAX = 100;
// //ステップ数
// int N = 1000000;
// double lamda = 6.0;
// double service_time = 8.0;
// double myu = 60.0 / service_time;
// double rou = lamda / myu;
constexpr int FLOAT_MIN = 0;
constexpr int FLOAT_MAX = 1;

//double urand(){
//    double m, a;
//    m = RAND_MAX + 1;
//    a = (rand() + 0.5)/m;
//    a = (rand() + a)/m;
//    return (rand() + a)/m;
//}

//擬似乱数発生
double my_rand() {
    random_device rd;
    default_random_engine eng(rd());
    uniform_real_distribution<float> distr(FLOAT_MIN, FLOAT_MAX);
    double g = distr(eng);
    return g;
}

//到着間隔
double Arrival_interval(double lamda) {
    double g = my_rand();
    double tau = - log(1 - g) / lamda;
    return tau;
}

//サービス時間
double Service_time(double myu) {
    double g = my_rand();
    double psi = - log(1 - g) / myu;
    return psi;
}


//メイン関数
int main() {
    //ステップ数
    int N;
    cout << "Input N:"; cin >> N;
    double lamda = 6.0;
    double service_time;
    cout << "Input Service time:"; cin >> service_time;
    cout << endl;
    double myu = 60.0 / service_time;
    double rou = lamda / myu;
    queue<double> que;
    double wait_time_avg = 0, wait_time_avg0 = 0;
    double length_avg = 0, length_avg0 = 0;
    double wait_time_sum = 0;
    double length_sum = 0;
    double current_time = Arrival_interval(lamda);
    double arrived_time_sum = current_time;
    int cnt = 0, pop_cnt = 0, arv_cnt = 0, rjc_cnt = 0;
    double st = 3.0;  //初めの何分の１を切り捨てるか
    
    while (1) {
        //Nステップ目終了後break
        if (cnt > N) break;
        cnt++;
        //先頭客のサービスが終了するまで客を到着させる
        if (arrived_time_sum < current_time) {
            double tau = Arrival_interval(lamda);
            arrived_time_sum += tau;
            arv_cnt++;  //到着した客をカウント
            //待ち行列長がシステム容量を超えていれば並ばせない
            if (que.size() >= K_MAX) {
                if (cnt > N / st) {  //平均の逐次計算 x~_n+1 = x~_n + (x_n+1 - x~_n) / (n + 1)
                    length_sum += que.size();
                    length_avg = length_avg0 + (que.size() - length_avg0) / (double)(cnt - N / st + 1);
                    length_avg0 = length_avg;
                }
                rjc_cnt++;  //棄却された客をカウント
                continue;
            }
            
            que.push(arrived_time_sum);
        } else {
            //待ち客がいれば待ち時間を足して先頭の客を解放
            if (cnt > N / st) {
                if (!(que.empty())) {
                    double WT = current_time - que.front();
                    wait_time_sum += WT;
                    if (!pop_cnt) wait_time_avg = WT;
                    else wait_time_avg = wait_time_avg0 + (WT - wait_time_avg0) / (pop_cnt + 1);
                    wait_time_avg0 = wait_time_avg;
                }
                pop_cnt++; //サービスを終えた客をカウント
            }
            if (!(que.empty())) que.pop();
            current_time += Service_time(myu);
        }
        //行列長を加算
        if (cnt > N / st) {
            length_sum += que.size();
            length_avg = length_avg0 + (que.size() - length_avg0) / (double)(cnt - N / st + 1);
            length_avg0 = length_avg;
        }
    }
    
    
    //出力
    cout << "Test results" << endl; //2種類の方法で平均値を計算(オーバーフロー対策)
    cout << "Length average = " << length_sum / (cnt - N / st) * rou << endl;
    cout << "Length average = " << length_avg * rou << endl;
    bool flag = false;
    if (rou < 1.0) flag = true;  //rou < 1.0 のとき，1人分のサービス時間を足す
    cout << "Wait time average = " << wait_time_sum / pop_cnt + service_time / 60.0 * flag << endl;
    cout << "Wait time average = " << wait_time_avg << endl;
    cout << "Rejection rate = " << (double)rjc_cnt / (double)arv_cnt * 100 << "%" << endl;
    //cout << pop_cnt / (double)N * 100 << "% had been served" << endl;
    cout << "lamda1 = " << length_sum / (cnt - N / st) * rou / (wait_time_sum / pop_cnt + service_time / 60.0 * flag) << endl;
    cout << "lamda2 = " << length_avg * rou / wait_time_avg << endl;
    
    //理論値，期待値の計算
    double L = rou / (1 - rou);
    double Lq = L - rou;
    //double W = L / lamda;
    double Wq = Lq / lamda;
    cout << endl;

    cout << "Theoretical value" << endl;
    //cout << "L = " << L << endl;
    //cout << "W = " << W << endl;
    cout << "Lq = " << Lq << endl;
    cout << "Wq = " << Wq << endl;
    if (rou < 1 || rou > 1) {
        cout << "Rejection rate = " << (1 - rou)*pow(rou, K_MAX) / (double)(1 - pow(rou, K_MAX+1)) * 100 << "%" << endl;
    } else {
        cout << "Rejection rate = " << 1 / (double)(K_MAX + 1) * 100 << "%" << endl;
    }
    cout << endl;
    
    cout << "Expectation of Takine" << endl;
    if (rou < 1 || rou > 1) {
        double E = 0;
        for (int i = 1; i <= 100; i++) {
            E += i * (1 - rou)*pow(rou, i) / (double)(1 - pow(rou, K_MAX+1));
        }
        cout << "E(Lq) = " << E * rou << endl;
        cout << "E(Wq) = " << E * rou / lamda << endl;
    } else {
        double E = 0;
        for (int i = 1; i <= 100; i++) {
            E += i  / (double)(K_MAX + 1);
        }
        cout << "E(Lq) = " << E * rou << endl;
        cout << "E(Wq) = " << E * rou / lamda << endl;
    }
}
