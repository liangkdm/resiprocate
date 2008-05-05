#if !defined(RemoteParticipantDialogSet_hxx)
#define RemoteParticipantDialogSet_hxx

#include <map>
#include <list>
#include <resip/dum/AppDialogSet.hxx>
#include <resip/dum/AppDialog.hxx>
#include <resip/dum/InviteSessionHandler.hxx>
#include <resip/dum/DialogSetHandler.hxx>
#include <resip/dum/SubscriptionHandler.hxx>

// FlowManager Includes
#include "MediaStream.hxx"

#include "ConversationManager.hxx"
#include "ConversationProfile.hxx"
#include "Participant.hxx"


namespace resip
{
class DialogUsageManager;
class SipMessage;
}

namespace useragent
{
class ConversationManager;
class RemoteParticipant;

/**
  This class is used by Invite DialogSets.  Non-Invite DialogSets
  are managed by DefaultDialogSet.  This class contains logic
  to handle forking and RTP connections.

  Author: Scott Godin (sgodin AT SipSpectrum DOT com)
*/

class RemoteParticipantDialogSet : public resip::AppDialogSet, private flowmanager::MediaStreamHandler
{
public:
   RemoteParticipantDialogSet(ConversationManager& conversationManager,        
                              ConversationManager::ParticipantForkSelectMode forkSelectMode = ConversationManager::ForkSelectAutomatic);

   virtual ~RemoteParticipantDialogSet();

   virtual RemoteParticipant* createUACOriginalRemoteParticipant(ConversationManager::ParticipantHandle handle);
   virtual resip::AppDialog* createAppDialog(const resip::SipMessage& msg);
   virtual unsigned int getLocalRTPPort();

   virtual void setUACConnected(const resip::DialogId& dialogId, ConversationManager::ParticipantHandle partHandle);
   virtual bool isUACConnected();
   virtual bool isStaleFork(const resip::DialogId& dialogId);

   virtual void removeDialog(const resip::DialogId& dialogId);
   virtual ConversationManager::ParticipantForkSelectMode getForkSelectMode();
   virtual ConversationManager::ParticipantHandle getActiveRemoteParticipantHandle() { return mActiveRemoteParticipantHandle; }

   // DialogSetHandler
   virtual void onTrying(resip::AppDialogSetHandle, const resip::SipMessage& msg);
   virtual void onNonDialogCreatingProvisional(resip::AppDialogSetHandle, const resip::SipMessage& msg);

   // sipX Media Stuff
   virtual int getMediaConnectionId() { return mMediaConnectionId; }
   virtual int getConnectionPortOnBridge() { return mConnectionPortOnBridge; }
   void setActiveDestination(const char* address, unsigned short rtpPort, unsigned short rtcpPort);
   void startDtlsClient(const char* address, unsigned short rtpPort, unsigned short rtcpPort);
   void setRemoteSDPFingerprint(const resip::Data& fingerprint);
   bool createSRTPSession(flowmanager::MediaStream::SrtpCryptoSuite cryptoSuite, const char* remoteKey, unsigned int remoteKeyLen);

   // Media Stream Processing
   void processMediaStreamReadyEvent(const StunTuple& remoteRtpTuple, const StunTuple& remoteRtcpTuple);
   void processMediaStreamErrorEvent(unsigned int errorCode);

   void sendInvite(resip::SharedPtr<resip::SipMessage> invite);
   void provideOffer(std::auto_ptr<resip::SdpContents> offer, resip::InviteSessionHandle& inviteSessionHandle, bool postOfferAccept);
   void provideAnswer(std::auto_ptr<resip::SdpContents> answer, resip::InviteSessionHandle& inviteSessionHandle, bool postAnswerAccept, bool postAnswerAlert);
   void accept(resip::InviteSessionHandle& inviteSessionHandle);
   ConversationProfile::SecureMediaMode getSecureMediaMode() { return mSecureMediaMode; }
   flowmanager::MediaStream::SrtpCryptoSuite getSrtpCryptoSuite() { return mSrtpCryptoSuite; }
   bool getSecureMediaRequired() { return mSecureMediaRequired; }

   const resip::Data& getLocalSrtpSessionKey() { return mLocalSrtpSessionKey; }

protected:
   virtual resip::SharedPtr<resip::UserProfile> selectUASUserProfile(const resip::SipMessage&); 

private:
   ConversationManager& mConversationManager;   
   RemoteParticipant* mUACOriginalRemoteParticipant;
   std::list<ConversationManager::ConversationHandle> mUACOriginalConversationHandles;
   unsigned int mNumDialogs;
   unsigned int mLocalRTPPort;
   ConversationManager::ParticipantForkSelectMode mForkSelectMode;
   resip::DialogId mUACConnectedDialogId;
   ConversationManager::ParticipantHandle mActiveRemoteParticipantHandle;
   std::map<resip::DialogId, RemoteParticipant*> mDialogs;

   // Media Stream stuff
   flowmanager::MediaStream::NatTraversalMode mNatTraversalMode;
   flowmanager::MediaStream* mMediaStream;
   reTurn::StunTuple mRtpTuple;
   reTurn::StunTuple mRtcpTuple;

   // SDP Negotiations that may need to be delayed due to FlowManager binding/allocation
   resip::SharedPtr<resip::SipMessage> mPendingInvite;
   void doSendInvite(resip::SharedPtr<resip::SipMessage> invite);
   class PendingOfferAnswer
   {
   public:
      PendingOfferAnswer() {}
      bool mOffer;
      std::auto_ptr<resip::SdpContents> mSdp;
      resip::InviteSessionHandle mInviteSessionHandle;
      bool mPostOfferAnswerAccept;
      bool mPostAnswerAlert;
   };
   PendingOfferAnswer mPendingOfferAnswer;
   void doProvideOfferAnswer(bool offer, std::auto_ptr<resip::SdpContents> sdp, resip::InviteSessionHandle& inviteSessionHandle, bool postOfferAnswerAccept, bool postAnswerAlert);

   // Secure Media 
   resip::Data mLocalSrtpSessionKey;
   ConversationProfile::SecureMediaMode mSecureMediaMode;
   bool mSecureMediaRequired;
   flowmanager::MediaStream::SrtpCryptoSuite mSrtpCryptoSuite;

   // sipX media stuff
   int mMediaConnectionId; 
   int mConnectionPortOnBridge;

   virtual void onMediaStreamReady(const StunTuple& remoteRtpTuple, const StunTuple& remoteRtcpTuple);
   virtual void onMediaStreamError(unsigned int errorCode);
};

}

#endif


/* ====================================================================

 Original contribution Copyright (C) 2008 Plantronics, Inc.
 Provided under the terms of the Vovida Software License, Version 2.0.

 The Vovida Software License, Version 2.0 
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 
 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution. 
 
 THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
 OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 DAMAGE.

 ==================================================================== */
