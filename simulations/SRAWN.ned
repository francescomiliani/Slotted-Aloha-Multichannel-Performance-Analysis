//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package srawn.simulations;

import srawn.TransmitterCluster;
import srawn.ReceiverCluster;

network SRAWN
{
    submodules:
        transmitter_cluster: TransmitterCluster {
            parameters:
                @display("p=143,130;is=vl;i=device/antennatower");
        }

        receiver_cluster: ReceiverCluster {
            @display("p=348,138;i=device/receiverdish;is=vl");
        }

    connections:
        for i = 0..(transmitter_cluster.transmitter_size * transmitter_cluster.channel_size) - 1 {
        	transmitter_cluster.channel_array[i] --> {  delay = 100ms; } --> receiver_cluster.channel_array[i];
        }
}
