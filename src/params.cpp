#include "params.h"

std::vector<Param*> params;

//hst: history
Param hst_gravity = Param("hst_gravity", 921, 512, 2048, 80);

//bfh: butterfly history
Param bfh_gravity = Param("bfh_gravity", 882, 512, 2048, 80);

//cph: capture history
Param cph_gravity = Param("cph_gravity", 932, 512, 2048, 80);

//cth: continuation history
Param cth_gravity = Param("cth_gravity", 588, 256, 1024, 40);

//crh: correction history
Param crh_gravity = Param("crh_gravity", 207, 128, 512, 20);
Param crh_limit = Param("crh_limit", 253, 128, 512, 20);

//rfp: reverse futility pruning
Param rfp_base = Param("rfp_base", 31, 0, 200, 20);
Param rfp_margin = Param("rfp_margin", 110, 50, 200, 20);

//rzr: razoring
Param rzr_base = Param("rzr_base", 31, 0, 100, 15);
Param rzr_margin = Param("rzr_margin", 200, 50, 250, 40);

//pbc: probcut
Param pbc_margin = Param("pbc_margin", 319, 150, 450, 30);

//klr: killers
Param klr_bonus_0 = Param("klr_bonus_0", 919, 0, 1500, 100);
Param klr_bonus_1 = Param("klr_bonus_1", 452, 0, 750, 40);

//spr: see pruning
Param spr_quiet_threshold = Param("spr_quiet_threshold", 32, 0, 100, 8);

//sxt: singular extensions
Param sxt_margin = Param("sxt_margin", 44, 16, 128, 10);

//dxt: double extensions
Param dxt_margin = Param("dxt_margin", 23, 20, 200, 20);

//lmr: late move reductions
Param lmr_base = Param("lmr_base", 95, 0, 100, 20);
Param lmr_divisor = Param("lmr_divisor", 227, 100, 300, 20);

//hsl: history lmr
Param hsl_subtractor = Param("hsl_subtractor", 14932, 12500, 17500, 100);
Param hsl_divisor = Param("hsl_divisor", 338, 200, 600, 50);

//asp: aspiration windows
Param asp_initial = Param("asp_initial", 17, 10, 60, 3);
Param asp_multiplier = Param("asp_multiplier", 206, 110, 300, 15);
