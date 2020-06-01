// begin Add by TangramTeam
#include "tangram.h"
#include "tangram_node.h"
#include "tangram_event.h"
#include "tangram_xobj.h"
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
#include "third_party/blink/renderer/platform/bindings/callback_function_base.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_application_callback.h"

namespace blink {

	TangramControl::TangramControl(LocalFrame* frame) : DOMWindowClient(frame) {
		handle_ = 0;
		type_ = L"";
		events_ = L"";
		id_ = WTF::CreateCanonicalUUIDString();
		m_pRenderframeImpl = nullptr;
	}

	TangramControl::~TangramControl() {
		Tangram* pTangram = tangram_.Get();
		if (pTangram)
		{
			auto it = pTangram->m_mapTangramControlforEvent.find(handle_);
			if (it != pTangram->m_mapTangramControlforEvent.end())
				pTangram->m_mapTangramControlforEvent.erase(it);
		}
		mapTangramEventCallback_.clear();
	}

	TangramControl* TangramControl::Create(LocalFrame* frame, const String& strNodeName) {
		return MakeGarbageCollected<TangramControl>(frame, strNodeName);
	}

	TangramControl::TangramControl(LocalFrame* frame, const String& strNodeName) : DOMWindowClient(frame)
	{
		name_ = strNodeName;
	}

	int64_t TangramControl::handle() {
		return handle_;
	}

	TangramNode* TangramControl::parentNode()
	{
		return m_pParentNode;
	}

	TangramWinform* TangramControl::parentForm()
	{
		return m_pParentForm;
	}

	TangramControl* TangramControl::parentControl()
	{
		return m_pParentControl;
	}

	void TangramControl::BindCtrlEvents(const String& strEvents)
	{
		if (handle_)
		{
			if (m_pRenderframeImpl) {
				m_pRenderframeImpl->m_mapTangramControl[handle_] = (long)this;
				WebString webstr = strEvents;
				std::wstring _strBindEvents = webstr.Utf16();
				webstr = name_;
				std::wstring _strCtrlID = webstr.Utf16();
				webstr = type_;
				std::wstring _strCtrlType = webstr.Utf16();
				m_pRenderframeImpl->SendTangramMessage(L"TANGRAM_CTRL_BIND_MESSAGE", _strCtrlID, handle_, 2020, _strBindEvents, _strCtrlType);
			}
		}
	}

	void TangramControl::addEventListener(const String& eventName, V8ApplicationCallback* callback)
	{
		blink::Tangram* pTangram = (blink::Tangram*)DomWindow()->tangram();
		if (callback)
		{
			auto it = innerXobj_->session_.m_mapString.find(L"objID");
			if (it != innerXobj_->session_.m_mapString.end())
			{
				pTangram->m_mapTangramControlforEvent[handle_] = this;
				mapTangramEventCallback_.insert(eventName, callback);
				innerXobj_->session_.m_mapString[L"listenerType"] = WebString(eventName).Utf16();
				innerXobj_->session_.m_mapString[L"SenderType"] = L"TangramControl";
				innerXobj_->session_.m_mapString[L"Sender"] = WebString(id_).Utf16();
				sendMessage(innerXobj_, nullptr);
			}
		}
	}

	void TangramControl::removeEventListener(const String& eventName)
	{
		auto it = mapTangramEventCallback_.find(eventName);
		if (it != mapTangramEventCallback_.end())
			mapTangramEventCallback_.erase(it);
	}

	void TangramControl::fireEvent(const String& eventName, TangramXobj* eventParam)
	{
		auto itcallback = mapTangramEventCallback_.find(eventName);
		if (itcallback != mapTangramEventCallback_.end())
		{
			blink::V8ApplicationCallback* callback = (blink::V8ApplicationCallback*)itcallback->value.Get();
			ScriptState* callback_relevant_script_state = callback->CallbackRelevantScriptState();
			ScriptState::Scope callback_relevant_context_scope(callback_relevant_script_state);
			callback->InvokeAndReportException(nullptr, eventParam);
		}
	}

	void TangramControl::sendMessage(TangramXobj* msg, V8ApplicationCallback* callback)
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

	void TangramControl::invokeCallback(wstring callbackid, TangramXobj* callbackParam)
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

	void TangramControl::Trace(blink::Visitor* visitor) {
		EventTargetWithInlineData::Trace(visitor);
		ScriptWrappable::Trace(visitor);
		DOMWindowClient::Trace(visitor);
		visitor->Trace(tangram_);
		visitor->Trace(innerXobj_);
		visitor->Trace(m_pParentNode);
		visitor->Trace(m_pParentForm);
		visitor->Trace(m_pParentControl);
		visitor->Trace(mapTangramEventCallback_);
	}

	void TangramControl::AddedEventListener(const AtomicString& event_type,
		RegisteredEventListener& registered_listener) {
		EventTargetWithInlineData::AddedEventListener(event_type,
			registered_listener);
	}

	String TangramControl::name() {
		return name_;
	}

	String TangramControl::type() {
		return type_;
	}

	TangramXobj* TangramControl::xobj()
	{
		return innerXobj_;
	}

	const AtomicString& TangramControl::InterfaceName() const {
		return event_target_names::kTangramControl;
	}

	ExecutionContext* TangramControl::GetExecutionContext() const {
		return DomWindow()->document();
	}

}  // namespace blink
// end Add by TangramTeam
