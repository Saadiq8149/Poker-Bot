#ifndef CB_TRAINER_H
#define CB_TRAINER_H

#include <vector>
#include <string>

class CBTrainer {
    public:
        std::vector<std::string> STRATEGIES;
        std::vector<double> regretSum;
        std::vector<double> strategy;
        std::vector<double> strategySum;
        std::vector<double> oppRegretSum;
        std::vector<double> oppStrategySum;
        std::vector<double> opponentStrategy;

        CBTrainer(int numBattlefields = 3, int numTroops = 5);
        void train(int iterations);

    private:
        void getStrategy();
        int getAction(std::vector<double> strat);
        std::vector<double> getActionUtilities(int opponentAction);
        std::vector<std::string> initializeStrategies(int numBattlefields, int numTroops);
};

#endif
