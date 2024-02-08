#include <stdio.h>

#include "dreid/lib/dreid.h"
#include "dreid/lib/dd_gfx.h"
#include "dreid/lib/dd_log.h"

void setup(dd_ctx* ctx) 
{

}

void destroy(dd_ctx* ctx) 
{

}

int main(int argc, char** argv) 
{
    return dd_app(&(dd_app_desc) {
        .width = 1024,
        .height = 1024,
        .title = "orui example application",
        .setup = setup,
        .frame = null,
        .destroy = destroy,
        .is_reactive = true,
    });
}