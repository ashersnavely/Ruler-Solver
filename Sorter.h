//
// Created by ash on 9/11/19.
//

#ifndef SORTER_H
#define SORTER_H

#include <type_traits>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/has_less.hpp>
#include <vector>

using namespace std;

class Sorter {
private:
    template <typename Comparable, enable_if_t<boost::has_less<Comparable>::value, long long int> = 0>
    static void mergeSortAux(vector<Comparable> &vec, long long int start, long long int mid, long long int end);

public:
    template <typename Comparable, enable_if_t<boost::has_less<Comparable>::value, long long int> = 0>
    static vector<Comparable> &mergeSort(vector<Comparable> &vec, long long int start = -1, long long int end = -1);
};

template<typename Comparable, enable_if_t<boost::has_less<Comparable>::value, long long int>>
vector<Comparable> &Sorter::mergeSort(vector<Comparable> &vec, long long int start, long long int end) {
    if(start < 0) {start = 0;}
    if(end < 0) {end = vec.size() - 1;}

    if  (start < end) {
        long long int mid = (start + end)/ 2;

        mergeSort(vec, start, mid);
        mergeSort(vec, mid + 1, end);

        mergeSortAux(vec, start, mid, end);
    }

    return vec;
}

template<typename Comparable, enable_if_t<boost::has_less<Comparable>::value, long long int>>
void Sorter::mergeSortAux(vector<Comparable> &vec, long long int start, long long int mid, long long int end) {
    //for work with iterators
    end++;
    mid++;

    vector<Comparable> temp(vec.begin() + start, vec.begin() + end);
    long long int indx = 0, lIndx = 0, lBound = mid - start, rIndx = lBound, rBound = end - start;

    for(; lIndx < lBound && rIndx < rBound; indx++) {
        vec[start + indx] = temp[lIndx] < temp[rIndx] ? temp[lIndx++] : temp[rIndx++];
    }

    if(lIndx < lBound) {
        copy(temp.begin() + lIndx, temp.begin() + lBound, vec.begin() + start + indx);
    } else if(rIndx < rBound) {
        copy(temp.begin() + rIndx, temp.begin() + rBound, vec.begin() + start + indx);
    }
}

#endif //SORTER_H
