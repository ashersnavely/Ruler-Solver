//
// Created by ash on 9/22/19.
//

#include "Ruler.h"

Ruler* Ruler::singleton = nullptr;
int Ruler::length, Ruler::order;
set<int> Ruler::_set;

Ruler* Ruler::init() {
    return singleton ? singleton : (singleton = new Ruler);
}

void Ruler::free() {
    if (singleton) {
        delete singleton;
        singleton = nullptr;
    }
}

Ruler::Ruler() {
    // seed rng first:
    rng_type::result_type const seedval = time(NULL);
    rng.seed(seedval);

    load();
}

void Ruler::load() {
    int l, o;

    //not able to use when compiling for windows
    // !filesystem::exists(fName)
    if(!ifstream(fName.c_str()).good()) {
        ofstream temp;
        temp.open(fName, ios::app);
        temp.close();
    }

    perfectLOLog.open(fName, ios_base::in | ios_base::binary);
    while(perfectLOLog) {
        perfectLOLog.read(reinterpret_cast<char*>(&l), sizeof(int));
        perfectLOLog.read(reinterpret_cast<char*>(&o), sizeof(int));
        if(perfectLOLog) {
            perfectLengthOrder.insert(pair<int, int>(l, o));
        }
    }
    perfectLOLog.close();
}

Ruler::~Ruler() {
    save();
}

void Ruler::save() {
    perfectLOLog.open(fName, ios_base::out | ios_base::trunc | ios_base::binary);
    for(auto iter = perfectLengthOrder.begin(); iter != perfectLengthOrder.end(); iter++) {
        perfectLOLog.write(reinterpret_cast<const char *>(&iter->first), sizeof(int));
        perfectLOLog.write(reinterpret_cast<const char *>(&iter->second), sizeof(int));
    }
    perfectLOLog.close();
}

int Ruler::estimate(vector<int> x, vector<int> y, int degree = 3) {
    vector<double> coefficients = polynomialRegression(x, y, degree);

    double estimation = 0;
    auto cIter = coefficients.begin();
    for(int i = 0; cIter != coefficients.end() && i <= degree; i++, cIter++) {
        estimation += ((*cIter) * pow(Ruler::length, i));
    }

    return round(estimation);
}

void Ruler::log(vector<int>& solution) {
    if(length) {
        perfectLengthOrder.insert(pair<int, int>(solution.back(), solution.size()));
    }
}

vector<int> Ruler::findBestGolombBrute(int order) {
    this->length = 1;
    this->order = order;

    vector<int> solutionSet;
    do{
        //[&choice]() -> bool { return choice < 'a' || choice > 'c'; }
        solutionSet = FBBAux(&Ruler::verifyGolomb, [](int length, int order) -> void {;});
        length++;
    }while(solutionSet.empty());

    return solutionSet;
}

vector<int> Ruler::findBestPerfectBrute(int length) {
    this->length = length;
    this->order = 0;

    return FBBAux(&Ruler::verifyPerfect, [this](vector<int>& solution) -> void {this->log(solution);});
}

template <typename solutionLogger>
vector<int> Ruler::FBBAux(bool (Ruler::*verify)(vector<int> &, vector<typename vector<int>::iterator>&), solutionLogger logger){
    vector<int> _set;
    for (int i = 0; i <= length; i++){
        _set.push_back(i);
    }

    //base set to branch off to each sub set from
    queue<vector<typename vector<int>::iterator>> powerset;
    for(auto start = _set.begin(); start < _set.end(); start++) {
        vector<typename vector<int>::iterator> baseset;
        baseset.push_back(start);

        powerset.push(baseset);
    }

    //process each set until they're done
    for(int combination = 1; !powerset.empty(); combination++) {
        vector<typename vector<int>::iterator> subset = powerset.front();
        powerset.pop();

        int size = subset.size();
        if ((!order || size == order) && (this->*verify)(_set, subset)) {
            vector<int> solution;
            for (auto iter = subset.begin(); iter < subset.end(); iter++) {
                solution.push_back(**iter);
            }

            log(solution);

            return solution;
        }else if (order && size > order) {
            break;
        }

        //generate new subsets if there are any
        for (typename vector<int>::iterator iter = next(subset.back()); iter < _set.end(); iter++) {
            vector<typename vector<int>::iterator> nextset(subset);
            nextset.push_back(iter);

            powerset.push(nextset);
        }
    }

    return vector<int>();
}

bool Ruler::verifyPerfect(vector<int> &_set, vector<typename vector<int>::iterator> &subset) {
    //get all differences and avoid subtracting each from itself
    set<int> diffset = {0};

    for(int i = 0; i < subset.size(); i++) {
        for(int n = i + 1; n < subset.size(); n++) {
            diffset.insert(abs(*subset[i] - *subset[n]));
        }
    }

    //convert to vector
    vector<int> sortedset(diffset.begin(), diffset.end());
    Sorter::mergeSort(sortedset);

    //compare
    return sortedset == _set;
}

