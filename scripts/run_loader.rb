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

Orocos.run  'traversability_loader::Task' => 'trav',
            "valgrind" => false,
            'output' => nil,
            "wait" => 1000 do

    trav = TaskContext::get 'trav'
    trav.traversability_map_id = "trav_map"

    trav.traversability_map_scalex =  0.03
    trav.traversability_map_scaley =  0.03
    trav.filename = "data/sbpl_costmap.txt"
    
    trav.configure
    trav.start

    t1 = Thread.new do
        while true do
            Readline::readline("Hit enter to trigger ... ")
            trav.trigger
        end
    end 

    Vizkit.display trav.port("traversability_map")
    Vizkit.exec

    Readline::readline("Hit ENTER to stop")
end
