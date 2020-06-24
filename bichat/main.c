#include <MKL25Z4.H>

#define NONE                 "\e[0m"
#define BLACK                "\e[0;30m"
#define L_BLACK              "\e[1;30m"
#define RED                  "\e[0;31m"
#define L_RED                "\e[1;31m"
#define GREEN                "\e[0;32m"
#define L_GREEN              "\e[1;32m"
#define BROWN                "\e[0;33m"
#define YELLOW               "\e[1;33m"
#define BLUE                 "\e[0;34m"
#define L_BLUE               "\e[1;34m"
#define PURPLE               "\e[0;35m"
#define L_PURPLE             "\e[1;35m"
#define CYAN                 "\e[0;36m"
#define L_CYAN               "\e[1;36m"
#define GRAY                 "\e[0;37m"
#define WHITE                "\e[1;37m"

char stack_sent[1000],stack_receive[1000],stack_input[1000],stack_reply[1000];
unsigned int charflag=0,receiveflag=0,inputflag=0,replyflag=0;
int is_transmitting=0;
int is_receiving=0;

char afk_select[100] = "1-afk setting, 2-afk on, 3-name setting, 4-clear, 5-cancel";
int afk_select_length = 58;
char afk_msg[100] = "sorry, I'm busy now.";
int afk_msg_length = 20;
char afk_change[100] = "Please enter new afk Message:";
int afk_change_length = 29;
char afk_app[20] = "[automatic response]";
int afk_app_length = 20;
char name_change[100] = "Please enter your new name:";
int name_change_length = 27;

int afk_mode = 0;					//0:off,1:on

/*
char reply_app = "  Reply: \n-----------------------------------\n";
int reply_app_length = 46;

int quite_mode = 0;					//0:off,1:on
int is_quote_recorded = 0;			//0:0ff,1:on
*/
char username[1000] = "Guest";
int name_length = 5;
int name_length_max = 8;
//int cur_flag = 0;

int input_mode = 0;				//0:input,1:emoji-select
								//2:afk-select,3:afk-input
								//4:name-input
char emoji[40] = "😘😂😇👴👊👀🐸🐶🐎🚀";
int emoji_hash[100];		//hash-table of emoji in input-mode0
int emojiflag = 0;
int emoji_id=-1;


void delay(int n){
	int i=0; 
	while(i<n)
	{
		i=i+1;
	}
}

void UART0_init (void){
	SIM ->SCGC4 |= 0x0400 ; // enable clock for UART0
	SIM ->SOPT2 |= 0x04000000 ; // use MCGFLLCLK output for UART Baud rate generator
	UART0 ->C2 = 0 ; // turn off UART0 while changing configurations */
	UART0 ->BDH = 0x00;
	UART0 ->BDL = 0x17 ; //57600 Baud ?? Please double check
	UART0 ->C4 = 0x0F ; // Over Sampling Ratio 16
	UART0 ->C1 = 0x00 ; // 8 bit data
	UART0 ->C2 = 0b00001100 ; // enable transmit
	SIM ->SCGC5 |= 0x0200; // enable clock for PORTA
	PORTA ->PCR[1] = 0x0200; // make PTA1 UART0_Rx pin
	PORTA ->PCR[2] = 0x0200; // make PTA2 UART0_Tx pin
}

void UART1_init (void){
	SIM ->SCGC4 |= (1u<<11) ; // enable clock for UART0
	SIM ->SOPT2 |= 0x04000000 ; // use MCGFLLCLK output for UART Baud rate generator
	UART1 ->C2 = 0 ; // turn off UART0 while changing configurations */
	UART1 ->BDH = 0x00;
	UART1 ->BDL = 0x17 ; //57600 Baud ?? Please double check
	UART1 ->C4 = 0x0F ; // Over Sampling Ratio 16
	UART1 ->C1 = 0x00 ; // 8 bit data
	UART1 ->C2 = 0b00001100 ; // enable transmit and receive 
	SIM ->SCGC5 |= (1u<<13); // enable clock for PORTA
	PORTE ->PCR[1] = (0b011<<8); // make PTA1 UART0_Rx pin
	PORTE ->PCR[0] = (0b011<<8); // make PTA2 UART0_Tx pin
}

void UART0_Tx (uint8_t data){		//transmit to PC(terminal)
	while (!(UART0 ->S1 & 0x80));
	UART0 ->D = data;
}

