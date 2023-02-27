#ifndef AT_COMMANDS_SMS
#define AT_COMMANDS_SMS

void configure_serial_sms(char* port_name, struct sp_port** port_pointer);

//Envia sms
void send_sms(struct sp_port* port, char *tel, char *msg);

void construct_final_msg_sms(char* final_msg, char* msg);

//Ler os resultados dos comandos AT
void read_serial_sms(struct sp_port* port);

#endif
