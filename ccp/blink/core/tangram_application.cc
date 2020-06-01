// begin Add by TangramTeam
#include "tangram.h"
#include "tangram_xobj.h"
#include "tangram_node.h"
#include "tangram_event.h"
#include "tangram_winform.h"
#include "tangram_application.h"

#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/html/html_element.h"
#include "third_party/blink/renderer/core/html/html_head_element.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"

namespace blink {

	TangramApplication::TangramApplication(LocalFrame* frame) : DOMWindowClient(frame) {
		web_local_frame_client = nullptr;
		innerXobj_ = TangramXobj::Create(L"");
	}

	TangramApplication::~TangramApplication() {
	}

	TangramApplication* TangramApplication::Create(LocalFrame* frame, const String& strNodeXml) {
		return MakeGarbageCollected<TangramApplication>(frame, strNodeXml);
	}

	TangramApplication::TangramApplication(LocalFrame* frame, const String& strNodeXml) : DOMWindowClient(frame)
	{
		name_ = strNodeXml;
	}

	void TangramApplication::Trace(blink::Visitor* visitor) {
		EventTargetWithInlineData::Trace(visitor);
		ScriptWrappable::Trace(visitor);
		DOMWindowClient::Trace(visitor);
		visitor->Trace(innerXobj_);
	}

	void TangramApplication::AddedEventListener(const AtomicString& event_type,
		RegisteredEventListener& registered_listener) {
		EventTargetWithInlineData::AddedEventListener(event_type,
			registered_listener);
	}

	String TangramApplication::name() {
		return name_;
	}

	void TangramApplication::addChannel(const String& channel) {
	}

	void TangramApplication::removeChannel(const String& channel) {
	}

	void TangramApplication::sendMessage(const String& id, const String& param1, const String& param2, const String& param3, const String& param4, const String& param5) {
		WebLocalFrameImpl* web_local_frame_impl = WebLocalFrameImpl::FromFrame(GetFrame());
		// Null when opening a new tab.
		if (web_local_frame_impl != nullptr) {
			WebLocalFrameClient* web_local_frame_client = web_local_frame_impl->Client();
			if (web_local_frame_client) {
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
				web_local_frame_client->SendTangramMessage(u16_id, u16_param1, u16_param2, u16_param3, u16_param4, u16_param5);
			}
		}
	}

	const AtomicString& TangramApplication::InterfaceName() const {
		return event_target_names::kTangramApplication;
	}

	ExecutionContext* TangramApplication::GetExecutionContext() const {
		return DomWindow()->document();
	}

}  // namespace blink
// end Add by TangramTeam
