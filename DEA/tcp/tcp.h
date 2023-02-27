#ifndef AT_COMMANDS_TCP
#define AT_COMMANDS_TCP

//Configura a rotina mqtt para enviar dados
void configure_tcp_send_routine(struct sp_port* port);

void start_tcp_routine(struct sp_port* port);

void read_serial_tcp(struct sp_port* port);

void configure_serial_tcp(char *port_name, struct sp_port** port_pointer);

void configure_tcp_listen_routine(struct sp_port* port);

void start_tcp_routine_listen(struct sp_port* port);

void read_serial_for_listen_tcp(struct sp_port* port);

#endif
