#pragma once

#include <cassert>

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Events/StanzaEvent.h"
#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"

namespace Swift {
	class SubscriptionRequestEvent : public StanzaEvent {
		public:
			SubscriptionRequestEvent(const JID& jid, const String& reason) : jid_(jid), reason_(reason){};
			virtual ~SubscriptionRequestEvent(){};
			const JID& getJID() const {return jid_;};
			const String& getReason() const {return reason_;};
			boost::signal<void()> onAccept;
			boost::signal<void()> onDecline;
			void accept() {
				onAccept();
				onConclusion();
			}

			void decline() {
				onDecline();
				onConclusion();
			}

		private:
			JID jid_;
			String reason_;
	};
}

