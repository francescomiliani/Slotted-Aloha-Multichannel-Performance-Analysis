
package srawn;

module TransmitterCluster
{
    parameters:
        int transmitter_size;
        int channel_size;
        double slot_time @unit(s);
        double collision_detection_time @unit(s);
        double clear_time @unit(s);
        

    gates:
        output channel_array[ channel_size * transmitter_size ];

    submodules:
        tx_array[ transmitter_size ]: Transmitter {
            @display("p=384,103;i=device/wifilaptop;is=vl");
        }
        generator_array[ transmitter_size ]: Generator {
            @display("p=125,103");
        }
    connections:
        //Connect each generator to its own transmitter module
        for i=0..transmitter_size-1 { 
        	generator_array[ i ].out --> tx_array[ i ].in;
        }
        //Connect each channel of transmitter i to the respective channel of the transmitter compound module        	
        for i=0..transmitter_size-1, for j=0..channel_size-1 
        {      	
        	//tx_array[ i ].channel_array++ --> channels_array++;
        		tx_array[ i ].channel_array[j] --> channel_array[ i*channel_size + j ];
        }
}
