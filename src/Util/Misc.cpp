#include "Misc.h"

Misc::Misc()
{
  //PAPI related
  eventList=NULL;
  eventSet=0;
}

Misc::~Misc()
{
  if (eventList) {
    delete [] eventList;
    eventList = NULL;
  }
} 

int Misc::compare_one_int(Db* dbp, const Dbt* a, const Dbt* b)
{
  int ai = *((int*)a->get_data());
  int bi = *((int*)b->get_data());
  return (ai - bi);
}

#ifdef CSTORE_PAPI

int Misc::init_PAPI()
{
  this->eventSet=PAPI_NULL;

  this->eventList = new unsigned int[9];
  this->eventList[0] = PAPI_TOT_CYC;
  this->eventList[1] = PAPI_TOT_INS;
  this->eventList[2] = PAPI_TLB_TL;
  this->eventList[3] = PAPI_BR_MSP;
  this->eventList[4] = PAPI_L1_DCM;
  this->eventList[5] = PAPI_L1_ICM;
  this->eventList[6] = PAPI_L2_TCM;
  this->eventList[7] = 0x40270001;
  this->eventList[8] = 0;

  int retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT) {
    std::cout << "*** PAPI_library_init failed! *** error:" << retval << std::endl;
    exit (1);
  }
  if ((retval = PAPI_create_eventset(&this->eventSet)) != PAPI_OK) {
    std::cout << "*** PAPI_create_eventset failed! *** error:" << retval << std::endl;
    exit (1);
  }
  return PAPI_OK;
}

void Misc::setup_PAPI_event()
{
  for (int i = 0; this->eventList[i] != 0; i++)
  {
    if (PAPI_query_event(this->eventList[i]) == PAPI_OK) {
      eventInfo[eventList[i]] = 0;
    }
  }
  /*
  int numCounters = 0;
  if ((numCounters = PAPI_get_opt(PAPI_MAX_HWCTRS,NULL)) > 0) {
    std::cout << "\t total " << numCounters << " counter(s) available!" << std::endl;
  }
  */
  char descr[PAPI_MAX_STR_LEN];
  std::map<unsigned int, long_long>::iterator iter;
  for (iter = this->eventInfo.begin(); iter != this->eventInfo.end(); ++iter) {
    PAPI_event_code_to_name((*iter).first, descr);
    PAPI_add_event(this->eventSet, (*iter).first);
    PAPI_event_code_to_name((*iter).first, descr);
    // std::cout << "added " << "event " << descr << " " << (*iter).first << std::endl;
  }
}

void Misc::start_PAPI_event()
{
  int retval = 0;
  if ( ( (retval = PAPI_start(this->eventSet) ) ) != PAPI_OK ) {
    std::cout << "*** PAPI_start failed: " << retval << std::endl;
  }
  if ( ( (retval = PAPI_reset(this->eventSet) ) ) != PAPI_OK ) {
    std::cout << "*** PAPI_reset failed: " << retval << std::endl;
  }
}

void Misc::read_PAPI_event()
{
  if (this->eventInfo.size() > 0) {
    int retval = 0;
    long_long* values = new long_long[eventInfo.size()];
    if (retval = PAPI_read(this->eventSet, values) != PAPI_OK) {
      std::cout << "*** PAPI_read failed! *** error:" << retval << std::endl;
      exit(1);
    }
    /*
    for (unsigned int k=0; k <eventInfo.size(); ++k) {
      std::cout << "=>" << values[k] << std::endl;
    }
    */
    std::map<unsigned int, long_long>::iterator iter;
    int i = 0;
    for (iter = eventInfo.begin(); iter != eventInfo.end(); ++iter) {
      (*iter).second = values[i];
      ++i;
    }
    delete [] values;
  } 
}

void Misc::print_PAPI_event()
{
  char descr[PAPI_MAX_STR_LEN];
  PAPI_event_info_t evinfo;

  std::cout << std::endl;
  std::map<unsigned int, long_long>::iterator iter;
  for (iter = eventInfo.begin(); iter != eventInfo.end(); ++iter) {
    PAPI_event_code_to_name((*iter).first, descr);
    if (PAPI_get_event_info((*iter).first, &evinfo) != PAPI_OK) {
      std::cout << "*** PAPI_get_event_info failed! *** error!" << std::endl;
      exit(1);
    }
    std::cout.setf(std::ios::left, std::ios::adjustfield);
    std::cout.width(15);
    std::cout << evinfo.symbol << ' ';
    std::cout.setf(std::ios::left, std::ios::adjustfield);
    std::cout.width(60);
    std::cout << evinfo.long_descr << ' ';
    std::cout.setf(std::ios::right, std::ios::adjustfield);
    std::cout.width(20);
    std::cout << (*iter).second << std::endl;
  }
}

long_long Misc::get_usec()
{
  return PAPI_get_real_usec();
}

long_long Misc::get_cycle()
{
  return PAPI_get_real_cyc();
}

#endif
