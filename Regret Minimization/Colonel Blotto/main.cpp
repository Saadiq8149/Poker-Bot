#include <iostream>
#include "cb.h"

using namespace std;

int main() {
    CBTrainer trainer(3, 5);
    cout << "Training..." << endl;
    trainer.train(1000000);
    return 0;
}
