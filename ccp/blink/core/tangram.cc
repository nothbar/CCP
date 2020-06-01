#include "tangram.h"
#include "tangram_xobj.h"
#include "tangram_node.h"
#include "tangram_event.h"
#include "tangram_window.h"
#include "tangram_winform.h"
#include "tangram_control.h"
#include "tangram_compositor.h"
#include <stdlib.h>
#include <string>
#include <iostream>

#include "base/guid.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/html/html_element.h"
#include "third_party/blink/renderer/core/html/html_head_element.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/platform/bindings/callback_function_base.h"

#include "third_party/blink/renderer/bindings/core/v8/v8_tangram_callback.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_application_callback.h"

#include "C:\src\Tangram\Source\chrome_proxy\third_party\ChromeRenderDomProxy.h"

namespace blink {

	Tangram::Tangram(LocalFrame* frame) : DOMWindowClient(frame) {
		is_pending_ = false;
		innerXobj_ = newVar(L"");
		url_ = L"";
		m_pRenderframeImpl = nullptr;
	}

	Tangram::~Tangram() {
	}

	void Tangram::Trace(blink::Visitor* visitor) {
		EventTargetWithInlineData::Trace(visitor);
		ScriptWrappable::Trace(visitor);
		DOMWindowClient::Trace(visitor);
		visitor->Trace(m_mapTangramNode);
		visitor->Trace(m_mapTangramWindow);
		visitor->Trace(m_mapWinForm);
		visitor->Trace(m_mapTangramCompositor);
		visitor->Trace(mapTangramCallback_);
		visitor->Trace(mapCallbackFunction_);
		visitor->Trace(innerXobj_);
		visitor->Trace(mapCloudSession_);
	}

	String Tangram::url()
	{
		return DomWindow()->document()->Url().GetString();
	}

	TangramXobj* Tangram::xobj()
	{
		return innerXobj_;
	}

	void Tangram::wait(bool bwait)
	{
		if (bwait)
			run_loop_.Run();
		else
			run_loop_.Quit();
	}

	void Tangram::AddedEventListener(const AtomicString& event_type,
		RegisteredEventListener& registered_listener) {
		EventTargetWithInlineData::AddedEventListener(event_type,
			registered_listener);
	}

	// User level message
	void Tangram::addChannel(const String& channel) {
		sendMessage(L"ADD_CHANNEL", channel, L"", L"", L"", L"");
	}

	void Tangram::removeChannel(const String& channel) {
		sendMessage(L"REMOVE_CHANNEL", channel, L"", L"", L"", L"");
	}

	void Tangram::sendMessage(const String& id, const String& param1, const String& param2, const String& param3, const String& param4, const String& param5) {
		if (m_pRenderframeImpl == nullptr)
		{
			m_pRenderframeImpl = WebLocalFrameImpl::FromFrame(GetFrame())->Client();
			innerXobj_->m_pRenderframeImpl = m_pRenderframeImpl;
		}
		if (m_pRenderframeImpl) {
			if (is_pending_) {
				String message = id + "%%%" + param1 + "%%%" + param2 + "%%%" + param3 + "%%%" + param4 + "%%%" + param5;
				pending_messages_.push_back(message);
			}
			else {
				WebString webstr = id;
				std::wstring u16_id = webstr.Utf16();
				webstr = param1;
				std::wstring u16_param1 = webstr.Utf16();
				webstr = param2;
				std::wstring u16_param2 = webstr.Utf16();
				webstr = param3;
				std::wstring u16_param3 = webstr.Utf16();
				webstr = param4;
				std::wstring u16_param4 = webstr.Utf16();
				webstr = param5;
				std::wstring u16_param5 = webstr.Utf16();
				m_pRenderframeImpl->SendTangramMessage(u16_id, u16_param1, u16_param2, u16_param3, u16_param4, u16_param5);
			}
		}
	}

	// Define a element
	void Tangram::defineElement(const String& tagName, const String& html) {
		sendMessage(L"DEFINE_ELEMENT", tagName, html, L"", L"", L"");
	}

