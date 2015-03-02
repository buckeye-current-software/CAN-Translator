#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
        // Check once every second forever
        FILE * signal_file;
	FILE * led_file;
        double result_value[2];
	double min_voltage;

	char * bim_1_path = "/home/cancorder/data/BIM1MinCellVoltage";
	char * bim_2_path = "/home/cancorder/data/BIM2MinCellVoltage";
	char * bim_3_path = "/home/cancorder/data/BIM3MinCellVoltage";
	char * bim_4_path = "/home/cancorder/data/BIM4MinCellVoltage";
	char * bim_5_path = "/home/cancorder/data/BIM5MinCellVoltage";
	int bim1_last_time = 0;
	int bim1_count = 0;
	int bim2_last_time = 0;;
 	int bim2_count = 0;
	int bim3_last_time = 0;
	int bim3_count = 0;
	int bim4_last_time = 0;
	int bim4_count = 0;
	int bim5_last_time = 0;
	int bim5_count = 0;
	time_t startTime = time(0);

	int bytes_read = 0;
        while(1)
        {

		// Lowest cell voltage check
		// Triggers on 3v
		signal_file = fopen(bim_1_path, "rb");
		bytes_read = fread(result_value, sizeof(double), 2, signal_file);
		fclose(signal_file);
		if(bim1_last_time == (int)result_value[0] || bytes_read != 2)
		{
			bim1_count++;
		}
		else
		{
			printf("Reset count\n");
			bim1_count = 0;
		}
		bim1_last_time = result_value[0];
		min_voltage = result_value[1];

		signal_file = fopen(bim_2_path, "rb");
		bytes_read = fread(result_value, sizeof(double), 2, signal_file);
		fclose(signal_file);
		if(bim2_last_time == (int)result_value[0] || bytes_read != 2)
		{
			bim2_count++;
		}
		else
		{
			bim2_count = 0;
		}
		bim2_last_time = result_value[0];

		if(result_value[1] < min_voltage)
		{
			min_voltage = result_value[1];
		}

		signal_file = fopen(bim_3_path, "rb");
		bytes_read = fread(result_value, sizeof(double), 2, signal_file);
		fclose(signal_file);
		if(bim3_last_time == (int)result_value[0] || bytes_read != 2)
		{
			bim3_count++;
		}
		else
		{
			bim3_count = 0;
		}
		bim3_last_time = result_value[0];
		if(result_value[1] < min_voltage)
		{
			min_voltage = result_value[1];
		}


		signal_file = fopen(bim_4_path, "rb");;
		bytes_read = fread(result_value, sizeof(double), 2, signal_file);
		fclose(signal_file);
		if(bim4_last_time == (int)result_value[0] || bytes_read != 2)
		{
			bim4_count++;
		}
		else
		{
			bim4_count = 0;
		}
		bim4_last_time = result_value[0];

		if(result_value[1] < min_voltage)
		{
			min_voltage = result_value[1];
		}

		signal_file = fopen(bim_5_path, "rb");
		bytes_read = fread(result_value, sizeof(double), 2, signal_file);
		fclose(signal_file);
		if(bim5_last_time == (int)result_value[0] || bytes_read != 2)
		{
			bim5_count++;
		}
		else
		{
			bim5_count = 0;
		}
		bim5_last_time = result_value[0];
		if(result_value[1] < min_voltage)
		{
			min_voltage = result_value[1];
		}

		if(min_voltage <= 3000)
		{
			led_file = fopen("/sys/class/gpio/gpio44/value", "a+");
			fprintf(led_file, "1");
			fclose(led_file);
			led_file = fopen("/sys/class/gpio/gpio26/value", "a+");
			fprintf(led_file, "1");
			fclose(led_file);
		}
		else
		{
			led_file = fopen("/sys/class/gpio/gpio26/value", "a+");
			fprintf(led_file, "0");
			fclose(led_file);
		}
		
		if ((bim1_count >= 5 || bim2_count >= 5
				|| bim3_count >= 5 || bim4_count >= 5 || bim5_count >= 5))
		{
			led_file = fopen("/sys/class/gpio/gpio44/value", "a+");
			fprintf(led_file, "0");
			fclose(led_file);
			led_file = fopen("/sys/class/gpio/gpio26/value", "a+");
			fprintf(led_file, "1");
			fclose(led_file);
		}
		
		// Frame fault check
                char * frame_fault_path = "/home/cancorder/data/FrameFault";
                signal_file = fopen(frame_fault_path, "rb");
                fread(result_value, sizeof(double), 2, signal_file);
		fclose(signal_file);
		if(((int)result_value[1]) == 1)
		{
			//execvp(led74_on_args[0],led74_on_args);
			led_file = fopen("/sys/class/gpio/gpio68/value", "a+");
			fprintf(led_file, "1");
			fclose(led_file);
			led_file = fopen("/sys/class/gpio/gpio67/value", "a+");
			fprintf(led_file, "0");
			fclose(led_file);
		}
		else
		{
			//execvp(led74_off_args[0], led74_off_args);
			led_file = fopen("/sys/class/gpio/gpio68/value", "a+");
			fprintf(led_file, "0");
			fclose(led_file);
			led_file = fopen("/sys/class/gpio/gpio67/value", "a+");
			fprintf(led_file, "1");
			fclose(led_file);
		}
		min_voltage = 0;
		usleep(500000);
        }
        return 0;
}

