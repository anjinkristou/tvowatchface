#include "pebble_app_info.h"
#include "src/resource_ids.auto.h"

const PebbleAppInfo __pbl_app_info __attribute__ ((section (".pbl_header"))) = {
  .header = "PBLAPP",
  .struct_version = { APP_INFO_CURRENT_STRUCT_VERSION_MAJOR, APP_INFO_CURRENT_STRUCT_VERSION_MINOR },
  .sdk_version = { APP_INFO_CURRENT_SDK_VERSION_MAJOR, APP_INFO_CURRENT_SDK_VERSION_MINOR },
  .app_version = { 0, 1 },
  .load_size = 0xb6b6,
  .offset = 0xb6b6b6b6,
  .crc = 0xb6b6b6b6,
  .name = "TVO",
  .company = "TVO",
  .icon_resource_id = DEFAULT_MENU_ICON,
  .sym_table_addr = 0xA7A7A7A7,
  .flags = APP_INFO_WATCH_FACE,
  .num_reloc_entries = 0xdeadcafe,
  .uuid = { 0xEF, 0xEA, 0x6B, 0xE1, 0xFE, 0xCA, 0x4F, 0x10, 0xBF, 0x97, 0xA3, 0x96, 0xB4, 0x25, 0xF3, 0xF6 },
  .virtual_size = 0xb6b6
};