	// Render an element.
	void Tangram::renderElement(const String& tagName, const String& html) {
		sendMessage(L"RENDER_ELEMENT", tagName, html, L"", L"", L"");
	}

	void Tangram::waitMessage() {
		is_pending_ = true;
	}

	void Tangram::releaseMessage() {
		is_pending_ = false;
		if (m_pRenderframeImpl) {
			if (pending_messages_.size() > 0) {
				String stringBuffer = "";
				for (wtf_size_t i = 0; i < pending_messages_.size(); i++) {
					if (i > 0) {
						stringBuffer = stringBuffer + "$$$";
					}
					stringBuffer = stringBuffer + pending_messages_[i];
				}
				WebString bundle = stringBuffer;
				std::wstring type = L"AGGREGATED_MESSAGE";
				std::wstring param1 = bundle.Utf16();
				m_pRenderframeImpl->SendTangramMessage(type, param1, L"", L"", L"", L"");
				pending_messages_.clear();
			}
		}
	}

	TangramXobj* Tangram::newVar(const String& strName)
	{
		TangramXobj* var = TangramXobj::Create(strName);
		var->m_pRenderframeImpl = m_pRenderframeImpl;
		var->id_ = WTF::CreateCanonicalUUIDString();
		var->session_.m_mapString[L"sessionid"] = WebString(var->id_).Utf16();
		var->tangram_ = this;
		return var;
	}

	TangramWinform* Tangram::createWinForm(const String& strFormXml, const long FormType, V8ApplicationCallback* callback)
	{
		TangramWinform* form = TangramWinform::Create(DomWindow()->GetFrame(), strFormXml);
		form->tangram_ = this;
		form->innerXobj_ = newVar(L"");
		form->handle_ = (int64_t)form;
		if (callback)
			mapCallbackFunction_.insert(form->handle_, callback);
		form->m_pRenderframeImpl = m_pRenderframeImpl;
		m_mapWinForm.insert(form->handle_, form);
		if (m_pRenderframeImpl) {
			WebString webstr = strFormXml;
			std::wstring _strFormXml = webstr.Utf16();
			form->innerXobj_->setStr(L"msgID", L"CREATE_WINFORM");
			form->innerXobj_->setStr(L"objID", L"WinForm");
			form->innerXobj_->setInt64(L"form", (int64_t)form);
			form->innerXobj_->setInt64(L"innerXobj", (int64_t)form->innerXobj_.Get());
			form->innerXobj_->setStr(L"formXml", strFormXml);
			form->innerXobj_->setLong(L"formType", FormType);
			m_pRenderframeImpl->SendTangramMessageEx(form->innerXobj_->session_);
		}
		return form;
	}

	TangramWinform* Tangram::newWinForm(int64_t handle, TangramXobj* obj)
	{
		TangramWinform* form = TangramWinform::Create(DomWindow()->GetFrame(), L"");
		form->tangram_ = this;
		form->innerXobj_ = obj;
		form->handle_ = handle;
		form->innerXobj_->setStr(L"objID", L"WinForm");
		form->m_pRenderframeImpl = m_pRenderframeImpl;
		m_mapWinForm.insert(form->handle_, form);
		return form;
	}