uint8_t UART0_Rx ( void ){		//receive from PC(keyboard)
	while (!(UART0 ->S1 & 0x20));
	return UART0 -> D;
}

void UART1_Tx (uint8_t data){	//transmit to remote
	while (!(UART1 ->S1 & 0x80));
	UART1 ->D = data;
}

uint8_t UART1_Rx ( void ){		//receive from remote
	while (!(UART1 ->S1 & 0x20));
	return UART1 -> D;
}

void uparrow2pc(void){
	UART0_Tx(27);
	UART0_Tx(91);
	UART0_Tx(65);
}

void enter2pc(void){
	UART0_Tx('\r');
	UART0_Tx('\n');
}

void bs2pc(void){		//backspace
	UART0_Tx('\b');
	UART0_Tx(' ');
	UART0_Tx('\b');
}

void terminal_erase_back(int n){	//clear chars from the end to beginning
	for(int i=0;i<n;i++){
		UART0_Tx('\b');
		UART0_Tx(' ');
		UART0_Tx('\b');
	}
}

void terminal_erase_forward(int n){	//clear chars from beginning to begninning
	for(int i=0;i<n;i++){
		UART0_Tx(' ');
	}
	for(int i=0;i<n;i++){
		UART0_Tx('\b');
	}
}

void clear_screen(void){
	for(int i=0;i<100;i++){
		enter2pc();
	}
	for(int i=0;i<50;i++){
		uparrow2pc();
	}
}

void change_color_white(){		//change self msg color to white
	char color[7] = WHITE;
	for(int i=0;i<7;i++){
		UART0_Tx(color[i]);
	}
}

void change_color_lred(){		//change self msg color to light red
	char color[7] = L_RED;
	for(int i=0;i<7;i++){
		UART0_Tx(color[i]);
	}
}

void change_color_lblue(){		//change self msg color to light blue
	char color[7] = L_BLUE;
	for(int i=0;i<7;i++){
		UART0_Tx(color[i]);
	}
}

void change_transmit_lred(){	//change transmit msg color to light red
	char color[7] = L_RED;
	for(int i=0;i<7;i++){
		UART1_Tx(color[i]);
	}
}

void change_transmit_white(){	//change transmit msg color to white
	char color[7] = WHITE;
	for(int i=0;i<7;i++){
		UART1_Tx(color[i]);
	}
}

void rewrite(void){				//rewrite sent stack to PC(terminal)
	for(int i=0;i<charflag;i++){
		UART0_Tx(stack_sent[i]);
	}
}

void rewrite_afk_select(){		//rewrite afk-select to PC(terminal)
	for(int i=0;i<afk_select_length;i++){
		UART0_Tx(afk_select[i]);
	}
}

void rewrite_afk_change(){		//rewrite afk-change to PC(terminal)
	for(int i=0;i<afk_change_length;i++){
		UART0_Tx(afk_change[i]);
	}
}

void rewrite_input(){		//rewrite stack-ctrl-input to PC(terminal)
	for(int i=0;i<inputflag;i++){
		UART0_Tx(stack_input[i]);
	}
}

void rewrite_name_change(){
	for(int i=0;i<name_change_length;i++){
		UART0_Tx(name_change[i]);
	}
}

void ctrl_line_reset(void){
	if(input_mode == 3){				//afk-change
		for(int i=0;i<inputflag;i++){
			afk_msg[i] = stack_input[i];
		}
		afk_msg_length = inputflag;
		inputflag = 0;
		input_mode = 0;
		terminal_erase_back(afk_msg_length);
		uparrow2pc();
		terminal_erase_forward(afk_change_length);
		uparrow2pc();
		rewrite();					//rewrite input-line
		return;
	}
	if(input_mode == 4){				//name-change
		if(inputflag>name_length_max){
			terminal_erase_back(inputflag);
			inputflag = 0;
			return;
		}
		terminal_erase_back(inputflag);
		
		for(int i=0;i<inputflag;i++){
			username[i] = stack_input[i];
		}
		name_length = inputflag;
		inputflag = 0;
		uparrow2pc();
		terminal_erase_forward(name_change_length);
		uparrow2pc();
		rewrite();
		input_mode = 0;
		return;
	}
}

void afk_respond(void){			//send afk msgs
	is_transmitting = 1;
	for(int i=0;i<afk_msg_length;i++){	//transmit afk-msg
		UART1_Tx(afk_msg[i]);
		delay(100);				
	}
	for(int i=0;i<afk_app_length;i++){	//transmit auto-response-tip
		UART1_Tx(afk_app[i]);
		delay(100);	
	}
	UART1_Tx('\n');			//end mark
	is_transmitting = 0;
}

