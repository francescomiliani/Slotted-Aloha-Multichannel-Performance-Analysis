package srawn;

//
// TODO auto-generated module
//
simple Transmitter
{
   parameters:
       	int channel_size;
    	bool choosen_one = default(false);
    	double bernoullian_prob = default(0.5);
    	
    	// DEBUG SIGNAL: QUEUE DIMENSION PER SLOT-TIME
    	@signal [queue_dimension_per_slot_time]( type=long);
    	@statistic[ queue_dimension_per_slot_time_statistic ](
    		source ="queue_dimension_per_slot_time";
    		title = "queue_dimension_per_slot_time";
    		unit = packet;
    		record = mean, vector;
    		interpolationmode = none;
    	);
    	
    	// These signals store the throughput of the channels passing time.
    	@signal[ channel_throughput* ]( type=double; );
        @statisticTemplate[ channel_throughput_signal ](        
        	title	= "It stores the throghput of each channel passing time";
        	unit 	= packet; 
        	record 	= last, mean,vector;
        	interpolationmode=none;
        );
        
        // OPTIONAL SIGNAL: throughput of the transmitters.> it has to be calculated only if C >= N.
        @signal[ transmitter_throughput ]( type=double);
        @statistic[ transmitter_th ](
        source = "transmitter_throughput";
        title = "throughput of the transmitter";
        unit = packet;
        record = mean, stats;
        interpolationmode=none;
        );
		
   	gates:
   	    output  channel_array[ channel_size ];
   	    input	in;//It receives packets from its own generator
}
