//
// Created by atmelfan on 4/1/18.
//

#ifndef SRC_CARRIER_SCPI_H
#define SRC_CARRIER_SCPI_H


#include <stdint.h>
#include <stdbool.h>

#define SCPI_IDN_STR "GPA Robotics, GPA-AIX-323b, 1.0.0"

#define SCPI_END_LIST {.name = NULL}

typedef struct scpi_command_t scpi_command_t;
typedef struct scpi_context_t scpi_context_t;

typedef enum {
    SCPI_SUCCESS = 0,
    SCPI_NOCOMMAND
} scpi_status_t;


struct scpi_command_t{

    /*Name of command*/
    char* name;

    /**
     *
     * @param context
     * @param args
     * @return
     */
    scpi_status_t (*set)(const scpi_context_t* context, char* args);

    /**
     *
     * @param context
     * @param args
     * @return
     */
    scpi_status_t (*get)(const scpi_context_t* context, char* args);

    /**
     * Subcommands
     * Must be terminated with a command where name=NULL
     */
    scpi_command_t* sub;
};

struct scpi_context_t {
    /* Root command */
    scpi_command_t root;

    /* Last command */
    scpi_command_t* last;

    /* Event status */
    uint8_t esr, ese;

    /* Status byte */
    uint8_t _stb, sre;

    /* Output buffer */
    void (*push_out)(char);
    bool (*has_out)(void);

    uint8_t len;
    uint8_t begin, end;

    int16_t errors[0];
};

