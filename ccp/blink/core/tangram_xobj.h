// begin Add by TangramTeam
#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_VAR_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_VAR_H_

#include <map>

#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/bindings/core/v8/serialization/transferables.h"
#include "third_party/blink/renderer/core/execution_context/context_lifecycle_observer.h"
#include "third_party/blink/renderer/core/dom/events/event_target.h"
#include "third_party/blink/renderer/core/event_type_names.h"

#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/assertions.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

#include "c:/src/tangram/source/chrome_proxy/third_party/ChromeRenderDomProxy.h"

namespace blink {

using namespace std;

class Tangram;
class TangramNode;
class TangramWindow;
class TangramWinform;
class Document;
class ScriptState;
class ExceptionState;
class V8TangramCallback;
class WebLocalFrameClient;
class SerializedScriptValue;
class V8ApplicationCallback;

class CORE_EXPORT TangramXobj final : public ScriptWrappable{
  DEFINE_WRAPPERTYPEINFO();
friend class Tangram;
 public:
  static TangramXobj* Create() { return MakeGarbageCollected<TangramXobj>(); }
  static TangramXobj* Create(const String& strName);

  void Trace(blink::Visitor*) override;

  TangramXobj();
  TangramXobj(const String& strNodeName);

  ~TangramXobj() override;

  String name();
  String getid();
  String getStr(const String& strKey);
  TangramNode* node();
  TangramWinform* form();
  TangramWindow* window();

  void setStr(const String& strKey, const String& value);
  long getLong(const String& strKey);
  void setLong(const String& strKey, long value);
  int64_t getInt64(const String& strKey);
  void setInt64(const String& strKey, int64_t value);
  float getFloat(const String& strKey);
  void setFloat(const String& strKey, float value);

  void addEventListener(const String& eventName, V8ApplicationCallback* callback);
  void addEventListener(const String& subObjName, const String& eventName, V8ApplicationCallback* callback);
  void removeEventListener(const String& eventName);
  void disConnect();
  void fireEvent(const String& eventName, TangramXobj* eventParam);
  void sendMessage(TangramXobj* msg, V8ApplicationCallback* callback);
  void invokeCallback(wstring callbackid, TangramXobj* callbackParam);

  String id_;
  TangramCommon::IPCSession session_;
  WebLocalFrameClient* m_pRenderframeImpl;
  mutable Member<Tangram> tangram_;
  HeapHashMap<String, Member<V8ApplicationCallback>> mapTangramEventCallback_;
private:
  String name_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_VAR_H_
// end Add by TangramTeam
