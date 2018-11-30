
package srawn;

//
// TODO auto-generated module
//
simple Receiver
{
    parameters:
        // Number of channels: it defines the number of gates through which this module connects it self to the parent module.
    	int channel_size;
    	
    	@display("i=block/sink");
        @signal[delay_time](source="delay_time"; type="simtime_t");
        @statistic[delay_time](title="Delay time of arrived packets"; unit=s; record=vector,mean,min,max; interpolationmode=none);
    	
   	gates:
   	    input channel_array[ channel_size ]; 
}
