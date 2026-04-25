#include "Utils.h"

String shortChipId() {
  uint64_t chipid = ESP.getEfuseMac();
  char buf[10];
  snprintf(buf, sizeof(buf), "%04X", (uint16_t)(chipid & 0xFFFF));
  return String(buf);
}
