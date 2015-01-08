#ifndef PARSER_H_
#define PARSER_H_

/* Parse through a .dbc file and convert all the information into meaningful data
 *
 * Stores all the signals found into a both a signal tree and message tree. The
 * information stored here will later be used when translating a incoming CAN
 * message
 *
 * NOTE: CAN NOT HAVE EMPTY MESSAGES IN THE .DBC FILE!!!
 *
 * Params:
 * 		fileName - filename of .dbc file to parse
 */
void parseFile(char *fileName);

#endif //PARSER_H_
