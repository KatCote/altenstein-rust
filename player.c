#include<stdio.h>
#include<stdlib.h>
#include<curses.h>
#include<string.h>
#include<pthread.h>
#include<windows.h> // Comment for linux build
#include"render.h"
#include"items.h"

bool dev_mode = 0;
bool quit_diu_flag;

int player_askp = 27;

int player_subrace = 0;
int player_class = 0;
int player_subclass = 0;

int player_str = 8;
int player_dex = 8;
int player_con = 8;
int player_int = 8;
int player_wis = 8;
int player_char = 8;

int player_add_str = 0;
int player_add_dex = 0;
int player_add_con = 0;
int player_add_int = 0;
int player_add_wis = 0;
int player_add_char = 0;

int player_x;
int player_y;
int player_hp_max = 100;
int player_hp = 23;
int player_balance;
int player_level = 0;
int player_exp = 0;
int player_potion_cooldown = 0;

int player_selected_cell = 1;
int player_inventory_limit = 5;
int player_additional_limit = 5;
int player_inventory_used = 0;
int player_spell_book_limit = 3;
int player_spell_book_used = 0;

int buffer_inventory_selected_cell = 1;
int buffer_spell_book_selected_cell = 1;
int buffer_item_to_move;
int buffer_cell_to_move;
int buffer_player_y;
int buffer_player_x;	

int current_inventory_item = 0;

int defult_interface_usage(interface_tile map)
{
	if(dev_mode == 1) { player_hp = 4200; player_hp_max = 5000; player_balance = 5000; player_inventory_limit = 24; } // Not 25 so that you can use a backpack.
	
	player_additional_limit = player_inventory_limit;
	
	attron(COLOR_PAIR(100));
	mvaddch(player_y, player_x, '@');
	attroff(COLOR_PAIR(100));
	
	render_selected_cell(player_selected_cell, action_6_flag);
	render_player_info();
	
	if (action_6_flag == 1) render_inventory();
	
	char player_action;
	
	do
	{
		if (quit_diu_flag == 1) { return 1; }
		
		buffer_player_y = player_y;
		buffer_player_x = player_x;
		
		if(dev_mode == 1 && player_hp < 1000) player_hp = 1000;
		attron(COLOR_PAIR(004));
		if(dev_mode == 1 ) mvprintw(19, 1, "DEV MODE");
		attroff(COLOR_PAIR(004));
		
		if ((player_action == 'w') && ((player_y - 1) != 0) && (map.tile[player_y - 1][player_x] == ' ')) player_y--;
		else if ((player_action == 's') && ((player_y + 1) != 20) && (map.tile[player_y + 1][player_x] == ' ')) player_y++;
		else if ((player_action == 'a') && ((player_x - 1) != 0) && (map.tile[player_y][player_x - 1] == ' ')) player_x--;
		else if ((player_action == 'd') && ((player_x - 1) != 77) && (map.tile[player_y][player_x + 1] == ' ')) player_x++;
		
		char key_buffer = player_action;
		
		if(action_6_flag == 1){ // Player cell selection
			
			if((key_buffer) == 2) player_selected_cell += 5;
			else if((key_buffer) == 3) player_selected_cell -= 5;
			else if((key_buffer) == 4) player_selected_cell -= 1;
			else if((key_buffer) == 5) player_selected_cell += 1;
			
			if(player_selected_cell > player_additional_limit) player_selected_cell = 1;
			else if(player_selected_cell < 1) player_selected_cell = player_additional_limit;
			
		} else if (action_6_flag == 0){
			
			if((key_buffer) == 2  && player_selected_cell + 1 <= player_spell_book_limit) player_selected_cell += 1;
			else if((key_buffer) == 3) player_selected_cell -= 1;
			
			if(player_selected_cell > 10) player_selected_cell = 1;
			else if(player_selected_cell < 1) player_selected_cell = 10;
		}
		
		//int *asd = &player_y;
		//int *bsd = &player_x;
	
		//attron(COLOR_PAIR(001));
		//mvprintw(29, 0, "%p %p", asd, bsd); // Underline information
		//attroff(COLOR_PAIR(001));
		
		if (player_action == '6')
		{ 
			action_6_switch_inv(action_6_mod, map); // Action 6 button
			if(action_6_flag == 1) render_inventory();
		}
		
		else if (player_action == '1')
		{
			if (action_1_mod == 1) action_1_special(11, map);
			else if (action_1_mod == 2) { action_1_special(22, map); return 1; }
			else if (action_1_mod == 3) { action_1_special(22, map); return 1; }
			else if (action_1_mod == 4)
			{
				action_1_special(44, map);
				
				//stop_render_flag = 0;
		
				render_default_interface(current_map_tile, tile_inventory, tile_character_info, tile_actions, tile_world_info);
				render_map_entities(current_map_tile);
				render_selected_cell(player_selected_cell, action_6_flag);
				render_player_info();
				
				action_6_switch_inv(1, current_map_tile);
				action_6_switch_inv(1, current_map_tile);
				
				if (action_6_flag == 1) render_inventory();
			}
			else if (action_1_mod == 5) { action_1_special(55, current_map_tile); chargen_interface_usage(); return 1; }
		}
		
		else if (player_action == '\n') 
		{
			if(action_6_flag == 1) { action_transfer_inside_inventory(); render_inventory(); }
		}
		
		else if (player_action == '2') 
		{
			if(action_2_mod == 1) { action_2_inventory_usage(3, inventory_cell[buffer_inventory_selected_cell]); }
			//render_inventory();
		}
		
		attron(COLOR_PAIR(200));
		mvaddch(buffer_player_y, buffer_player_x, ' ');
		attroff(COLOR_PAIR(200));
		attron(COLOR_PAIR(100));
		mvaddch(player_y, player_x, '@');
		attroff(COLOR_PAIR(100));
		
		render_selected_cell(player_selected_cell, action_6_flag);
		render_map_entities(map);
		render_player_info();
		
		//Sleep(50);
		
		player_action = getch();
	}
	while (player_action != 27);
	
	
	stop_render_flag = 1;
	
	bool quit_res = render_message(20000, 3);
	
	if (quit_res == 1) return 1;
	else if (quit_res == 0)
	{
		stop_render_flag = 0;
		
		clear();
		
		render_default_interface(current_map_tile, tile_inventory, tile_character_info, tile_actions, tile_world_info);
		render_map_entities(current_map_tile);
		render_selected_cell(player_selected_cell, action_6_flag);
		render_player_info();
		
		action_6_switch_inv(1, current_map_tile);
		action_6_switch_inv(1, current_map_tile);
		
		if (action_6_flag == 1) render_inventory();
		
		defult_interface_usage(current_map_tile);
	}
	
	return 0;
}

