#ifndef PATTERN_LOOP_FUNCTIONS_H
#define PATTERN_LOOP_FUNCTIONS_H

#include <buzz/argos/buzz_controller.h>
#include <argos3/core/simulator/loop_functions.h>
#include <argos3/core/simulator/entity/floor_entity.h>
#include <argos3/core/utility/math/range.h>
#include <argos3/core/utility/math/rng.h>
#include <argos3/plugins/robots/kheperaiv/simulator/kheperaiv_entity.h>

using namespace argos;

class CPatternLoopFunctions : public CLoopFunctions {

public:

   virtual ~CPatternLoopFunctions() {}
   virtual void Init(TConfigurationNode& t_tree);
   virtual void Destroy();
   virtual CColor GetFloorColor(const CVector2& c_pos);
   virtual void PostStep();

private:

   /*
    * Places the robots uniformly.
    * @param un_robots    The total number of robots
    * @param un_liars     The number of liars
    * @param f_commrange  The communication range of the robots
    * @param f_density    The target density of the robots
    * @param str_good_fun Buzz function executed by good robots
    * @param str_bad_fun  Buzz function executed by bad robots
    */
   void PlaceRobots(UInt32 un_robots,
                    UInt32 un_liars,
                    Real f_commrange,
                    Real f_density,
                    const std::string& str_good_fun,
                    const std::string& str_bad_fun);

private:

   /* List of origin anchors associated to the non-lying robots */
   std::vector<SAnchor*> m_vecGoodAnchors;
   /* List of Buzz VMs associated to the non-lying robots */
   std::vector<buzzvm_t> m_vecGoodVMs;

   /* Arena side */
   Real m_fArenaSide;
   /* Number of individual black/white cells on a side of the arena */
   UInt32 m_unNumCellsOnSide;

   /* Pattern binary encoding */
   UInt64 m_unPattern;

   /* Output file name */
   std::string m_strOutput;
   /* Output file stream */
   std::ofstream m_cOutput;

   /* Random number generator */
   CRandom::CRNG* m_pcRNG;
};

#endif
