#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_SIZE 50
#define SENSOR_SIMULATOR_SLEEP_TIME_S 1 
#define PRINTER_SLEEP_TIME_S 10


// STRUCT TO HOLD THE CIRCULAR BUFFER VALUES
typedef struct {
    unsigned char data[BUFFER_SIZE];
    int head; // INDEX TO ADD THE NEXT BYTE
    int tail; // INDEX TO READ FROM
    pthread_mutex_t mutex; // FOR CONCORRUCT DATA HANDLING
} CircularBuffer; 

CircularBuffer global_buffer; // Global circular buffer

// Function prototypes 
void circular_buffer_init(CircularBuffer *cb);
bool circular_buffer_write(CircularBuffer *cb, unsigned char data);
bool circular_buffer_read(CircularBuffer *cb, unsigned char *data);
void generate_random_bytes(unsigned char *data_buffer, int size);
void *sensor_simulator_thread();
void *data_printer_thread();   

int main() {
    srand(time(NULL)); // GIVING NULL SEED to random function

            circular_buffer_init(&global_buffer); 
    pthread_mutex_init(&global_buffer.mutex, NULL);

        pthread_t sensor_thread, printer_thread;
    pthread_create(&sensor_thread, NULL, sensor_simulator_thread, NULL); 
     pthread_create(&printer_thread, NULL, data_printer_thread, NULL);     

       pthread_join(sensor_thread, NULL);
    pthread_join(printer_thread, NULL);

    return 0;
}

void circular_buffer_init(CircularBuffer *cb) {
    cb->head = 0;
    cb->tail = 0;
}

bool circular_buffer_write(CircularBuffer *cb, unsigned char data) { 
    // INCREASE HEAD BY 1 IN WRITE function
    int next_head = (cb->head + 1) % BUFFER_SIZE;
           if (next_head == cb->tail) {  
               cb->tail = (cb->tail + 1) % BUFFER_SIZE;  
    }

        cb->data[cb->head] = data;
         cb->head = next_head;
    return true;
}

bool circular_buffer_read(CircularBuffer *cb, unsigned char *data) {
    
    // INCREASE TAIL BY ONE IN READ function
    if (cb->head == cb->tail) {
        return false;
    }

    *data = cb->data[cb->tail];
          cb->tail = (cb->tail + 1) % BUFFER_SIZE;
    return true;
}

void generate_random_bytes(unsigned char *data_buffer, int size) {
              for (int i = 0; i < size; i++) {
          data_buffer[i] = rand() % 256;
    }
}

void *sensor_simulator_thread() { 
    while (1) {
        int random_arr_size = rand() % 5 + 1;
        unsigned char new_random_data[random_arr_size];
        generate_random_bytes(new_random_data, random_arr_size);
        
        //printf("the random array size is %d \n",random_arr_size );

        pthread_mutex_lock(&global_buffer.mutex); 
        printf("Sensor generating %d bytes: ", random_arr_size);
        for (int i = 0; i < random_arr_size; i++) {
            printf("%02X ", new_random_data[i]);
                    circular_buffer_write(&global_buffer, new_random_data[i]); 
        }
        printf("\n");
                 pthread_mutex_unlock(&global_buffer.mutex);
        sleep(SENSOR_SIMULATOR_SLEEP_TIME_S);
    }
    return NULL;
}

void *data_printer_thread() { 
    
    
    while (1) {
          sleep(PRINTER_SLEEP_TIME_S);
          pthread_mutex_lock(&global_buffer.mutex); 
        int data_available;
        printf("head is %d, tail is :%d",global_buffer.head , global_buffer.tail);
        if (global_buffer.head < global_buffer.tail) {
            data_available = (global_buffer.head + BUFFER_SIZE) - global_buffer.tail + 1;
        } else {
            data_available = global_buffer.head - global_buffer.tail;
        }
 // CHECK IF THE BUFFER IS FULL OR NOT 
        if (data_available == BUFFER_SIZE) {
            printf("Printing entire buffer (%d bytes):\n", BUFFER_SIZE);
            for (int i = 0; i < BUFFER_SIZE; i++) {
                int index = (global_buffer.tail + i -1) % BUFFER_SIZE;
                printf("%02X ", global_buffer.data[index]);
            }
            printf("\n");
            global_buffer.tail = (global_buffer.tail + BUFFER_SIZE) % BUFFER_SIZE;
        } else {
            printf("NOT ENOUGH DATA. Current size is %d\n", data_available);
        }

        pthread_mutex_unlock(&global_buffer.mutex);
    }
    return NULL;
}