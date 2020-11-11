void init(char* modulation_mode, unsigned long int frequency, unsigned float signal_bandwidth, bool crcON, unsigned long int watchdog_timer);
void transmit(char* transmit_data_in_hex);
void receive(unsigned int recv_window_size, char* receive_buffer);
