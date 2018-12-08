#include "libfreenect.h"
#include "libfreenect_sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080


/* This is a simple demo. It connects to the kinect and plays with the motor,
   the accelerometers, and the LED. It doesn't do anything with images. And,
   unlike the other examples, no OpenGL is required!
   So, this should serve as the reference example for working with the motor,
   accelerometers, and LEDs.   */

void no_kinect_quit(void)
{
  printf("Error: Kinect not connected?\n");
  exit(1);
}

int main(int argc, char *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        double dx, dy, dz;
        freenect_raw_tilt_state *state = 0;

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        valread = read( new_socket , buffer, 1024);
        if (valread > 0)
        {
            buffer[ valread ] = 0;
            char *ptr = buffer - 1;
            freenect_led_options led;
            int tilt;

            while(ptr != 0)
            {
                 ptr++;
                 if (strncmp(ptr, "tilt=", 5) == 0)
                 {
                     ptr += 5;
                     tilt = atoi(ptr);
                     printf("Setting tilt to %d degrees\n", tilt);
                     if (freenect_sync_set_tilt_degs(tilt, 0)) no_kinect_quit();
                 }
                 else if (strncmp(ptr, "led=", 4) == 0)
                 {
                     ptr += 4;
                     led = (freenect_led_options) atoi(ptr);
                     printf("Setting led to %d\n", led);
                     if (freenect_sync_set_led(led,0)) no_kinect_quit();
                 }
                 ptr = strchr(ptr, ',');
            }
        }

        // Get the raw accelerometer values and tilt data
        if (freenect_sync_get_tilt_state(&state, 0)) no_kinect_quit();

        // Get the processed accelerometer values (calibrated to gravity)
        freenect_get_mks_accel(state, &dx, &dy, &dz);

        sprintf(buffer, "dx=%lf,dy=%lf,dz=%lf\n\0", dx,dy,dz);
        printf("%s", buffer);
        send(new_socket, buffer, strlen(buffer), 0);
        close(new_socket);
    }
    return 0;

/*
	srand(time(0));

	while (1) {
		// Pick a random tilt and a random LED state
		freenect_led_options led = (freenect_led_options) (rand() % 6); // explicit cast
		int tilt = (rand() % 30)-15;
		freenect_raw_tilt_state *state = 0;
		double dx, dy, dz;

		// Set the LEDs to one of the possible states
		if (freenect_sync_set_led(led, 0)) no_kinect_quit();

		// Set the tilt angle (in degrees)
		if (freenect_sync_set_tilt_degs(tilt, 0)) no_kinect_quit();

		// Get the raw accelerometer values and tilt data
		if (freenect_sync_get_tilt_state(&state, 0)) no_kinect_quit();

		// Get the processed accelerometer values (calibrated to gravity)
		freenect_get_mks_accel(state, &dx, &dy, &dz);

		printf("led[%d] tilt[%d] accel[%lf,%lf,%lf]\n", led, tilt, dx,dy,dz);

		sleep(3);
	}
*/
}

