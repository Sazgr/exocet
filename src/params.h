#ifndef EXOCET_PARAMS
#define EXOCET_PARAMS

#include <string>
#include <vector>

#ifdef EXOCET_TUNE
#define tunable
#else
#define tunable const
#endif

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

extern tunable Param hst_gravity;
extern tunable Param bfh_gravity;
extern tunable Param cph_gravity;
extern tunable Param cth_gravity;
extern tunable Param crh_gravity;
extern tunable Param crh_limit;
extern tunable Param rfp_base;
extern tunable Param rfp_margin;
extern tunable Param rzr_base;
extern tunable Param rzr_margin;
extern tunable Param pbc_margin;
extern tunable Param klr_bonus_0;
extern tunable Param klr_bonus_1;
extern tunable Param spr_quiet_threshold;
extern tunable Param sxt_margin;
extern tunable Param dxt_margin;
extern tunable Param lmp_base;
extern tunable Param lmp_margin;
extern tunable Param lmp_improving;
extern tunable Param lmr_base;
extern tunable Param lmr_divisor;
extern tunable Param hsl_divisor;
extern tunable Param chl_divisor;
extern tunable Param ntm_base;
extern tunable Param ntm_multiplier;
extern tunable Param ntm_minimum;
extern tunable Param ntm_maximum;
extern tunable Param asp_initial;
extern tunable Param asp_multiplier;

#endif
