#include "params.h"

std::vector<Param*> params;

//rfp: reverse futility pruning
Param rfp_base = Param("rfp_base", 38, 0, 200, 30);
Param rfp_margin = Param("rfp_margin", 125, 50, 300, 30);

//klr: killers
Param klr_bonus_0 = Param("klr_bonus_0", 946, 0, 1500, 100);
Param klr_bonus_1 = Param("klr_bonus_1", 353, 0, 750, 50);

//spr: see pruning
Param spr_quiet_threshold = Param("spr_quiet_threshold", 33, 0, 100, 10);

//sxt: singular extensions
Param sxt_margin = Param("sxt_margin", 42, 16, 128, 16);

//lmr: late move reductions
Param lmr_base = Param("lmr_base", 59, 0, 100, 20);
Param lmr_divisor = Param("lmr_divisor", 206, 200, 400, 30);

//hsl: history lmr
Param hsl_subtractor = Param("hsl_subtractor", 15069, 12500, 17500, 100);
Param hsl_divisor = Param("hsl_divisor", 383, 200, 600, 50);

//asp: aspiration windows
Param asp_initial = Param("asp_initial", 18, 10, 60, 5);
Param asp_multiplier = Param("asp_multiplier", 193, 110, 300, 10);
