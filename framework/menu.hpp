#pragma once
#include "gui/gui.hpp"

void save( ) {
	std::cout << "this works\n";
}

namespace ui {
	inline sdk::math::vec2_t spawn_position = { 500, 500 };

	class c_menu {
	public:
		__forceinline void draw_ui( ) {
			auto initialize_menu_handling = [ ]( ) -> void {
				dependency::dep_input->init_input( );
				dependency::dep_controller->set_control_index( 0 );

				if ( dependency::dep_input->key_pressed( VK_INSERT ) )
					dependency::dep_controller->set_menu_state( !dependency::dep_controller->get_menu_state( ) );

				/* state handling */
				dependency::dep_controller->control_opened[ dependency::dependency_controller::handling_t::combo ] = -1;
				dependency::dep_controller->control_opened[ dependency::dependency_controller::handling_t::multicombo ] = -1;
				dependency::dep_controller->control_opened[ dependency::dependency_controller::handling_t::bind ] = -1;
			}; 
			initialize_menu_handling( );

			static std::vector< std::string > cfg_list{};

			auto window = new dep_framework::window_t( "dep window", &spawn_position,
													   dependency::dep_controller->dependency.window_size, &dependency::dep_controller->dependency.tab_index, &dependency::dep_controller->dependency.subtab_index_legit );
			{
				{
					static bool add = false;
					if ( !add ) { /* add tabs once */
						/* tab / subtab adding */
						window->add_tab( "Rage", 0 );
						window->add_tab( "Rage 2", 1 );
						window->add_tab( "Visuals", 2 );
						window->add_tab( "Misc", 3 );
						window->add_tab( "Profile", 4 );
				
						window->add_subtab( "Aimbot", 0 ); // 0
						window->add_subtab( "Anti-aim", 0 );
						window->add_subtab( "Other", 0 );
						window->add_subtab( "Part 1", 1 );
						window->add_subtab( "Part 2", 1 ); // 4
						window->add_subtab( "Esp", 2 ); // 4
						window->add_subtab( "Models", 2 ); // 4
						window->add_subtab( "World", 2 ); // 4
						window->add_subtab( "General", 3 ); // 4
						window->add_subtab( "Skins", 3 ); // 4
						window->add_subtab( "Configuration", 4 ); // 4

						add = true;
					}
				} /* we wont need to init the element anymore now since we have it shared */

				static bool salut = false;
				static bool salut2 = false;
				static float slider = 0.f;
				static int dropdown = 0;
				static int dropdown2 = 0;

				static bool a = false, b = false, c = false;
				static sdk::color::col_t stat_color = sdk::color::col_t( );
				static sdk::color::col_t stat_color2 = sdk::color::col_t( );
				static float stat_hue;
				static float stat_hue2;

				static int ab, bb, cc;
				static std::string filt;
				static std::string label;

				auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;
				auto y = dependency::dep_controller->dependency.ry - dependency::dep_controller->dependency.ory;

				auto g1 = new dep_framework::group_t( "Group 1", sdk::math::rect_t( 70, 50, 263 + ( x / 2 ), 200 + ( y / 2 ) ), 0, 0 );
				{
					window->sub_window( g1 );
					g1->obj( new dep_framework::colorpicker_t( &stat_color2, &stat_hue2 ) ); /* how is this being used, put it upwards the element that u want to have this shit */
					g1->obj( new dep_framework::checkbox_t( "Enabled", &salut ) );

					g1->obj( new dep_framework::colorpicker_t( &stat_color, &stat_hue ) ); /* how is this being used, put it upwards the element that u want to have this shit */
					g1->obj( new dep_framework::checkbox_t( "Silent aim", &salut2 ) );
					g1->obj( new dep_framework::slider_t( "Target limit", &slider, 0, 10 ) );
					g1->obj( new dep_framework::keybind_t( &ab, &ab ) );
					g1->obj( new dep_framework::text_t( "Just a simple text!" ) );
					g1->obj( new dep_framework::dropdown_t( "Target selection", &dropdown, { "Damage", "Smart shot", "Nearest", "Lag" } ) );

					auto hit = new dep_framework::multi_dropdown_t( "Hitboxes" );
					hit->add_item( "Head", &a );
					hit->add_item( "Body", &b );
					hit->add_item( "Legs", &c );
					g1->obj( hit );

					g1->scroll( );
					
				} delete g1;

				auto g2 = new dep_framework::group_t( "Group 2", sdk::math::rect_t( 70 + 263 + 10 + ( x / 2 ), 50, 263 + ( x / 2 ), 415 + y ), 0, 1 );
				{
					window->sub_window( g2 );


					std::vector< std::string > sal = { "Legit", "Rage", "dutuhvh", "abc", "asd" };

					g2->obj( new dep_framework::textbox_t( "Config name", &label ) );

					g2->obj( new dep_framework::listbox_t( "Config list", &cc, cfg_list, filt ) );
				
					static bool confirmation = false;
					g2->obj( new dep_framework::button_t( !confirmation ? "Save config" : "Are you sure?", [ ]( ) {
						if ( !confirmation ) {
							confirmation = true;
							return;
						}

						if ( confirmation ) {
							save( );
							confirmation = false;
						}
					} ) );

					g2->obj( new dep_framework::button_t( "Create config", [ & ]( ) {
						cfg_list.push_back( label );
						label.clear( );
														  } ) );
				} delete g2;

				auto g3 = new dep_framework::group_t( "Group 3", sdk::math::rect_t( 70, 50 + 210 + ( y / 2 ), 263 + x / 2, 200 + ( y / 2 ) ), 0, 0 );
				{
					window->sub_window( g3 );
				} delete g3;
			}
			delete window;
			

			dependency::dep_externals->begin( );
		}
	};

	inline const auto g_ui = std::make_unique < c_menu >( );
}