int chargen_interface_usage(void)
{	
	stop_render_flag = 1;
	clear();
	
	int chargen_page = 1;
	int chargen_column = 1;
	int chargen_line = 1;
	
	int chargen_subrace_choose = 0;
	int chargen_class_choose = 0;
	int chargen_subclass_choose = 0;
	
	bool char_done_flag = 0;
	char key_buffer = 0;
	
	do
	{
		if((key_buffer) == 2) // DOWN (+1)
		{
			if (chargen_page == 1 && chargen_column == 1 && chargen_line != 2) chargen_line += 1;
			if (chargen_page == 1 && chargen_column == 2 && chargen_line != 6) chargen_line += 1;
			
			if (chargen_page == 2 && chargen_column == 1 && chargen_line != 2) chargen_line += 1;
			
			if (chargen_page == 2 && chargen_column == 2 && chargen_class_choose == 1 && chargen_line != 2) chargen_line += 1;
			if (chargen_page == 2 && chargen_column == 2 && chargen_class_choose == 2 && chargen_line != 4) chargen_line += 1;
		}
		
		else if((key_buffer) == 3) // UP (-1)
		{
			if (chargen_page == 1 && chargen_line != 1) chargen_line -= 1;
			
			if (chargen_page == 2 && chargen_line != 1) chargen_line -= 1;
		}
		
		else if((key_buffer) == 4) // LEFT (-1)
		{
			if (chargen_page == 1 && chargen_column != 1)
			{
				chargen_column--;
				
					 if (chargen_line == 1) chargen_line = 1; // GNOME
				else if (chargen_line == 2) chargen_line = 1;
				else if (chargen_line == 3) chargen_line = 1;
				
				else if (chargen_line == 4) chargen_line = 2; // ELF
				else if (chargen_line == 5) chargen_line = 2;
				else if (chargen_line == 6) chargen_line = 2;
			}
		}
		
		else if((key_buffer) == 5) // RIGHT (+1)
		{ 
			if (chargen_page == 1 && chargen_column != 2)
			{
				chargen_column++;
				
					 if (chargen_line == 1) chargen_line = 1; // GNOME (SUB)
				else if (chargen_line == 2) chargen_line = 4; // ELF (SUB)
			}
		}
		
		else if((key_buffer) == '\n') // ENTER
		{
			if (chargen_page == 2 && chargen_column == 2)
			{
				chargen_subclass_choose = chargen_line;
				chargen_page = 3;
				chargen_column = 1;
				chargen_line = 1;
				
				player_subrace = chargen_subrace_choose;
				player_class = chargen_class_choose;
				player_subclass = chargen_subclass_choose;
			}
			
			if (chargen_page == 2 && chargen_column == 1)
			{
				chargen_class_choose = chargen_line;
				chargen_column = 2;
				chargen_line = 1;
			}
			
			else if (chargen_page == 1 && chargen_column == 2)
			{
				chargen_subrace_choose = chargen_line;
				chargen_column = 1;
				chargen_line = 1;
				
				player_add_str = 0;
				player_add_dex = 0;
				player_add_con = 0;
				player_add_int = 0;
				player_add_wis = 0;
				player_add_char = 0;
				
				     if (chargen_subrace_choose == 1) player_add_int = 4;
				else if (chargen_subrace_choose == 2) player_add_int = 2;
				else if (chargen_subrace_choose == 3) player_add_int = 2;
				else if (chargen_subrace_choose == 4) player_add_dex = 4;
				else if (chargen_subrace_choose == 5) player_add_dex = 2;
				else if (chargen_subrace_choose == 6) player_add_dex = 2;
				
				chargen_page++;
			}
			
			else if (chargen_page == 1 && chargen_column == 1)
			{
				chargen_column = 2;
				if (chargen_line == 2) chargen_line = 4;
			}
			
		}
		
		else if((key_buffer) == 27) // ESC
		{
			
			if (chargen_page == 1 && chargen_column == 2)
			{
				if (1 <= chargen_line && chargen_line <= 3) chargen_line = 1;
				if (4 <= chargen_line && chargen_line <= 6) chargen_line = 2;
				
				chargen_column--;
			}
			
			else if (chargen_page == 2 && chargen_column == 1)
			{
				chargen_line = chargen_subrace_choose;
				chargen_column++;
				chargen_page--;
			}
			
			else if (chargen_page == 2 && chargen_column == 2)
			{
				chargen_line = chargen_class_choose;
				chargen_column--;
			}
		}
		
		clear();
		render_chargen_interface(chargen_page, chargen_column, chargen_line, chargen_class_choose);
		refresh();
		
		//mvprintw(29, 0, "line: %d, column: %d, page: %d cl: %d", chargen_line, chargen_column, chargen_page, chargen_class_choose);
		
		key_buffer = getch();
		
	} while (!char_done_flag);
	
	// ^^^ Replace with chargen
	//current_map_tile = tile_map_0002_dev;
	//quit_diu_flag = 0;
	//stop_render_flag = 0;
	
	return 0;
}

int launch(interface_tile current_map)
{
	//---------------------------------------------------------------------------------------
	
	int res;
	bool work = 1;
	
	//---------------------------------------------------------------------------------------
	
	pthread_t thread_render_engine;
	
	void *thread_func_render_engine(void * arg) 
	{
		do
		{
			// TODO: RENDER ENGINE REALIZATION (Work in progress)													<---------[TODO]---------<<<
		
			pthread_exit(NULL);
		}
		while(work);
	}
	
	res = pthread_create (&thread_render_engine, NULL, thread_func_render_engine, NULL);
	
	if (res != 0) {
		mvprintw(29, 0, "main error: can't create thread, status = %d\n", res);
		exit(-10);
	}

	res = pthread_detach(thread_render_engine);
	
	if (res != 0) {
		mvprintw(29, 0, "main error: can't detach thread, status = %d\n", res);
		exit(-11);
	}
	
	//---------------------------------------------------------------------------------------
	
	defult_interface_usage(current_map);
	
	//---------------------------------------------------------------------------------------
	
	return 0;
	
	//---------------------------------------------------------------------------------------
}
