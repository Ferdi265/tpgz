#include "utils.h"
#include "libtp_c/include/tp.h"
#include "libtp_c/include/system.h"
#include "controller.h"
#include "save_injector.h"
#include "commands.h"
#include "gorge.h"
#include "log.h"
#include "fs.h"
#include "menu.h"

bool loading_initiated = false;

namespace Utilities {
    static Log log;

    void move_cursor(int& cursor, int LINES) {
        if (button_is_pressed(Controller::DPAD_DOWN)) {
            if (cursor < LINES - 1) {
                cursor++;
            } else if (cursor == LINES - 1) {
                cursor = 2;
            }
        };

        if (button_is_pressed(Controller::DPAD_UP)) {
            if (cursor > 2) {
                cursor--;
            } else if (cursor == 2) {
                cursor = LINES - 1;
            }
        };
    }

    void render_lines(Font& font, Line input_lines[], int cursor, int LINES) {
        font.renderChars("tpgz v0.1", 13.0f, 15.0f, 0x008080FF);

        for (int i = 0; i < LINES; i++) {
            float offset = (60.0f + ((float)i * 20.0f));
            int cursor_color = 0xFFFFFF00;
            int description_color = 0xFFFFFF00;
            int cursor_alpha = 0xFF;
            int description_alpha = 0xFF;
            int drop_shadows_color = 0x00000080;

            if (input_lines[i].idx != cursor) {
                cursor_alpha = 0x80;
            }
            if (input_lines[i].idx != cursor) {
                description_alpha = 0x00;
            }
            cursor_color |= cursor_alpha;
            description_color |= description_alpha;
            
            if (input_lines[i].toggleable) {
                char toggleline[54];
                for (int j = 0; j < 50; j++) {
                    toggleline[j] = input_lines[i].line[j];
                }
                if (*input_lines[i].activation_flag) {
                    strcat(toggleline, " [X]");
                } else {
                    strcat(toggleline, " [ ]");
                }

                font.renderChars(toggleline, 15.0f, offset, cursor_color);
                if (g_drop_shadows) {
                    font.renderChars(toggleline, 15.0f + 2.0f, offset + 2.0f, drop_shadows_color);
                };
            } else {
                font.renderChars(input_lines[i].line, 15.0f, offset, cursor_color);
                if (g_drop_shadows) {
                    font.renderChars(input_lines[i].line, 15.0f + 2.0f, offset + 2.0f, drop_shadows_color);
                };
            }

            font.renderChars(input_lines[i].description, 200.0f, 440.f, description_color);
            if (g_drop_shadows && input_lines[i].idx == cursor) {
                font.renderChars(input_lines[i].description, 200.0f + 2.0f, 440.0f + 2.0f, drop_shadows_color);
            };
        };
    }  // namespace Utilities

    // hacky, rework into one render_lines
    // for this to work correctly, Cheat Items[] should be in the same order as Line lines[], minus the menu title and space in lines[]
    void render_lines_with_items(Font& font, Line input_lines[], int cursor, int LINES, Cheats::Cheat Items[]) {
        font.renderChars("tpgz v0.1", 13.0f, 15.0f, 0x008080FF);

        for (int i = 0; i < LINES; i++) {
            float offset = (60.0f + ((float)i * 20.0f));
            int cursor_color = 0xFFFFFF00;
            int description_color = 0xFFFFFF00;
            int cursor_alpha = 0xFF;
            int description_alpha = 0xFF;
            int drop_shadows_color = 0x00000080;
            if (input_lines[i].idx != cursor) {
                cursor_alpha = 0x80;
            }
            if (input_lines[i].idx != cursor) {
                description_alpha = 0x00;
            }

            cursor_color |= cursor_alpha;
            description_color |= description_alpha;
            if (input_lines[i].toggleable) {
                char toggleline[54];
                for (int j = 0; j < 50; j++) {
                    toggleline[j] = input_lines[i].line[j];
                }
                if (Items[i - 2].active) {
                    strcat(toggleline, " [X]");
                } else {
                    strcat(toggleline, " [ ]");
                }
                font.renderChars(toggleline, 15.0f, offset, cursor_color);
                if (g_drop_shadows) {
                    font.renderChars(toggleline, 15.0f + 2.0f, offset + 2.0f, drop_shadows_color);
                };
            } else {
                font.renderChars(input_lines[i].line, 15.0f, offset, cursor_color);
                if (g_drop_shadows) {
                    font.renderChars(input_lines[i].line, 15.0f + 2.0f, offset + 2.0f, drop_shadows_color);
                };
            }
            font.renderChars(input_lines[i].description, 15.0f, 440.f, description_color);
            if (g_drop_shadows && input_lines[i].idx == cursor) {
                font.renderChars(input_lines[i].description, 15.0f + 2.0f, 440.f + 2.0f, drop_shadows_color);
            }
        };
    }

