
#include <femtorv32.h>

#define write_csr(reg, val) asm volatile ("csrw " #reg ", %0" :: "r"(val))

#define PUMP_ON *(volatile uint32_t*)(IO_BASE + IO_BIT_TO_OFFSET(13))
#define PUMP_ALARM *(volatile uint32_t*)(IO_BASE + IO_BIT_TO_OFFSET(14))
#define PUMP_PRESSURE *(volatile uint32_t*)(IO_BASE + IO_BIT_TO_OFFSET(15))

#define BUFFER_SIZE 8 

//Ring buffer 
char rx_buffer[BUFFER_SIZE]; 
uint8_t write_index = 0 ; 
uint8_t read_index = 0; 

uint32_t timeout = 0;

typedef enum {IDLE, RAMP_UP, ACTIVE , ERROR} e_State_machine_states; 

e_State_machine_states current_state = IDLE; 

int strlength(char *p) {
  int i=0;
  while(*p != '\0') {
    i++;
    p++;
  }
  return i;
}

char receiver_driver() {
  return IO_IN(IO_UART_DAT);
}

void transmitter_driver(char *word, int parity_or_not) {
  *(volatile uint32_t*)(IO_BASE + IO_BIT_TO_OFFSET(12)) = parity_or_not; //Will translate to STORE instruction in RISC-V

  int N = strlength(word);
  for(char *p = word; p < (word + N); p++) {
   IO_OUT(IO_UART_DAT,(*p - 32));  // Maybe takes 2-3 cycles ;; enough for asserting busy 
  //Constantly Polling the Tx serial busy 
   while(IO_IN(IO_UART_DAT) & (1 << 9)){} 
   //char ch = receiver_driver(); // [[WITHOUT FPGA BOARD:: ONLY FOR TESTING RECEIVER DRIVER, SHOULD BE INDEPENDENT]]

//   if(ch == (*p-32)) printf("All OK!!"); PRINTF NOT AVAILABLE FOR BARE METAL EMBEDDED CODING ;; but WAVES VERIFY its is fine 
  }

}

__attribute__((interrupt("machine"), aligned(4)))
void uart_isr(void) { // This ISR was called on interrupt of valid ,, so no need to check if the information of receiver_driver is valid or not 
  rx_buffer[write_index] = (uint8_t)receiver_driver(); // rx_buffer is on the stack with new information [Will take last 8 bits]
  write_index = (write_index + 1) & (BUFFER_SIZE - 1); // stop write_index overflow 
}

char* get_uart_characters(void) {
  if(read_index != write_index) {
    if(write_index >= 4) {
      int i = 0; 
      static char readData[5];
      while (i < 5) {
        readData[i] = rx_buffer[read_index]; 
        read_index = (read_index+1) & (BUFFER_SIZE-1);
        i++;
      }
      return readData;
    }
  } 
  return 0x00;
}

int compare(char* check, char* base) {
  if (strlength(check) != strlength(base)) return 0; 
  for(int i = 0 , n = strlength(check) ; i < n ; i++) {
    if(check[i] != base[i]) return 0;
  }
  return 1;
}

void actuator_machine(void) {

  switch(current_state) {
    case IDLE     :{ 
                     if(compare(get_uart_characters(),"START")) {
                       current_state = RAMP_UP;               // Change state 
                       PUMP_ON = 1;  // Turn on the pump
                     }
                     break; 
                   } 
    case RAMP_UP  :{ 
                     if(++timeout >= 800) { 
                       PUMP_ON = 0;
                       PUMP_ALARM = 1;
                       current_state = ERROR;
                       timeout = 0;
                     }
                     if(PUMP_PRESSURE) {
                       PUMP_ON = 0;
                       current_state = ACTIVE;  
                       timeout = 0; 
                     }
                     break; 
                   } 
    case ACTIVE   :{break;} 
    case ERROR    :{break;} 
  }
}

int main () 
{
write_csr(mstatus,(1<<3)); // Enabling the mstatus :: Interrupts
write_csr(mtvec,uart_isr); // Setting up the UART Interrupt address on mtvec 
  //Testing the UART Driver 
 char *word = "start";
 transmitter_driver(word,1);

  while(1) {
    actuator_machine(); 
  } 

  return 0;
}



