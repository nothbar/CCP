// begin Add by TangramTeam
#include "tangram.h"
#include "tangram_xobj.h"
#include "tangram_node.h"
#include "tangram_event.h"
#include "tangram_winform.h"
#include "tangram_window.h"
#include "tangram_control.h"
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

TangramNode::TangramNode(LocalFrame* frame) : DOMWindowClient(frame) {
	m_pParentForm = nullptr;
	m_pRenderframeImpl = nullptr;
	id_ = WTF::CreateCanonicalUUIDString();
}

TangramNode::~TangramNode() {
	Tangram* pTangram = tangram_.Get();
	if (pTangram)
	{
		auto it = pTangram->m_mapTangramNodeforEvent.find(handle_);
		if (it != pTangram->m_mapTangramNodeforEvent.end())
			pTangram->m_mapTangramNodeforEvent.erase(it);
		
		auto it2 = pTangram->m_mapTangramNode.find(handle_);
		if (it2 != pTangram->m_mapTangramNode.end())
			pTangram->m_mapTangramNode.erase(it2);
	}
	mapTangramEventCallback_.clear();
}

TangramNode* TangramNode::Create(LocalFrame* frame, const String& strNodeName) {
	return MakeGarbageCollected<TangramNode>(frame, strNodeName);
}

TangramNode::TangramNode(LocalFrame* frame, const String& strNodeName) : DOMWindowClient(frame)
{
	name_ = strNodeName;
}

String TangramNode::name() {
	return name_;
}

String TangramNode::getid()
{
	return id_;
}

long TangramNode::row()
{
	return innerXobj_->getLong(L"row");
}

long TangramNode::col()
{
	return innerXobj_->getLong(L"col");
}

long TangramNode::rows()
{
	return innerXobj_->getLong(L"rows");
}

long TangramNode::cols()
{
	return innerXobj_->getLong(L"cols");
}

TangramXobj* TangramNode::xobj()
{
	return innerXobj_;
}

TangramWinform* TangramNode::parentForm()
{
	return m_pParentForm;
}

TangramNode* TangramNode::getChild(long nIndex)
{
	auto it = m_mapChildNode.find(nIndex);
	if (it != m_mapChildNode.end())
		return it->second;
	return nullptr;
}

TangramNode* TangramNode::getChild(long row, long col)
{
	//auto it = m_mapChildNode.find(nIndex);
	//if (it != m_mapChildNode.end())
	//	return it->second;
	return nullptr;
}

TangramNode* TangramNode::getChild(const String& strName)
{
	WebString str = strName;
	auto it = m_mapChildNode2.find(str.Utf16());
	if (it != m_mapChildNode2.end())
		return it->second;
	return nullptr;
}

void TangramNode::Trace(blink::Visitor* visitor) {
  EventTargetWithInlineData::Trace(visitor);
  ScriptWrappable::Trace(visitor);
  DOMWindowClient::Trace(visitor);
  visitor->Trace(tangram_);
  visitor->Trace(innerXobj_);
  visitor->Trace(m_pParentForm);
  visitor->Trace(mapTangramEventCallback_);
}

void TangramNode::addEventListener(const String& eventName, V8ApplicationCallback* callback)
{
	if (callback)
	{
		innerXobj_->addEventListener(eventName, callback);
	}
}

void TangramNode::addEventListener(const String& subObjName, const String& eventName, V8ApplicationCallback* callback)
{
	if (callback)
	{
		innerXobj_->addEventListener(subObjName, eventName, callback);
	}
}

void TangramNode::open(const String& strKey, const String& xml, V8ApplicationCallback* callback)
{
	if (m_pRenderframeImpl)
	{
		innerXobj_->setStr(L"senderid", id_);
		String callbackid_ = WTF::CreateCanonicalUUIDString();
		innerXobj_->setStr(L"msgID", L"OPEN_XML");
		innerXobj_->setStr(L"open_callbackid", callbackid_);
		innerXobj_->setStr(L"openkey", strKey);
		innerXobj_->setStr(L"openxml", xml);
		WebString strID = callbackid_;
		m_pRenderframeImpl->m_mapTangramSession[strID.Utf16()] = this;
		if (callback)
		{
			innerXobj_->mapTangramEventCallback_.insert(callbackid_, callback);
		}
		m_pRenderframeImpl->SendTangramMessageEx(innerXobj_->session_);
	}
}

void TangramNode::openEx(const String& strKey, const String& xml, long row, long col, V8ApplicationCallback* callback)
{
	if (m_pRenderframeImpl)
	{
		innerXobj_->setStr(L"senderid", id_);
		innerXobj_->setStr(L"msgID", L"OPEN_XML_SPLITTER");
		innerXobj_->setStr(L"openkey", strKey);
		innerXobj_->setStr(L"openxml", xml);
		innerXobj_->setLong(L"opencol", col);
		innerXobj_->setLong(L"openrow", row);
		String callbackid_ = WTF::CreateCanonicalUUIDString();
		innerXobj_->setStr(L"opencallbackid", callbackid_);
		WebString strID = callbackid_;
		m_pRenderframeImpl->m_mapTangramSession[strID.Utf16()] = this;
		if (callback)
		{
			innerXobj_->mapTangramEventCallback_.insert(callbackid_, callback);
		}
		m_pRenderframeImpl->SendTangramMessageEx(innerXobj_->session_);
	}
}

