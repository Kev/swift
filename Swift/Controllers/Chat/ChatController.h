/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/Chat/ChatControllerBase.h>

#include <map>
#include <string>

#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

namespace Swift {
	class AvatarManager;
	class ChatStateNotifier;
	class ChatStateTracker;
	class NickResolver;
	class EntityCapsProvider;
	class FileTransferController;
	class SettingsProvider;
	class HistoryController;
	class HighlightManager;
	class ClientBlockListManager;
	class UIEvent;

	class ChatController : public ChatControllerBase {
		public:
			ChatController(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &contact, NickResolver* nickResolver, PresenceOracle* presenceOracle, AvatarManager* avatarManager, bool isInMUC, bool useDelayForLatency, UIEventStream* eventStream, EventController* eventController, TimerFactory* timerFactory, EntityCapsProvider* entityCapsProvider, bool userWantsReceipts, SettingsProvider* settings, HistoryController* historyController, MUCRegistry* mucRegistry, HighlightManager* highlightManager, ClientBlockListManager* clientBlockListManager, boost::shared_ptr<ChatMessageParser> chatMessageParser, AutoAcceptMUCInviteDecider* autoAcceptMUCInviteDecider);
			virtual ~ChatController();
			virtual void setToJID(const JID& jid);
			virtual void setAvailableServerFeatures(boost::shared_ptr<DiscoInfo> info);
			virtual void setOnline(bool online);
			virtual void handleNewFileTransferController(FileTransferController* ftc);
			virtual void handleWhiteboardSessionRequest(bool senderIsSelf);
			virtual void handleWhiteboardStateChange(const ChatWindow::WhiteboardSessionState state);
			virtual void setContactIsReceivingPresence(bool /*isReceivingPresence*/);
			virtual ChatWindow* detachChatWindow();

		protected:
			void cancelReplaces();
			JID getBaseJID();
			void logMessage(const std::string& message, const JID& fromJID, const JID& toJID, const boost::posix_time::ptime& timeStamp, bool isIncoming);

		private:
			void handlePresenceChange(boost::shared_ptr<Presence> newPresence);
			std::string getStatusChangeString(boost::shared_ptr<Presence> presence);
			bool isIncomingMessageFromMe(boost::shared_ptr<Message> message);
			void postSendMessage(const std::string &body, boost::shared_ptr<Stanza> sentStanza);
			void preHandleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent);
			void postHandleIncomingMessage(boost::shared_ptr<MessageEvent> messageEvent, const HighlightAction&);
			void preSendMessageRequest(boost::shared_ptr<Message>);
			std::string senderHighlightNameFromMessage(const JID& from);
			std::string senderDisplayNameFromMessage(const JID& from);
			virtual boost::optional<boost::posix_time::ptime> getMessageTimestamp(boost::shared_ptr<Message>) const;
			void handleStanzaAcked(boost::shared_ptr<Stanza> stanza);
			void dayTicked() {lastWasPresence_ = false;}
			void handleContactNickChanged(const JID& jid, const std::string& /*oldNick*/);
			void handleBareJIDCapsChanged(const JID& jid);

			void handleFileTransferCancel(std::string /* id */);
			void handleFileTransferStart(std::string /* id */, std::string /* description */);
			void handleFileTransferAccept(std::string /* id */, std::string /* filename */);
			void handleSendFileRequest(std::string filename);

			void handleWhiteboardSessionAccept();
			void handleWhiteboardSessionCancel();
			void handleWhiteboardWindowShow();

			void handleSettingChanged(const std::string& settingPath);
			void checkForDisplayingDisplayReceiptsAlert();

			void handleBlockingStateChanged();
			void handleBlockUserRequest();
			void handleUnblockUserRequest();

			void handleInviteToChat(const std::vector<JID>& droppedJIDs);

			void handleWindowClosed();

			void handleUIEvent(boost::shared_ptr<UIEvent> event);

		private:
			NickResolver* nickResolver_;
			ChatStateNotifier* chatStateNotifier_;
			ChatStateTracker* chatStateTracker_;
			std::string myLastMessageUIID_;
			bool isInMUC_;
			bool lastWasPresence_;
			std::string lastStatusChangeString_;
			std::map<boost::shared_ptr<Stanza>, std::string> unackedStanzas_;
			std::map<std::string, std::string> requestedReceipts_;
			StatusShow::Type lastShownStatus_;
			UIEventStream* eventStream_;

			ChatWindow::Tristate contactSupportsReceipts_;
			bool receivingPresenceFromUs_;
			bool userWantsReceipts_;
			std::map<std::string, FileTransferController*> ftControllers;
			SettingsProvider* settings_;
			std::string lastWbID_;

			ClientBlockListManager* clientBlockListManager_;
			boost::bsignals::scoped_connection blockingOnStateChangedConnection_;
			boost::bsignals::scoped_connection blockingOnItemAddedConnection_;
			boost::bsignals::scoped_connection blockingOnItemRemovedConnection_;

			boost::optional<ChatWindow::AlertID> deliveryReceiptAlert_;
			boost::optional<ChatWindow::AlertID> blockedContactAlert_;
	};
}

