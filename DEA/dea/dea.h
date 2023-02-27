#include <stdint.h>

//Configura a rotina mqtt para enviar dados do DEA
void configure_dea_publish_routine(struct sp_port* port);

//Loop infinito de enviar publish mqtt 
void start_dea_routine(struct sp_port* port);

//Ler os dados vindos da porta serial o modem
void read_serial_dea(struct sp_port* port);

//Configura a porta serial para envia e receber dados do momdem
void configure_serial_dea(char* port_name, struct sp_port** port_pointer);

// Configura modem para enviar dados do DEA
void configure_dea_subscribe_routine(struct sp_port* port);

//Configura modem para enviar dados do DEA
void start_dea_routine_subscribe(struct sp_port* port);

// Ler os dados recebidos do servidor no modo subscrito
void read_dea_serial_for_subscribe(struct sp_port* port);

//Envia os dados do GPS ou AutoTest para o servidor MqTT
void start_dea_gps_autotest(uint8_t* buffer, uint16_t buffer_size);

void read_serial_mqtt(struct sp_port* port);

void configure_serial_mqtt(char* port_name, struct sp_port** port_pointer);

