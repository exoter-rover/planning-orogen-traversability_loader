/* Generated from orogen/lib/orogen/templates/tasks/Task.hpp */

#ifndef TRAVERSABILITY_LOADER_TASK_TASK_HPP
#define TRAVERSABILITY_LOADER_TASK_TASK_HPP

#include "traversability_loader/TaskBase.hpp"


namespace envire {
    class Environment;
    class FrameNode;
    class TraversabilityGrid;
}

namespace traversability_loader {

    struct NullDeleter
    {
        void operator()(void const *) const {}
    };


    class Task : public TaskBase
    {
	friend class TaskBase;
    protected:
        envire::Environment* mpEnv;
        envire::FrameNode* mpFrameNode;
        envire::TraversabilityGrid* mpTravGrid;
        // Traversability data as loaded from the file
        std::vector< std::vector<int> > groundTruth;
        // Grid size
        int Nrow, Ncol;
        static const unsigned char SBPL_MAX_COST = 20;
        


    public:

        Task(std::string const& name = "traversability_loader::Task");
        Task(std::string const& name, RTT::ExecutionEngine* engine);

 
	~Task();

        bool configureHook();
        bool startHook();
        void updateHook();
        void errorHook();
        void stopHook();
        void cleanupHook();
    private:
        void loadTraversabilityMap(std::string filename);
    };
}

#endif

