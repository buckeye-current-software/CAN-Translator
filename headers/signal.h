#ifndef SIGNAL_H_
#define SIGNAL_H_

/*
 *  Holds information needed to decode messages
 *	Data:
 *		1: id - The signal name
 *		2: startBit - Where the signal data in a can_frame starts
 *		3: length - How many bits after the startBit for the signal
 *		4: byteOrder - Endianness (big - 1, little - 0)
 *		5: dataType - signed int, unsigned int, float, or double (1-4)
 *		6: unit - The units for the value to be datalogged
 */

struct signal_structure
{
	char id[50];
	int startBit;
	int length;
	int byteOrder;
	int dataType;
	char unit[25];
};

#endif // SIGNAL_H_
