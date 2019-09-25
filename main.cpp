#include <iostream>
#include "Sorter.h"
#include "Ruler.h"

//To keep i/o clean
template <typename var, typename function>
void getInput(string&, var&, function, istream& = cin, ostream& = cout);
void printChoiceError(istream&, ostream&);
void log(vector<int>& solution);

int main() {
    char choice = 0;
    int var;

    string promptChoices = "Please choose one of the following choices by entering its letter... \n";
    string promptVecGen = promptChoices +
                          "\ta). Find A Golomb Ruler Brute Force\n"
                          "\tb). Find A Perfect Ruler Brute Force\n"
                          "\tc). Find A Perfect Ruler Pidgeon-Hole\n";
    getInput(promptVecGen, choice, [&choice]() -> bool { return choice < 'a' || choice > 'c'; });

    string promptSize = (choice == 'a' ?
            "Please enter the order of the of the Golomb Ruler as an int: " :
            "Please enter the length of the Perfect Ruler as an int: ");
    getInput(promptSize, var, [&var]() -> bool { return var <= 0; });

    Ruler* ruler = Ruler::init();
    vector<int> solution;
    if(choice == 'a') {
        solution = ruler->findBestGolombBrute(var);
    }else if(choice == 'b') {
        solution = ruler->findBestPerfectBrute(var);
    }else if(choice == 'c') {
        string promptUpTo = promptChoices +
                            "\ta). Find up to the order (can take a long time!)\n"
                            "\tb). Find only the specific order\n";
        getInput(promptChoices, choice, [&choice]() -> bool { return choice != 'a' && choice != 'b'; });
        bool UpTo = choice == 'a';

        string promptOrderAcq = promptChoices +
                              "\ta). Specify an order\n"
                              "\tb). Try to estimate the order\n";
        getInput(promptOrderAcq, choice, [&choice]() -> bool { return choice != 'a' && choice != 'b'; });

        if(choice == 'a') {
            int order;
            cout << "Please enter the order: ";
            cin >> order;

            solution = ruler->findBestPerfect(var, order);
        }else if(choice == 'b') {
            if(UpTo) {
                bool found = false;
                for(int i = 1; i < var; i++, found = false) {
                    for(int n = 0; n < ruler->perfectLengthOrder.size(); n++) {
                        auto iter = ruler->perfectLengthOrder.begin();
                        advance(iter, n);

                        if(iter->first == i) {
                            found = true;
                        }
                    }

                    if(!found) {
                        if(i <= 20) {
                            solution = ruler->findBestPerfectBrute(i);
                        }else {
                            solution = ruler->findBestPerfect(i);
                        }

                        ruler->save();
                        log(solution);
                    }
                }
            }

            solution = ruler->findBestPerfect(var);
        }
    }
    Ruler::free();
    log(solution);

    if(solution.size()) {
        cout << endl << "An optimal solution of order " << solution.size() << " and length " << *prev(solution.end())
             << " was found -" << endl << "(";
        for(auto iter = solution.begin(); iter != solution.end();) {
            cout << *iter++;

            if(iter != solution.end()) {
                cout << ", ";
            }
        }
        cout << ")";
    }else{
        cout << endl << "An optimal solution was not found...";
    }
    cout << endl << "Enter any key to quit";
    char wait;
    cin >> wait;

    return 0;
}

template <typename var, typename function>
void getInput(string& prompt, var& destination, function trueIfFail, istream& in, ostream& out) {
    bool chosen = false;

    do{
        if(cin.fail() || chosen) { printChoiceError(cin, cout); }

        cout << prompt;
        cin >> destination;

        chosen = true;
    } while (cin.fail() || trueIfFail());
}

void printChoiceError(istream& in, ostream& out) {
    in.clear();
    in.ignore();
    out << "That wasn't an option, please try again." << endl << endl;
}

void log(vector<int> &solution) {
    ofstream out;
    out.open(to_string(solution.back()) + "_solution.txt", ios::trunc);

    for(int i = 0; i < solution.size(); i++) {
        out << to_string(solution[i]) + " ";
    }

    out.close();
}
