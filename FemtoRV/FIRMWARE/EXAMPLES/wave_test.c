
#include <femtorv32.h>


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
   char ch = receiver_driver(); // [[WITHOUT FPGA BOARD:: ONLY FOR TESTING RECEIVER DRIVER, SHOULD BE INDEPENDENT]]

//   if(ch == (*p-32)) printf("All OK!!"); PRINTF NOT AVAILABLE FOR BARE METAL EMBEDDED CODING ;; but WAVES VERIFY its is fine 
  }

}


int main () 
{

  //Testing the UART Driver 
 char *word = "hello";
 transmitter_driver(word,1);


  while(1) {} 

  return 0;
}



