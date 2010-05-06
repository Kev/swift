/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/JID/JID.h"
#include "Swiften/Roster/RosterItemOperation.h"
#include "Swiften/Roster/ContactRosterItem.h"

namespace Swift {

class RosterItem;

class SetName : public RosterItemOperation {
	public:
		SetName(const String& name, const JID& jid, JID::CompareType compareType = JID::WithoutResource) : RosterItemOperation(true, jid), name_(name), jid_(jid), compareType_(compareType) {
		}

		virtual void operator() (RosterItem* item) const {
			ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
			if (contact && contact->getJID().equals(jid_, compareType_)) {
				contact->setDisplayName(name_);
			}
		}
	
	private:
		String name_;
		JID jid_;
		JID::CompareType compareType_;
};

}


