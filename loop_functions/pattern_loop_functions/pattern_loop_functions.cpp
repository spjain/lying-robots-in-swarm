#include "pattern_loop_functions.h"
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/plugins/simulator/entities/box_entity.h>
#include <cmath>
#include <iostream>

/****************************************/
/****************************************/

static const std::string THECONTROLLER = "bc";
static const size_t MAX_PLACE_TRIALS = 100;
static const Real WALL_THICKNESS   = 0.1;
static const Real WALL_HEIGHT      = 0.5;

/****************************************/
/****************************************/

void CPatternLoopFunctions::Init(TConfigurationNode& t_tree) {
   try {
      /* Create a RNG (it is automatically disposed of by ARGoS) */
      m_pcRNG = CRandom::CreateRNG("argos");
      /* Parse output file name and attempt to open it */
      GetNodeAttribute(t_tree, "output", m_strOutput);
      m_cOutput.open(m_strOutput.c_str(), std::ofstream::out);
      if(m_cOutput.fail()) {
         THROW_ARGOSEXCEPTION("Can't open file '" << m_strOutput << "'");
      }
      m_cOutput << "Time\t"
                << "Robot\t"
                << "X\t"
                << "Y\t"
                << "Pattern\t"
                << "Prob\n";
      /* Parse pattern-related parameters */
      TConfigurationNode& tPatterns = GetNode(t_tree, "patterns");
      GetNodeAttribute(tPatterns, "cells_on_side", m_unNumCellsOnSide);
      GetNodeAttribute(tPatterns, "pattern", m_unPattern);
      /* Parse robot-related parameters */
      TConfigurationNode& tRobots = GetNode(t_tree, "robots");
      UInt32 unNumRobots;
      GetNodeAttribute(tRobots, "num_robots", unNumRobots);
      UInt32 unNumLiars;
      GetNodeAttribute(tRobots, "num_liars",  unNumLiars);
      Real fCommRange;
      GetNodeAttribute(tRobots, "comm_range", fCommRange);
      Real fDensity;
      GetNodeAttribute(tRobots, "density",    fDensity);
      std::string strGoodFun;
      GetNodeAttribute(tRobots, "good_fun",   strGoodFun);
      std::string strBadFun;
      GetNodeAttribute(tRobots, "bad_fun",    strBadFun);
      /* Place robots */
      PlaceRobots(unNumRobots, unNumLiars,
                  fCommRange, fDensity,
                  strGoodFun, strBadFun);
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("Error parsing loop functions!", ex);
   }
}

/****************************************/
/****************************************/

void CPatternLoopFunctions::Destroy() {
   m_cOutput.close();
}

/****************************************/
/****************************************/

CColor CPatternLoopFunctions::GetFloorColor(const CVector2& c_pos) {
   /* Make sure position is within arena limits */
   if(c_pos.GetX() >= m_fArenaSide ||
      c_pos.GetY() >= m_fArenaSide) {
      return CColor::GRAY50;
   }
   /* Calculate cell index from position */
   UInt32 unI = m_unNumCellsOnSide * c_pos.GetX() / m_fArenaSide;
   UInt32 unJ = m_unNumCellsOnSide * c_pos.GetY() / m_fArenaSide;
   UInt32 unIdx = unJ * m_unNumCellsOnSide + unI;
   /* Return color */
   return (m_unPattern & (1 << unIdx)) ? CColor::WHITE : CColor::BLACK;
}

/****************************************/
/****************************************/

void CPatternLoopFunctions::PostStep() {
   UInt32 unPattern;
   Real fPatternProb;
   /* Go through non-lying robots */
   for(size_t i = 0; i < m_vecGoodVMs.size(); ++i) {
      /* Get the highest probability pattern */
      unPattern = 0; // TODO
      fPatternProb = 0.5; // TODO
      /* Save data to disk */
      m_cOutput << GetSpace().GetSimulationClock()      << "\t"  // time step
                << m_vecGoodVMs[i]->robot               << "\t"  // robot id
                << m_vecGoodAnchors[i]->Position.GetX() << "\t"  // pos x
                << m_vecGoodAnchors[i]->Position.GetY() << "\t"  // pos y
                << unPattern                            << "\t"  // pattern code
                << fPatternProb                         << "\n"; // probability
   }
}

/****************************************/
/****************************************/

