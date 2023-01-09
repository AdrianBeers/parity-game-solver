#include <map>
#include <memory>
#include <vector>

#include "ParityGame.h"

using namespace std;

enum LiftStrategy { Input,
                Random };

typedef vector<uint32_t> Measure;
typedef map<shared_ptr<NodeSpec>, shared_ptr<Measure>> ProgressMeasure;

class Solver {
    shared_ptr<ParityGame> G;
    vector<vector<uint32_t>> M;
    unsigned int d;

    shared_ptr<Measure> prog(shared_ptr<ProgressMeasure> &rho, shared_ptr<NodeSpec> &v, shared_ptr<NodeSpec> &w);
    bool progLessOrEqual(shared_ptr<Measure> &progA, shared_ptr<Measure> &progB) const;
    shared_ptr<ProgressMeasure> lift(shared_ptr<ProgressMeasure> &rho, shared_ptr<NodeSpec> &v);
    bool isStabilised(shared_ptr<ProgressMeasure> &rho, shared_ptr<ProgressMeasure> &rhoLifted);

public:
    void initialize(shared_ptr<ParityGame> &parityGame);
    shared_ptr<ProgressMeasure> SPM(LiftStrategy strategy);
};