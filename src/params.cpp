#include "params.h"

std::vector<Param*> params;

//hst: history
Param hst_gravity = Param("hst_gravity", 555, 256, 2048, 50);

//bfh: butterfly history
Param bfh_gravity = Param("bfh_gravity", 596, 256, 2048, 50);

//cph: capture history
Param cph_gravity = Param("cph_gravity", 819, 256, 2048, 80);

//sch: small capture history
Param sch_gravity = Param("sch_gravity", 819, 256, 2048, 80);

//cth: continuation history
Param cth_gravity = Param("cth_gravity", 719, 256, 1024, 50);

//crh: correction history
Param crh_gravity = Param("crh_gravity", 134, 64, 512, 10);
Param crh_limit = Param("crh_limit", 256, 128, 512, 20);

//rfp: reverse futility pruning
Param rfp_base = Param("rfp_base", 11, 0, 100, 10);
Param rfp_margin = Param("rfp_margin", 89, 50, 200, 10);

//rzr: razoring
Param rzr_base = Param("rzr_base", 46, 0, 100, 10);
Param rzr_margin = Param("rzr_margin", 201, 50, 250, 30);

//pbc: probcut
Param pbc_margin = Param("pbc_margin", 242, 150, 450, 30);

//klr: killers
Param klr_bonus_0 = Param("klr_bonus_0", 903, 0, 1500, 100);
Param klr_bonus_1 = Param("klr_bonus_1", 371, 0, 750, 40);

//spr: see pruning
Param spr_quiet_threshold = Param("spr_quiet_threshold", 32, 0, 100, 8);

//sxt: singular extensions
Param sxt_margin = Param("sxt_margin", 44, 16, 128, 8);

//dxt: double extensions
Param dxt_margin = Param("dxt_margin", 15, 5, 100, 5);

//lmp: late move pruning
Param lmp_base = Param("lmp_base", 396, 200, 800, 40);
Param lmp_margin = Param("lmp_margin", 169, 100, 400, 20);
Param lmp_improving = Param("lmp_improving", 191, 100, 400, 20);

//lmr: late move reductions
Param lmr_base = Param("lmr_base", 16, -100, 100, 20);
Param lmr_divisor = Param("lmr_divisor", 208, 100, 300, 20);

//hsl: history lmr
Param hsl_subtractor = Param("hsl_subtractor", 14849, 12500, 17500, 100);
Param hsl_divisor = Param("hsl_divisor", 227, 100, 600, 30);

//ntm: node time management
Param ntm_base = Param("ntm_base", 147, 100, 200, 10);
Param ntm_multiplier = Param("ntm_multiplier", 154, 100, 250, 15);
Param ntm_minimum = Param("ntm_minimum", 89, 0, 150, 10);
Param ntm_maximum = Param("ntm_maximum", 191, 100, 250, 15);

//asp: aspiration windows
Param asp_initial = Param("asp_initial", 20, 10, 60, 3);
Param asp_multiplier = Param("asp_multiplier", 216, 110, 300, 15);
