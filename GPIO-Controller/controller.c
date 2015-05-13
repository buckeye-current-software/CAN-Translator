#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
        // Check once every second forever
        FILE * signal_file;
	FILE * led_file;
        double result_value;
        while(1)
        {
		// Frame fault check
                char * frame_fault_path = "/home/cancorder/data/FrameFault";
                signal_file = fopen(frame_fault_path, "rb");
                fread(&result_value, sizeof(double), 1, signal_file);
		if(((int)result_value) == 1)
		{
			//execvp(led74_on_args[0],led74_on_args);
			led_file = fopen("/sys/class/gpio/gpio74/value", "a+");
			fprintf(led_file, "1");
			fclose(led_file);
			led_file = fopen("/sys/class/gpio/gpio76/value", "a+");
			fprintf(led_file, "0");
			fclose(led_file);
		}
		else
		{
			//execvp(led74_off_args[0], led74_off_args);
			led_file = fopen("/sys/class/gpio/gpio74/value", "a+");
			fprintf(led_file, "0");
			fclose(led_file);
			led_file = fopen("/sys/class/gpio/gpio76/value", "a+");
			fprintf(led_file, "1");
			fclose(led_file);
		}

		
		sleep(1);
        }
        return 0;
}