bool Ruler::verifyGolomb(vector<int> &_set, vector<typename vector<int>::iterator> &subset) {
    //get all differences and avoid subtracting each from itself
    set<int> diffset = {0};

    for(int i = 0; i < subset.size(); i++) {
        for(int n = i + 1; n < subset.size(); n++) {
            //if false then it means a new element wasnt inserted
            if(!diffset.insert(abs(*subset[i] - *subset[n])).second) {
                return false;
            }
        }
    }

    return true;
}

set<int> Ruler::findMissing(set<int>& _set, set<int>& subset) {
    //get all differences and avoid subtracting each from itself
    set<int> diffset = {0};

    for(int i = 0; i < subset.size(); i++) {
        for(int n = i + 1; n < subset.size(); n++) {
            auto iterL = subset.begin(), iterR = subset.begin();
            advance(iterL, i);
            advance(iterR, n);
            diffset.insert(abs(*iterL - *iterR));
        }
    }

    set<int> diff;
    auto iter = set_difference(_set.begin(), _set.end(), diffset.begin(), diffset.end(), inserter(diff, diff.begin()));

    return diff;
}

vector<int> Ruler::findBestPerfect(int length, int specificOrder) {
    this->length = length;

    if (perfectLengthOrder.empty()) { return vector<int>(); }

    if (specificOrder > 0) {
        this->order = specificOrder;
    }else{
        vector<int> x, y;
        for(auto pIter = perfectLengthOrder.begin(); pIter != perfectLengthOrder.end(); pIter++) {
            x.push_back(pIter->first);
            y.push_back(pIter->second);
        }

        this->order = estimate(x, y);
    }

    vector<int> solution;
    //just in case estimation is bad and only if user didnt specify order
    if((solution = FBPAttempt()).empty() && specificOrder <= 0) {
        for(int low = order - 1, high = order + 1; low > 1 && solution.empty(); low--, high++) {
            order = high;
            if(!(solution = FBPAttempt()).empty()) { break; }
        }
    }

    return solution;
}

vector<int> Ruler::FBPAttempt() {
    //will always have 0 difference in the set, so can ignore in missing
    this->_set.clear();
    for (int i = 1; i <= length; i++){
        this->_set.insert(i);
    }
    set<int> missing = _set;
    _set.insert(0);

    set<int> marked, solution;
    FBPAux(0, length, missing, marked, solution);

    if(!solution.empty()) {
        vector<int> ret(solution.begin(), solution.end());
        log(ret);

        return ret;
    }else{
        return vector<int>();
    }
}

void Ruler::FBPAux(int left, int right, set<int> missing, set<int> marked, set<int>& solution) {
    int m = missing.size();
    int toMark = order - marked.size();

    if(!m && (solution.empty() ? true : marked.size() < solution.size()) && marked.size() <= order) {
        solution = marked;

        //only care about solutions smaller now
        order = solution.size() - 1;
        return;
    }

    /*
     *  if marked is greater than or equal to the order
     *  if nothing to mark
     *  if none is missing but it is worse than a previous solution
     *  if a missing distance is too long to be covered by adding a mark in remaining space
     *  if more distances missing than can be generated by adding toMark marks
     *      toMark*(toMark - 1) / 2 distances can be generated between the new marks
     *      toMark*(order - toMark) distances can be generated between the existing and new marks
     *  if not enough space left to mark remaining
     */
    if (marked.size() >= order ||
        toMark <= 0 ||
        missing.empty() ||
        (length - left < *prev(missing.end()) && length - right < *prev(missing.end())) ||
        (toMark * (toMark - 1) / 2) + (toMark * (order - toMark)) < m ||
        toMark > (right - left) + 1) {
        return;
    }

    set<int> copy;
    typename set<int>::iterator lIter, rIter;

    //try putting a mark at the left bound
    copy = marked;
    lIter = _set.begin();
    advance(lIter, left);
    copy.insert(*lIter);

    FBPAux(left + 1, right - 1, findMissing(_set, copy), copy, solution);

    //try putting a mark at the right bound
    copy = marked;
    rIter = _set.begin();
    advance(rIter, right);
    copy.insert(*rIter);

    FBPAux(left + 1, right - 1, findMissing(_set, copy), copy, solution);

    //try putting a mark at both bounds
    copy = marked;
    copy.insert(*lIter);
    copy.insert(*rIter);

    FBPAux(left + 1, right - 1, findMissing(_set, copy), copy, solution);

    //try not putting a mark
    FBPAux(left + 1, right - 1, missing, marked, solution);
}

int Ruler::getLength() {
    return length;
}

void Ruler::setLength(int length) {
    Ruler::length = length;
}

int Ruler::getOrder() {
    return order;
}

void Ruler::setOrder(int order) {
    Ruler::order = order;
}

const set<int> &Ruler::getSet() {
    return _set;
}

void Ruler::setSet(const set<int> &set) {
    _set = set;
}
