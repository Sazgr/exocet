#include "params.h"

std::vector<Param*> params;

//hst: history
Param hst_gravity = Param("hst_gravity", 1024, 512, 2048, 80);

//bfh: butterfly history
Param bfh_gravity = Param("bfh_gravity", 1024, 512, 2048, 80);

//cph: capture history
Param cph_gravity = Param("cph_gravity", 1024, 512, 2048, 80);

//cth: continuation history
Param cth_gravity = Param("cth_gravity", 512, 256, 1024, 40);

//crh: correction history
Param crh_gravity = Param("crh_gravity", 256, 128, 512, 20);
Param crh_limit = Param("crh_limit", 256, 128, 512, 20);

//rfp: reverse futility pruning
Param rfp_base = Param("rfp_base", 19, 0, 200, 20);
Param rfp_margin = Param("rfp_margin", 128, 50, 200, 20);

//rzr: razoring
Param rzr_base = Param("rzr_base", 47, 0, 100, 15);
Param rzr_margin = Param("rzr_margin", 179, 50, 250, 40);

//pbc: probcut
Param pbc_margin = Param("pbc_margin", 342, 150, 450, 30);

//klr: killers
Param klr_bonus_0 = Param("klr_bonus_0", 971, 0, 1500, 100);
Param klr_bonus_1 = Param("klr_bonus_1", 435, 0, 750, 40);

//spr: see pruning
Param spr_quiet_threshold = Param("spr_quiet_threshold", 40, 0, 100, 8);

//sxt: singular extensions
Param sxt_margin = Param("sxt_margin", 38, 16, 128, 10);

//dxt: double extensions
Param dxt_margin = Param("dxt_margin", 57, 20, 200, 20);

//lmr: late move reductions
Param lmr_base = Param("lmr_base", 61, 0, 100, 20);
Param lmr_divisor = Param("lmr_divisor", 227, 100, 300, 20);

//hsl: history lmr
Param hsl_subtractor = Param("hsl_subtractor", 15047, 12500, 17500, 100);
Param hsl_divisor = Param("hsl_divisor", 440, 200, 600, 50);

//asp: aspiration windows
Param asp_initial = Param("asp_initial", 19, 10, 60, 3);
Param asp_multiplier = Param("asp_multiplier", 220, 110, 300, 15);
