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
History input_history() {
    cout << "n (guessed count): ";
    int n;
    cin >> n;
    vector<Round> history;
    cout << "now input word state (example: \"lares -YY-g\", Y for yellow, g for green):" << endl;
    for (int i = 0; i < n; i++) {
        string word, state_string; 
        cout << "word_" << i << " state_" << i << ": ";
        cin >> word >> state_string;
        int p = 1;
        int state = 0;
        for (int i = 0; i < 5; i++) {
            int j = 4 - i;
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

int main() {
    // shared global vector
    vector<int> have_char;
    for (int i = 0; i < 26; i++) {
        have_char.push_back(0);
    }

    vector<string> candidates = load_data("dictionary.json");

    History history = input_history();
    for (auto &r : history) {
        cout << r.first << "  " << r.second << endl;
    }
    candidates = filter_candidates(candidates, history, have_char);

    int n = candidates.size();    
    cout << "number of candidates: " << n << endl;

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
        if (exp < min_exp) {
            min_exp = exp;
            min_guess = guess;
        }
    }

    cout << "Best guess: " << min_guess
         << ", with E(# candidates after guess) = " << 1. * min_exp / n << endl;
    
    return 0;
}
