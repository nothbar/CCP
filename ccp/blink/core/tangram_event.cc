#include "tangram_xobj.h"
#include "tangram_event.h"
#include "tangram_node.h"
#include "tangram_winform.h"

namespace blink {

  TangramEvent* TangramEvent::Create(const AtomicString& event_type,
    const String& eventId,
    const String& param1,
    const String& param2,
    const String& param3,
    const String& param4,
    const String& param5,
    const int64_t handleSource,
    const int64_t handleTarget, TangramXobj* session) {
      return MakeGarbageCollected<TangramEvent>(event_type, eventId, param1, param2, param3, param4, param5, handleSource, handleTarget, session);
  }

  TangramEvent* TangramEvent::Create(const AtomicString& event_type,
    const String& eventId,
    const String& param1,
    const int64_t handleSource,
    const String& param3,
    const int64_t handleTarget,
    const String& param5) {
      return MakeGarbageCollected<TangramEvent>(event_type, eventId, param1, "", param3, "", param5, handleSource, handleTarget, nullptr);
  }
  
  TangramEvent* TangramEvent::Create(const AtomicString& event_type, TangramXobj* session)
  {
      return MakeGarbageCollected<TangramEvent>(event_type, "", "", "", "", "", "", 0, 0, session);
  }

  void TangramEvent::Trace(blink::Visitor* visitor) {
      Event::Trace(visitor);
      visitor->Trace(session_);
  }

  TangramXobj* TangramEvent::xobj()
  {
      return session_;
  }

  void TangramEvent::setXobj(TangramXobj* session)
  {
      session_ = session;
  }

  String TangramEvent::eventId() {
      return eventId_;
  }

  int64_t TangramEvent::handleSource() {
      return handleSource_;
  }

  void TangramEvent::setHandleSource(const int64_t handleSource) {
      handleSource_ = handleSource;
  }

  int64_t TangramEvent::handleTarget() {
      return handleTarget_;
  }

  void TangramEvent::setHandleTarget(const int64_t handleTarget) {
      handleTarget_ = handleTarget;
  }

  void TangramEvent::setEventId(const String& eventId) {
      eventId_ = eventId;
  }

  String TangramEvent::param1() {
      return param1_;
  }

  void TangramEvent::setParam1(const String& param1) {
      param1_ = param1;
  }

  String TangramEvent::param2() {
    return param2_;
  }

  void TangramEvent::setParam2(const String& param2) {
    param2_ = param2;
  }

  String TangramEvent::param3() {
    return param3_;
  }

  void TangramEvent::setParam3(const String& param3) {
    param3_ = param3;
  }

  String TangramEvent::param4() {
    return param4_;
  }

  void TangramEvent::setParam4(const String& param4) {
      param4_ = param4;
  }

  String TangramEvent::param5() {
    return param5_;
  }

  void TangramEvent::setParam5(const String& param5) {
    param5_ = param5;
  }

  int64_t TangramEvent::hwnd()
  {
      int64_t h = session_->getInt64(L"nodehandle");
      if(h==0)
          h = session_->getInt64(L"formhandle");
      if(h==0)
          h = session_->getInt64(L"hwnd");
      if(h==0)
          h = session_->getInt64(L"hWnd");
      return h;
  }

  String TangramEvent::name() {
    return session_->name();
  }

  String TangramEvent::objtype() {
    return session_->getStr(L"objtype");
  }

  TangramWinform* TangramEvent::form()
  {
      if (session_)
          return session_->form();
      return nullptr;
  }

  TangramNode* TangramEvent::node()
  {
      if (session_)
          return session_->node();
      return nullptr;
  }

  TangramWindow* TangramEvent::window()
  {
      if (session_)
      {
          TangramNode* node = session_->node();
          if (node)
          {
              return node->parentWindow();
          }
      }
      return nullptr;
  }

  TangramEvent::TangramEvent() : Event("", Bubbles::kNo, Cancelable::kNo) {

  }

  TangramEvent::TangramEvent(const AtomicString& event_type,
    const String& eventId,
    const String& param1,
    const String& param2,
    const String& param3,
    const String& param4,
    const String& param5,
    const int64_t handleSource,
    const int64_t handleTarget,
      TangramXobj* session)
    : Event(event_type, Bubbles::kNo, Cancelable::kNo) {
    eventId_ = eventId;
    param1_ = param1;
    param2_ = param2;
    param3_ = param3;
    param4_ = param4;
    param5_ = param5;
    handleSource_ = handleSource;
    handleTarget_ = handleTarget;
    session_ = session;
  }

  TangramEvent::~TangramEvent() = default;

}  // namespace blink
