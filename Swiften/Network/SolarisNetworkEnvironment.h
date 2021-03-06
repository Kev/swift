/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
* Copyright (c) 2013-2014 Isode Limited.
* All rights reserved.v3.
* See the COPYING file for more information.
*/

#pragma once

#include <vector>

#include <Swiften/Base/boost_bsignals.h>

#include <Swiften/Network/NetworkEnvironment.h>
#include <Swiften/Network/NetworkInterface.h>

namespace Swift {

	class SolarisNetworkEnvironment : public NetworkEnvironment {
		public:
			std::vector<NetworkInterface> getNetworkInterfaces() const;
	};

}
