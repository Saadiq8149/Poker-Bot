#ifndef RPS_TRAINER_H
#define RPS_TRAINER_H

#include <vector>

class RPSTrainer {
    public:
        static const int ROCK = 0;
        static const int PAPER = 1;
        static const int SCISSORS = 2;
        static const int NUM_ACTIONS = 3;
        std::vector<double> regretSum;
        std::vector<double> strategy;
        std::vector<double> strategySum;
        std::vector<double> oppRegretSum;
        std::vector<double> oppStrategy;
        std::vector<double> oppStrategySum;
        std::vector<double> opponentStrategy;

        RPSTrainer();
        void train(int iterations);
    
    private:
        void getStrategy();
        int getAction(std::vector<double> strat);
        std::vector<double> getActionUtilities(int opponentAction);
};

#endif
