#ifndef AT_COMMANDS_MQTT
#define AT_COMMANDS_MQTT

//Configura a rotina mqtt para enviar dados
void configure_mqtt_publish_routine(struct sp_port* port);

//Loop infinito de enviar publish mqtt 
void start_mqtt_routine(struct sp_port* port);

void read_serial_mqtt(struct sp_port* port);

void configure_serial_mqtt(char *port_name, struct sp_port** port_pointer);

void configure_mqtt_subscribe_routine(struct sp_port* port);

void start_mqtt_routine_subscribe(struct sp_port* port);

void read_serial_for_subscribe(struct sp_port* port);

#endif
