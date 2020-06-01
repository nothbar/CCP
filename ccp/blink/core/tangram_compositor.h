// begin Add by TangramTeam
#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_COMPOSITOR_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_COMPOSITOR_H_

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

class Document;
class TangramXobj;
class ScriptState;
class ExceptionState;
class V8TangramCallback;
class WebLocalFrameClient;
class SerializedScriptValue;

class CORE_EXPORT TangramCompositor final : public EventTargetWithInlineData,
									  public DOMWindowClient {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(TangramCompositor);

 public:
  static TangramCompositor* Create(LocalFrame* frame) { return MakeGarbageCollected<TangramCompositor>(frame); }
  static TangramCompositor* Create(LocalFrame* frame, const String& strHandle);

  void Trace(blink::Visitor*) override;

  // Called when an event listener has been successfully added.
  void AddedEventListener(const AtomicString& event_type,
                          RegisteredEventListener&) override;

  String name();
  String getid();

  // Message method

  void addChannel(const String& channel);
  void removeChannel(const String& channel);
  void sendMessage(const String& id, const String& param1, const String& param2, const String& param3, const String& param4, const String& param5);
  

  DEFINE_ATTRIBUTE_EVENT_LISTENER(MessageReceived, kTangramcompositor)
  DEFINE_ATTRIBUTE_EVENT_LISTENER(TangramNodeCreated, kTangramnodecreated)

  // EventTarget overrides:
  const AtomicString& InterfaceName() const override;
  ExecutionContext* GetExecutionContext() const override;

  TangramCompositor(LocalFrame*);
  TangramCompositor(LocalFrame*, const String& strNodeXml);

  ~TangramCompositor() override;

  WebLocalFrameClient* m_pRenderframeImpl;
  mutable Member<TangramXobj> innerXobj_;
private:
  String id_;
  String name_;
  String m_strNodeXml;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_COMPOSITOR_H_
// end Add by TangramTeam
