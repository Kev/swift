/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Elements/SecurityLabelsCatalog.h>


namespace Swift {
	class GetSecurityLabelsCatalogRequest : public GenericRequest<SecurityLabelsCatalog> {
		public:
			typedef boost::shared_ptr<GetSecurityLabelsCatalogRequest> ref;

			static ref create(const JID& recipient, IQRouter* router) {
				return ref(new GetSecurityLabelsCatalogRequest(recipient, router));
			}

		private:
			GetSecurityLabelsCatalogRequest(
					const JID& recipient, 
					IQRouter* router) :
						GenericRequest<SecurityLabelsCatalog>(
							IQ::Get, JID(), boost::make_shared<SecurityLabelsCatalog>(recipient), router) {
			}
	};
}
