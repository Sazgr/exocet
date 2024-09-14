#include "params.h"

std::vector<Param*> params;

//rfp: reverse futility pruning
Param rfp_base = Param("rfp_base", 40, 0, 200, 20);
Param rfp_margin = Param("rfp_margin", 150, 50, 200, 20);

//rzr: razoring
Param rzr_base = Param("rzr_base", 50, 0, 100, 15);
Param rzr_margin = Param("rzr_margin", 150, 50, 250, 40);

//pbc: probcut
Param pbc_margin = Param("pbc_margin", 300, 150, 450, 30);

//klr: killers
Param klr_bonus_0 = Param("klr_bonus_0", 882, 0, 1500, 100);
Param klr_bonus_1 = Param("klr_bonus_1", 441, 0, 750, 40);

//spr: see pruning
Param spr_quiet_threshold = Param("spr_quiet_threshold", 35, 0, 100, 8);

//sxt: singular extensions
Param sxt_margin = Param("sxt_margin", 45, 16, 128, 10);

//dxt: double extensions
Param dxt_margin = Param("dxt_margin", 100, 20, 200, 20);

//lmr: late move reductions
Param lmr_base = Param("lmr_base", 64, 0, 100, 20);
Param lmr_divisor = Param("lmr_divisor", 248, 100, 300, 20);

//hsl: history lmr
Param hsl_subtractor = Param("hsl_subtractor", 15080, 12500, 17500, 100);
Param hsl_divisor = Param("hsl_divisor", 417, 200, 600, 50);

//asp: aspiration windows
Param asp_initial = Param("asp_initial", 20, 10, 60, 3);
Param asp_multiplier = Param("asp_multiplier", 200, 110, 300, 15);
