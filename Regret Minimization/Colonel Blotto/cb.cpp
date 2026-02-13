#include "cb.h"
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

// Random number in [0,1)
static double random01() {
    static mt19937 rng(random_device{}());
    static uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}

CBTrainer::CBTrainer(int numBattlefields, int numTroops) {
    STRATEGIES = initializeStrategies(numBattlefields, numTroops);

    int numActions = STRATEGIES.size();

    regretSum.assign(numActions, 0.0);
    strategy.assign(numActions, 0.0);
    strategySum.assign(numActions, 0.0);
    opponentStrategy.assign(numActions, 0.0);
    oppRegretSum.assign(numActions, 0.0);
    oppStrategySum.assign(numActions, 0.0);
}

void generateRecursiveStrategy(
    int battlefield,
    int remaining,
    int N,
    vector<int>& current,
    vector<string>& strategies
) {
    if (battlefield == N - 1) {
        current[battlefield] = remaining;

        string s = "";
        for (int i = 0; i < N; i++) {
            s += to_string(current[i]);
        }
        strategies.push_back(s);
        return;
    }

    for (int i = 0; i <= remaining; i++) {
        current[battlefield] = i;
        generateRecursiveStrategy(
            battlefield + 1,
            remaining - i,
            N,
            current,
            strategies
        );
    }
}


vector<string> CBTrainer::initializeStrategies(int numBattlefields, int numTroops) {
    vector<string> strategies;
    vector<int> current(numBattlefields, 0);

    generateRecursiveStrategy(
        0,
        numTroops,
        numBattlefields,
        current,
        strategies
    );

    return strategies;
}

void CBTrainer::getStrategy() {
    double normalizingSum = 0.0;
    double opponentNormalizingSum = 0.0;

    for (int i = 0; i < STRATEGIES.size(); i++) {
        strategy[i] = regretSum[i] > 0 ? regretSum[i] : 0;
        normalizingSum += strategy[i];

        opponentStrategy[i] = oppRegretSum[i] > 0 ? oppRegretSum[i] : 0;
        opponentNormalizingSum += opponentStrategy[i];
    }
    for (int i = 0; i < STRATEGIES.size(); i++) {
        if (normalizingSum > 0)
            strategy[i] /= normalizingSum;
        else
            strategy[i] = 1.0 / STRATEGIES.size();

        if (opponentNormalizingSum > 0)
            opponentStrategy[i] /= opponentNormalizingSum;
        else
            opponentStrategy[i] = 1.0 / STRATEGIES.size();

        oppStrategySum[i] += opponentStrategy[i];
        strategySum[i] += strategy[i];
    }
}

int CBTrainer::getAction(vector<double> strat) {
    double r = random01();
    double cumulative = 0.0;

    for (int i = 0; i < STRATEGIES.size(); i++) {
        cumulative += strat[i];
        if (r < cumulative)
            return i;
    }
    return STRATEGIES.size() - 1;
}

vector<double> CBTrainer::getActionUtilities(int opponentAction) {
    vector<double> utility(STRATEGIES.size(), 0.0);

    for (int i = 0; i < STRATEGIES.size(); i++) {
        int wins = 0;
        int losses = 0;

        for (int j = 0; j < STRATEGIES[i].size(); j++) {
            int ours = STRATEGIES[i][j] - '0';
            int opp  = STRATEGIES[opponentAction][j] - '0';

            if (ours > opp) wins++;
            else if (ours < opp) losses++;
        }

        if (wins > losses) utility[i] = 1.0;
        else if (losses > wins) utility[i] = -1.0;
        else utility[i] = 0.0;
    }

    return utility;
}


void CBTrainer::train(int iterations) {
    for (int i = 0; i < iterations; i++) {
        getStrategy();

        int myAction = getAction(strategy);
        int oppAction = getAction(opponentStrategy);

        vector<double> myUtility = getActionUtilities(oppAction);
        vector<double> oppUtility = getActionUtilities(myAction);

        for (int j = 0; j < STRATEGIES.size(); j++) {
            regretSum[j] += myUtility[j] - myUtility[myAction];
            oppRegretSum[j] += oppUtility[j] - oppUtility[oppAction];
        }
    }
    cout << "Average strategy: ";
    for (int i = 0; i < STRATEGIES.size(); i++) {
        double avg = strategySum[i] / iterations;
        cout << STRATEGIES[i] << ": " << avg << endl;
    }
    cout << endl;
}
