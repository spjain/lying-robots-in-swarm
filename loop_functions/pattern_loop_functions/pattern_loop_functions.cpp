#include "pattern_loop_functions.h"
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/utility/configuration/argos_configuration.h>
//#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
//#include <controllers/footbot_foraging/footbot_foraging.h>
#include <cmath>
#include <iostream>

const int totTemplate = 3;
const int correctTemplate = 3;
int numRobots = 20;
int truthfulRobots;
int correct_belief_num_robot = 0;
/****************************************/
/****************************************/

CForagingLoopFunctions::CForagingLoopFunctions() :
   m_cForagingArenaSideX(-0.9f, 1.7f),
   m_cForagingArenaSideY(-1.7f, 1.7f),
   m_pcFloor(NULL),
   m_pcRNG(NULL),
   m_unCollectedFood(0),
   m_nEnergy(0),
   m_unEnergyPerFoodItem(1),
   m_unEnergyPerWalkingRobot(1) {
}

/****************************************/
/****************************************/
void CForagingLoopFunctions::Init(TConfigurationNode& t_node) {
   try {
      TConfigurationNode& tForaging = GetNode(t_node, "foraging");
      /* Get a pointer to the floor entity */
      m_pcFloor = &GetSpace().GetFloorEntity();
      std::string lyingRobot;
      GetNodeAttribute(tForaging, "lying_robots", lyingRobot);
      truthfulRobots = numRobots - std::atoi(lyingRobot.c_str());
      // int nRobot = std::atoi(numRobot.c_str());
      std::string comm;
      GetNodeAttribute(tForaging, "comm_range", comm);
      // DEBUG("robots: %d",nRobot);
      std::string filename;
      GetNodeAttribute(tForaging, "output", filename);
      // DEBUG("filename %s",filename.c_str());
      /* Create the random number generator */
      m_pcRNG = CRandom::CreateRNG("argos");
      CRange<Real> Range(-1.5, 1.5);
      CQuaternion angle;
      for(int i=0;i < numRobots; ++i){
         CKheperaIVEntity* khepera = new CKheperaIVEntity(
            ToString(i),
            "foraging",
            CVector3(m_pcRNG->Uniform(Range),m_pcRNG->Uniform(Range),0),
            angle.FromAngleAxis(m_pcRNG->Uniform(CRadians::UNSIGNED_RANGE),CVector3::Z),
            std::atoi(comm.c_str()),
            500);
         AddEntity(*khepera);
         m_pcKheperas.push_back(khepera);
         m_pcControllers.push_back(
            &dynamic_cast<CBuzzController&>(
               khepera->GetControllableEntity().GetController()));
     }
     /* File for saving data */
     ResultFile.open(filename, std::ios_base::trunc | std::ios_base::out);
     // ResultFile << "timestep, numRobot" << std::endl;
   }
   catch(CARGoSException& ex) {
      THROW_ARGOSEXCEPTION_NESTED("Error parsing loop functions!", ex);
   }
}

/****************************************/
/****************************************/

void CForagingLoopFunctions::Reset() {
}

/****************************************/
/****************************************/

void CForagingLoopFunctions::Destroy() {
   /* Close the file */
   //m_cOutput.close();
}


/****************************************/
/****************************************/

CColor CForagingLoopFunctions::GetFloorColor(const CVector2& c_position_on_plane) {
  int grid[4][4] = {{1,1,1,0},{0,0,0,1},{1,0,1,1},{0,1,0,1}};
  float shiftX = 6.0;
  float shiftY = 5.0;
  if( grid[int(floor(c_position_on_plane.GetX()+shiftX))%4][int(floor(c_position_on_plane.GetY()+shiftY))%4] == 0)
    {
      return CColor::BLACK;
    }
  else if(grid[int(floor(c_position_on_plane.GetX()+shiftX))%4][int(floor(c_position_on_plane.GetY()+shiftY))%4] == 1)
      return CColor::WHITE;
  else{
    printf("%d %d \n", int(floor(c_position_on_plane.GetX()))%4, int(floor(c_position_on_plane.GetY()))%4);
    printf("%f %f\n",c_position_on_plane.GetX(), c_position_on_plane.GetY());
  }
   //if((int(floor(c_position_on_plane.GetX()))+int(floor(c_position_on_plane.GetY())))%2 == 0) {
  //    return CColor::GRAY50;
   //}
   //if((int(floor(c_position_on_plane.GetX()))+int(floor(c_position_on_plane.GetY())))%2 == 1) {
  //    return CColor::WHITE;
   //}
   //for(UInt32 i = 0; i < m_cFoodPos.size(); ++i) {
    //  if((c_position_on_plane - m_cFoodPos[i]).SquareLength() < m_fFoodSquareRadius) {
      //   return CColor::BLACK;
    //  }
   //}
   //return CColor::WHITE;
}
/****************************************/
/****************************************/
bool CForagingLoopFunctions::IsExperimentFinished() {
  if(GetSpace().GetSimulationClock() >= 1500){
    ResultFile << GetSpace().GetSimulationClock() << std::endl;
    ResultFile << correct_belief_num_robot << std::endl;
    return true;
  }
  if(correct_belief_num_robot == truthfulRobots){
    ResultFile << GetSpace().GetSimulationClock() << std::endl;
    return true;
  }
  return false;
}

/****************************************/
/****************************************/
void CForagingLoopFunctions::PreStep() {
}

/****************************************/
/****************************************/
void CForagingLoopFunctions::PostStep() {
  // Time step
  int ts = GetSpace().GetSimulationClock();
  correct_belief_num_robot = 0;
  for(int i = 0; i < m_pcKheperas.size() ; ++i) {
    CKheperaIVEntity& robot_i = *m_pcKheperas[i];
    CBuzzController& cController = *m_pcControllers[i];
    buzzvm_t tBuzzVM = cController.GetBuzzVM();
    const std::string& strRobotId = robot_i.GetId();
    int belief_id = -1;
    Real belief_val = -1.0;
    for(int j=0; j < totTemplate;++j){
      Real val_j = belief(tBuzzVM,j);
      if(belief_val < val_j ){
        belief_val = val_j;
        belief_id = j+1;
      }
    }
    if(belief_id == correctTemplate) {
      correct_belief_num_robot +=1;
    }
  }
  // ResultFile<<ts<<","<<correct_belief_num_robot<<std::endl;
}


Real CForagingLoopFunctions::belief(buzzvm_t t_buzz_vm, int key) {
   /* Push var name on the stack */
   buzzvm_pushs(t_buzz_vm, buzzvm_string_register(t_buzz_vm, "belief", 1));
   /* Load global variable value */
   buzzvm_gload(t_buzz_vm);
   /* Get pointer to actual object */
   buzzobj_t tVar = buzzvm_stack_at(t_buzz_vm, 1);
   if (tVar->o.type != BUZZTYPE_TABLE) {
    DEBUG("Not table\n");
    buzzvm_pop(t_buzz_vm);
    return -1.0;
   }
   /* Push var name on the stack */
   buzzvm_pushi(t_buzz_vm, key);
   /* Load table variable value */
   buzzvm_tget(t_buzz_vm);
   /* Get pointer to actual object */
   tVar= buzzvm_stack_at(t_buzz_vm, 1);
   if (tVar->o.type != BUZZTYPE_FLOAT) {
    buzzvm_pop(t_buzz_vm);
    return -1.0;
   }
   Real val = tVar->f.value;
   /* Remove the var name from the stack */
   buzzvm_pop(t_buzz_vm);
   return val;

}
REGISTER_LOOP_FUNCTIONS(CForagingLoopFunctions, "pattern_loop_functions")
