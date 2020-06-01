// begin Add by TangramTeam
#include "tangram.h"
#include "tangram_xobj.h"
#include "tangram_node.h"
#include "tangram_event.h"
#include "tangram_winform.h"

#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/html/html_element.h"
#include "third_party/blink/renderer/core/html/html_head_element.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_application_callback.h"

namespace blink {

	TangramXobj::TangramXobj() {
		m_pRenderframeImpl = nullptr;
		id_ = WTF::CreateCanonicalUUIDString();
		session_.m_mapString[L"sessionid"] = WebString(id_).Utf16();
	}

	TangramXobj::~TangramXobj() {
		session_.m_mapString.clear();
		session_.m_mapLong.clear();
		session_.m_mapFloat.clear();
		mapTangramEventCallback_.clear();
	}

	TangramXobj* TangramXobj::Create(const String& strNodeXml) {
		return MakeGarbageCollected<TangramXobj>(strNodeXml);
	}

	TangramXobj::TangramXobj(const String& strName)
	{
		name_ = strName;
	}

	void TangramXobj::Trace(blink::Visitor* visitor) {
		ScriptWrappable::Trace(visitor);
		visitor->Trace(mapTangramEventCallback_);
		visitor->Trace(tangram_);
	}

	String TangramXobj::name() {
		return getStr(L"name@page");
	}

	String TangramXobj::getid()
	{
		return id_;
	}

	TangramWinform* TangramXobj::form()
	{
		__int64 nHandle = getInt64(L"formhandle");
		auto it = tangram_->m_mapWinForm.find(nHandle);
		if (it != tangram_->m_mapWinForm.end())
			return it->value.Get();
		return nullptr;
	}

	TangramNode* TangramXobj::node()
	{
		__int64 nHandle = getInt64(L"nodehandle");
		auto it = tangram_->m_mapTangramNode.find(nHandle);
		if (it != tangram_->m_mapTangramNode.end())
			return it->value.Get();
		return nullptr;
	}

	TangramWindow* TangramXobj::window()
	{
		__int64 nHandle = getInt64(L"nodehandle");
		auto it = tangram_->m_mapTangramNode.find(nHandle);
		if (it != tangram_->m_mapTangramNode.end())
		{
			return it->value->parentWindow();
		}
		return nullptr;
	}

	void TangramXobj::setStr(const String& strKey, const String& value)
	{
		WebString str = strKey;
		WebString val = value;
		session_.m_mapString[str.Utf16()] = val.Utf16();
		auto it = session_.m_mapint64.find(WebString(strKey).Utf16());
		if(it!= session_.m_mapint64.end())
		{
			setStr(L"msgID", L"MODIFY_CTRL_VALUE");
			setStr(L"currentsubobjformodify", strKey);
			m_pRenderframeImpl->SendTangramMessageEx(session_);
		}
	}

	String TangramXobj::getStr(const String& strKey)
	{
		WebString str = strKey;
		auto it = session_.m_mapString.find(str.Utf16());
		if(it!= session_.m_mapString.end())
		{
			return String(it->second.c_str());
		}
		return L"";
	}

	void TangramXobj::setLong(const String& strKey, long value)
	{
		WebString str = strKey;
		session_.m_mapLong[str.Utf16()] = value;
	}

	long TangramXobj::getLong(const String& strKey)
	{
		WebString str = strKey;
		auto it = session_.m_mapLong.find(str.Utf16());
		if (it != session_.m_mapLong.end())
		{
			return it->second;
		}
		return 0;
	}

	void TangramXobj::setInt64(const String& strKey, int64_t value)
	{
		WebString str = strKey;
		auto it = session_.m_mapint64.find(str.Utf16());
		if (it != session_.m_mapint64.end())
		{
			session_.m_mapint64.erase(it);
		}
		session_.m_mapint64[str.Utf16()] = value;
	}

	int64_t TangramXobj::getInt64(const String& strKey)
	{
		WebString str = strKey;
		auto it = session_.m_mapint64.find(str.Utf16());
		if (it != session_.m_mapint64.end())
		{
			return it->second;
		}
		return 0;
	}

