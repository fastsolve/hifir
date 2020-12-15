///////////////////////////////////////////////////////////////////////////////
//                  This file is part of HIF project                         //
///////////////////////////////////////////////////////////////////////////////

#include "common.hpp"
// line break to avoid sorting
#include "hif/Options.h"
#include "hif/ds/CompressedStorage.hpp"
#include "hif/ds/DenseMatrix.hpp"
#include "hif/small_scale/solver.hpp"

#include <gtest/gtest.h>

using namespace hif;

constexpr static double tol(1e-12);

TEST(LU, d) {
  // randomized matrix from MATLAB of 20x20, in row order
  // generate rhs b then solve with backslash and save in x_ref
  const static double
      a[400] = {0.8147236863931789,  0.6557406991565868,   0.4387443596563982,
                0.7512670593056529,  0.3516595070629968,   0.1621823081932428,
                0.1066527701805844,  0.8530311177218937,   0.7802520683211379,
                0.547008892286345,   0.6443181301936917,   0.3111022866504128,
                0.08551579709004398, 0.03773886623955214,  0.03054094630463666,
                0.05961886757963919, 0.1733886131190056,   0.9516304647777269,
                0.03260082053052804, 0.2518061224723128,   0.9057919370756192,
                0.03571167857418955, 0.3815584570930084,   0.2550951154592691,
                0.8308286278962909,  0.794284540683907,    0.9618980808550537,
                0.622055131485066,   0.3897388369612534,   0.2963208056077732,
                0.3786093826602684,  0.9233796421032439,   0.2624822346983327,
                0.8851680082024753,  0.7440742603674624,   0.6819719041490632,
                0.3909378023237355,  0.9203320398365638,   0.5611997927096601,
                0.2904406642769793,  0.1269868162935061,   0.8491293058687771,
                0.7655167881490024,  0.5059570516651424,   0.5852640911527243,
                0.3112150420448049,  0.004634224134067444, 0.3509523808922709,
                0.2416912859138327,  0.7446928070741562,   0.8115804582824772,
                0.430207391329584,   0.8010146227697388,   0.913286827639239,
                0.5000224355902009,  0.04243113750074168,  0.8313797428390696,
                0.05267699768079259, 0.8818665004518099,   0.6170908843932233,
                0.9133758561390194,  0.9339932477575505,   0.7951999011370632,
                0.699076722656686,   0.5497236082911395,   0.5285331355062127,
                0.7749104647115024,  0.5132495398670534,   0.4039121455881147,
                0.1889550150325445,  0.5328255887994549,   0.1848163201241361,
                0.02922027756214629, 0.796183873585212,    0.4799221411460605,
                0.07144546460064238, 0.8033643916024402,   0.7378580955169965,
                0.6691753045343938,  0.2652809098100294,   0.6323592462254095,
                0.6787351548577735,  0.1868726045543786,   0.8909032525357985,
                0.91719366382981,    0.1656487294997809,   0.817303220653433,
                0.4018080337519417,  0.09645452516838859,  0.686775433365315,
                0.3507271035768833,  0.9048809686798929,   0.9288541394780446,
                0.0987122786555743,  0.9047222380673627,   0.5216498424642837,
                0.06047117916989364, 0.2691194263985559,   0.1904332671799541,
                0.824376266688835,   0.09754040499940952,  0.7577401305783334,
                0.4897643957882311,  0.9592914252054443,   0.2858390188203735,
                0.6019819414016365,  0.8686947053635097,   0.07596669169084191,
                0.1319732926063351,  0.1835111557372697,   0.9390015619998868,
                0.9797483783560852,  0.7303308628554529,   0.2618711838707161,
                0.6098666484225584,  0.09673002578086698,  0.3992577706135757,
                0.4228356150088078,  0.3689165460638949,   0.9826633997219503,
                0.2784982188670484,  0.7431324681249162,   0.4455862007108995,
                0.5472155299638031,  0.7572002291107213,   0.2629712845401443,
                0.08443584551091032, 0.239916153553658,    0.9420505907754851,
                0.3684845964903365,  0.8759428114929838,   0.4388699731261032,
                0.4886089738035791,  0.3353568399627965,   0.6176663895884547,
                0.8181485538596247,  0.5268758305082959,   0.5478709012148447,
                0.4607259372604116,  0.7302487922675976,   0.5468815192049838,
                0.3922270195341682,  0.6463130101112646,   0.1386244428286791,
                0.7537290942784953,  0.6540790984767823,   0.3997826490988965,
                0.1233189348351655,  0.9561345402298023,   0.6256185607296904,
                0.5501563428984222,  0.1111192234405988,   0.5785250610234389,
                0.679727951377338,   0.8594423056462123,   0.8175470920792863,
                0.4167994679307869,  0.9427369842769343,   0.9816379509707497,
                0.3438770041149831,  0.9575068354342976,   0.6554778901775566,
                0.7093648308580726,  0.1492940055590575,   0.3804458469753567,
                0.6892145031400078,  0.2598704028506542,   0.1839077882824167,
                0.5752085950784656,  0.7802274351513768,   0.6224750860012275,
                0.2580646959120669,  0.2372835797715215,   0.1365531373553697,
                0.8054894245296856,  0.7224395923668423,   0.6568598909737072,
                0.4177441043166622,  0.1564049522265635,   0.584069333278452,
                0.9648885351992765,  0.1711866878115618,   0.7546866819823609,
                0.2575082541237365,  0.5678216407252211,   0.7481515928237095,
                0.8000684802243075,  0.2399525256649028,   0.05977954294715582,
                0.08112576886578526, 0.5870447045314168,   0.4087198461125521,
                0.4588488281799311,  0.7212274985817402,   0.5767215156146851,
                0.1498654424779668,  0.6279733591901042,   0.9830524664698561,
                0.8555228058459113,  0.107769015243743,    0.1576130816775483,
                0.7060460880196088,  0.2760250769985784,   0.8407172559836625,
                0.07585428956306361, 0.4505415985024978,   0.4314138274635446,
                0.4172670690843695,  0.2347799133724063,   0.92938597096873,
                0.2077422927330285,  0.5948960740086143,   0.963088539286913,
                0.1067618616072414,  0.182922469414914,    0.6596052529083072,
                0.2919840799617149,  0.3014549487120655,   0.6447645368700879,
                0.9063081506497329,  0.9705927817606157,   0.03183284637742068,
                0.6797026768536748,  0.254282178971531,    0.05395011866660715,
                0.08382137799693257, 0.9106475944295229,   0.04965443032574213,
                0.3531585712220711,  0.7757126786084023,   0.3012463302794907,
                0.2622117477808454,  0.546805718738968,    0.6537573486685596,
                0.2399320105687174,  0.5185949425105382,   0.4316511702487202,
                0.7010987559009263,  0.3762722102788316,   0.8796537244819048,
                0.9571669482429456,  0.27692298496089,     0.6550980039738407,
                0.8142848260688164,  0.5307975530089727,   0.2289769687168188,
                0.1818470283028525,  0.9027161099152811,   0.8211940401979591,
                0.4867916324031724,  0.4709233485175907,   0.602843089382083,
                0.5211358308040015,  0.4941739366392701,   0.8865119330761013,
                0.9729745547638625,  0.01548712563601895,  0.6663388515844256,
                0.190923695236303,   0.8177605593706418,   0.4853756487228412,
                0.04617139063115394, 0.1626117351946306,   0.2435249687249893,
                0.7791672301020112,  0.9133373615016696,   0.2638029165219901,
                0.944787189721646,   0.01540343765155505,  0.4358585885809191,
                0.2304881602115585,  0.7112157804336829,   0.2315943867085238,
                0.7790517232312751,  0.0286741524641061,   0.6489914927123561,
                0.9840637243791538,  0.5391264650428567,   0.428252992979386,
                0.2607279990554646,  0.8002804688888001,   0.09713178123584754,
                0.1189976815583766,  0.9292636231872278,   0.934010684229183,
                0.152378018969223,   0.145538980384717,    0.4908640924680799,
                0.04302380165780784, 0.4467837494298063,   0.8443087926953891,
                0.2217467340172401,  0.4888977439201669,   0.7150370784006941,
                0.4899013885122239,  0.8003305753524015,   0.167168409914656,
                0.6981055201803084,  0.4820220610318563,   0.5943562506643308,
                0.1418863386272153,  0.8234578283272926,   0.498364051982143,
                0.3499837659848087,  0.1299062084737301,   0.8258169774895474,
                0.1360685587086637,  0.4892526384000189,   0.1689900294627044,
                0.3063494720165574,  0.1947642895670493,   0.1174176508558059,
                0.6240600881736895,  0.9037205605563163,   0.1679271456822568,
                0.4537977087269195,  0.1062163449286638,   0.6665279134025869,
                0.1206116132971623,  0.02251259274023176,  0.421761282626275,
                0.694828622975817,   0.9597439585160811,   0.1965952504312082,
                0.5688236608721927,  0.5383424352600571,   0.8692922076400893,
                0.3377194098213772,  0.6491154749564521,   0.508508655381127,
                0.2259217809723988,  0.2966758732183269,   0.6791355408657477,
                0.8909225043307892,  0.9786806496411588,   0.4323915037834617,
                0.372409740055537,   0.1781324544003378,   0.589507484695059,
                0.4252593202141349,  0.9157355251890671,   0.3170994800608605,
                0.3403857266661332,  0.2510838579760311,   0.4693906410582058,
                0.9961347166268855,  0.5797045873655702,   0.900053846417662,
                0.7317223856586703,  0.5107715641721097,   0.1707080471478586,
                0.3187783019258823,  0.395515215668593,    0.3341630527374962,
                0.7126944716789141,  0.8253137954020456,   0.1981184025429746,
                0.1280143997201726,  0.2261876797526757,   0.3127188868206155,
                0.7922073295595544,  0.9502220488383549,   0.5852677509797773,
                0.6160446761466392,  0.0119020695012414,   0.07817552875318368,
                0.549860201836332,   0.369246781120215,    0.6477459631363067,
                0.8176277083222621,  0.2276642978165535,   0.4241667597138072,
                0.3674366485444766,  0.6987458323347945,   0.500471624154843,
                0.08346981485891403, 0.4896876380160239,   0.9990803947613607,
                0.3846191243694108,  0.16148474431175,     0.959492426392903,
                0.03444608050290876, 0.223811939491137,    0.4732888489027293,
                0.3371226443988815,  0.4426782697754463,   0.1449547982237268,
                0.1112027552937874,  0.4509237064309449,   0.794831416883453,
                0.4356986841038991,  0.5078582846611182,   0.9879820031616328,
                0.1978098266859292,  0.4710883745419393,   0.1331710076071617,
                0.3394934133907577,  0.1711210663564321,   0.5829863827476739,
                0.1787661867523682},
      b[20]  = {0.4228856891000845, 0.09422933888773466, 0.5985236687567411,
               0.470924256358334,  0.6959493133016079,  0.6998878499282916,
               0.6385307582718379, 0.03360383606642947, 0.06880609911805124,
               0.3195997351804961, 0.5308642806941265,  0.6544457077570663,
               0.4076191970411526, 0.8199812227819406,  0.7183589432058837,
               0.9686493302310937, 0.5313339065656745,  0.32514568182056,
               0.1056292033290219, 0.6109586587462006},
      x_ref[20] = {
          0.003009209418301826, -0.1808823407174718,  0.3903320188800833,
          0.6976790078080168,   0.6329999062009535,   0.1115921067153488,
          0.2458022318227288,   -0.2426829454772559,  0.2946883767093276,
          -0.381962323356989,   -0.07725912868916707, -0.3263460035794618,
          1.141114945977927,    0.3152635035555698,   -0.9351393116694836,
          0.1765450918559331,   0.5963465784583344,   -0.2143943324393459,
          -1.051376033855315,   -0.1687234412167575};

  Array<double> x(20);
  std::copy(b, b + 20, x.begin());

  using crs_t = CRS<double, int>;
  crs_t               crs(20, 20);
  std::vector<double> buf(20);
  const static int    inds[20] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                               10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
  const double*       v_ptr    = a;
  crs.begin_assemble_rows();
  for (int i = 0; i < 20; ++i) {
    std::copy(v_ptr, v_ptr + 20, buf.begin());
    crs.push_back_row(i, inds, inds + 20, buf);
    v_ptr += 20;
  }
  crs.end_assemble_rows();

  SmallScaleSolverTrait<false>::solver_type<double> lup;
  lup.set_matrix(crs);
  lup.factorize(get_default_options());
  ASSERT_TRUE(lup.full_rank()) << "should be full rank!\n";
  lup.solve(x);
  for (int i = 0; i < 20; ++i)
    EXPECT_NEAR(x[i], x_ref[i], tol)
        << i << " entry doesn\'t agree with reference solution\n";
}
