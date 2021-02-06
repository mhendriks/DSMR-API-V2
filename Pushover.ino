#ifdef USE_PUSHOVER

#include "Pushover.h"

void SetupPushOver() {
  Pushover po = Pushover("avuys4cauaoea18o3qwyvzbmeixfsg","u4i1ycf8rzuvbrdzr75zyykakfzsrw", UNSAFE);
  po.setDevice("X_mh");
  po.setMessage("Testnotification");
  po.setSound("bike");
  Debugln("Pushover:" + po.send()); //should return 1 on success
}

#endif
