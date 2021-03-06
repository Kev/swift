/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/RawXMLPayload.h>

namespace Swift {
	class RawXMLPayloadSerializer : public GenericPayloadSerializer<RawXMLPayload> {
		public:
			RawXMLPayloadSerializer() : GenericPayloadSerializer<RawXMLPayload>() {}

			virtual std::string serializePayload(boost::shared_ptr<RawXMLPayload> p)  const {
				return p->getRawXML();
			}
	};
}
