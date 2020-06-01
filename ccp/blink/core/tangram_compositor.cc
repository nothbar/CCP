// begin Add by TangramTeam
#include "tangram.h"
#include "tangram_xobj.h"
#include "tangram_node.h"
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

namespace blink {

TangramCompositor::TangramCompositor(LocalFrame* frame) : DOMWindowClient(frame) {
	m_pRenderframeImpl = nullptr;
	id_ = WTF::CreateCanonicalUUIDString();
}

TangramCompositor::~TangramCompositor() {
}

TangramCompositor* TangramCompositor::Create(LocalFrame* frame, const String& strNodeXml) {
	return MakeGarbageCollected<TangramCompositor>(frame, strNodeXml);
}

TangramCompositor::TangramCompositor(LocalFrame* frame, const String& strNodeXml) : DOMWindowClient(frame)
{
	m_strNodeXml = strNodeXml;
}

void TangramCompositor::Trace(blink::Visitor* visitor) {
  EventTargetWithInlineData::Trace(visitor);
  ScriptWrappable::Trace(visitor);
  DOMWindowClient::Trace(visitor);
  visitor->Trace(innerXobj_);
}

void TangramCompositor::AddedEventListener(const AtomicString& event_type,
                                 RegisteredEventListener& registered_listener) {
  EventTargetWithInlineData::AddedEventListener(event_type,
                                                registered_listener);
}

String TangramCompositor::getid()
{
	return id_;
}

String TangramCompositor::name() {
	return name_;
}

void TangramCompositor::addChannel(const String& channel) {
}

void TangramCompositor::removeChannel(const String& channel) {
}

void TangramCompositor::sendMessage(const String& id, const String& param1, const String& param2, const String& param3, const String& param4, const String& param5) {
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

const AtomicString& TangramCompositor::InterfaceName() const {
  return event_target_names::kTangramCompositor;
}

ExecutionContext* TangramCompositor::GetExecutionContext() const {
  return DomWindow()->document();
}

}  // namespace blink
// end Add by TangramTeam
