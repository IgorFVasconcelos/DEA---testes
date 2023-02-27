//Inicia o tratamento de dados do gps
void start_gps_routine(struct sp_port* port);
//Loop infinito paralelo para leitura dos dados do GPS
void read_serial_gps(struct sp_port* port);
//Configura o hardware para leitura da serial (baud rate, )
void configure_serial_gps(char* port_name, struct sp_port** port_pointer);
