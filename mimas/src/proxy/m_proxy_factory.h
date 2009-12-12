#ifndef MIMAS_SRC_PROXY_M_PROXY_FACTORY_
#define MIMAS_SRC_PROXY_M_PROXY_FACTORY_
#include "oscit/proxy_factory.h"
#include "m_root_proxy.h"
#include "m_object_proxy.h"
#include "proxy_view_builder.h"
#include "proxy_view.h"

class MProxyFactory : public ProxyFactory {
public:
  MProxyFactory(Component *main_view) : main_view_(main_view) {}

  virtual RootProxy *build_root_proxy(const Location &end_point) {
    return new MRootProxy(end_point);
  }

  virtual ObjectProxy *build_object_proxy(Object *parent, const std::string &name, const Value &type) {
    if (parent->url() == "" && name == ".views") {
      return new ViewsBuilder(name, type);
    } else if (parent->url() == "/.views" && type.type_id() == H("sss")) {
      // build view
      return new ProxyView(name, type, main_view_);
    } else if (!is_meta_method(name) || parent->url() != "") {
      return new MObjectProxy(name, type);
    } else {
      return NULL;
    }
  }
private:
  Component *main_view_;
};

#endif // MIMAS_SRC_PROXY_M_PROXY_FACTORY_