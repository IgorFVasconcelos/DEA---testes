#ifndef AT_COMMANDS
#define AT_COMMANDS

//Configura a rotina mqtt para enviar dados
void configure_udp_send_routine(struct sp_port* port);

//Loop infinito de enviar publish mqtt 
void start_udp_routine(struct sp_port* port);

void read_serial_udp(struct sp_port* port);

void configure_serial_udp(char *port_name, struct sp_port** port_pointer);

void configure_udp_listen_routine(struct sp_port* port);

void start_udp_routine_listen(struct sp_port* port);

void read_serial_for_listen_udp(struct sp_port* port);

#endif
