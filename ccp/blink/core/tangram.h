#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_H_

#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <iterator>
#include <regex>

#include "base/run_loop.h"
#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/bindings/core/v8/serialization/transferables.h"
#include "third_party/blink/renderer/core/execution_context/context_lifecycle_observer.h"
#include "third_party/blink/renderer/core/dom/events/event_target.h"
#include "third_party/blink/renderer/core/event_type_names.h"

#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/assertions.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/uuid.h"

#define IPC_CLR_CONTROL_CREARED							20200220
#define IPC_CLR_CONTROL_CREARED_ID						_T("Tangram_CLR_Control_Created")
#define IPC_NODE_CREARED								20200221
#define IPC_NODE_CREARED_ID								_T("Tangram_WndNode_Created")
#define IPC_NODE_ONMOUSEACTIVATE						20200222
#define IPC_NODE_ONMOUSEACTIVATE_ID						_T("Tangram_WndNode_OnMouseActivate")
#define IPC_MDIWINFORM_ACTIVEMDICHILD					20200224
#define IPC_MDIWINFORM_ACTIVEMDICHILD_ID				_T("MdiWinForm_ActiveMdiChild")

namespace blink {
	using namespace std;

	class Element;
	class LocalFrame;
	class Document;
	class ScriptState;
	class TangramXobj;
	class TangramNode;
	class TangramWindow;
	class TangramControl;
	class TangramWinform;
	class ExceptionState;
	class TangramCompositor;
	class WebLocalFrameClient;
	class SerializedScriptValue;

	//for callback:
	class CallbackFunctionBase;

	class V8TangramCallback;
	class V8ApplicationCallback;

	class CORE_EXPORT Tangram final :
	public EventTargetWithInlineData,
		public DOMWindowClient{
	DEFINE_WRAPPERTYPEINFO();
	USING_GARBAGE_COLLECTED_MIXIN(Tangram);
		friend  class TangramWinform;

	public:
	 static Tangram* Create(LocalFrame* frame) { return MakeGarbageCollected<Tangram>(frame); }

	 void Trace(blink::Visitor*) override;

	 // Called when an event listener has been successfully added.
	 void AddedEventListener(const AtomicString& event_type,
							 RegisteredEventListener&) override;

	 DEFINE_ATTRIBUTE_EVENT_LISTENER(MessageReceived, kTangram)
	 DEFINE_ATTRIBUTE_EVENT_LISTENER(TangramMessageReceived, kTangrammessage)
	 DEFINE_ATTRIBUTE_EVENT_LISTENER(MdiChildActivate, kMdichildactivate)
	 DEFINE_ATTRIBUTE_EVENT_LISTENER(TangramNodeCreated, kTangramnodecreated)
	 DEFINE_ATTRIBUTE_EVENT_LISTENER(TangramWindowCreated, kTangramwindowcreated)
	 DEFINE_ATTRIBUTE_EVENT_LISTENER(WinFormCreated, kWinformcreated)
	 DEFINE_ATTRIBUTE_EVENT_LISTENER(WinFormClosed, kWinformclosed)
	 DEFINE_ATTRIBUTE_EVENT_LISTENER(BindCLRObject, kBindclrobject)

	 TangramXobj* xobj();
	 String url();
	 TangramXobj* getNamedItem(const AtomicString&) const;
	 TangramXobj* setNamedItem(TangramXobj*, ExceptionState&);
	 void setNamedItem(const String& name, TangramXobj* var, ExceptionState&);

	 void NamedPropertyEnumerator(Vector<String>& names, ExceptionState&) const;
	 bool NamedPropertyQuery(const AtomicString&, ExceptionState&) const;

	 void wait(bool bwait);
	 void addChannel(const String& channel);
	 void removeChannel(const String& channel);
	 void sendMessage(const String& id, const String& param1, const String& param2, const String& param3, const String& param4, const String& param5);
	 void sendMessage(TangramXobj* msg, V8ApplicationCallback* callback, bool bwait);
	 void addEventListener(const String& eventName, V8ApplicationCallback* callback);
	 void removeEventListener(const String& eventName);
	 void disConnect();
	 void fireEvent(const String& eventName, TangramXobj* eventParam);

	 // DOM method
	 void defineElement(const String& tagName, const String& html);
	 void renderElement(const String& tagName, const String& html);

	 //WinForm API:
	 TangramWinform* createWinForm(const String& strFormXml, const long FormType, V8ApplicationCallback* callback);
	 TangramWinform* newWinForm(int64_t handle, TangramXobj* obj);

	 //TangramNode API
	 TangramNode* createTangramNode(TangramXobj* xobj);
	 TangramNode* getNode(const int64_t nodeHandle);

	 TangramWindow* getWindow(const String& wndName);
	 TangramWindow* getWindow(const int64_t wndHandle);

	 // Non-js method
	 void waitMessage();
	 void releaseMessage();

	 // EventTarget overrides:
	 const AtomicString& InterfaceName() const override;
	 ExecutionContext* GetExecutionContext() const override;

	 explicit Tangram(LocalFrame*);
	 ~Tangram() override;
	 void Close();

	 TangramXobj* newVar(const String& strName);
	 void invokeWinFormCreatedCallback(TangramWinform* form);

	 WebLocalFrameClient* m_pRenderframeImpl;
	 String url_;
	 mutable Member<TangramXobj> innerXobj_;
	 HeapHashMap<String, Member<TangramXobj>> mapCloudSession_;
	 HeapHashMap<int64_t, Member<TangramNode>> m_mapTangramNode;
	 HeapHashMap<int64_t, Member<TangramWindow>> m_mapTangramWindow;
	 HeapHashMap<int64_t, Member<TangramWinform>> m_mapWinForm;
	 HeapHashMap<int64_t, Member<TangramCompositor>> m_mapTangramCompositor;

	 map<int64_t, TangramNode*> m_mapTangramNodeforEvent;
	 map<int64_t, TangramWinform*> m_mapTangramWinformforEvent;
	 map<int64_t, TangramControl*> m_mapTangramControlforEvent;
	 map<wstring, TangramWindow*> m_mapTangramWindow2;
   private:
	 base::RunLoop run_loop_;
	 bool is_pending_;
	 Vector<String> pending_messages_;
	 HeapHashMap<String, Member<V8TangramCallback>> mapTangramCallback_;
	 HeapHashMap<int64_t, Member<CallbackFunctionBase>> mapCallbackFunction_;
	};
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_TANGRAM_H_
