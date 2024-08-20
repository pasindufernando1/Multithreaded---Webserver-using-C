/*This file will contain all the reusable helper functions that we'll be using when serving the requests*/ 
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

//Function to get the file_type from the file name
const char *get_file_type(const char *file_name){
    const char *file_type = strrchr(file_name, '.');
    if(!file_type || file_type == file_name){
        return "text/plain";
    }

    return file_type + 1;
}

//Get the mimetype from the file extension
const char *getMimeType(const char *file_extension)
{
    if (strcmp(file_extension, "html") == 0 || strcmp(file_extension, "htm") == 0)
    {
        return "text/html";
    }
    else if (strcmp(file_extension, "css") == 0)
        return "text/css";
    else if (strcmp(file_extension, "mp4") == 0)
        return "video/mp4";
    else if (strcmp(file_extension, "js") == 0)
        return "text/javascript";
    else if (strcmp(file_extension, "json") == 0)
        return "application/json";
    else if (strcmp(file_extension, "ico") == 0)
        return "image/vnd.microsoft.icon";
    else if (strcmp(file_extension, "jpg") == 0)
        return "image/jpg";
    else if (strcmp(file_extension, "jpeg") == 0)
        return "image/jpeg";
    else if (strcmp(file_extension, "gif") == 0)
        return "image/gif";
    else if (strcmp(file_extension, "png") == 0)
        return "image/png";
    else if (strcmp(file_extension, "svg") == 0)
        return "image/svg+xml";
    else if (strcmp(file_extension, "mp3") == 0)
        return "audio/mpeg";
    else if (strcmp(file_extension, "txt") == 0)
        return "text/plain";
    else if(strcmp(file_extension, "pdf") == 0)
        return "application/pdf";
    else
        return "application/octet-stream";
}

//Function to open a file and return a descriptor to the file
int openFile(const char *path, const char *file_name)
{
    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s%s", path, file_name);
    int file_descriptor = open(file_path, O_RDONLY);
    return file_descriptor;
}
