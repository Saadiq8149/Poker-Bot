#include "kp.h"
#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

Node::Node() {
    regretSum.assign(KPTrainer::NUM_ACTIONS, 0.0);
    strategy.assign(KPTrainer::NUM_ACTIONS, 0.0);
    strategySum.assign(KPTrainer::NUM_ACTIONS, 0.0);
}

string Node::toString() {
    std::ostringstream oss;

    oss << std::setw(4) << infoSet << ": [";

    vector<double> avg = getAverageStrategy();
    for (int i = 0; i < avg.size(); i++) {
        oss << avg[i];
        if (i + 1 < avg.size())
            oss << ", ";
    }
    oss << "]";

    return oss.str();
}

vector<double> Node::getStrategy(double realizationWeight) {
    double normalizingSum = 0;

    for (int i = 0; i < KPTrainer::NUM_ACTIONS; i++) {
        strategy[i] = regretSum[i] > 0 ? regretSum[i] : 0;
        normalizingSum += strategy[i];
    }
    for (int i = 0; i < KPTrainer::NUM_ACTIONS; i++) {
        if (normalizingSum > 0)
            strategy[i] /= normalizingSum;
        else
            strategy[i] = 1.0 / KPTrainer::NUM_ACTIONS;
        // Realization weight is used to weight the fact that all the
        // states dont have the same probability of occurence, some are
        // more likely than others.
        strategySum[i] += strategy[i] * realizationWeight;
    }
    return strategy;
}

vector<double> Node::getAverageStrategy() {
    vector<double> averageStrategy(KPTrainer::NUM_ACTIONS, 0);
    double normalizingSum = 0;

    for (int i = 0; i < KPTrainer::NUM_ACTIONS; i++) {
        normalizingSum += strategySum[i];
    }
    for (int i = 0; i < KPTrainer::NUM_ACTIONS; i++) {
        if (normalizingSum > 0) {
            averageStrategy[i] = strategySum[i] / normalizingSum;
        } else {
            averageStrategy[i] = 1.0 / KPTrainer::NUM_ACTIONS;
        }
    }
    return averageStrategy;
}

KPTrainer::KPTrainer() {}

void KPTrainer::shuffleCards(vector<int>& cards) {
    random_device rd;
    mt19937 gen(rd());
    shuffle(cards.begin(), cards.end(), gen);
}

void KPTrainer::train(int iterations) {
    vector<int> cards = {1, 2, 3};
    double util = 0;
    
    for (int i = 0; i < iterations; i++) {
        shuffleCards(cards);
        util += cfr(cards, "", 1, 1);
    }

    cout << "Average game value: " << util / iterations << endl;

    for (auto it : nodeMap) {
        cout << it.second.toString() << endl;
    }
}

double KPTrainer::cfr(vector<int>& cards, string history, double p0, double p1) {
    int plays = history.size();
    int player = plays % 2;

    int opponent = 1 - player;

    if (plays > 1) {
        bool terminalPass = history[plays - 1] == 'p';
        bool doubleBet = history.substr(plays - 2, plays) == "bb";
        bool isPlayerCardHigher = cards[player] > cards[opponent];

        if (terminalPass) {
            if (history == "pp") {
                return isPlayerCardHigher ? 1 : -1;
            } else {
                return 1;
            }
        } else if (doubleBet) {
            return isPlayerCardHigher ? 2 : -2;
        }
    }
    
    string infoSet = to_string(cards[player]) + history;
    
    Node& node = nodeMap[infoSet];
    if (node.infoSet.empty()) {
        node.infoSet = infoSet;
    }


    vector<double> strategy = node.getStrategy(player == 0 ? p0 : p1);
    vector<double> util(KPTrainer::NUM_ACTIONS, 0);
    double nodeUtil = 0;
    for (int i = 0; i < KPTrainer::NUM_ACTIONS; i++) {
        string nextHistory = history + (i == 0 ? "p" : "b");
        util[i] = player == 0
            ? -cfr(cards, nextHistory, p0 * strategy[i], p1)
            : -cfr(cards, nextHistory, p1, p0 * strategy[i]);
        nodeUtil += strategy[i] * util[i];
    }

    for (int i = 0; i < KPTrainer::NUM_ACTIONS; i++) {
        double regret = util[i] - nodeUtil;
        node.regretSum[i] += (player == 0 ? p1 : p0) * regret;
    }

    return nodeUtil;
}