void transmit(void){			//transmit sent stack to remote
	is_transmitting = 1;
	change_transmit_lred();		//received name color : light red
	for(int i=0;i<name_length;i++){
		UART1_Tx(username[i]);
		delay(100);
	}
	for(int i=0;i<name_length_max-name_length;i++){
		UART1_Tx(' ');
		delay(100);
	}
	UART1_Tx(':');
	UART1_Tx(' ');
	change_transmit_white();
	for(int i=0;i<charflag;i++){
		UART1_Tx(stack_sent[i]);
		delay(100);
	}
	UART1_Tx('\n');			//end mark
	is_transmitting = 0;
}

void emoji_select(void){
	enter2pc();
	for(int i=0;i<10;i++){
		UART0_Tx((i+1)%10+0x30);
		UART0_Tx(emoji[i*4]);
		UART0_Tx(emoji[i*4+1]);
		UART0_Tx(emoji[i*4+2]);
		UART0_Tx(emoji[i*4+3]);
		UART0_Tx(' ');
	}
	input_mode = 1;
	delay(1000);
}

void ctrl_input(void){		//respond to final process
	if(input_mode == 0){
		return;
	}
	if(!(UART0 ->S1 & 0x20)){	//if receive a letter
		return;
	}
	char input_char;
	input_char = UART0 -> D;
	if(input_mode == 1){					//emoji-select
		if(input_char == '\\'){
			input_mode = 2;					//AFK-select
			terminal_erase_back(40);		//clear emoji line
			rewrite_afk_select();
			delay(100);
			return;
		}
		emoji_id = input_char - 0x31;
		if(emoji_id == -1){
			emoji_id += 10;
		}
		if(emoji_id <0 && emoji_id > 9){	//block other keys
			return;
		}
		terminal_erase_back(40);		//clear emoji line
		uparrow2pc();
		stack_sent[charflag++] = emoji[emoji_id*4];
		stack_sent[charflag++] = emoji[emoji_id*4+1];
		stack_sent[charflag++] = emoji[emoji_id*4+2];
		stack_sent[charflag++] = emoji[emoji_id*4+3];
		emoji_hash[emojiflag++] = charflag;		//store emoji-position to hash-table
		rewrite();
		input_mode = 0;
		return;
	}
	if(input_mode == 2){			//afk-select
		if(input_char == '1'){		//afk-input
			//afk_input
			input_mode = 3;
			terminal_erase_back(afk_select_length);
			for(int i=0;i<afk_change_length;i++){
				UART0_Tx(afk_change[i]);
			}
			enter2pc();
			return;
		}
		if(input_char == '2'){		//afk on
			afk_mode = 1;
			input_mode = 0;
			terminal_erase_back(afk_select_length);
			uparrow2pc();
			rewrite();
			return;
		}
		if(input_char == '3'){		//name input
			input_mode = 4;
			terminal_erase_back(afk_select_length);
			rewrite_name_change();
			enter2pc();
			return;
		}
		if(input_char == '4'){		//clear 
			afk_mode = 0;
			input_mode = 0;
			clear_screen();
			return;
		}
		if(input_char == '5'){		//cancel
			afk_mode = 0;
			input_mode = 0;
			terminal_erase_back(afk_select_length);
			uparrow2pc();
			rewrite();
			return;
		}
		return;
	}
	if(!is_transmitting && !is_receiving){	//input-line
		if(input_char == 27){
			return;
		}
		if(input_char == '\r'){
			if(inputflag==0){			//if nothing cached, skip enter
				return;
			}
			ctrl_line_reset();
			return;
		}
		if(input_char == '\b'){			//-backspace
			if(inputflag==0){			//if nothing cached, skip backspace
				return;
			}
			bs2pc();
			inputflag -= 1;
			return;
		}
		stack_input[inputflag++] = input_char;
		UART0_Tx(input_char);	//echo back
	}
}

