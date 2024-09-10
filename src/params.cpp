#include "params.h"

std::vector<Param*> params;

//rfp: reverse futility pruning
Param rfp_base = Param("rfp_base", 40, 0, 200, 30);
Param rfp_margin = Param("rfp_margin", 150, 50, 300, 30);

//klr: killers
Param klr_bonus_0 = Param("klr_bonus_0", 882, 0, 1500, 80);
Param klr_bonus_1 = Param("klr_bonus_1", 441, 0, 750, 40);

//spr: see pruning
Param spr_quiet_threshold = Param("spr_quiet_threshold", 35, 0, 100, 10);

//sxt: singular extensions
Param sxt_margin = Param("sxt_margin", 45, 16, 128, 16);

//lmr: late move reductions
Param lmr_base = Param("lmr_base", 64, 0, 100, 20);
Param lmr_divisor = Param("lmr_divisor", 248, 200, 400, 30);

//hsl: history lmr
Param hsl_subtractor = Param("hsl_subtractor", 15080, 12500, 17500, 100);
Param hsl_divisor = Param("hsl_divisor", 417, 200, 600, 50);

//asp: aspiration windows
Param asp_initial = Param("asp_initial", 20, 10, 60, 5);
