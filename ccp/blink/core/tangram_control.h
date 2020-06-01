// begin Add by TangramTeam
#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_CONTROL_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_CONTROL_H_

#include <map>

#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/bindings/core/v8/serialization/transferables.h"
#include "third_party/blink/renderer/core/execution_context/context_lifecycle_observer.h"
#include "third_party/blink/renderer/core/dom/events/event_target.h"
#include "third_party/blink/renderer/core/event_type_names.h"

#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/assertions.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

using namespace std;

class Tangram;
class TangramXobj;
class TangramNode;
class TangramWindow;
class TangramWinform;
class TangramControl;

class Document;
class ScriptState;
class ExceptionState;
class V8TangramCallback;
class V8ApplicationCallback;

class WebLocalFrameClient;
class SerializedScriptValue;

class CORE_EXPORT TangramControl final : public EventTargetWithInlineData,
									  public DOMWindowClient {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(TangramControl);

 public:
  static TangramControl* Create(LocalFrame* frame) { return MakeGarbageCollected<TangramControl>(frame); }
  static TangramControl* Create(LocalFrame* frame, const String& strHandle);

  void Trace(blink::Visitor*) override;

  // Called when an event listener has been successfully added.
  void AddedEventListener(const AtomicString& event_type,
                          RegisteredEventListener&) override;

  int64_t handle();
  String name();
  String type();
  String getid();
  TangramXobj* xobj();
  TangramNode* parentNode();
  TangramWinform* parentForm();
  TangramControl* parentControl();

  // Message method
  void sendMessage(TangramXobj* msg, V8ApplicationCallback* callback);
  void invokeCallback(wstring callbackid, TangramXobj* callbackParam);

  DEFINE_ATTRIBUTE_EVENT_LISTENER(MessageReceived, kTangramcontrol)

  // EventTarget overrides:
  const AtomicString& InterfaceName() const override;
  ExecutionContext* GetExecutionContext() const override;

  TangramControl(LocalFrame*);
  TangramControl(LocalFrame*, const String& strNodeXml);

  ~TangramControl() override;

  // Binding Event:
  void BindCtrlEvents(const String& strEvents); 
  void addEventListener(const String& eventName, V8ApplicationCallback* callback);
  void fireEvent(const String& eventName, TangramXobj* eventParam);
  void removeEventListener(const String& eventName);

  int64_t handle_ = 0;
  String name_;
  String type_;
  String events_;
  String webpageid_;

  mutable Member<Tangram> tangram_;
  mutable Member<TangramXobj> innerXobj_;
  mutable Member<TangramNode> m_pParentNode;
  mutable Member<TangramWinform> m_pParentForm;
  mutable Member<TangramControl> m_pParentControl;
  WebLocalFrameClient* m_pRenderframeImpl;
  map<std::wstring, int> m_mapEvent;
private:
  String id_;
  HeapHashMap<String, Member<V8ApplicationCallback>> mapTangramEventCallback_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_CONTROL_H_
// end Add by TangramTeam