void collect(void){				//collect and echo back to PC(terminal)
	if(input_mode != 0){
		return;
	}
	if(!(UART0 ->S1 & 0x20)){	//receive a letter
		return;
	}
	char add_char;
	add_char = UART0 -> D;
	afk_mode = 0;

	if(!is_transmitting && !is_receiving){
		if(add_char == '\r'){			//-enter
			if(charflag == 0){			//if nothing cached, skip transmition
				return;
			}
			transmit();					//transmit
			UART0_Tx('\r');
			change_color_lblue();		//self-username-color : light blue
			for(int i=0;i<name_length;i++){
				UART0_Tx(username[i]);
				delay(100);
			}
			for(int i=0;i<name_length_max-name_length;i++){
				UART0_Tx(' ');
				delay(100);
			}
			UART0_Tx(':');
			UART0_Tx(' ');
			change_color_white();
			rewrite();
			charflag = 0;
			emojiflag = 0;
			enter2pc();
			delay(100);
			return;
		}
		if(add_char == '\b'){			//-backspace
			if(charflag==0){			//if nothing cached, skip backspace
				return;
			}
			if(emojiflag!=0 && emoji_hash[emojiflag]==charflag){ //back delete emoji
				charflag -= 4;		//delete emoji in send stack
				bs2pc();			//delete emoji in pc terminal screen
				bs2pc();
				emojiflag -= 1;
			}
			else{
				charflag -= 1;
				bs2pc();
			}
			
			return;
		}
		if(add_char == '\\'){			//-'\'special function key
			emoji_select();
			input_mode = 1;
			return;
		}
		stack_sent[charflag++] = add_char;
		UART0_Tx(add_char);				//echo back
	}
}

void receive(void){
	if(!(UART1 ->S1 & 0x20)){
		return;
	}
	is_receiving = 1;
	while(1){	//loop until find \r
		char receive_char = UART1_Rx();
		delay(100);
		if(receive_char != '\n'){	//receiving
			stack_receive[receiveflag++] = receive_char;
		}
		else{						//end receiving, print to PC(host/terminal)
			if(input_mode == 0){	//input-mode
				terminal_erase_back(charflag);	//clear currect line
				for(int i=0;i<receiveflag;++i){	//print msgs
					UART0_Tx(stack_receive[i]);
				}
				receiveflag =0;
				enter2pc();
				rewrite();		//rewrite sent stack to PC(host/terminal)
				if(afk_mode){
					afk_respond();
				}
			}
			if(input_mode == 1){			//emoji-select-mode
				terminal_erase_back(40);	//clear emoji line
				uparrow2pc();
				terminal_erase_forward(charflag);
				for(int i=0;i<receiveflag;++i){		//print received msgs
					UART0_Tx(stack_receive[i]);
				}
				enter2pc();
				rewrite();
				receiveflag =0;
				if(afk_mode){
					afk_respond();
				}
				emoji_select();
			}
			if(input_mode == 2){	//afk-select-mode
				terminal_erase_back(afk_select_length);	//erase afk-select-line
				uparrow2pc();							//uparrow2pc	
				terminal_erase_forward(charflag);
				for(int i=0;i<receiveflag;++i){			//print received msgs
					UART0_Tx(stack_receive[i]);
				}
				receiveflag =0;
				//rewrite cached msgs
				enter2pc();
				rewrite();
				enter2pc();
				rewrite_afk_select();
			}
			if(input_mode == 3){	//afk-inputing mode
				terminal_erase_back(inputflag);
				uparrow2pc();
				terminal_erase_forward(afk_change_length);
				uparrow2pc();
				terminal_erase_forward(charflag);
				for(int i=0;i<receiveflag;++i){		//print received msgs
					UART0_Tx(stack_receive[i]);
				}
				receiveflag = 0;
				//rewrite cached msgs
				enter2pc();
				rewrite();
				enter2pc();
				rewrite_afk_change();
				enter2pc();
				rewrite_input();
			}
			if(input_mode == 4){		//name-input mode
				terminal_erase_back(inputflag);
				uparrow2pc();
				terminal_erase_forward(name_change_length);
				uparrow2pc();
				terminal_erase_forward(charflag);
				for(int i=0;i<receiveflag;++i){		//print received msgs
					UART0_Tx(stack_receive[i]);
				}
				receiveflag = 0;
				//rewrite cached msgs
				enter2pc();
				rewrite();
				enter2pc();
				rewrite_name_change();
				enter2pc();
				rewrite_input();
			}
			break;
		}
	}
	is_receiving = 0;
}

int main(){
	UART0_init();		//initialization
	UART1_init();
	clear_screen();
	while(1){			//forever loop
		collect();		//detect input
		receive();		//detect receive
		ctrl_input();	//detect control and functions
	}
	return 0;
}
