#ifndef BED954E2_F4DF_4097_8062_D563D1109188
#define BED954E2_F4DF_4097_8062_D563D1109188

#define PINFO_STATE_STR_MAX_LEN (16)
#define PINFO_NAME_MAX_LEN (16)

struct pinfo {
  int32_t pid;                             ///<- process ID
  int32_t priority;                        ///<- not used
  uint8_t cpuPercent;                      ///<- 0 to 100
  char stateStr[PINFO_NAME_MAX_LEN];       ///<- sting name of state
  char programNameStr[PINFO_NAME_MAX_LEN]; ///<- name of process
};

#endif /* BED954E2_F4DF_4097_8062_D563D1109188 */