void TangramNode::SyncCtrlTextChange(const String& strcontrols, V8ApplicationCallback* callback)
{
	if (callback)
	{
		innerXobj_->setStr(L"eventtype", L"SyncCtrlTextChange");
		innerXobj_->setStr(L"ctrls", strcontrols);
		innerXobj_->addEventListener(L"SyncCtrlTextChange", L"OnTextChanged", callback);
	}
}

void TangramNode::fireEvent(const String& eventName, TangramXobj* eventParam)
{
	innerXobj_->fireEvent(eventName, eventParam);
}

void TangramNode::removeEventListener(const String& eventName)
{
	innerXobj_->removeEventListener(eventName);
}

void TangramNode::disConnect()
{
	innerXobj_->disConnect();
}

void TangramNode::sendMessage(TangramXobj* msg, V8ApplicationCallback* callback)
{
	if (m_pRenderframeImpl)
	{
		if (msg == nullptr)
			msg = innerXobj_;
		msg->setStr(L"senderid", id_);
		String callbackid_ = WTF::CreateCanonicalUUIDString();
		msg->setStr(L"callbackid", callbackid_);
		WebString strID = callbackid_;
		m_pRenderframeImpl->m_mapTangramSession[strID.Utf16()] = this;
		if (callback)
		{
			mapTangramEventCallback_.insert(callbackid_, callback);
		}
		m_pRenderframeImpl->SendTangramMessageEx(msg->session_);
	}
}

void TangramNode::invokeCallback(wstring callbackid, TangramXobj* callbackParam)
{
	innerXobj_->invokeCallback(callbackid, callbackParam);
}

void TangramNode::setControlVal(const String& CtrlID, int64_t CtrlHandle, const String& strVal)
{
	int64_t nHandle = 0;
	if (CtrlID != "")
	{
		Element* const tangramelement = DomWindow()->document()->getElementById(WTF::AtomicString(CtrlID));
		if (tangramelement)
		{
			WTF::AtomicString handle = tangramelement->getAttribute("hwnd");
			if (handle != "")
			{
				WebString webstr = handle;
				std::wstring u16_handle = webstr.Utf16();
				nHandle = _wtoi(u16_handle.c_str());
			}
		}
	}
	else
		nHandle = handle_;
	if (nHandle)
	{
		if (m_pRenderframeImpl) {
			WebString webstr = strVal;
			std::wstring _val = webstr.Utf16();
			webstr = CtrlID;
			std::wstring _strCtrlID = webstr.Utf16();
			m_pRenderframeImpl->SendTangramMessage(L"TANGRAM_CTRL_VALUE_MESSAGE", _strCtrlID, nHandle, 0, _val, L"");
		}
	}
}

int64_t TangramNode::hwnd()
{
	return handle_;
}

String TangramNode::objtype() {
	return innerXobj_->getStr(L"objtype");
}

void TangramNode::AddedEventListener(const AtomicString& event_type,
                                 RegisteredEventListener& registered_listener) {
  EventTargetWithInlineData::AddedEventListener(event_type,
                                                registered_listener);
}

void TangramNode::addChannel(const String& channel) {
	sendMessage(L"ADD_CHANNEL", channel, L"", L"", L"", L"");
}

void TangramNode::removeChannel(const String& channel) {
	sendMessage(L"REMOVE_CHANNEL", channel, L"", L"", L"", L"");
}

void TangramNode::sendMessage(const String& id, const String& param1, const String& param2, const String& param3, const String& param4, const String& param5) {
	if (m_pRenderframeImpl) {
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

void TangramNode::GetControlsNames(V8ApplicationCallback* callback)
{

}

int64_t TangramNode::handle() {
	return handle_;
}

TangramNode* TangramNode::parentNode()
{
	__int64 nHandle = innerXobj_->getInt64(L"parentnodehandle");
	if (nHandle)
	{
		auto it = tangram_->m_mapTangramNode.find(nHandle);
		if (it != tangram_->m_mapTangramNode.end())
		{
			return it->value.Get();
		}
	}
	return nullptr;
}

TangramNode* TangramNode::rootNode()
{
	__int64 nHandle = innerXobj_->getInt64(L"rootnodehandle");
	if (nHandle)
	{
		auto it = tangram_->m_mapTangramNode.find(nHandle);
		if (it != tangram_->m_mapTangramNode.end())
		{
			return it->value.Get();
		}
	}
	return nullptr;
}

TangramWindow* TangramNode::parentWindow() {
	__int64 nHandle = innerXobj_->getInt64(L"rootnodehandle");
	auto it = tangram_->m_mapTangramWindow.find(nHandle);
	if (it != tangram_->m_mapTangramWindow.end())
		return it->value.Get();
	return nullptr;
}

TangramNode* TangramNode::AddChild(int64_t nHandle, const String& strNodeName, blink::Tangram* pTangram)
{
	TangramNode* node = nullptr;
	auto it = pTangram->m_mapTangramNode.find(nHandle);
	if (it != pTangram->m_mapTangramNode.end())
	{
		node = it->value;
		int nSize = m_mapChildNode.size();
		m_mapChildNode[nSize] = node;
	}
	return node;
}

const AtomicString& TangramNode::InterfaceName() const {
  return event_target_names::kTangramNode;
}

ExecutionContext* TangramNode::GetExecutionContext() const {
  return DomWindow()->document();
}

}  // namespace blink
// end Add by TangramTeam