    void set_after_load(Caller caller) {
        if (caller == AreaReload) {
            tp_gameInfo.warp.entrance.void_flag = 0;
            tp_gameInfo.event_to_play = 0;
            tp_gameInfo.respawn_animation = 0;
            log.PrintLog("Setting link's position", DEBUG);
            tp_zelAudio.link_debug_ptr->position = practice_file.options.position;
            log.PrintLog("Setting link's angle", DEBUG);
            tp_zelAudio.link_debug_ptr->facing = practice_file.options.angle;
            log.PrintLog("Setting camera matrix", DEBUG);
            tp_matrixInfo.matrix_info->camera0 = practice_file.options.camera_matrix.camera0;
            tp_matrixInfo.matrix_info->camera1 = practice_file.options.camera_matrix.camera1;
            tp_matrixInfo.matrix_info->camera2 = practice_file.options.camera_matrix.camera2;
            tp_matrixInfo.matrix_info->camera3 = practice_file.options.camera_matrix.camera3;
            tp_matrixInfo.matrix_info->camera4 = practice_file.options.camera_matrix.camera4;
            tp_matrixInfo.matrix_info->camera5 = practice_file.options.camera_matrix.camera5;
            tp_matrixInfo.matrix_info->camera6 = practice_file.options.camera_matrix.camera6;
            tp_matrixInfo.matrix_info->camera7 = practice_file.options.camera_matrix.camera7;
            log.PrintLog("Turning off area reload trigger", DEBUG);
            reload_area_flag = false;
        }

        if (caller == SaveInjection) {
            // moving to a general temp flag saving function

            log.PrintLog("Saving temp flags to RAM", DEBUG);
            memcpy(&practice_file.options.temp_flags, &tp_gameInfo.temp_flags, 40);

            if (practice_file.inject_options) {
                //log.PrintLog("setting link's position -- %d", practice_file.options.position, DEBUG);
                tp_zelAudio.link_debug_ptr->position = practice_file.options.position;
                log.PrintLog("setting link's angle -- %d", practice_file.options.angle, DEBUG);
                tp_zelAudio.link_debug_ptr->facing = practice_file.options.angle;
                //log.PrintLog("Setting camera matrix -- %f",practice_file.options.camera_matrix, DEBUG);
                tp_matrixInfo.matrix_info->camera0 = practice_file.options.camera_matrix.camera0;
                tp_matrixInfo.matrix_info->camera1 = practice_file.options.camera_matrix.camera1;
                tp_matrixInfo.matrix_info->camera2 = practice_file.options.camera_matrix.camera2;
                tp_matrixInfo.matrix_info->camera3 = practice_file.options.camera_matrix.camera3;
                tp_matrixInfo.matrix_info->camera4 = practice_file.options.camera_matrix.camera4;
                tp_matrixInfo.matrix_info->camera5 = practice_file.options.camera_matrix.camera5;
                tp_matrixInfo.matrix_info->camera6 = practice_file.options.camera_matrix.camera6;
                tp_matrixInfo.matrix_info->camera7 = practice_file.options.camera_matrix.camera7;
            } else {
                practice_file.options.position = tp_zelAudio.link_debug_ptr->position;
                practice_file.options.angle = tp_zelAudio.link_debug_ptr->facing;
                //log.PrintLog("Setting camera matrix -- %f",practice_file.options.camera_matrix, DEBUG);
                practice_file.options.camera_matrix.camera0 = tp_matrixInfo.matrix_info->camera0;
                practice_file.options.camera_matrix.camera1 = tp_matrixInfo.matrix_info->camera1;
                practice_file.options.camera_matrix.camera2 = tp_matrixInfo.matrix_info->camera2;
                practice_file.options.camera_matrix.camera3 = tp_matrixInfo.matrix_info->camera3;
                practice_file.options.camera_matrix.camera4 = tp_matrixInfo.matrix_info->camera4;
                practice_file.options.camera_matrix.camera5 = tp_matrixInfo.matrix_info->camera5;
                practice_file.options.camera_matrix.camera6 = tp_matrixInfo.matrix_info->camera6;
                practice_file.options.camera_matrix.camera7 = tp_matrixInfo.matrix_info->camera7;
            }
            //
            log.PrintLog("Turning off save injection trigger", DEBUG);
            inject_save_flag = false;
        }

        if (caller == Gorge) {
            log.PrintLog("Saving temp flag #14 to 0x20", DEBUG);
            tp_gameInfo.temp_flags.temp_flag_bit_field_14 = 0x20;
            log.PrintLog("Setting cs_val to 0x900", DEBUG);
            tp_gameInfo.cs_val = 0x900;
            inject_gorge_flag = false;
        }

        if (caller == SaveTempFlags) {
            log.PrintLog("Saving temp flags to RAM", DEBUG);
            memcpy(&practice_file.options.temp_flags, &tp_gameInfo.temp_flags, 40);
        }
    }

