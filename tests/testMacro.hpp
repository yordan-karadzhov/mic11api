#include "../src/Macros.h"

#define stateMap1(STATE) \
  STATE(DEFAULT_FAILURE) \
  STATE(DEFAULT_IDLE) \
  STATE(USER_STANDBY(t_Standby)) \
  STATE(USER_ACTIVE(t_Active)) \
  STATE(USER_FATALLERROR(t_Fatal))

IMPLEMENT_STATE_FACTORY(stateMap1, t1_StateFactory)

