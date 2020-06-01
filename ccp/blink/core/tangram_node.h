// begin Add by TangramTeam
#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_NODE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_NODE_H_

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
class TangramWindow;
class TangramWinform;
class TangramControl;

class Document;
class ScriptState;
class ExceptionState;
class V8ApplicationCallback;
class WebLocalFrameClient;
class SerializedScriptValue;

class CORE_EXPORT TangramNode final : public EventTargetWithInlineData,
									  public DOMWindowClient {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(TangramNode);

 public:
  static TangramNode* Create(LocalFrame* frame) { return MakeGarbageCollected<TangramNode>(frame); }
  static TangramNode* Create(LocalFrame* frame, const String& strHandle);

  void Trace(blink::Visitor*) override;

  // Called when an event listener has been successfully added.
  void AddedEventListener(const AtomicString& event_type,
                          RegisteredEventListener&) override;

  long row();
  long col();
  long rows();
  long cols();
  int64_t handle();
  TangramXobj* xobj();

  TangramNode* rootNode();
  TangramNode* parentNode();
  TangramWindow* parentWindow();
  TangramWinform* parentForm();

  int64_t hwnd();
  String name();
  String objtype();
  String getid();

  // Message method

  void addChannel(const String& channel);
  void removeChannel(const String& channel);
  void sendMessage(const String& id, const String& param1, const String& param2, const String& param3, const String& param4, const String& param5);
  void addEventListener(const String& eventName, V8ApplicationCallback* callback);
  void addEventListener(const String& subObjName, const String& eventName, V8ApplicationCallback* callback);
  void removeEventListener(const String& eventName);
  void disConnect();
  void fireEvent(const String& eventName, TangramXobj* eventParam);
  void sendMessage(TangramXobj* msg, V8ApplicationCallback* callback);
  void invokeCallback(wstring callbackid, TangramXobj* callbackParam);
  void SyncCtrlTextChange(const String& strcontrols, V8ApplicationCallback* callback);

  //Control Bind API:
  void setControlVal(const String& CtrlID, int64_t CtrlHandle, const String& strVal);
  void GetControlsNames(V8ApplicationCallback* callback);

  // Node API:
  TangramNode* getChild(long nIndex);
  TangramNode* getChild(long row, long col);
  TangramNode* getChild(const String& strName);
  void open(const String& strKey, const String& xml, V8ApplicationCallback* callback);
  void openEx(const String& strKey, const String& xml, long row, long col, V8ApplicationCallback* callback);

  DEFINE_ATTRIBUTE_EVENT_LISTENER(MessageReceived, kTangram)
  DEFINE_ATTRIBUTE_EVENT_LISTENER(MdiChildActivate, kMdichildactivate)
  DEFINE_ATTRIBUTE_EVENT_LISTENER(TangramNodeCreated, kTangramnodecreated)
  DEFINE_ATTRIBUTE_EVENT_LISTENER(TangramControlCreated, kTangramcontrolcreated)
  DEFINE_ATTRIBUTE_EVENT_LISTENER(WinFormCreated, kWinformcreated)
  DEFINE_ATTRIBUTE_EVENT_LISTENER(WinFormClosed, kWinformclosed)

  // EventTarget overrides:
  const AtomicString& InterfaceName() const override;
  ExecutionContext* GetExecutionContext() const override;

  TangramNode(LocalFrame*);
  TangramNode(LocalFrame*, const String& strNodeXml);
  TangramNode* AddChild(int64_t nHandle, const String& strNodeName, blink::Tangram*);
  //TangramControl* AddCtrl(int64_t nCtrlHandle, const String& strCtrlName, const String& strCtrlType, const String& strCtrlData, const String& strWebPageID, blink::Tangram*);

  ~TangramNode() override;

  int64_t handle_ = 0;

  String name_;

  mutable Member<Tangram> tangram_;
  mutable Member<TangramXobj> innerXobj_;
  mutable Member<TangramWinform> m_pParentForm;

  WebLocalFrameClient* m_pRenderframeImpl;
  map<int, TangramNode*> m_mapChildNode;
  map<wstring, TangramNode*> m_mapChildNode2;

private:
  String id_;
  HeapHashMap<String, Member<V8ApplicationCallback>> mapTangramEventCallback_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_NODE_H_
// end Add by TangramTeam
