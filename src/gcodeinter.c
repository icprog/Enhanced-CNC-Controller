#include <string.h>
#include "gcodeinter.h"
#include "cnc-controller.h"
#define MAX_F 100.0
#define MAX_A 1
#define X_STEP_LENGTH 1.0
#define Y_STEP_LENGTH 1.0
#define Z_STEP_LENGTH 1.0
int abs_mode = 1;
float curr_x = 0;
float curr_y = 0;
float curr_z = 0;
float curr_v = 0;
float atof(const char* s){
    float rez = 0, fact = 1;
    if (*s == '-'){
        s++;
        fact = -1;
    };
    for (int point_seen = 0; *s; s++){
        if (*s == '.'){
            point_seen = 1; 
            continue;
        };
    int d = *s - '0';
    if (d >= 0 && d <= 9){
        if (point_seen) fact /= 10.0f;
        rez = rez * 10.0f + (float)d;
        };
    };
    return rez * fact;
};
void line_move(uint32_t gnum, char gcode[], struct Exist *exist){
    struct Vector v;
    int t = strlen(gcode);
    char tmp;

    for (int i=strlen(gcode)-1; i>=1; i--){
        if ((gcode[i]<48 || gcode[i]>57) && gcode[i]!='.'){
            tmp = gcode[t];
            gcode[t] = '\0';
            if(gcode[i] == 'F')v.f = atof(gcode+i+1);
            else if(gcode[i] == 'Z')v.z = atof(gcode+i+1);
            else if(gcode[i] == 'Y')v.y = atof(gcode+i+1);
            else if(gcode[i] == 'X')v.x = atof(gcode+i+1);
            
            gcode[t] = tmp;
            t = i;
        }
    }
    if (abs_mode){
        v.x = v.x - curr_x;    
        v.y = v.y - curr_y;
        v.z = v.z - curr_z;
    } 
    if(!exist->x)v.x = 0;
    if(!exist->y)v.y = 0;
    if(!exist->z)v.z = 0;
    
    if(!gnum){
        v.f = MAX_F;
    }else{
        if(!exist->f)
            v.f = curr_v;
    }
    if(v.f != curr_v){
        CNC_SetFeedrate(v.f);
        curr_v = v.f;
    }
    CNC_Move((int)(v.x/X_STEP_LENGTH), (int)(v.y/Y_STEP_LENGTH), (int)(v.z/Z_STEP_LENGTH));
     
}/*
void G02(char gcode[], struct Exist *exist){
    struct XYZ_Vector v;
    float vi, vj, vk, R, i_pos, j_pos, x_pos, y_pos;   
    char tmp;
    for (int i=strlen(gcode)-1; i>=1; i--){
        if ((gcode[i]<48 || gcode[i]>57) && gcode[i]!='.'){
            tmp = gcode[t];
            gcode[t] = '\0';
            printf("%s ", gcode+i+1);
            if(gcode[i] == 'F')v.f = atof(gcode+i+1);
            else if(gcode[i] == 'Z')v.z = atof(gcode+i+1);
            else if(gcode[i] == 'Y')v.y = atof(gcode+i+1);
            else if(gcode[i] == 'X')v.x = atof(gcode+i+1);
            else if(gcode[i] == 'K')vk = atof(gcode+i+1);
            else if(gcode[i] == 'J')vj = atof(gcode+i+1);
            else if(gcode[i] == 'I')vi = atof(gcode+i+1);
            else if(gcode[i] == 'R')R = atof(gcode+i+1);
            gcode[t] = tmp;
            t = i;
        }
    }
    
    if (abs_mode){
        v.x = v.x - curr_x;    
        v.y = v.y - curr_y;
        v.z = v.z - curr_z;
    }
    
    if(!exist->x)v.x = 0;
    if(!exist->y)v.y = 0;
    if(!exist->z)v.z = 0; 
    if(!exist->i)vi = 0;
    if(!exist->j)vj = 0;
    if(!exist->k)vk = 0;
    if(!exist->f)v.f = 0;
    float circle_x, circle_y, circle_z;
    float e;
    float new_R = (R+e)/2;
    x_pos = (x_pos/2);
    if (circle_x == 0)
    for (float theta = theta1; theta<theta2; theta+=0.1)
        
}*/
void CheckExist(char* gcode,struct Exist *exist){
    for (int i=0; i<strlen(gcode); i++){
        switch (gcode[i]){
            case 'X':
                exist->x = 1;
                break;
            case 'Y':
                exist->y = 1;
                break;
            case 'Z':
                exist->z = 1;
                break;
            case 'I':
                exist->i = 1;
                break;
            case 'J':
                exist->j = 1;
                break;
            case 'K':
                exist->k = 1;
                break;
            case 'F':
                exist->f = 1;
                break;
            case 'S':
                exist->s = 1;
                break;
        }
    }
    
}
void ExcuteGCode(char *gcode){
    struct Exist exist;
    // G00 G01 G02 G03 G90 G91 G92 M02 M03 M04 M17 M18 
    if (strncmp(gcode, "G00", 3) == 0){// ||
        //strncmp(gcode, "G0", 2) == 0  ){ will sucks all other commands away

        CheckExist(gcode, &exist);
        line_move(0, gcode, &exist);
    }else if( strncmp(gcode, "G01", 3) == 0 ||
        strncmp(gcode, "G1", 2) == 0  ){

        CheckExist(gcode, &exist);
        line_move(1, gcode, &exist);
    }else if (strncmp(gcode, "G04", 3) == 0){

    }else if (strncmp(gcode, "G90", 3) == 0){
        abs_mode = 1;
    }else if (strncmp(gcode, "G91", 3) == 0){
        abs_mode = 0;
    }else if (strncmp(gcode, "M03", 3) == 0){
        CNC_EnableSpindle();
    }else if (strncmp(gcode, "M17", 3) == 0){
        CNC_EnableStepper();
    }else if (strncmp(gcode, "M18", 3) == 0){
        CNC_DisableStepper();
    }
}


/*
int main(){

    char gcode[] = "G00 X123.1 Y231.2 F100";
    ExcuteGCode(gcode);
    return 0;
}*/
