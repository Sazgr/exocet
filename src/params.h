#ifndef EXOCET_PARAMS
#define EXOCET_PARAMS

#include <string>
#include <vector>

struct Param;

extern std::vector<Param*> params;

struct Param {
    std::string name;
    int value;
    int min;
    int max;
    int step;
    Param(std::string param_name, int param_value, int param_min, int param_max, int param_step) {
        name = param_name;
        value = param_value;
        min = param_min;
        max = param_max;
        step = param_step;
        params.push_back(this);
    }
    operator int() const {
        return value;
    }
};

extern Param hst_gravity;
extern Param bfh_gravity;
extern Param cph_gravity;
extern Param cth_gravity;
extern Param crh_gravity;
extern Param crh_limit;
extern Param rfp_base;
extern Param rfp_margin;
extern Param rzr_base;
extern Param rzr_margin;
extern Param pbc_margin;
extern Param klr_bonus_0;
extern Param klr_bonus_1;
extern Param spr_quiet_threshold;
extern Param sxt_margin;
extern Param dxt_margin;
extern Param lmp_base;
extern Param lmp_margin;
extern Param lmp_improving;
extern Param lmr_base;
extern Param lmr_divisor;
extern Param hsl_divisor;
extern Param chl_divisor;
extern Param ntm_base;
extern Param ntm_multiplier;
extern Param ntm_minimum;
extern Param ntm_maximum;
extern Param asp_initial;
extern Param asp_multiplier;

#endif
