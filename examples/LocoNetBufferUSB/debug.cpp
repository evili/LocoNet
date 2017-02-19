#include "debug.h"
#include "LocoNet.h"
#include "LocoNetBufferUSB.h"

Console_State state =   CMD_BEFORE_START;

char cmd[CMD_BUFFER_LEN];

processConsole(char buffer, int len)
{
  uint8_t adr1=0, adr2=0;
  
  int nread = sscanf(buffer, " %s %d %d", cmd, &adr1, &adr2);
  if(nread == EOF)
    return;
  for(int i = 0; i<sizeof(OPC_Map); i++)
  {
    if((OPC_Map[i]->n_args == (nread-1)) &&
       (strncmp(OPC_Map[i]->name, cmd, CMD_BUFFER_LEN) == 0))
      LocoNet.send(OPC_Map[i]->code, adr1, adr2);
  }
}

void getConsoleInput()
{
  while(console.available())
  {
    int rx = console.read();
    switch(state)
    {
    case CMD_BEFORE_START:
      if(START_CMD == rx)
        state = CMD_STARTED;
      break;
    case CMD_STARTED:
      if(END_CMD == rx)
        processConsole(buffer, cmdIndex);
        state = CMD_END;
      break;
    case CMD_STARTED:
      buffer[cmdIndex++] = rx;
      if(cmdIndex>=CMD_BUFFER_LEN)
        cmdIndex--;
    }
    
  }
}
