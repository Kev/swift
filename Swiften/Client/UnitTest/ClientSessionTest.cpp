#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <deque>
#include <boost/bind.hpp>
#include <boost/optional.hpp>

#include "Swiften/Session/SessionStream.h"
#include "Swiften/Client/ClientSession.h"
#include "Swiften/Elements/StartTLSRequest.h"
#include "Swiften/Elements/StreamFeatures.h"
#include "Swiften/Elements/TLSProceed.h"
#include "Swiften/Elements/StartTLSFailure.h"
#include "Swiften/Elements/AuthRequest.h"
#include "Swiften/Elements/AuthSuccess.h"
#include "Swiften/Elements/AuthFailure.h"

using namespace Swift;

class ClientSessionTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(ClientSessionTest);
		CPPUNIT_TEST(testStart_Error);
		CPPUNIT_TEST(testStartTLS);
		CPPUNIT_TEST(testStartTLS_ServerError);
		CPPUNIT_TEST(testStartTLS_ConnectError);
		CPPUNIT_TEST(testAuthenticate);
		CPPUNIT_TEST(testAuthenticate_Unauthorized);
		CPPUNIT_TEST(testAuthenticate_NoValidAuthMechanisms);
		/*
		CPPUNIT_TEST(testResourceBind);
		CPPUNIT_TEST(testResourceBind_ChangeResource);
		CPPUNIT_TEST(testResourceBind_EmptyResource);
		CPPUNIT_TEST(testResourceBind_Error);
		CPPUNIT_TEST(testSessionStart);
		CPPUNIT_TEST(testSessionStart_Error);
		CPPUNIT_TEST(testSessionStart_AfterResourceBind);
		CPPUNIT_TEST(testWhitespacePing);
		CPPUNIT_TEST(testReceiveElementAfterSessionStarted);
		CPPUNIT_TEST(testSendElement);
		*/
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			server = boost::shared_ptr<MockSessionStream>(new MockSessionStream());
			sessionFinishedReceived = false;
			needCredentials = false;
		}

		void testStart_Error() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->breakConnection();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Finished, session->getState());
			CPPUNIT_ASSERT(sessionFinishedReceived);
			CPPUNIT_ASSERT(sessionFinishedError);
		}

		void testStartTLS() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithStartTLS();
			server->receiveStartTLS();
			CPPUNIT_ASSERT(!server->tlsEncrypted);
			server->sendTLSProceed();
			CPPUNIT_ASSERT(server->tlsEncrypted);
			server->onTLSEncrypted();
			server->receiveStreamStart();
			server->sendStreamStart();
		}

		void testStartTLS_ServerError() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithStartTLS();
			server->receiveStartTLS();
			server->sendTLSFailure();

			CPPUNIT_ASSERT(!server->tlsEncrypted);
			CPPUNIT_ASSERT_EQUAL(ClientSession::Finished, session->getState());
			CPPUNIT_ASSERT(sessionFinishedReceived);
			CPPUNIT_ASSERT(sessionFinishedError);
		}

		void testStartTLS_ConnectError() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithStartTLS();
			server->receiveStartTLS();
			server->sendTLSProceed();
			server->breakTLS();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Finished, session->getState());
			CPPUNIT_ASSERT(sessionFinishedReceived);
			CPPUNIT_ASSERT(sessionFinishedError);
		}

		void testAuthenticate() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithPLAINAuthentication();
			CPPUNIT_ASSERT(needCredentials);
			CPPUNIT_ASSERT_EQUAL(ClientSession::WaitingForCredentials, session->getState());
			session->sendCredentials("mypass");
			server->receiveAuthRequest("PLAIN");
			server->sendAuthSuccess();
			server->receiveStreamStart();
		}

		void testAuthenticate_Unauthorized() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithPLAINAuthentication();
			CPPUNIT_ASSERT(needCredentials);
			CPPUNIT_ASSERT_EQUAL(ClientSession::WaitingForCredentials, session->getState());
			session->sendCredentials("mypass");
			server->receiveAuthRequest("PLAIN");
			server->sendAuthFailure();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Finished, session->getState());
			CPPUNIT_ASSERT(sessionFinishedReceived);
			CPPUNIT_ASSERT(sessionFinishedError);
		}

		void testAuthenticate_NoValidAuthMechanisms() {
			boost::shared_ptr<ClientSession> session(createSession());
			session->start();
			server->receiveStreamStart();
			server->sendStreamStart();
			server->sendStreamFeaturesWithUnknownAuthentication();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Finished, session->getState());
			CPPUNIT_ASSERT(sessionFinishedReceived);
			CPPUNIT_ASSERT(sessionFinishedError);
		}

	private:
		boost::shared_ptr<ClientSession> createSession() {
			boost::shared_ptr<ClientSession> session = ClientSession::create(JID("me@foo.com"), server);
			session->onFinished.connect(boost::bind(&ClientSessionTest::handleSessionFinished, this, _1));
			session->onNeedCredentials.connect(boost::bind(&ClientSessionTest::handleSessionNeedCredentials, this));
			return session;
		}

		void handleSessionFinished(boost::shared_ptr<Error> error) {
			sessionFinishedReceived = true;
			sessionFinishedError = error;
		}

		void handleSessionNeedCredentials() {
			needCredentials = true;
		}

		class MockSessionStream : public SessionStream {
			public:
				struct Event {
					Event(boost::shared_ptr<Element> element) : element(element), footer(false) {}
					Event(const ProtocolHeader& header) : header(header), footer(false) {}
					Event() : footer(true) {}
					
					boost::shared_ptr<Element> element;
					boost::optional<ProtocolHeader> header;
					bool footer;
				};

				MockSessionStream() : available(true), canTLSEncrypt(true), tlsEncrypted(false), whitespacePingEnabled(false), resetCount(0) {
				}

				virtual bool isAvailable() {
					return available;
				}

				virtual void writeHeader(const ProtocolHeader& header) {
					receivedEvents.push_back(Event(header));
				}

				virtual void writeFooter() {
					receivedEvents.push_back(Event());
				}

				virtual void writeElement(boost::shared_ptr<Element> element) {
					receivedEvents.push_back(Event(element));
				}

				virtual bool supportsTLSEncryption() {
					return canTLSEncrypt;
				}

				virtual void addTLSEncryption() {
					tlsEncrypted = true;
				}

				virtual void setWhitespacePingEnabled(bool enabled) {
					whitespacePingEnabled = enabled;
				}

				virtual void resetXMPPParser() {
					resetCount++;
				}

				void breakConnection() {
					onError(boost::shared_ptr<SessionStream::Error>(new SessionStream::Error(SessionStream::Error::ConnectionReadError)));
				}

				void breakTLS() {
					onError(boost::shared_ptr<SessionStream::Error>(new SessionStream::Error(SessionStream::Error::TLSError)));
				}


				void sendStreamStart() {
					ProtocolHeader header;
					header.setTo("foo.com");
					return onStreamStartReceived(header);
				}

				void sendStreamFeaturesWithStartTLS() {
					boost::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
					streamFeatures->setHasStartTLS();
					onElementReceived(streamFeatures);
				}

				void sendTLSProceed() {
					onElementReceived(boost::shared_ptr<TLSProceed>(new TLSProceed()));
				}

				void sendTLSFailure() {
					onElementReceived(boost::shared_ptr<StartTLSFailure>(new StartTLSFailure()));
				}

				void sendStreamFeaturesWithPLAINAuthentication() {
					boost::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
					streamFeatures->addAuthenticationMechanism("PLAIN");
					onElementReceived(streamFeatures);
				}

				void sendStreamFeaturesWithUnknownAuthentication() {
					boost::shared_ptr<StreamFeatures> streamFeatures(new StreamFeatures());
					streamFeatures->addAuthenticationMechanism("UNKNOWN");
					onElementReceived(streamFeatures);
				}

				void sendAuthSuccess() {
					onElementReceived(boost::shared_ptr<AuthSuccess>(new AuthSuccess()));
				}

				void sendAuthFailure() {
					onElementReceived(boost::shared_ptr<AuthFailure>(new AuthFailure()));
				}

				void receiveStreamStart() {
					Event event = popEvent();
					CPPUNIT_ASSERT(event.header);
				}

				void receiveStartTLS() {
					Event event = popEvent();
					CPPUNIT_ASSERT(event.element);
					CPPUNIT_ASSERT(boost::dynamic_pointer_cast<StartTLSRequest>(event.element));
				}

				void receiveAuthRequest(const String& mech) {
					Event event = popEvent();
					CPPUNIT_ASSERT(event.element);
					boost::shared_ptr<AuthRequest> request(boost::dynamic_pointer_cast<AuthRequest>(event.element));
					CPPUNIT_ASSERT(request);
					CPPUNIT_ASSERT_EQUAL(mech, request->getMechanism());
				}

				Event popEvent() {
					CPPUNIT_ASSERT(receivedEvents.size() > 0);
					Event event = receivedEvents.front();
					receivedEvents.pop_front();
					return event;
				}

				bool available;
				bool canTLSEncrypt;
				bool tlsEncrypted;
				bool whitespacePingEnabled;
				int resetCount;
				std::deque<Event> receivedEvents;
		};

		boost::shared_ptr<MockSessionStream> server;
		bool sessionFinishedReceived;
		bool needCredentials;
		boost::shared_ptr<Error> sessionFinishedError;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ClientSessionTest);