	TangramNode* Tangram::createTangramNode(TangramXobj* xobj)
	{
		__int64 handle = xobj->getInt64(L"nodehandle");
		__int64 nRootHandle = xobj->getInt64(L"rootnodehandle");
		String strname = xobj->getStr(L"name@page");
		blink::TangramWindow* window = nullptr;
		bool bNewWindow = false;
		if (nRootHandle)
		{
			auto it1 = m_mapTangramWindow.find(nRootHandle);
			if (it1 != m_mapTangramWindow.end())
				window = it1->value;
			else
			{
				bNewWindow = true;
				window = TangramWindow::Create(DomWindow()->GetFrame(), strname);
				window->tangram_ = this;
				window->innerXobj_ = xobj;
				window->handle_ = handle;
				window->m_pRenderframeImpl = m_pRenderframeImpl;
				m_mapTangramWindow.insert(handle, window);
				WebString str = strname;
				m_mapTangramWindow2[str.Utf16()] = window;
			}
		}

		TangramNode* node = TangramNode::Create(DomWindow()->GetFrame(), strname);
		node->tangram_ = this;
		node->innerXobj_ = xobj;
		node->handle_ = handle;
		node->m_pRenderframeImpl = m_pRenderframeImpl;
		m_mapTangramNode.insert(handle, node);
		if(bNewWindow)
			DispatchEvent(*blink::TangramEvent::Create(blink::event_type_names::kTangramwindowcreated, xobj));

		__int64 nPHandle = xobj->getInt64(L"parentnodehandle");
		if (nPHandle)
		{
			auto it = m_mapTangramNode.find(nPHandle);
			if (it != m_mapTangramNode.end())
			{
				WebString str = strname;
				it->value->m_mapChildNode2[str.Utf16()] = node;
			}
		}
		if (window)
		{
			window->m_mapTangramNode[handle] = node;
			window->DispatchEvent(*blink::TangramEvent::Create(blink::event_type_names::kTangramnodecreated, xobj));
		}
		
		nPHandle = xobj->getInt64(L"parentFormHandle");
		if (nPHandle)
		{
			auto it = m_mapWinForm.find(nPHandle);
			if (it != m_mapWinForm.end())
			{
				TangramWinform* form = it->value.Get();
				form->DispatchEvent(*blink::TangramEvent::Create(blink::event_type_names::kTangramnodecreated, xobj));
			}
		}

		DispatchEvent(*blink::TangramEvent::Create(blink::event_type_names::kTangramnodecreated, xobj));

		return node;
	}

	TangramNode* Tangram::getNode(const int64_t nodeHandle)
	{
		auto it = m_mapTangramNode.find(nodeHandle);
		if (it != m_mapTangramNode.end())
			return it->value;
		return nullptr;
	}

	TangramWindow* Tangram::getWindow(const String& wndName)
	{
		WebString str = wndName;
		auto it = m_mapTangramWindow2.find(str.Utf16());
		if (it != m_mapTangramWindow2.end())
			return it->second;
		return nullptr;
	}

	TangramWindow* Tangram::getWindow(const int64_t wndHandle)
	{
		if (wndHandle)
		{
			auto it = m_mapTangramWindow.find(wndHandle);
			if (it != m_mapTangramWindow.end())
				return it->value;
		}
		return nullptr;
	}

	//void Tangram::sendIPCMessage(const String& type, const String& param1, const String& param2) {
	//	if (m_pRenderframeImpl) {
	//		if (is_bundling_) {
	//			String message = type + "%%%" + param1 + "%%%" + param2;
	//			pending_messages_.push_back(message);
	//		}
	//		else {
	//			WebString webstr = type;
	//			std::wstring u16_type = webstr.Utf16();
	//			webstr = param1;
	//			std::wstring u16_param1 = webstr.Utf16();
	//			webstr = param2;
	//			std::wstring u16_param2 = webstr.Utf16();
	//			//m_pRenderframeImpl->SendTangramMessage(u16_type, u16_param1, u16_param2);
	//		}
	//	}
	//}

	const AtomicString& Tangram::InterfaceName() const {
		return event_target_names::kTangram;
	}

	ExecutionContext* Tangram::GetExecutionContext() const {
		return DomWindow()->document();
	}

	void Tangram::addEventListener(const String& eventName, V8ApplicationCallback* callback)
	{
		if (callback)
		{
			innerXobj_->mapTangramEventCallback_.insert(eventName, callback);
		}
	}

	void Tangram::removeEventListener(const String& eventName)
	{
		auto it = innerXobj_->mapTangramEventCallback_.find(eventName);
		if (it != innerXobj_->mapTangramEventCallback_.end())
			innerXobj_->mapTangramEventCallback_.erase(it);
	}

	void Tangram::disConnect()
	{
		int nSize = innerXobj_->mapTangramEventCallback_.size();
		if (nSize)
		{
			while (innerXobj_->mapTangramEventCallback_.size())
			{
				auto it = innerXobj_->mapTangramEventCallback_.begin();
				innerXobj_->mapTangramEventCallback_.erase(it);
			}
		}
	}

