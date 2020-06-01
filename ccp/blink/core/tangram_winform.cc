// begin Add by TangramTeam
#include "tangram.h"
#include "tangram_node.h"
#include "tangram_xobj.h"
#include "tangram_event.h"
#include "tangram_winform.h"
#include "tangram_control.h"
#include "tangram_window.h"
#include "tangram_compositor.h"

#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/html/html_element.h"
#include "third_party/blink/renderer/core/html/html_head_element.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_application_callback.h"
#include "third_party/blink/renderer/platform/wtf/uuid.h"

namespace blink {

	TangramWinform::TangramWinform(LocalFrame* frame) : DOMWindowClient(frame) {
		m_pRenderframeImpl = nullptr;
		id_ = WTF::CreateCanonicalUUIDString();
	}

	TangramWinform::~TangramWinform() {
		Tangram* pTangram = tangram_.Get();
		if (pTangram)
		{
			auto it = pTangram->m_mapTangramWinformforEvent.find(handle_);
			if (it != pTangram->m_mapTangramWinformforEvent.end())
				pTangram->m_mapTangramWinformforEvent.erase(it);

			auto it2 = pTangram->m_mapWinForm.find(handle_);
			if (it2 != pTangram->m_mapWinForm.end())
				pTangram->m_mapWinForm.erase(it2);
		}
		mapTangramEventCallback_.clear();
	}

	TangramWinform* TangramWinform::Create(LocalFrame* frame, const String& strNodeName) {
		return MakeGarbageCollected<TangramWinform>(frame, strNodeName);
	}

	TangramWinform::TangramWinform(LocalFrame* frame, const String& strNodeName) : DOMWindowClient(frame)
	{
		name_ = strNodeName;
	}

	void TangramWinform::Trace(blink::Visitor* visitor) {
		EventTargetWithInlineData::Trace(visitor);
		ScriptWrappable::Trace(visitor);
		DOMWindowClient::Trace(visitor);
		visitor->Trace(tangram_);
		visitor->Trace(innerXobj_);
		visitor->Trace(mapTangramEventCallback_);
	}

	void TangramWinform::AddedEventListener(const AtomicString& event_type,
		RegisteredEventListener& registered_listener) {
		EventTargetWithInlineData::AddedEventListener(event_type,
			registered_listener);
	}

	String TangramWinform::name() {
		return name_;
	}

	TangramXobj* TangramWinform::xobj()
	{
		return innerXobj_;
	}

	String TangramWinform::getid()
	{
		return id_;
	}

	int64_t TangramWinform::handle() {
		return handle_;
	}

	void TangramWinform::SyncCtrlTextChange(const String& strcontrols, V8ApplicationCallback* callback)
	{
		if (callback)
		{
			innerXobj_->setStr(L"eventtype", L"SyncCtrlTextChange");
			innerXobj_->setStr(L"ctrls", strcontrols);
			innerXobj_->addEventListener(L"SyncCtrlTextChange", L"OnTextChanged", callback);
		}
	}

	void TangramWinform::addEventListener(const String& eventName, V8ApplicationCallback* callback)
	{
		blink::Tangram* pTangram = (blink::Tangram*)DomWindow()->tangram();
		if (callback)
		{
			auto it = innerXobj_->session_.m_mapString.find(L"objID");
			if (it != innerXobj_->session_.m_mapString.end())
			{
				pTangram->m_mapTangramWinformforEvent[handle_] = this;
				mapTangramEventCallback_.insert(eventName, callback);
				innerXobj_->session_.m_mapString[L"listenerType"] = WebString(eventName).Utf16();
				innerXobj_->session_.m_mapString[L"SenderType"] = L"TangramForm";
				innerXobj_->session_.m_mapString[L"Sender"] = WebString(id_).Utf16();
				sendMessage(innerXobj_, nullptr);
			}
		}
	}

	void TangramWinform::addEventListener(const String& subObjName, const String& eventName, V8ApplicationCallback* callback)
	{
		if (callback)
		{
			innerXobj_->addEventListener(subObjName, eventName, callback);
		}
	}

	void TangramWinform::fireEvent(const String& eventName, TangramXobj* eventParam)
	{
		auto itcallback = mapTangramEventCallback_.find(eventName);
		if (itcallback != mapTangramEventCallback_.end())
		{
			blink::V8ApplicationCallback* callback = (blink::V8ApplicationCallback*)itcallback->value;
			ScriptState* callback_relevant_script_state = callback->CallbackRelevantScriptState();
			ScriptState::Scope callback_relevant_context_scope(callback_relevant_script_state);
			callback->InvokeAndReportException(nullptr, eventParam);
		}
	}

	const AtomicString& TangramWinform::InterfaceName() const {
		return event_target_names::kTangramWinForm;
	}

	ExecutionContext* TangramWinform::GetExecutionContext() const {
		return DomWindow()->document();
	}

	void TangramWinform::removeEventListener(const String& eventName)
	{
		auto it = mapTangramEventCallback_.find(eventName);
		if (it != mapTangramEventCallback_.end())
			mapTangramEventCallback_.erase(it);
	}

	void TangramWinform::disConnect()
	{
		int nSize = mapTangramEventCallback_.size();
		if (nSize)
		{
			while (mapTangramEventCallback_.size())
			{
				auto it = mapTangramEventCallback_.begin();
				mapTangramEventCallback_.erase(it);
			}
		}
	}

	void TangramWinform::sendMessage(TangramXobj* msg, V8ApplicationCallback* callback)
	{
		if (m_pRenderframeImpl)
		{
			if (msg == nullptr)
				msg = innerXobj_;
			msg->setStr(L"senderid", id_);
			if (callback)
			{
				String callbackid_ = WTF::CreateCanonicalUUIDString();
				msg->setStr(L"callbackid", callbackid_);
				mapTangramEventCallback_.insert(callbackid_, callback);
				WebString strID = callbackid_;
				m_pRenderframeImpl->m_mapTangramSession[strID.Utf16()] = this;
			}
			m_pRenderframeImpl->SendTangramMessageEx(msg->session_);
		}
	}

	void TangramWinform::invokeCallback(wstring callbackid, TangramXobj* callbackParam)
	{
		auto itcallback = mapTangramEventCallback_.find(String(callbackid.c_str()));
		if (itcallback != mapTangramEventCallback_.end())
		{
			blink::V8ApplicationCallback* callback = (blink::V8ApplicationCallback*)itcallback->value.Get();
			mapTangramEventCallback_.erase(itcallback);
			ScriptState* callback_relevant_script_state = callback->CallbackRelevantScriptState();
			ScriptState::Scope callback_relevant_context_scope(callback_relevant_script_state);
			callback->InvokeAndReportException(nullptr, callbackParam);
		}
	}

}  // namespace blink
// end Add by TangramTeam
