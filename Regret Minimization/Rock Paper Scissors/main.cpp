#include <iostream>
#include "rps.h"

using namespace std;

int main() {
    RPSTrainer trainer;
    cout << "Training..." << endl;
    trainer.train(1000000);
    return 0;
}