	void TangramXobj::setFloat(const String& strKey, float value)
	{
		WebString str = strKey;
		session_.m_mapFloat [str.Utf16()] = value;
	}

	float TangramXobj::getFloat(const String& strKey)
	{
		WebString str = strKey;
		auto it = session_.m_mapFloat.find(str.Utf16());
		if (it != session_.m_mapFloat.end())
			return it->second;
		return 0;
	}

	void TangramXobj::addEventListener(const String& eventName, V8ApplicationCallback* callback)
	{
		if (callback)
		{
			auto it = session_.m_mapString.find(L"objID");
			if (it != session_.m_mapString.end())
			{
				if(id_==nullptr)
					id_ = WTF::CreateCanonicalUUIDString();
				tangram_->mapCloudSession_.insert(id_, this);
				//插入callbackID:
				String callbackid_ = WTF::CreateCanonicalUUIDString();
				std::wstring strID = WebString(callbackid_).Utf16();
				setStr(L"callbackid", callbackid_);

				mapTangramEventCallback_.insert(eventName, callback);
				//绑定事件名称与callbackid建立对应关系：
				session_.m_mapString[strID] = WebString(eventName).Utf16();
				session_.m_mapString[L"currentsubobjforlistener"] = L"";

				//允许RenderFrameImpl根据回调id查找对应的session：
				m_pRenderframeImpl->m_mapTangramSession[strID] = this;
				//通知客户端建立监听连接：
				m_pRenderframeImpl->SendTangramMessageEx(session_);
			}
		}
	}

	void TangramXobj::addEventListener(const String& SubObjName, const String& eventName, V8ApplicationCallback* callback)
	{
		if (callback)
		{
			auto it = session_.m_mapString.find(L"objID");
			if (it != session_.m_mapString.end())
			{
				tangram_->mapCloudSession_.insert(id_, this);
				String eventName_ = eventName + "@" + SubObjName;
				auto it = mapTangramEventCallback_.find(eventName_);
				if (it == mapTangramEventCallback_.end())
				{
					mapTangramEventCallback_.insert(eventName_, callback);
					//插入callbackID:
					String callbackid_ = WTF::CreateCanonicalUUIDString();
					std::wstring strID = WebString(callbackid_).Utf16();
					setStr(L"callbackid", callbackid_);
					//绑定事件名称与callbackid建立对应关系：
					session_.m_mapString[strID] = WebString(eventName_).Utf16();

					//允许RenderFrameImpl根据回调id查找对应的session：
					m_pRenderframeImpl->m_mapTangramSession[strID] = this;
				}
				else {
					mapTangramEventCallback_.erase(it);
					mapTangramEventCallback_.insert(eventName_, callback);
				}
				//通知客户端建立监听连接：
				m_pRenderframeImpl->SendTangramMessageEx(session_);
			}
		}
	}

	void TangramXobj::removeEventListener(const String& eventName)
	{
		auto it = mapTangramEventCallback_.find(eventName);
		if (it != mapTangramEventCallback_.end())
			mapTangramEventCallback_.erase(it);
	}

	void TangramXobj::disConnect()
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

	void TangramXobj::fireEvent(const String& eventName, TangramXobj* eventParam)
	{
		auto itcallback = mapTangramEventCallback_.find(eventName);
		if (itcallback != mapTangramEventCallback_.end())
		{
			setStr(L"callbackid", L"");
			blink::V8ApplicationCallback* callback = (blink::V8ApplicationCallback*)itcallback->value.Get();
			ScriptState* callback_relevant_script_state = callback->CallbackRelevantScriptState();
			ScriptState::Scope callback_relevant_context_scope(callback_relevant_script_state);
			callback->InvokeAndReportException(nullptr, eventParam);
		}
	}

	void TangramXobj::sendMessage(TangramXobj* msg, V8ApplicationCallback* callback)
	{
		if (m_pRenderframeImpl)
		{
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

	void TangramXobj::invokeCallback(wstring callbackid, TangramXobj* callbackParam)
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
