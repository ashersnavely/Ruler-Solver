//
// Created by ash on 9/12/19.
//

#ifndef RULER_H
#define RULER_H

#include <vector>
#include <set>
#include <algorithm>
#include <queue>
#include <fstream>
#include <cmath>
#include <random>
#include <ctime>
//#include <filesystem>
#include <thread>
#include "Sorter.h"
#include "PolynomialRegression.h"

using namespace std;

class Ruler {
private:
    static Ruler * singleton;
    static int length, order;
    static set<int> _set;
public:
    static int getLength();

    static void setLength(int);

    static int getOrder();

    static void setOrder(int);

    static const set<int>& getSet();

    static void setSet(const set<int>&);

private:

    typedef mt19937 rng_type;
    rng_type rng;

    string fName = "PerfectRulerLO.log";
    fstream perfectLOLog;
    void log(vector<int>&);

    int estimate(vector<int>, vector<int>, int);

    template <typename solutionLogger>
    vector<int> FBBAux(bool (Ruler::*)(vector<int> &, vector<typename vector<int>::iterator>&), solutionLogger);

    vector<int> FBPAttempt();
    void FBPAux(int, int, set<int>, set<int>, set<int>&);

    Ruler();
    ~Ruler();

public:
    set<pair<int, int>> perfectLengthOrder;

    bool verifyPerfect(vector<int>&, vector<typename vector<int>::iterator>&);
    bool verifyGolomb(vector<int>&, vector<typename vector<int>::iterator>&);
    set<int> findMissing(set<int>&, set<int>&);

    static Ruler* init();
    static void free();

    void load();
    void save();

    vector<int> findBestPerfectBrute(int);
    vector<int> findBestGolombBrute(int);
    vector<int> findBestPerfect(int, int = -1);
};

#endif //RULER_H