    void set_during_load(Caller caller) {
        if (caller == Gorge) {
            log.PrintLog("Setting respawn id to 2", DEBUG);
            tp_gameInfo.respawn_next_spawn_id = 2;
            log.PrintLog("Setting respawn position: {-11856.857f, -5700.0f, 56661.5}", DEBUG);
            tp_gameInfo.respawn_position = {-11856.857f, -5700.0f, 56661.5};
            log.PrintLog("Setting respawn angle: 24169", DEBUG);
            tp_gameInfo.respawn_angle = 24169;
            log.PrintLog("Clearing rupee flags", DEBUG);
            Inventory::clear_rupee_flags();
        }

        if (caller == AreaReload) {
            if (g_reset_temp_flags) {
                log.PrintLog("Writing temp flags to RAM", DEBUG);
                memcpy(&tp_gameInfo.temp_flags, &practice_file.options.temp_flags, 40);
            }
        }

        if (caller == SaveInjection) {
            int spawn = practice_file.qlog_bytes[SPAWN_INDEX];
            int room = practice_file.qlog_bytes[ROOM_INDEX];
            char stage[8];
            log.PrintLog("Copying stage to load from practice file obj", DEBUG);
            for (int i = 0; i < 8; i++) {
                stage[i] = practice_file.qlog_bytes[STAGE_INDEX + i];
            }
            int state = tp_getLayerNo(stage, room, 0xFF);
            log.PrintLog("Setting spawn to: %d", spawn, DEBUG);
            tp_gameInfo.warp.entrance.spawn = spawn;
            log.PrintLog("Setting room to: %d", room, DEBUG);
            tp_gameInfo.warp.entrance.room = room;
            log.PrintLog("Setting stage to: %s", stage, DEBUG);
            strcpy((char*)tp_gameInfo.warp.entrance.stage, stage);
            log.PrintLog("Setting state to: %d", state, DEBUG);
            tp_gameInfo.warp.entrance.state = state;
        }
    }

    void trigger_load(Caller caller) {
        // trigger loading
        if (tp_fopScnRq.isLoading == 0 && !loading_initiated && caller != SaveTempFlags) {
            log.PrintLog("Initiating warp", INFO);
            SaveInjector::inject_save();
            tp_gameInfo.warp.enabled = true;
        }

        if (loading_initiated == true) {
            if (tp_fopScnRq.isLoading == 0) {
                set_after_load(caller);
                loading_initiated = false;
            }
        }

        if (tp_fopScnRq.isLoading == 1) {
            set_during_load(caller);
            loading_initiated = true;
        }
    }



    
}  // namespace Utilities