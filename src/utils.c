#include "utils.h"

int utils_file_exists(const char* filename)
{
    FILE *file = fopen(filename, "r");

    if (file) 
    {
        fclose(file);
        return 1; 
    }
    return 0;  
}
