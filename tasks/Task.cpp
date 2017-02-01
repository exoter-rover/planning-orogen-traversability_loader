/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */
#include "Task.hpp"

#include <stdlib.h>
#include <time.h>

#include <envire/core/Environment.hpp>
#include <envire/maps/TraversabilityGrid.hpp>
#include <envire/operators/SimpleTraversability.hpp>
#include <orocos/envire/Orocos.hpp>

using namespace traversability_loader;

Task::Task(std::string const& name)
    :   TaskBase(name),
        mpEnv(NULL), 
        mpFrameNode(NULL), 
        mpTravGrid(NULL)
{
}

Task::Task(std::string const& name, RTT::ExecutionEngine* engine)
    :   TaskBase(name, engine),
        mpEnv(NULL), 
        mpFrameNode(NULL), 
        mpTravGrid(NULL)
{
}

Task::~Task()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Task.hpp for more detailed
// documentation about them.

bool Task::configureHook()
{
    if (! TaskBase::configureHook())
        return false;
    loadTraversabilityMap(_filename.get());
    envire::OrocosEmitter emitter_tmp(mpEnv, _traversability_map);
    emitter_tmp.setTime(base::Time::now());
    emitter_tmp.flush();
    return true;
}
bool Task::startHook()
{
    if (! TaskBase::startHook())
        return false;
    return true;
}
void Task::updateHook()
{
    TaskBase::updateHook();
    envire::OrocosEmitter emitter_tmp(mpEnv, _traversability_map);
    emitter_tmp.setTime(base::Time::now());
    emitter_tmp.flush();    
}
void Task::errorHook()
{
    TaskBase::errorHook();
}
void Task::stopHook()
{
    TaskBase::stopHook();
}
void Task::cleanupHook()
{
    TaskBase::cleanupHook();
}

/* --------------------------------------------------------------
    PRIVATE METHODS
---------------------------------------------------------------*/

void Task::loadTraversabilityMap(std::string filename){
    std::cout<< "TraversabilityExplorer: map being loaded from: "
        << filename << ", correct?" << std::endl;
   
    /* OPEN THE FILE AND LOAD THE TRAVERSABILITY NUMERIC DATA INTO STD VECTOR */
    std::string line;
    std::ifstream datafile(filename.c_str(), std::ios::in);
    Nrow = 0; Ncol = 0;    
    std::vector <int> row;

    int MIN_COST, MAX_COST;
    MIN_COST = std::numeric_limits<int>::max();
    MAX_COST = 0;

    if( datafile.is_open() ){
        while ( std::getline(datafile, line) ){
            std::stringstream ss(line);
            std::string cell;
           while( std::getline(ss, cell, ' ') ){ 
                int val;
                std::stringstream numericValue(cell);
                numericValue >> val;
                if(val>MAX_COST){
                    MAX_COST = val;
                }
                if(val<MIN_COST){
                    MIN_COST = val;
                }
                row.push_back(val);
                Ncol++;
            }
            groundTruth.push_back(row);
            row.clear();
            Nrow++;
        }
        datafile.close(); 
        Ncol /= Nrow;
        std::cout << "Cost map of " << Nrow 
                  << " x "          << Ncol << " loaded." << std::endl;
        std::cout << "Cost values are in integer values of {" 
                  << MIN_COST << ", ..., " << MAX_COST << "}."<< std::endl;
        bool validCostRange = MIN_COST>0 && MAX_COST <= SBPL_MAX_COST+1;
        if (!validCostRange){
            std::cout << "Invalid cost range!!!" << std::endl;
            assert(validCostRange);    
        }
        
    } else {
        std::cout << "Problem opening the file" << std::endl;
        return;
    }
    /* INITIALIZE UNEXPLORED TRAVERSABILITY MAP */
    if(mpEnv != NULL) {
        delete mpEnv;
    }
    mpEnv = new envire::Environment(); 
    envire::TraversabilityGrid* trav = new envire::TraversabilityGrid(
            (size_t) Nrow, (size_t) Ncol,
            _traversability_map_scalex.get(),
            _traversability_map_scaley.get());
    mpEnv->attachItem(trav);
    mpTravGrid = trav;
    // Creates a shared-pointer from the passed reference.
    envire::TraversabilityGrid::ArrayType& travData = mpTravGrid->getGridData(envire::TraversabilityGrid::TRAVERSABILITY);
    boost::shared_ptr<envire::TraversabilityGrid::ArrayType> mpTravData;
    mpTravData = boost::shared_ptr<envire::TraversabilityGrid::ArrayType>(&travData, NullDeleter());

    // Defines driveability values.

    // Defines driveability values.
    // trav->setTraversabilityClass(0, envire::TraversabilityClass(0.5)); // unknown
    

    
    trav->setTraversabilityClass(0, envire::TraversabilityClass(0.5));
    std::cout << "Setting class #0 to 0.5 traversability."<< std::endl;
    
    
    // SBPL COST TO CLASS ID MAPPING
    int sbplCostToClassID[SBPL_MAX_COST+1];
    for (int i = 0; i < SBPL_MAX_COST; ++i)
    {
        sbplCostToClassID[i] = i+2;
        //std::cout << "Mapping " << i+1 << " to " << i+2 << std::endl;   
    }
    sbplCostToClassID[SBPL_MAX_COST] = 1;
    //std::cout << "Mapping " << SBPL_MAX_COST+1 << " to " << 1 << std::endl;   

    // ASSOCIATING CLASS ID WITH TRAVERSABILITY
    

    double travVal;
    // class 1 reserved for obstacle (will be used instead of 21)
    trav->setTraversabilityClass(1, envire::TraversabilityClass(0.0));
    std::cout << "Setting class #" << 1
            <<  " to " << 0.0 << " traversability." << std::endl;

    int cost;
    // TODO: MAY SKIP CLASSES BELOW MIN_COST IN ORDER NOT TO BIAS UNKNOWN AREAS
    // those are set to 2nd highest traversability
    // but 2nd highest registered class, or 2nd highest value in the grid??
    for(cost = 1; cost < SBPL_MAX_COST+1; ++cost) {
        travVal = ((double)(SBPL_MAX_COST+1 - cost));
        travVal /= SBPL_MAX_COST;
        trav->setTraversabilityClass(sbplCostToClassID[cost-1], envire::TraversabilityClass(travVal));
        std::cout << "Setting class #" << sbplCostToClassID[cost-1]
            <<  " to " << travVal << " traversability." << std::endl;
    }
 
    for(int x=0; x < Nrow;  x++)
    {
        //row = groundTruth.at(x);
        for(int y=0; y < Ncol; y++)
        {
        cost = groundTruth[x][y];
        trav->setTraversability( sbplCostToClassID[cost-1], x, y);   
        trav->setProbability(1.0, x, y);
        }
    }
    std::cout << "Traversability map initialization completed." << std::endl;
}