#if 0
		void testAuthenticate() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			session->onNeedCredentials.connect(boost::bind(&ClientSessionTest::setNeedCredentials, this));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithAuthentication();
			session->startSession();
			processEvents();
			CPPUNIT_ASSERT_EQUAL(ClientSession::WaitingForCredentials, session->getState());
			CPPUNIT_ASSERT(needCredentials_);

			getMockServer()->expectAuth("me", "mypass");
			getMockServer()->sendAuthSuccess();
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			session->sendCredentials("mypass");
			CPPUNIT_ASSERT_EQUAL(ClientSession::Authenticating, session->getState());
			processEvents();
			CPPUNIT_ASSERT_EQUAL(ClientSession::Negotiating, session->getState());
		}

		void testAuthenticate_Unauthorized() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithAuthentication();
			session->startSession();
			processEvents();

			getMockServer()->expectAuth("me", "mypass");
			getMockServer()->sendAuthFailure();
			session->sendCredentials("mypass");
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
			CPPUNIT_ASSERT_EQUAL(ClientSession::AuthenticationFailedError, *session->getError());
		}

		void testAuthenticate_NoValidAuthMechanisms() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithUnsupportedAuthentication();
			session->startSession();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
			CPPUNIT_ASSERT_EQUAL(ClientSession::NoSupportedAuthMechanismsError, *session->getError());
		}

		void testResourceBind() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithResourceBind();
			getMockServer()->expectResourceBind("Bar", "session-bind");
			// FIXME: Check CPPUNIT_ASSERT_EQUAL(ClientSession::BindingResource, session->getState());
			getMockServer()->sendResourceBindResponse("me@foo.com/Bar", "session-bind");
			session->startSession();

			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStarted, session->getState());
			CPPUNIT_ASSERT_EQUAL(JID("me@foo.com/Bar"), session->getLocalJID());
		}

		void testResourceBind_ChangeResource() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithResourceBind();
			getMockServer()->expectResourceBind("Bar", "session-bind");
			getMockServer()->sendResourceBindResponse("me@foo.com/Bar123", "session-bind");
			session->startSession();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStarted, session->getState());
			CPPUNIT_ASSERT_EQUAL(JID("me@foo.com/Bar123"), session->getLocalJID());
		}

		void testResourceBind_EmptyResource() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithResourceBind();
			getMockServer()->expectResourceBind("", "session-bind");
			getMockServer()->sendResourceBindResponse("me@foo.com/NewResource", "session-bind");
			session->startSession();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStarted, session->getState());
			CPPUNIT_ASSERT_EQUAL(JID("me@foo.com/NewResource"), session->getLocalJID());
		}

		void testResourceBind_Error() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithResourceBind();
			getMockServer()->expectResourceBind("", "session-bind");
			getMockServer()->sendError("session-bind");
			session->startSession();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
			CPPUNIT_ASSERT_EQUAL(ClientSession::ResourceBindError, *session->getError());
		}

		void testSessionStart() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			session->onSessionStarted.connect(boost::bind(&ClientSessionTest::setSessionStarted, this));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithSession();
			getMockServer()->expectSessionStart("session-start");
			// FIXME: Check CPPUNIT_ASSERT_EQUAL(ClientSession::StartingSession, session->getState());
			getMockServer()->sendSessionStartResponse("session-start");
			session->startSession();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStarted, session->getState());
			CPPUNIT_ASSERT(sessionStarted_);
		}

		void testSessionStart_Error() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithSession();
			getMockServer()->expectSessionStart("session-start");
			getMockServer()->sendError("session-start");
			session->startSession();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::Error, session->getState());
			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStartError, *session->getError());
		}

		void testSessionStart_AfterResourceBind() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			session->onSessionStarted.connect(boost::bind(&ClientSessionTest::setSessionStarted, this));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeaturesWithResourceBindAndSession();
			getMockServer()->expectResourceBind("Bar", "session-bind");
			getMockServer()->sendResourceBindResponse("me@foo.com/Bar", "session-bind");
			getMockServer()->expectSessionStart("session-start");
			getMockServer()->sendSessionStartResponse("session-start");
			session->startSession();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(ClientSession::SessionStarted, session->getState());
			CPPUNIT_ASSERT(sessionStarted_);
		}

		void testWhitespacePing() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeatures();
			session->startSession();
			processEvents();
			CPPUNIT_ASSERT(session->getWhitespacePingLayer());
		}

		void testReceiveElementAfterSessionStarted() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeatures();
			session->startSession();
			processEvents();

			getMockServer()->expectMessage();
			session->sendElement(boost::shared_ptr<Message>(new Message()));
		}

		void testSendElement() {
			boost::shared_ptr<MockSession> session(createSession("me@foo.com/Bar"));
			session->onElementReceived.connect(boost::bind(&ClientSessionTest::addReceivedElement, this, _1));
			getMockServer()->expectStreamStart();
			getMockServer()->sendStreamStart();
			getMockServer()->sendStreamFeatures();
			getMockServer()->sendMessage();
			session->startSession();
			processEvents();

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(receivedElements_.size()));
			CPPUNIT_ASSERT(boost::dynamic_pointer_cast<Message>(receivedElements_[0]));
		}
#endif
