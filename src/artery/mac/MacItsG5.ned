//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

package artery.mac;

import org.car2x.veins.base.modules.BaseMacLayer;

//
// MAC layer for ETSI ITS-G5 stations
//
simple MacItsG5 extends BaseMacLayer
{
	parameters:
		@class(MacItsG5);
		
		// length of MAC header (including 8 byte LLC and 4 byte FCS) 
		headerLength @unit(bit) = default(304 bit);

		// bit rate 
		double bitrate @unit(bps) = default(6 Mbps);

		// transmit power [mW]
		double txPower @unit(mW) = default(20 mW);
}
