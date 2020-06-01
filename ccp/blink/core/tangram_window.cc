// begin Add by TangramTeam
#include "tangram.h"
#include "tangram_xobj.h"
#include "tangram_node.h"
#include "tangram_window.h"
#include "tangram_event.h"
#include "tangram_winform.h"
#include "tangram_compositor.h"

#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/html/html_element.h"
#include "third_party/blink/renderer/core/html/html_head_element.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_application_callback.h"

namespace blink {

	TangramWindow::TangramWindow(LocalFrame* frame) : DOMWindowClient(frame) {
		m_pHostNode = nullptr;
		m_pRenderframeImpl = nullptr;
		id_ = WTF::CreateCanonicalUUIDString();
	}

	TangramWindow::~TangramWindow() {
		Tangram* pTangram = tangram_.Get();
		if (pTangram)
		{
			WebString str = name_;
			auto it = pTangram->m_mapTangramWindow2.find(str.Utf16());
			if (it != pTangram->m_mapTangramWindow2.end())
				pTangram->m_mapTangramWindow2.erase(it);

			auto it2 = pTangram->m_mapTangramWindow.find(handle_);
			if (it2 != pTangram->m_mapTangramWindow.end())
				pTangram->m_mapTangramWindow.erase(it2);
		}
	}

	TangramWindow* TangramWindow::Create(LocalFrame* frame, const String& strWindowName) {
		return MakeGarbageCollected<TangramWindow>(frame, strWindowName);
	}

	TangramWindow::TangramWindow(LocalFrame* frame, const String& strWindowName) : DOMWindowClient(frame)
	{
		name_ = strWindowName;
	}

	void TangramWindow::Trace(blink::Visitor* visitor) {
		EventTargetWithInlineData::Trace(visitor);
		ScriptWrappable::Trace(visitor);
		DOMWindowClient::Trace(visitor);
		visitor->Trace(tangram_);
		visitor->Trace(innerXobj_);
		visitor->Trace(m_pHostNode);
		visitor->Trace(mapTangramEventCallback_);
	}

	void TangramWindow::AddedEventListener(const AtomicString& event_type,
		RegisteredEventListener& registered_listener) {
		EventTargetWithInlineData::AddedEventListener(event_type,
			registered_listener);
	}

	String TangramWindow::name() {
		return name_;
	}

	String TangramWindow::getid()
	{
		return id_;
	}

	int64_t TangramWindow::handle() {
		return handle_;
	}

	TangramXobj* TangramWindow::xobj()
	{
		return innerXobj_;
	}

	TangramNode* TangramWindow::getNode(const String& nodeName)
	{
		WebString strName = nodeName;
		auto it = m_mapTangramNode2.find(strName.Utf16());
		if (it != m_mapTangramNode2.end())
			return it->second;
		return nullptr;
	}

	TangramNode* TangramWindow::getNode(const long nodeHandle)
	{
		auto it = m_mapTangramNode.find(nodeHandle);
		if (it != m_mapTangramNode.end())
			return it->second;
		return nullptr;
	}

	const AtomicString& TangramWindow::InterfaceName() const {
		return event_target_names::kTangramWindow;
	}

	ExecutionContext* TangramWindow::GetExecutionContext() const {
		return DomWindow()->document();
	}

	void TangramWindow::addEventListener(const String& eventName, V8ApplicationCallback* callback)
	{
		if (callback)
		{
			auto it = innerXobj_->session_.m_mapString.find(L"objID");
			if (it != innerXobj_->session_.m_mapString.end())
			{
				tangram_->m_mapTangramWindow2[WebString(id_).Utf16()] = this;
				mapTangramEventCallback_.insert(eventName, callback);
				innerXobj_->session_.m_mapString[L"listenerType"] = WebString(eventName).Utf16();
				innerXobj_->session_.m_mapString[L"SenderType"] = L"TangramControl";
				innerXobj_->session_.m_mapString[L"Sender"] = WebString(id_).Utf16();
				sendMessage(innerXobj_, nullptr);
			}
		}
	}

	void TangramWindow::removeEventListener(const String& eventName)
	{
		auto it = mapTangramEventCallback_.find(eventName);
		if (it != mapTangramEventCallback_.end())
			mapTangramEventCallback_.erase(it);
	}

	void TangramWindow::disConnect()
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

	void TangramWindow::fireEvent(const String& eventName, TangramXobj* eventParam)
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

	void TangramWindow::sendMessage(TangramXobj* msg, V8ApplicationCallback* callback)
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

	void TangramWindow::invokeCallback(wstring callbackid, TangramXobj* callbackParam)
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
