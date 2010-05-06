/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/ContextMenus/QtRosterContextMenu.h"

#include <QMenu>
#include <QDebug>

#include <boost/shared_ptr.hpp>

#include "Swiften/Roster/ContactRosterItem.h"
#include "Swiften/Base/String.h"
#include "Swift/Controllers/UIEvents/UIEvent.h"
#include "Swift/Controllers/UIEvents/RemoveRosterItemUIEvent.h"
#include "Swift/QtUI/QtSwiftUtil.h"

namespace Swift {

QtRosterContextMenu::QtRosterContextMenu(UIEventStream* eventStream) {
	eventStream_ = eventStream;
}

void QtRosterContextMenu::show(RosterItem* item) {
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item);
	if (!contact) {
		return;
	}
	item_ = item;
	QMenu* contextMenu = new QMenu();
	contextMenu->addAction("Remove", this, SLOT(handleRemove()));
	contextMenu->exec(QCursor::pos());
}

void QtRosterContextMenu::handleRemove() {
	ContactRosterItem* contact = dynamic_cast<ContactRosterItem*>(item_);
	assert(contact);
	eventStream_->send(boost::shared_ptr<UIEvent>(new RemoveRosterItemUIEvent(contact->getJID())));
}

}
