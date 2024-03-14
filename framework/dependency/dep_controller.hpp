#pragma once
#include "../../c_includes.hpp"

/* maximum controll index is 1000 */
namespace dependency {
	class dependency_controller {
	public:
		/* controller */
		struct controller_t {
			bool opened{}, col_opened{};
			int control_index{};
			float next_mouse_pos{}, next_mouse_wheel{};

			/* resize stuff */
			bool resizing{ false };
			int rx{ 630 }, ry{ 500 };
			float orx{ 630 }, ory{ 500 };

			/* panel */
			sdk::math::vec2_t mouse_pos{}, window_size{ 630, 500 };

			sdk::color::col_t copied_color; /* iterate */

			/* tab \ subtab */
			int tab_index{}, subtab_index_rage{}, subtab_index_legit{}, subtab_index_vis{}, subtab_index_misc{}, subtab_index_profile{};
		};

		/* pallete */
		struct pallete_t {
			sdk::color::col_t backround = sdk::color::col_t( 20, 20, 26 );
			sdk::color::col_t outline = sdk::color::col_t( 35, 35, 42 );
			sdk::color::col_t color_1 = sdk::color::col_t( 29, 29, 36 );
			sdk::color::col_t accent = sdk::color::col_t( 166, 167, 243 );

			sdk::color::col_t text = sdk::color::col_t( 255, 255, 255 );
		};

		/* handler */
		enum handling_t {
			combo,
			multicombo,
			color_selection,
			textbox,
			textbox_main,
			bind,
			size
		};

		/* handling stuff */
		bool control_iterate[ 5 ];
		int control_opened[ handling_t::size ];
		bool control_state[ handling_t::size ][ 1000 /* maximum controll index */ ];

		/* dep themes */
		pallete_t theme;

		/* dep controller */
		controller_t dependency;

		/* get menu state */
		__forceinline bool get_menu_state( ) {
			return dependency.opened;
		}

		/* set menu state */
		__forceinline void set_menu_state( const bool state ) {
			dependency.opened = state;
		}

		/* get controll index */
		__forceinline int get_control_index( ) {
			return dependency.control_index;
		}

		/* set control index */
		__forceinline void set_control_index( const uintptr_t index ) {
			dependency.control_index = index;
		}

		/* fix element id iretation */
		__forceinline bool is_posible( ) {
			return this->control_opened[ handling_t::combo ] == -1 &&
				this->control_opened[ handling_t::multicombo ] == -1 &&
				this->control_opened[ handling_t::bind ] == -1;
		}
	};

	inline const auto dep_controller = std::make_unique< dependency_controller>( );
}