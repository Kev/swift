#pragma once

#include "boost/shared_ptr.hpp"

#include <QTreeView>

#include "Swift/Controllers/UIInterfaces/EventWindow.h"
#include "Swift/Controllers/UIEvents/UIEventStream.h"
#include "Swift/QtUI/EventViewer/EventView.h"
#include "Swift/QtUI/EventViewer/EventModel.h"
#include "Swift/QtUI/EventViewer/EventDelegate.h"

namespace Swift {
	class QtEventWindow : public QTreeView, public EventWindow {
		Q_OBJECT
		public:
			QtEventWindow(UIEventStream* eventStream, QWidget* parent = 0);
			~QtEventWindow();
			void addEvent(boost::shared_ptr<StanzaEvent> event, bool active);
			void removeEvent(boost::shared_ptr<StanzaEvent> event);
		private slots:
			void handleItemActivated(const QModelIndex& item);
		private:
			EventModel* model_;
			EventDelegate* delegate_;
			UIEventStream* eventStream_;
	};
		
}

