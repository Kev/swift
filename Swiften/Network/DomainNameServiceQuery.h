#pragma once

#include <boost/signals.hpp>
#include <boost/optional.hpp>
#include <vector>

#include "Swiften/Base/String.h"
#include "Swiften/Network/DomainNameResolveError.h"

namespace Swift {
	class DomainNameServiceQuery {
		public:
			struct Result {
				Result(const String& hostname = "", int port = -1, int priority = -1, int weight = -1) : hostname(hostname), port(port), priority(priority), weight(weight) {}
				String hostname;
				int port;
				int priority;
				int weight;
			};

			struct ResultPriorityComparator {
				bool operator()(const DomainNameServiceQuery::Result& a, const DomainNameServiceQuery::Result& b) const {
					return a.priority < b.priority;
				}
			};

			virtual ~DomainNameServiceQuery();

			virtual void run() = 0;

			boost::signal<void (const std::vector<Result>&)> onResult;
	};
}
