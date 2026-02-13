#ifndef KP_TRAINER_H
#define KP_TRAINER_H

#include <vector>
#include <string>
#include <map>

class Node {
    public:
        std::string infoSet;
        std::vector<double> regretSum;
        std::vector<double> strategy;
        std::vector<double> strategySum; 

        std::string toString();
        std::vector<double> getStrategy(double realizationWeight);
        std::vector<double> getAverageStrategy();
        Node();
};

class KPTrainer {
    public:
        const static int PASS = 0, BET = 1, NUM_ACTIONS = 2;
        std::map<std::string, Node> nodeMap;

        KPTrainer();
        void train(int iterations);
    
    private:
        void shuffleCards(std::vector<int>& cards);
        double cfr(std::vector<int>& cards, std::string history, double p0, double p1);
};

#endif