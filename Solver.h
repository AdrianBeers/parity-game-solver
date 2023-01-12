#include <map>
#include <memory>
#include <vector>

#include "ParityGame.h"

using namespace std;

enum LiftStrategy { Input,
                Random,
                Predecessor,
                PredecessorMax,
                FocusList};

typedef vector<uint32_t> Measure;
typedef map<uint32_t, shared_ptr<Measure>> ProgressMeasure;

class Solver {
    shared_ptr<ParityGame> G;
    Measure M;
    unsigned int d;

    shared_ptr<Measure> prog(shared_ptr<ProgressMeasure> &rho, shared_ptr<NodeSpec> &v, shared_ptr<NodeSpec> &w);
    bool progLessOrEqual(shared_ptr<Measure> &progA, shared_ptr<Measure> &progB) const;
    shared_ptr<ProgressMeasure> lift(shared_ptr<ProgressMeasure> &rho, shared_ptr<NodeSpec> &v);
public:
    void initialize(shared_ptr<ParityGame> &parityGame);
    shared_ptr<ProgressMeasure> SPM(LiftStrategy strategy);
};