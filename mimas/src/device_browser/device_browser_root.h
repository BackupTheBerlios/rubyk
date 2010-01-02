#ifndef MIMAS_SRC_DEVICE_BROWSER_DEVICE_BROWSER_ROOT_H_
#define MIMAS_SRC_DEVICE_BROWSER_DEVICE_BROWSER_ROOT_H_
#include "oscit/zeroconf_browser.h"

class MBrowserRoot : public TreeViewItem {
public:
  MBrowserRoot() {
    setOpen(true);
  }

  bool mightContainSubItems() {
    return true;
  }

  int get_item_position(TreeViewItem *item) {
    int sub_items_count = getNumSubItems();
    TreeViewItem *sub_item;
    for(int i=0; i < sub_items_count; ++i) {
      sub_item = getSubItem(i);
      if (sub_item == item) return i;
    }
    return -1;
  }

};

#endif // MIMAS_SRC_DEVICE_BROWSER_DEVICE_BROWSER_ROOT_H_