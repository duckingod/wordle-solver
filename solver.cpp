#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <utility>

using namespace std;

vector<string> load_data(string file_name) {
    fstream myfile;
    std::ifstream f(file_name);
    std::stringstream ss;
    ss << f.rdbuf(); //read the file
    std::string content = ss.str(); //str holds the content of the file
    vector<string> result;

    for (auto i = 2; i + 5 < content.size(); i += 9) {
        // skip `"aaaaa", ` -> 8 characters
        result.push_back(content.substr(i, 5));
    }
    return result;
}

int to_state(string &guess, string &target, vector<int> &have_char) {
    for (auto c : target) {
        have_char[c - 'a'] = 1;
    }
    int state = 0;
    int prod = 1;
    for (int i = 0; i < 5; i++) {
        if (guess[i] == target[i]) {
            state += (prod << 1);
        } else if (have_char[guess[i] - 'a']) {
            state += prod;
        }
        prod *= 3;
    }
    for (auto c : target) {
        have_char[c - 'a'] = 0;
    }
    return state;
}

typedef pair<string, int> Round;
typedef vector<Round> History;
History input_history(string history_string) {
    History history;
    for (int q = 0; q + 10 < history_string.size(); q += 12) {
        string word = history_string.substr(q, 5);
        string state_string = history_string.substr(q + 6, 5);
        int p = 1;
        int state = 0;
        for (int i = 0; i < 5; i++) {
            if (state_string[i] == 'G' || state_string[i] == 'g') {
                state += (p << 1);
            } else if (state_string[i] == 'Y' || state_string[i] == 'y') {
                state += p;
            } else if (state_string[i] != '-') {
                cout << "input a invalid state: " << state_string << endl;
                exit(-1);
            }
            p *= 3;
        }
        history.push_back(make_pair(word, state));
    }
    return history;
}

vector<string> filter_candidates(vector<string> candidates_in, History history, vector<int> &have_char) {
    vector<string> candidates;
    for (auto &candidate : candidates_in) {
        int flag = 1;
        for (auto round : history) {
            if (to_state(round.first, candidate, have_char) != round.second) {
                flag = 0;
                break;
            }
        }
        if (flag) {
            candidates.push_back(candidate);
        }
    }
    return candidates;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        // pre computed result
        cout << "lares" << endl;
        return 0;
    }
    if (argc == 2 && string(argv[1]) == "help") {
        cout << "solver <history> [v]" << endl;
        cout << "  solver" << endl;
        cout << "  solver weary,--yg-,pills,-----" << endl;
        cout << "  solver weary,--yg-,pills,----- v (verbose)" << endl;
        return 0;
    }
    int verbose = false;
    if (argc == 3) {
        verbose = true;
    }

    // shared global vector
    vector<int> have_char;
    for (int i = 0; i < 26; i++) {
        have_char.push_back(0);
    }
    History history = input_history(string(argv[1]));

    vector<string> candidates = load_data("dictionary.json");

    candidates = filter_candidates(candidates, history, have_char);

    int n = candidates.size();
    if (verbose) {
        cout << "number of candidates: " << n << endl;
    }

    int min_exp = n * n;
    string min_guess;
    for (auto &guess : candidates) {
        vector<int> freq;
        for (int i = 0; i < 243; i++) {
            freq.push_back(0);
        }
        for (auto &ans : candidates) {
            int state = to_state(guess, ans, have_char);
            freq[state] += 1;
        }
        int exp = 0;
        for (int i = 0; i < 243; i++) {
            exp += freq[i] * freq[i];
        }
        if (exp <= min_exp) {
            min_exp = exp;
            min_guess = guess;
        }
    }

    if (verbose) {
        cout << "Best guess: " << min_guess
            << ", with E(# candidates after guess) = " << 1. * min_exp / n << endl;
    } else {
        cout << min_guess << endl;
    }
    return 0;
}
