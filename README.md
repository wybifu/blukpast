# blukpast
blukpast Game engine

Features done by now:
  Sprite engine
  Collision
  Camera
  Basic animation
  Memory management.

main file for example:
int main(int argc, char **argv)
{
    void bkp_setOpenGLInfo(3, 1)    //minimal version 3.1
    bkp_setWindowInfo(0, 0, BKP_FALSE,"BKP Test"); // window mode, auto detect screen resolution 
    bkp_setWindowInfo(0, 0, BKP_TRUE,"BKP Test"); // window mode, auto detect screen resolution 
        
    if(bkp_startEngine(argv) != BKP_TRUE)
        return EXIT_FAILURE;

    bkp_input_Init();

    g_scr = bkp_graphics_getWindowDimensions();
    TWH = bkp_graphics_getTWH(1920, 1080);
   
    BKP_Entity_Agent * G = (BKP_Entity_Agent *) bkp_memory_getTicket(sizeof(BKP_Entity_Agent));
    G->spritesheet = bkp_graphics_2DloadSurface("data/graphics/girl_run.png");

    init_player(G[0]); /* not in blukpast, create your own*/

    BKP_Rect world = setMapPlatform(); /*not in blukpast, create your own */

    /* selection of camera */
    bkp_graphics_2DinitCamera(bkp_rect(g_scr.w / 4, g_scr.h * 2 / 5, 0.0f, 0.0f), world, BKP_CAMERAFUNC_SMOOTHSTD);
    bkp_graphics_camera2DsetFunc(BKP_CAMERAFUNC_FOCUS);
    bkp_graphics_2DsetCameraPanic(150,50);

    BKP_Font * myfont;
    myfont =  bkp_graphics_newFont("data/fonts/DejaVuSans.ttf", 64, 128);

    BKP_ScreenText * fps = bkp_graphics_appendTextPool("do not care", 64, myfont, bkp_vec2(20 * TWH.w,64 * TWH.h), bkp_vec2(.3 * TWH.w,.3 * TWH.h), bkp_vec4(0.98f,0.98f,0.2f,1.0f));
    BKP_ScreenText * mem = bkp_graphics_appendTextPool("can't see me", 128, myfont, bkp_vec2(20 * TWH.w,96 * TWH.h), bkp_vec2(.3 * TWH.w,.3 * TWH.h), bkp_vec4(1.0f,1.0f,1.0f,1.0f));

    while(G[0]->input->Cancel == 0)                                                                                                                                                                                                                                            
    {
        bkp_input_capture(G[0]->input);

        manage_player(G[0]);
        bkp_graphics_camera2D(G[0]->dyn);

        Ugp_draw();
        _update_fps_counter( fps);
        _update_memUsage(mem);

    }   

    bkp_graphics_releaseTextPool(fps);
    bkp_graphics_releaseTextPool(mem);

    bkp_graphics_freeFont(myfont);
    unsetMapCollider();
    unsetMapPlatform();
    bkp_memory_releaseTicket(G[0]->input);
    bkp_memory_releaseTicket(G[0]->spritesheet);
    bkp_memory_releaseTicket(G[1]->spritesheet);
    bkp_memory_releaseTicket(G[0]);
    bkp_memory_releaseTicket(G[1]);

    bkp_logger_write(BKP_LOGGER_INFO,"\t________________________________\n");
    bkp_logger_write(BKP_LOGGER_INFO,"\t*POOLS* Allocated : %.1f Kb\n",(float)bkp_memory_allocated() / 1024);
    bkp_logger_write(BKP_LOGGER_INFO,"\t*POOLS* Used : %.1f Kb\n",(float)bkp_memory_usage() / 1024);
    bkp_logger_write(BKP_LOGGER_INFO,"\t*POOLS* Free : %.1f Kb\n",(float)bkp_memory_free() / 1024);

    bkp_stopEngine();

    return EXIT_SUCCESS;
}

