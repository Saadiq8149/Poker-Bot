#include <iostream>
#include "kp.h"

using namespace std;

int main() {
    KPTrainer trainer;
    cout << "Training..." << endl;
    trainer.train(1000000);
    return 0;
}