typedef enum {
    SCPI_NOERROR = 0,

    /* Command errors */
    SCPI_COMMAND_ERROR = -100,
    SCPI_INVALID_CHAR = -101,
    SCPI_SYNTAX_ERROR = -102,
    SCPI_INVALID_SEPARATOR = -103,
    SCPI_DATA_TYPE_ERROR = -104,
    SCPI_GET_NOT_ALLOWED = -105,
    SCPI_PARAMETER_NOT_ALLOWED = -108,
    SCPI_MISSING_PARAMETER = -109,
    SCPI_COMMAND_HEADER_ERROR = -110,
    SCPI_HEADER_SEPERATOR_ERROR = -111,
    SCPI_PROGRAM_HEADER_TOO_LONG = -112,
    SCPI_UNDEFINED_HEADER = -113,
    SCPI_HEADER_SUFFIX_OUTARANGE = -114,
    SCPI_NUMERIC_DATA_ERROR = 120,
    SCPI_INVALID_CHARACTER_IN_NUMBER = -121,
    SCPI_EXPONENT_TOO_LARGE = -123,
    SCPI_TOO_MANY_DIGITS = -124,
    SCPI_NUMERIC_DATA_NOT_ALLOWED = -125,
    SCPI_SUFFIX_ERROR = -130,
    SCPI_INVALID_SUFFIX = -131,
    SCPI_SUFFIX_TOO_LONG = -134,
    SCPI_SUFFIX_NOT_ALLOWED = -138,
    SCPI_CHARACTER_DATA_ERROR = -140,
    SCPI_INVALID_CHARACTER = -141,
    SCPI_CHARACTER_DATA_TOO_LONG = -144,
    SCPI_CHARACTER_DATA_NOT_ALLOWED = -148,
    SCPI_STRING_DATA_ERROR = -150,
    SCPI_INVALID_STRING_DATA = -151,
    SCPI_STRING_DATA_NOT_ALLOWED = -158,
    SCPI_BLOCK_DATA_ERROR = -160,
    SCPI_INVALID_BLOCK_DATA = -161,
    SCPI_BLOCK_DATA_NOT_ALLOWED = -168,
    SCPI_EXPRESSION_ERROR = -170,
    SCPI_INVALID_EXPRESSION = -171,
    SCPI_EXPRESSION_DATA_NOT_ALLOWED = -178,
    SCPI_MACRO_ERROR = -180,
    SCPI_INVALID_OUTSIDE_MACRO_DEF = -181,
    SCPI_INVALID_INSIDE_MACRO_DEF = -183,
    SCPI_MACRO_PARAMETER_ERROR = -184,
    SCPI_WHY_SO_MANY_FUCKING_ERRORS = -190,

    /* Execution errors */
    SCPI_EXECUION_ERROR = -200,
    SCPI_INVALID_WHILE_IN_LOCAL= -201,
    SCPI_SETTINGS_LOST_DUE_TO_RTL = -202,
    SCPI_COMMAND_PROTECTED = -203,
    SCPI_TRIGGER_ERROR = -210,
    SCPI_TRIGGER_IGNORED = -211,
    SCPI_ARM_IGNORED = -212,
    SCPI_INIT_IGNORED = -213,
    SCPI_TRIGGER_DEADLOCK = -214,
    SCPI_ARM_DEADLOCK = -215,
    SCPI_PARAMETER_ERROR = -220,
    SCPI_SETTINGS_CONFLICT = -221,
    SCPI_DATA_OUT_OF_RANGE = -222,
    SCPI_TOO_MUCH_DATA = -223,
    SCPI_ILLEGAL_PARAMETER = -224,
    SCPI_OUT_OF_MEMORY = -225,
    SCPI_LISTS_NOT_SAME_LENGTH = -226,
    SCPI_DATA_CORRUPT_OR_STALE = -230,
    SCPI_DATA_QUESTIONABLE = -231,
    SCPI_INVALID_FORMAT = -232,
    SCPI_INVALID_VERSION = -233,
    SCPI_HARDWARE_ERROR = -240,
    SCPI_HARDWARE_MISSING = -241,
    SCPI_MASS_STORAGE_ERROR = -250,
    SCPI_MISSING_MASS_STORAGE = -251,
    SCPI_MISSING_MEDIA = -252,
    SCPI_CORRUPT_MEDIA = -253,
    SCPI_MEDIA_FULL = -254,
    SCPI_DIRECTORY_FULL = -255,
    SCPI_FILENAME_NOT_FOUND = -256,
    SCPI_FILENAME_ERROR = -257,
    SCPI_MEDIA_PROTECTED = -258,
    SCPI_EXPRESSION_EXECUTION_ERROR = -260,
    SCPI_MATH_ERROR_IN_EXPRESSIOn = -261,
    SCPI_MACRO_EXEC_ERROR = -270,
    SCPI_MACRO_SYNTAX_ERROR = -271,
    SCPI_MACRO_EXECUTION_ERROR = -272,
    SCPI_ILLEGAL_MACRO_LABEL = -273,
    SCPI_MACRO_PARAMETER_EXECUTION_ERROR = -274,
    SCPI_MACRO_DEFINITION_TOO_LONG = -275,
    SCPI_MACRO_RECURSION_ERROR = -276,
    SCPI_MACRO_REDEFINITION_NOT_ALLOWED = -277,
    SCPI_MACRO_HEADER_NOT_FOUND = -278,
    SCPI_PROGRAM_ERROR = -280,
    SCPI_CANNOT_CREATE_PROGRAM = -281,
    SCPI_ILLEGAL_PROGRAM_NAME = -282,
    SCPI_ILLEGAL_VARIABLE_NAME = -283,
    SCPI_PROGRAM_CURRENTLY_RUNNING = -284,
    SCPI_PROGRAM_SYNTAX_ERROR = -285,
    SCPI_PROGRAM_RUNTIME_ERROR = -286,
    SCPI_MEMORY_USE_ERROR = -290,
    SCPI_OUT_OF_PROGRAM_MEMORY = -291,
    SCPI_REFERENCED_NAME_DOES_NOT_EXIST = -292,
    SCPI_REFERENCED_NAME_ALREADY_EXISTS = -293,
    SCPI_INCOMPATIBLE_TYPE = -294,

    /* Device specific errors */
    SCPI_DEFICE_SPECIFIC_ERROR = -300,
    SCPI_SYSTEM_ERROR = -310,
    SCPI_MEMORY_ERROR = -311,
    SCPI_PUD_MEMORY_LOST = -312,
    SCPI_CALIBRATION_LOST = -313,
    SCPI_SAVE_RECALL_MEMORY_LOST = -314,
    SCPI_CONFIGURATION_MEMORY_LOST = -315,
    SCPI_STORAGE_FAULT = -320,
    SCPI_OUT_OF_INTERNAL_MEMORY = -321,
    SCPI_SELF_TEST_FAILED = -330,
    SCPI_CALIBRATION_FAILED = -340,
    SCPI_QUEUE_OVERFLOW = -350,
    SCPI_COMMUNICATION_ERROR = -360,
    SCPI_PARITY_ERROR = -361,
    SCPI_FRAMING_ERROR = -362,
    SCPI_INPUT_BUFFER_OVERFLOW = -363,

    /* Query errors */
    SCPI_QUERY_ERROR = -400,
    SCPI_QUERY_INTERRUPTED = -410,
    SCPI_QUERY_UNTERMINATED = -420,
    SCPI_QUERY_DEADLOCKED = -430,
    SCPI_QUERY_UNTERMINATED2 = -440,

    /* Other errors */
    SCPI_POWER_ON = -500, //(7)
    SCPI_USER_REQUEST = -600, //(6)
    SCPI_REQUEST_CONTROL = -700, //(1)
    SCPI_OPERATION_COMPLETE = -800 //(0)
} scpi_error_e;

uint8_t scpi_get_stb(scpi_context_t *context);

bool scpi_push_error(scpi_context_t *context, scpi_error_e err);

void scpi_init(scpi_context_t *context, const scpi_command_t *commands, void (*write)(char));

void scpi_puts(const scpi_context_t *context, const char *s);

void scpi_execute(scpi_context_t* context, char* s, char* buf);

#endif //SRC_CARRIER_SCPI_H