void CPatternLoopFunctions::PlaceRobots(UInt32 un_robots,
                                        UInt32 un_liars,
                                        Real f_commrange,
                                        Real f_density,
                                        const std::string& str_good_fun,
                                        const std::string& str_bad_fun) {
   try {
      /* Calculate area covered by the communication range */
      Real fCommArea = CRadians::PI.GetValue() * Square(f_commrange);
      /* Calculate side of the region in which the robots are scattered */
      m_fArenaSide = Sqrt((fCommArea * un_robots) / f_density);
      CRange<Real> cAreaRange(0.0, m_fArenaSide);
      /* Place walls */
      Real fArenaSide2 = m_fArenaSide / 2.0;
      AddEntity(*new CBoxEntity("wall_south", CVector3( fArenaSide2,            0, 0), CQuaternion(), false, CVector3(m_fArenaSide, WALL_THICKNESS, WALL_HEIGHT)));
      AddEntity(*new CBoxEntity("wall_north", CVector3( fArenaSide2, m_fArenaSide, 0), CQuaternion(), false, CVector3(m_fArenaSide, WALL_THICKNESS, WALL_HEIGHT)));
      AddEntity(*new CBoxEntity("wall_west",  CVector3(           0,  fArenaSide2, 0), CQuaternion(), false, CVector3(WALL_THICKNESS, m_fArenaSide, WALL_HEIGHT)));
      AddEntity(*new CBoxEntity("wall_east",  CVector3(m_fArenaSide,  fArenaSide2, 0), CQuaternion(), false, CVector3(WALL_THICKNESS, m_fArenaSide, WALL_HEIGHT)));
      /* Place robots */
      UInt32 unTrials;
      CKheperaIVEntity* pcKhIV;
      std::ostringstream cKhIVId;
      CVector3 cKhIVPos;
      CQuaternion cKhIVRot;
      /* For each robot */
      for(size_t i = 0; i < un_robots; ++i) {
         /* Make the id */
         cKhIVId.str("");
         cKhIVId << "kh" << i;
         /* Create the robot in the origin and add it to ARGoS space */
         pcKhIV = new CKheperaIVEntity(
            cKhIVId.str(),
            THECONTROLLER);
         AddEntity(*pcKhIV);
         /* Try to place it in the arena */
         unTrials = 0;
         bool bDone;
         do {
            /* Choose a random position */
            ++unTrials;
            cKhIVPos.Set(m_pcRNG->Uniform(cAreaRange),
                         m_pcRNG->Uniform(cAreaRange),
                         0.0f);
            cKhIVRot.FromAngleAxis(m_pcRNG->Uniform(CRadians::UNSIGNED_RANGE),
                                   CVector3::Z);
            bDone = MoveEntity(pcKhIV->GetEmbodiedEntity(), cKhIVPos, cKhIVRot);
         } while(!bDone && unTrials <= MAX_PLACE_TRIALS);
         if(!bDone) {
            THROW_ARGOSEXCEPTION("Can't place " << cKhIVId.str());
         }
         /* Placement successful */
         /* Save the VM */
         buzzvm_t tBuzzVM =
            dynamic_cast<CBuzzController&>(
               pcKhIV->GetControllableEntity().GetController()).
            GetBuzzVM();
         if(i >= un_liars) {
            m_vecGoodAnchors.push_back(&pcKhIV->GetEmbodiedEntity().GetOriginAnchor());
            m_vecGoodVMs.push_back(tBuzzVM);
         }
         /* Set number of liars */
         buzzvm_pushs(tBuzzVM, buzzvm_string_register(tBuzzVM, "num_liars", 1));
         buzzvm_pushi(tBuzzVM, un_liars);
         buzzvm_gstore(tBuzzVM);
         /* Set scripts */
         buzzvm_pushs(tBuzzVM, buzzvm_string_register(tBuzzVM, "good_fun", 1));
         buzzvm_pushs(tBuzzVM, buzzvm_string_register(tBuzzVM, str_good_fun.c_str(), 1));
         buzzvm_gstore(tBuzzVM);
         buzzvm_pushs(tBuzzVM, buzzvm_string_register(tBuzzVM, "bad_fun", 1));
         buzzvm_pushs(tBuzzVM, buzzvm_string_register(tBuzzVM, str_bad_fun.c_str(), 1));
         buzzvm_gstore(tBuzzVM);
      }
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("While placing robots", ex);
   }
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CPatternLoopFunctions, "pattern_loop_functions")
