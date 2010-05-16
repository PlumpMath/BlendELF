
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <malloc.h>
#include <sys/types.h>
#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(__CYGWIN__)
	// mbg: prevents windows.h from including winsock.h, this is later handled by enet 
	#define _WINSOCKAPI_
	#include <windows.h>
	#include <strsafe.h>
#else
	#include <dirent.h>
#endif

#include <GL/glfw.h>
#include <GL/gl.h>

#include <FreeImage.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "gfx.h"
#include "blendelf.h"
#include "types.h"

elf_context *ctx = NULL;
elf_engine *eng = NULL;
elf_resources *res = NULL;
int global_ref_count;
int global_ref_count_table[ELF_OBJECT_TYPE_COUNT];
int global_obj_count;

int luaopen_elf(lua_State* L);

#include "object.h"
#include "str.h"
#include "list.h"
#include "context.h"
#include "renderfactory.h"
#include "engine.h"
#include "frameplayer.h"
#include "timer.h"
#include "image.h"
#include "texture.h"
#include "material.h"
#include "ipo.h"
#include "camera.h"
#include "model.h"
#include "entity.h"
#include "light.h"
#include "scene.h"
#include "pak.h"
#include "postprocess.h"
#include "script.h"
#include "armature.h"
#include "property.h"
#include "actor.h"
#include "font.h"
#include "gui.h"
#include "sst.h"
#include "resources.h"
#include "particles.h"

// mbg: adds networking support to BlendELF
#include "network.h"

#ifdef ELF_PLAYER

int main()
{
	elf_game_config *config;
	elf_script *script;

	if(!(config = elf_read_game_config("config.txt")))
	{
		config = elf_create_game_config();
		config->window_size[0] = 1024;
		config->window_size[1] = 768;
		config->fullscreen = ELF_FALSE;
		config->texture_anisotropy = 1.0f;
		config->shadow_map_size = 1024;
		config->start = elf_create_string("game.pak");
	}

	if(!elf_init(config->window_size[0], config->window_size[1], "BlendELF", !config->fullscreen == ELF_FALSE))
	{
		elf_write_to_log("error: could not initialize engine\n");
		elf_destroy_game_config(config);
		return -1;
	}

	elf_set_texture_anisotropy(config->texture_anisotropy);
	elf_set_shadow_map_size(config->shadow_map_size);

	script = elf_create_script_from_file("init.lua");
	if(script)
	{
		elf_run_script(script);
		elf_destroy_script(script);
	}
	else
	{
		if(!elf_load_scene(config->start))
		{
			elf_destroy_game_config(config);
			elf_deinit();
			return -1;
		}

		while(elf_run());
	}

	elf_destroy_game_config(config);

	elf_deinit();

	return 0;
}

#endif	// ELF_PLAYER