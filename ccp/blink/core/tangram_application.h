// begin Add by TangramTeam
#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_APPLICATION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_APPLICATION_H_

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

class TangramXobj;
class TangramNode;
class TangramWindow;
class TangramControl;
class TangramWinform;
class ExceptionState;
class TangramCompositor;

//for callback:
class CallbackFunctionBase;

class V8TangramCallback;
class V8ApplicationCallback;

class Document;
class ScriptState;
class ExceptionState;
class V8TangramCallback;
class WebLocalFrameClient;
class SerializedScriptValue;

class CORE_EXPORT TangramApplication final : public EventTargetWithInlineData,
									  public DOMWindowClient {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(TangramApplication);

 public:
  static TangramApplication* Create(LocalFrame* frame) { return MakeGarbageCollected<TangramApplication>(frame); }
  static TangramApplication* Create(LocalFrame* frame, const String& strHandle);

  void Trace(blink::Visitor*) override;

  // Called when an event listener has been successfully added.
  void AddedEventListener(const AtomicString& event_type,
                          RegisteredEventListener&) override;

  String name();

  // Message method

  void addChannel(const String& channel);
  void removeChannel(const String& channel);
  void sendMessage(const String& id, const String& param1, const String& param2, const String& param3, const String& param4, const String& param5);
  

  DEFINE_ATTRIBUTE_EVENT_LISTENER(AppMessageReceived, kTangramapplication)

  // EventTarget overrides:
  const AtomicString& InterfaceName() const override;
  ExecutionContext* GetExecutionContext() const override;

  explicit TangramApplication(LocalFrame*);
  TangramApplication(LocalFrame*, const String& strNodeXml);

  ~TangramApplication() override;

  mutable Member<TangramXobj> innerXobj_;
  WebLocalFrameClient* web_local_frame_client;
private:
  String name_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_APPLICATION_H_
// end Add by TangramTeam
