#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <utility>
#include <algorithm>

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

typedef pair<string, int> Round;
typedef vector<Round> History;

struct Context {
    vector<string> vocabulary;
    vector<string> candidates;
    vector<int> have_char;
    int verbose;

    Context(int _verbose): verbose(_verbose) {
        // shared global vector
        for (int i = 0; i < 26; i++) {
            have_char.push_back(0);
        }
        vocabulary = load_data("dictionary.json");
        reset_candidates();
    }

    void reset_candidates() {
        candidates = vocabulary;
    }
};

int compute_state(const string &guess, const string &target, vector<int> &have_char) {
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

int to_state(string state_string) {
    int power = 1;
    int state = 0;
    for (int i = 0; i < 5; i++) {
        if (state_string[i] == 'G' || state_string[i] == 'g') {
            state += (power << 1);
        } else if (state_string[i] == 'Y' || state_string[i] == 'y') {
            state += power;
        } else if (state_string[i] != '-') {
            cout << "input a invalid state: " << state_string << endl;
            exit(-1);
        }
        power *= 3;
    }
    return state;
}

History input_history(string history_string) {
    History history;
    for (int q = 0; q + 10 < history_string.size(); q += 12) {
        string word = history_string.substr(q, 5);
        string state_string = history_string.substr(q + 6, 5);
        history.push_back(make_pair(word, to_state(state_string)));
    }
    return history;
}

vector<string> compute_candidates(Context &context, Round round) {
    vector<string> new_candidates;
    for (auto &candidate : context.candidates) {
        int flag = 1;
        const string &guess = round.first;
        const int &state = round.second;
        const int candidate_state = compute_state(guess, candidate, context.have_char);
        if (candidate_state == state) {
            new_candidates.push_back(candidate);
        }
    }
    return new_candidates;
}


int cmdOptionIndex(int argc, char* argv[], const std::string & option) {
    char **iter = std::find(argv, argv + argc, option);
    if (iter == argv + argc) {
        return -1;
    }
    return iter - argv;
}

string solve(Context &context) {
    if (context.candidates.size() == context.vocabulary.size()) {
        // pre computed result
        if (context.verbose) {
            cout << "using precomputed result when no clue" << endl; 
        }
        return "lares";
    }

    int n = context.candidates.size();
    if (n == 1) {
        if (context.verbose) {
            cout << "The only candidate is " << context.candidates[0] << endl;;
        }
        return context.candidates[0];
    } else if (n == 0) {
        if (context.verbose) {
            cout << "Bugged QQ, zero candidate from given history hence we cannot guess" << endl;;
        }
        return "";
    }

    int min_exp = n * n;
    string min_guess;
    for (auto &guess : context.vocabulary) {
        vector<int> freq;
        for (int i = 0; i < 243; i++) {
            freq.push_back(0);
        }
        for (auto &ans : context.candidates) {
            int state = compute_state(guess, ans, context.have_char);
            freq[state] += 1;
        }
        int exp = 0;
        for (int i = 0; i < 243; i++) {
            exp += freq[i] * freq[i];
        }
        if (min_exp >= exp) {
            min_exp = exp;
            min_guess = guess;
        }
    }

    if (context.verbose) {
        cout << "number of candidates: " << n << endl;
        if (n <= 20) {
            for (auto &ans : context.candidates) {
                 cout << ans << endl;
            }
        }
        cout << "Best guess: " << min_guess
            << ", with E(# candidates after guess) = " << 1. * min_exp / n << endl;
    }
    return min_guess;
}

string solve_one_time(Context &context, string &history_string) {
    History history = input_history(history_string);
    for (const auto &round: history) {
        vector<string> new_candidates = compute_candidates(context, round);
        context.candidates = new_candidates;
    }
    return solve(context);
}

int solve_interactive(Context &context) {
    bool terminate = false;
    context.reset_candidates();
    while (true) {
        string guess = solve(context);
        string state_string;
        cout << guess << endl;
        cin >> state_string;

        if (state_string == "next") {
            return 1;
        } else if (state_string.size() != 5){
            return 0;
        }

        Round round = make_pair(guess, to_state(state_string));
        context.candidates = compute_candidates(context, round);
    }
}

int main(int argc, char *argv[]) {
    if (cmdOptionIndex(argc, argv, "--help") != -1) {
        cout << "solver [options]" << endl;
        cout << "  solver (one time, no input)" << endl;
        cout << "  solver --input weary,--yg-,pills,----- (one time)" << endl;
        cout << "  solver --input weary,--yg-,pills,----- --verbose (one time, verbose)" << endl;
        cout << "  solver --interactive" << endl;
        return 0;
    }
    const int verbose = cmdOptionIndex(argc, argv, "--verbose") != -1;
    const int interactive = cmdOptionIndex(argc, argv, "--interactive") != -1;
    Context context = Context(verbose);
    if (interactive) {
        while (solve_interactive(context));
    } else {
        // + 1 to get the param after -s
        int input_index = cmdOptionIndex(argc, argv, "--input") + 1;
        string history_string;
        if (input_index != 0) {
            history_string = string(argv[input_index]);
        } else {
            history_string = string();
        }

        cout << solve_one_time(context, history_string) << endl;
    }

    return 0;
}