	void Tangram::invokeWinFormCreatedCallback(TangramWinform* form)
	{
		auto itcallback = mapCallbackFunction_.find((int64_t)form);
		if (itcallback != mapCallbackFunction_.end())
		{
			blink::V8ApplicationCallback* callback = (blink::V8ApplicationCallback*)itcallback->value.Get();
			ScriptState* callback_relevant_script_state = callback->CallbackRelevantScriptState();
			ScriptState::Scope callback_relevant_context_scope(callback_relevant_script_state);
			callback->InvokeAndReportException(nullptr, form->innerXobj_);
			mapCallbackFunction_.erase(itcallback);
		}
	}

	void Tangram::fireEvent(const String& eventName, TangramXobj* eventParam)
	{
		auto itcallback = innerXobj_->mapTangramEventCallback_.find(eventName);
		if (itcallback != innerXobj_->mapTangramEventCallback_.end())
		{
			blink::V8ApplicationCallback* callback = (blink::V8ApplicationCallback*)itcallback->value.Get();
			ScriptState* callback_relevant_script_state = callback->CallbackRelevantScriptState();
			ScriptState::Scope callback_relevant_context_scope(callback_relevant_script_state);
			callback->InvokeAndReportException(nullptr, eventParam);
		}
	}

	void Tangram::sendMessage(TangramXobj* msg, V8ApplicationCallback* callback, bool bwait=false)
	{
		if (m_pRenderframeImpl)
		{
			if (msg == nullptr)
				msg = innerXobj_;
			msg->setStr(L"senderid", innerXobj_->getid());
			if (callback)
			{
				String callbackid_ = WTF::CreateCanonicalUUIDString();
				msg->setStr(L"callbackid", callbackid_);
				innerXobj_->mapTangramEventCallback_.insert(callbackid_, callback);
				WebString strID = callbackid_;
				m_pRenderframeImpl->m_mapTangramSession[strID.Utf16()] = this;
			}
			m_pRenderframeImpl->SendTangramMessageEx(msg->session_);
		}
		//if (bwait)
		//	run_loop_.Run();
	}

	TangramXobj* Tangram::getNamedItem(const AtomicString& name) const {
		auto it = mapCloudSession_.find(name);
		if (it != mapCloudSession_.end())
			return it->value.Get();
		return nullptr;
	}

	void Tangram::setNamedItem(const String& name, TangramXobj* var, ExceptionState&)
	{
		DCHECK(var);
		mapCloudSession_.insert(name, var);
	}

	TangramXobj* Tangram::setNamedItem(TangramXobj* var, ExceptionState& exception_state) {
		DCHECK(var);
		mapCloudSession_.insert(var->name(), var);
		return var;
	}

	void Tangram::NamedPropertyEnumerator(Vector<String>& names,
		ExceptionState&) const {
		for (auto it : mapCloudSession_) {
			names.UncheckedAppend(it.value->name());
		}
	}

	bool Tangram::NamedPropertyQuery(const AtomicString& name,
		ExceptionState& exception_state) const {
		Vector<String> properties;
		NamedPropertyEnumerator(properties, exception_state);
		return properties.Contains(name);
	}

	void Tangram::Close()
	{
		while (mapCloudSession_.size())
		{
			mapCloudSession_.erase(mapCloudSession_.begin());
		}
		while (m_mapTangramNode.size())
		{
			m_mapTangramNode.erase(m_mapTangramNode.begin());
		}
		while (m_mapTangramWindow.size())
		{
			m_mapTangramWindow.erase(m_mapTangramWindow.begin());
		}
		while (m_mapWinForm.size())
		{
			m_mapWinForm.erase(m_mapWinForm.begin());
		}
		while (m_mapTangramCompositor.size())
		{
			m_mapTangramCompositor.erase(m_mapTangramCompositor.begin());
		}
		while (mapTangramCallback_.size())
		{
			mapTangramCallback_.erase(mapTangramCallback_.begin());
		}
		while (mapCallbackFunction_.size())
		{
			mapCallbackFunction_.erase(mapCallbackFunction_.begin());
		}
	}

}  // namespace blink
