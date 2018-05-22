#ifndef __API_SERIAL_H__
#define __API_SERIAL_H__

#include "serial_config.h"

typedef struct Lserial{

  ErrorType (*open_serial)(char *SerialName, int * const p_file_serial);
  ErrorType (*close_serial)(char *SerilaName, int * const p_file_serial);


}LSerial_Operation;

LSerial_Operation sSerial_Op = {

    open_serial,
    Close_Serial,


};











#endif // __API_SERIAL_H__