#include "params.h"

std::vector<Param*> params;

//hst: history
Param hst_gravity = Param("hst_gravity", 447, 256, 2048, 50);

//bfh: butterfly history
Param bfh_gravity = Param("bfh_gravity", 572, 256, 2048, 50);

//cph: capture history
Param cph_gravity = Param("cph_gravity", 624, 256, 2048, 50);

//cth: continuation history
Param cth_gravity = Param("cth_gravity", 676, 256, 1024, 50);

//crh: correction history
Param crh_gravity = Param("crh_gravity", 110, 64, 512, 10);
Param crh_limit = Param("crh_limit", 256, 128, 512, 20);

//rfp: reverse futility pruning
Param rfp_base = Param("rfp_base", 12, 0, 100, 10);
Param rfp_margin = Param("rfp_margin", 109, 50, 200, 10);

//rzr: razoring
Param rzr_base = Param("rzr_base", 81, 0, 100, 10);
Param rzr_margin = Param("rzr_margin", 225, 50, 250, 30);

//pbc: probcut
Param pbc_margin = Param("pbc_margin", 182, 150, 450, 20);

//klr: killers
Param klr_bonus_0 = Param("klr_bonus_0", 396, 0, 1500, 40);
Param klr_bonus_1 = Param("klr_bonus_1", 396, 0, 750, 40);

//spr: see pruning
Param spr_quiet_threshold = Param("spr_quiet_threshold", 35, 0, 100, 8);

//sxt: singular extensions
Param sxt_margin = Param("sxt_margin", 42, 16, 128, 8);

//dxt: double extensions
Param dxt_margin = Param("dxt_margin", 19, 5, 100, 5);

//lmp: late move pruning
Param lmp_base = Param("lmp_base", 496, 200, 800, 40);
Param lmp_margin = Param("lmp_margin", 164, 100, 400, 20);
Param lmp_improving = Param("lmp_improving", 202, 100, 400, 20);

//lmr: late move reductions
Param lmr_base = Param("lmr_base", 40, -100, 100, 20);
Param lmr_divisor = Param("lmr_divisor", 266, 100, 300, 20);

//hsl: history lmr
Param hsl_divisor = Param("hsl_divisor", 174, 100, 400, 20);

//chl: capture history lmr
Param chl_divisor = Param("chl_divisor", 70, 50, 200, 10);

//ntm: node time management
Param ntm_base = Param("ntm_base", 151, 100, 200, 10);
Param ntm_multiplier = Param("ntm_multiplier", 145, 100, 250, 15);
Param ntm_minimum = Param("ntm_minimum", 88, 0, 150, 10);
Param ntm_maximum = Param("ntm_maximum", 184, 100, 250, 15);

//asp: aspiration windows
Param asp_initial = Param("asp_initial", 18, 10, 50, 3);
Param asp_multiplier = Param("asp_multiplier", 186, 110, 300, 15);
