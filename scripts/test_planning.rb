require 'orocos'
require 'vizkit'
require 'readline'

Orocos::CORBA.max_message_size = 12000000 # stucks if > than this

include Orocos
Orocos.initialize

# Content of these language variables must not be german, otherwise '.' and ','
# are mixed reading numbers and a bad_alloc error occurs loading the scenes.
ENV['LANG'] = 'C'
ENV['LC_NUMERIC'] = 'C'

Orocos.run  'motion_planning_libraries::Task' => 'planner',
            'motion_planning_libraries::Test' => 'test',
            'traversability_loader::Task'     => 'trav',
            "valgrind" => false,
            'output' => nil,
            "wait" => 1000 do

    planner = TaskContext::get 'planner'
    planner.traversability_map_id = "trav_map"
    planner.planning_time_sec = 20.0

    planner.config do |p|
        p.mPlanningLibType = :LIB_SBPL
        p.mEnvType = :ENV_XYTHETA
        p.mPlanner = :ANYTIME_DSTAR
        p.mFootprintLengthMinMax.first = 0.70
        p.mFootprintLengthMinMax.second= 0.70
        p.mFootprintWidthMinMax.first  = 0.70
        p.mFootprintWidthMinMax.second = 0.70
        p.mMaxAllowedSampleDist = -1
        p.mNumFootprintClasses  = 10
        p.mTimeToAdaptFootprint = 10
        p.mAdaptFootprintPenalty = 2
        p.mSearchUntilFirstSolution = false
        p.mNumIntermediatePoints  = 8
        p.mNumPrimPartition = 2

        p.mReplanDuringEachUpdate = true
        # EO2
        p.mSpeeds.mSpeedForward         = 0.05
        p.mSpeeds.mSpeedBackward        = 0.05
        p.mSpeeds.mSpeedLateral         = 0.0
        p.mSpeeds.mSpeedTurn            = 0.065
        p.mSpeeds.mSpeedPointTurn       = 0.065
        p.mSpeeds.mMultiplierForward    = 1
        p.mSpeeds.mMultiplierBackward   = 500
        p.mSpeeds.mMultiplierLateral    = 500
        p.mSpeeds.mMultiplierTurn       = 1 
        p.mSpeeds.mMultiplierPointTurn  = 5

        # SBPL specific configuration
        p.mSBPLEnvFile = ""
        p.mSBPLMotionPrimitivesFile = ""
        #p.mSBPLMotionPrimitivesFile = File.join(ENV['AUTOPROJ_CURRENT_ROOT'], '/external/sbpl/matlab/mprim/pr2_10cm.mprim')
        #p.mSBPLMotionPrimitivesFile = File.join(ENV['AUTOPROJ_CURRENT_ROOT'], '/external/sbpl/matlab/mprim/output_unicycle.mprim')
        #p.mSBPLMotionPrimitivesFile = File.join(ENV['AUTOPROJ_CURRENT_ROOT'], 'planning/motion_planning_libraries/scripts/sbpl/unicycle_output.mprim')
        p.mSBPLForwardSearch = false # ADPlanner throws 'g-values are non-decreasing' if true
    end

    planner.configure
    planner.start

    # ---------------------- TRAVERSABILITY MAP COMPONENT --------------------------------
    trav = TaskContext::get 'trav'
    trav.traversability_map_id = "trav_map"
    trav.traversability_map_scalex =  0.03
    trav.traversability_map_scaley =  0.03
    trav.filename = "../../planning/orogen/traversability_explorer/data/costmap_january.txt"
    trav.configure
    # ---------------------- END - TRAVERSABILITY MAP COMPONENT -----------------------------
    

    test = TaskContext::get 'test'
    test.traversability_map_id = 'trav_unused'
    test.traversability_map_type = 'RANDOM_CIRCLES'# 'RANDOM_CIRCLES'
    test.traversability_map_width_m  = 8.4
    test.traversability_map_height_m = 8.7
    test.traversability_map_scalex =  0.03
    test.traversability_map_scaley = 0.03
    test.number_of_random_circles = 1
    test.opening_length = 1.2

    test.configure
    test.start
    trav.start



    trav.traversability_map.connect_to(planner.traversability_map)
    test.start_state.connect_to(planner.start_state)
    test.goal_state.connect_to(planner.goal_state)

    t1 = Thread.new do
        while true do
            Readline::readline("Hit enter to generate a new test environment ... ")
            test.trigger
            trav.trigger
        end
    end

    Vizkit.display planner
    Vizkit.display trav.port("traversability_map")
    #Vizkit.display test.port("traversability_map")
    Vizkit.display planner.trajectory
    Vizkit.display planner.start_pose_samples_debug, :widget => Vizkit.default_loader.RigidBodyStateVisualization
    Vizkit.display planner.goal_pose_samples_debug,  :widget => Vizkit.default_loader.RigidBodyStateVisualization
    

    Vizkit.exec

    Readline::readline("Hit ENTER to stop")
end
