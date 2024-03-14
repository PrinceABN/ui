#pragma once
#include "../dependency/dep.hpp"

static int slide[ 1000 ];
static bool slide_move[ 1000 ];
static float slide_offset[ 1000 ];

static auto str_index_dropdown = -1;
static bool state_dropdown = false;
static int time_dropdown;

static auto str_index_mdropdown = -1;
static bool state_mdropdown = false;
static int time_mdropdown;

static auto str_idx_color = -1;
static bool state_color = false;

static short int str_idx_bind = -1;
static bool state_bind = false;
static int key_bind = -1;
char buffer_bind[ 128 ];
static short int stored_index_list = -1;
static bool state_list = false;
static int time_list;
static bool is_input_good = false;
std::string bind_name[ 1000 ];

static short int str_indx_button = -1;
static bool state_button = false;
bool ask_again2[ 1000 ];

static int scroll_pos_list[ 1000 ];
static bool scroll_state_list[ 1000 ];

extern const char* stable_keys[ 254 ];
std::vector<std::string> bind_list = { "Always", "Hold on", "Toggle", "Hold off" };

extern const char* lower_case[ 254 ];
extern const char* upper_case[ 254 ];

static short int str_indx_textbar_list = -1;
static short int str_indx_textbar_list2 = -1;
static bool state_textbar_list = false;
static bool state_textbar_list2 = false;

float blink;

struct RGB { double r, g, b; };
struct HSV { double h, s, v; };

#define space_element 20
#define element_width 263 - 50

static sdk::color::col_t bar_colors[ 7 ] = {
			sdk::color::col_t( 255, 0, 0, 255 ),
			sdk::color::col_t( 255, 255, 0, 255 ),
			sdk::color::col_t( 0, 255, 0, 255 ),
			sdk::color::col_t( 0, 255, 255, 255 ),
			sdk::color::col_t( 0, 0, 255, 255 ),
			sdk::color::col_t( 255, 0, 255, 255 ),
			sdk::color::col_t( 255, 0, 0, 255 )
};

namespace dep_framework {
	/* subtab */
	struct tab_system_t {
		std::string name;
		int tab_id;
	};

	struct subtab_system_t {
		std::string name;
		int selected_tab;
	};

	inline std::vector < tab_system_t > tabs_out{};
	inline std::vector < subtab_system_t > subtabs_out{};

	/* element chebox */
	class checkbox_t {
	public: /* constructor */
		checkbox_t( std::string name, bool* value ) {
			this->name = name;
			this->value = value;
		}
	public: /* functions */
		__forceinline void think( ) {
			sdk::math::vec2_t checkbox_size = {
				sdk::drawing::get_text_size( this->name.c_str( ), sdk::drawing::font_4 ).x, sdk::drawing::get_text_size( this->name.c_str( ), sdk::drawing::font_4 ).y
			};

			if ( dependency::dep_controller->is_posible( ) && !dependency::dep_controller->control_iterate[ 0 ] ) {
				if ( dependency::dep_input->mouse_in_box( this->pos, { checkbox_size.x, 13 } ) && dependency::dep_input->key_pressed( VK_LBUTTON ) ) {
					*this->value = !*this->value;
				}
			}
		}

		__forceinline void draw( ) {
			/* animations */
			auto anim_1 = animation_controller.get( this->name + std::to_string( dependency::dep_controller->get_control_index( ) ) + animation_controller.current_child );
			anim_1.adjust( anim_1.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			auto anim_2 = animation_controller.get( this->name + "#activated" + std::to_string( dependency::dep_controller->get_control_index( ) ) + animation_controller.current_child );
			anim_2.adjust( anim_2.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( ( dependency::dep_controller->get_menu_state( ) && *this->value ) ? 1.f : -1.f ) );

			auto draw_checkmark = [ ]( ImVec2 pos, sdk::color::col_t col, float sz, float anim ) -> void {
				float thickness = std::fmax( sz / 5.0f, 1.0f );
				sz -= thickness * 0.5f;
				pos += ImVec2( thickness * 0.25f, thickness * 0.25f );

				float third = sz / 3.0f;
				float bx = pos.x + third;
				float by = pos.y + sz - third * 0.5f;
				draw_list->PathLineTo( ImVec2( bx - third * anim, by - third * anim ) );
				draw_list->PathLineTo( ImVec2( bx, by ) );
				draw_list->PathLineTo( ImVec2( bx + third * 2 * anim, by - third * 2 * anim ) );
				draw_list->PathStroke( col.convert( ), false, thickness );
			};

			sdk::drawing::rect( this->pos.x, this->pos.y, 13, 13, sdk::color::col_t( 37, 37, 44 ).modify_alpha( 255 * anim_1.value ), 2 );
			sdk::drawing::rect_filled( this->pos.x + 1, this->pos.y + 1, 13 - 2, 13 - 2, sdk::color::col_t( 20, 20, 26 ).blend( dependency::dep_controller->theme.accent, anim_2.value ).modify_alpha( 255 * anim_1.value ), 2 );

			draw_checkmark( ImVec2( this->pos.x + 2.6, this->pos.y + 2.5 ), sdk::color::col_t( 0, 0, 0 ).modify_alpha( anim_2.value * 255 ), 7, anim_2.value );

			sdk::drawing::text( this->pos.x + space_element, this->pos.y - 3, sdk::color::col_t( 220, 220, 220 ).modify_alpha( 255 * anim_1.value ), sdk::drawing::font_4, this->name.c_str( ), false );
		}
	public: /* public vars */
		sdk::math::vec2_t pos{};
	private: /* priv vars */
		bool* value{};
		std::string name{};
	};

	/* obj slider */
	class slider_t {
	public:
		slider_t( std::string name, float* value, int min, int max, std::string suffix = "%", bool transform = false ) {
			this->name = name;
			this->value = value;
			this->min = min;
			this->max = max;
			this->suffix = suffix;
			this->tr_int = transform;
		}
	public: 
		template <typename T>
		__forceinline std::string precision( const T a_value, const int n = 3 ) {
			std::ostringstream out;
			out.precision( n );
			out << std::fixed << a_value;
			return out.str( );
		}

		__forceinline void think( ) {
			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;
			auto delta = this->max - this->min;
			static auto str_index = - 1;

			if ( dependency::dep_controller->is_posible( ) && !dependency::dep_controller->dependency.col_opened ) {
				if ( dependency::dep_input->key_pressed( VK_LBUTTON ) & 1  && dependency::dep_input->mouse_in_box(
					{ this->pos.x + space_element, this->pos.y + 20 }, { ( float )element_width + ( x / 2 ), 13 } ) ) {
					str_index = dependency::dep_controller->get_control_index( );
				}
			}

			if ( GetAsyncKeyState( VK_LBUTTON ) && str_index == dependency::dep_controller->get_control_index( ) ) {
				float normalized_pos = ( dependency::dep_input->get_mouse_position( ).x - this->pos.x - space_element ) / ( element_width + ( x / 2 ) );
				float target_value = this->min + delta * normalized_pos;

				*this->value = animation_controller.lerp_single( *this->value, target_value, 0.2f );

				std::cout << std::to_string( *this->value ) + " v\n";

				/* gheto clamping */
				if ( *this->value < this->min )
					*this->value = this->min;

				if ( *this->value > this->max )
					*this->value = this->max;
			}

			/* update index */
			if ( !GetAsyncKeyState( VK_LBUTTON ) )
				str_index = -1;
		}

		__forceinline void draw( ) {
			/* animations */
			auto anim_1 = animation_controller.get( this->name + std::to_string( dependency::dep_controller->get_control_index( ) ) + animation_controller.current_child );
			anim_1.adjust( anim_1.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;

			sdk::drawing::text( this->pos.x + space_element, this->pos.y - 3, 
								sdk::color::col_t( 220, 220, 220 ).modify_alpha( 255 * anim_1.value), sdk::drawing::font_4, this->name.c_str( ), false );

			sdk::drawing::rect( this->pos.x + space_element, this->pos.y + 19, element_width + ( x / 2 ), 13 - 2, sdk::color::col_t( 37, 37, 44 ).modify_alpha( 255 * anim_1.value ), 2 );
			sdk::drawing::rect_filled( this->pos.x + 1 + space_element, this->pos.y + 20, element_width + ( x / 2 ) - 2, 13 - 2 - 2,
									   sdk::color::col_t( 20, 20, 26 ).modify_alpha( 255 * anim_1.value ), 2 );

			/* slider value */
			auto min_delta = *this->value - this->min;
			auto delta = this->max - this->min;
			auto total = ( min_delta / delta ) * ( element_width + ( x / 2 ) );
			total = std::fmin( total, ( element_width + ( x / 2 ) ) );

			if ( total > this->min ) {
				sdk::drawing::rect_filled( this->pos.x + 1 + space_element, this->pos.y + 20, total - 2, 13 - 2 - 2,
										   dependency::dep_controller->theme.accent.modify_alpha( 255 * anim_1.value ), 2 );
			}

			std::string full_txt{};
			full_txt = this->tr_int ? std::to_string( ( int )*this->value ) : precision( *this->value, 1 ) + this->suffix;

			auto limit_text = sdk::drawing::get_text_size( full_txt.c_str( ), sdk::drawing::font_4 ).x;
			sdk::drawing::text( this->pos.x + element_width + ( x / 2 ) - limit_text + 17, this->pos.y - 3, sdk::color::col_t( 220, 220, 220 ).modify_alpha( 255 * anim_1.value ), sdk::drawing::font_4, full_txt.c_str( ), false );
		}
	public: /* pub vars */	
		sdk::math::vec2_t pos{};
	private:
		std::string name{}, suffix{};
		float* value;
		int min{}, max{};

		bool tr_int{};
	};

	class dropdown_t {
	public:
		dropdown_t( std::string name, int* value, std::vector<std::string> items ) {
			this->name = name;
			this->value = value;
			this->items = items;
		}
	public:
		__forceinline void think( ) {
			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;

			/* setup this here */
			this->hovered_element = dependency::dep_input->mouse_in_box( this->pos + sdk::math::vec2_t( 0, 20 ), { element_width + ( x / 2 ) - 2, 23 } );

			/* handling */
			if ( str_index_dropdown == dependency::dep_controller->get_control_index() && dependency::dep_input->key_pressed( VK_LBUTTON )
				 && !dependency::dep_input->mouse_in_box( this->pos + sdk::math::vec2_t( 0, 40 ), { element_width + ( x / 2 ) - 2, float( this->items.size( ) ) * 20 } ) ) {
				state_dropdown = !state_dropdown;
				str_index_dropdown = -1;

				std::cout << "2\n";
			}

			if ( !dependency::dep_controller->control_iterate[ 0 ] && !dependency::dep_controller->dependency.col_opened
				 && dependency::dep_controller->control_opened[ dependency::dependency_controller::multicombo ] == -1 && time_dropdown == -1 &&
				 dependency::dep_input->key_pressed( VK_LBUTTON ) 
				 && dependency::dep_input->mouse_in_box( this->pos + sdk::math::vec2_t( 0, 20 ), { element_width + ( x / 2 ) - 2, 23 } ) ) {
				state_dropdown = !state_dropdown;
				str_index_dropdown = dependency::dep_controller->get_control_index( );

				std::cout << "1\n";
			}

			/* we are going to setup the main animation and time */
			if ( time_dropdown >= 12 )
				time_dropdown = 12;
			else if ( time_dropdown <= 0 )
				time_dropdown = 0;

			state_dropdown ? time_dropdown++ : time_dropdown--;

			/* other checks for sanity */
			dependency::dep_controller->control_opened[ dependency::dependency_controller::combo ] = time_dropdown;

			/* check for stored index and setup trought state */
			if ( str_index_dropdown == dependency::dep_controller->get_control_index( ) )
				this->focused_element = state_dropdown;

			/* check the focus and setup it */
			dependency::dep_controller->control_state[ dependency::dependency_controller::combo ][ dependency::dep_controller->get_control_index( ) ] = this->focused_element;
		}

		__forceinline void draw( ) {
			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;

			/* animations */
			auto anim_1 = animation_controller.get( this->name + std::to_string( dependency::dep_controller->get_control_index( ) ) + animation_controller.current_child );
			anim_1.adjust( anim_1.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			auto anim_3 = animation_controller.get( this->name + std::to_string( dependency::dep_controller->get_control_index( ) ) + "#focused" + animation_controller.current_child );
			anim_3.adjust( anim_3.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) && ( this->focused_element ) ? 1.f : -1.f ) );

			/* animations */
			auto anim_2 = animation_controller.get( this->name + std::to_string( dependency::dep_controller->get_control_index( ) ) + "#choverdrpp" + animation_controller.current_child );
			anim_2.adjust( anim_2.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) && dependency::dep_input->mouse_in_box(
				{ this->pos.x + space_element, this->pos.y + 20 }, { element_width + ( x / 2 ), 23 }
			) ? 1.f : -1.f ) );

			sdk::drawing::text( this->pos.x + space_element, this->pos.y - 3,
								sdk::color::col_t( 220, 220, 220 ).modify_alpha( 255 * anim_1.value ), sdk::drawing::font_4, this->name.c_str( ), false );

			sdk::drawing::rect( this->pos.x + space_element, this->pos.y + 19, element_width + ( x / 2 ), 23, sdk::color::col_t( 37, 37, 44 ).modify_alpha( 255 * anim_1.value ), 2 );
			sdk::drawing::rect_filled( this->pos.x + 1 + space_element, this->pos.y + 20, element_width + ( x / 2 ) - 2, 23 - 2,
									   sdk::color::col_t( 20, 20, 26 ).blend( sdk::color::col_t( 20, 20, 26 ).darker( -5 ), anim_2.value ).modify_alpha( 255 * anim_1.value ), 2 );

			sdk::drawing::text( this->pos.x + space_element + 5, this->pos.y + 21,
								sdk::color::col_t( 220, 220, 220 ).blend( dependency::dep_controller->theme.accent, anim_3.value ).modify_alpha( 200 * anim_1.value ), sdk::drawing::font_4, this->items[ *value ].c_str( ), false );

			sdk::drawing::text( this->pos.x + space_element + 5 + element_width + ( x / 2 ) - 30, this->pos.y + 21,
								sdk::color::col_t( 220, 220, 220 ).blend( dependency::dep_controller->theme.accent, anim_3.value ).modify_alpha( 200 * anim_1.value ), sdk::drawing::font_3, this->focused_element ? "a" : "d", false );
		
			/* external drawing */
			auto rect_fill = [ ]( int x, int y, int w, int h, sdk::color::col_t col, int r, bool overwrite ) -> void {
				if ( overwrite ) {
					dependency::external.rect_filled_a.insert( dependency::external.rect_filled_a.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
				} else {
					dependency::external.rect_filled.insert( dependency::external.rect_filled.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
				}
				};

			auto rect = [ ]( int x, int y, int w, int h, sdk::color::col_t col, int r, bool overwrite ) -> void {
				if ( overwrite ) {
					dependency::external.rect_a.insert( dependency::external.rect_a.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
				} else {
					dependency::external.rect.insert( dependency::external.rect.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
				}
				};

			auto image = [ ]( int x, int y, int w, int h, ImTextureID texture, sdk::color::col_t col ) -> void {
				dependency::external.image.insert( dependency::external.image.begin( ), dependency::draw_image( x, y, w, h, texture, col ) );
				};

			auto text = [ ]( int x, int y, int font_index, std::string text, sdk::color::col_t col, bool ds ) -> void {
				dependency::external.text.insert( dependency::external.text.begin( ), dependency::draw_text( x, y, text, col, font_index, ds ) );
				};


			if ( anim_3.value > 0.01f ) {
				for ( size_t i{ 0 }; i < this->items.size( ); i++ ) {
					if ( dependency::dep_input->key_pressed( VK_LBUTTON ) &&
						 dependency::dep_input->mouse_in_box( { this->pos.x + 7, this->pos.y + 45 + ( i * 23 ) }, { ( float )( element_width + ( x / 2 ) ), 23 } ) ) {

						*this->value = i;
						state_dropdown = false;
						str_index_dropdown = -1;
					}

					auto anim_4 = animation_controller.get( this->items[i] + std::to_string( i ) + "#selected" + animation_controller.current_child );
					anim_4.adjust( anim_4.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) && ( dependency::dep_input->mouse_in_box( { this->pos.x + 7, this->pos.y + 45 + ( i * 23 ) }, { ( float )( element_width + ( x / 2 ) ), 23 } ) ) ? 1.f : -1.f ) );

					sdk::color::col_t color = *this->value == i ? dependency::dep_controller->theme.accent.modify_alpha( 170 * anim_3.value ) :
						sdk::color::col_t( 220, 220, 220 ).blend( dependency::dep_controller->theme.accent, anim_4.value ).modify_alpha( 200 * anim_3.value );

					text( this->pos.x + space_element + 5, this->pos.y + 19 + 30 + ( i * 21 ) * anim_3.value, sdk::drawing::font_4, this->items[ i ].c_str( ), color, false );
				}

				rect( this->pos.x + space_element, this->pos.y + 19 + 25, element_width + ( x / 2 ), this->items.size() * 23 * anim_3.value, sdk::color::col_t( 37, 37, 44 ).modify_alpha( 255 * anim_1.value ), 2, true );
				rect_fill( this->pos.x + 1 + space_element, this->pos.y + 20 + 25, element_width + ( x / 2 ) - 2, ( ( this->items.size( ) * 23 ) - 2 ) * anim_3.value,
						   sdk::color::col_t( 20, 20, 26 ).modify_alpha( 255 * anim_1.value ), 2, false );
			}
			dependency::dep_controller->control_iterate[ 1 ] = state_dropdown;
		}
	public:
		bool focused_element{};
		bool hovered_element{};
		sdk::math::vec2_t pos{};
	private:
		std::string name{};
		std::vector<std::string> items{};
		int* value{};
	};

	struct multi_dropdown_system {
		multi_dropdown_system( std::string name, bool* value ) {
			this->name = name;
			this->value = value;
		}

		std::string name{};
		bool* value{};
	};

	class multi_dropdown_t {
	public:
		multi_dropdown_t( std::string name ) {
			this->name = name;
		}
	public:
		__forceinline void think( ) {
			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;

			/* setup this here */
			this->hovered_element = dependency::dep_input->mouse_in_box( this->pos + sdk::math::vec2_t( 0, 20 ), { element_width + ( x / 2 ) - 2, 23 } );

			/* handling */
			if ( str_index_mdropdown == dependency::dep_controller->get_control_index( ) && dependency::dep_input->key_pressed( VK_LBUTTON )
				 && !dependency::dep_input->mouse_in_box( this->pos + sdk::math::vec2_t( 0, 40 ), { element_width + ( x / 2 ) - 2, float( this->items.size( ) ) * 20 } ) ) {
				state_mdropdown = !state_mdropdown;
				str_index_mdropdown = -1;

				std::cout << "2\n";
			}

			if ( !dependency::dep_controller->control_iterate[ 0 ] && !dependency::dep_controller->dependency.col_opened
				 && dependency::dep_controller->control_opened[ dependency::dependency_controller::combo ] == -1 && time_mdropdown == -1 &&
				 dependency::dep_input->key_pressed( VK_LBUTTON )
				 && dependency::dep_input->mouse_in_box( this->pos + sdk::math::vec2_t( 0, 20 ), { element_width + ( x / 2 ) - 2, 23 } ) ) {
				state_mdropdown = !state_mdropdown;
				str_index_mdropdown = dependency::dep_controller->get_control_index( );

				std::cout << "1\n";
			}

			/* we are going to setup the main animation and time */
			if ( time_mdropdown >= 12 )
				time_mdropdown = 12;
			else if ( time_mdropdown <= 0 )
				time_mdropdown = 0;

			state_mdropdown ? time_mdropdown++ : time_mdropdown--;

			/* other checks for sanity */
			dependency::dep_controller->control_opened[ dependency::dependency_controller::multicombo ] = time_mdropdown;

			/* check for stored index and setup trought state */
			if ( str_index_mdropdown == dependency::dep_controller->get_control_index( ) )
				this->focused_element = state_mdropdown;

			/* check the focus and setup it */
			dependency::dep_controller->control_state[ dependency::dependency_controller::multicombo ][ dependency::dep_controller->get_control_index( ) ] = this->focused_element;
		}

		__forceinline void draw( ) {
			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;


			/* animations */
			auto anim_1 = animation_controller.get( this->name + std::to_string( dependency::dep_controller->get_control_index( ) ) + "#g" + animation_controller.current_child );
			anim_1.adjust( anim_1.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			/* animations */
			auto anim_2 = animation_controller.get( this->name + std::to_string( dependency::dep_controller->get_control_index( ) ) + "#choverdrpp2" + animation_controller.current_child );
			anim_2.adjust( anim_2.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) && dependency::dep_input->mouse_in_box(
				{ this->pos.x + space_element, this->pos.y + 20 }, { element_width + ( x / 2 ), 23 }
			) ? 1.f : -1.f ) );

			auto anim_3 = animation_controller.get( this->name + std::to_string( dependency::dep_controller->get_control_index( ) ) + "#gfocused" + animation_controller.current_child );
			anim_3.adjust( anim_3.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) && ( this->focused_element ) ? 1.f : -1.f ) );

			sdk::drawing::text( this->pos.x + space_element, this->pos.y - 3,
								sdk::color::col_t( 220, 220, 220 ).modify_alpha( 255 * anim_1.value ), sdk::drawing::font_4, this->name.c_str( ), false );

			sdk::drawing::rect( this->pos.x + space_element, this->pos.y + 19, element_width + ( x / 2 ), 23, sdk::color::col_t( 37, 37, 44 ).modify_alpha( 255 * anim_1.value ), 2 );
			sdk::drawing::rect_filled( this->pos.x + 1 + space_element, this->pos.y + 20, element_width + ( x / 2 ) - 2, 23 - 2,
									   sdk::color::col_t( 20, 20, 26 ).blend( sdk::color::col_t( 20, 20, 26 ).darker( -5 ), anim_2.value ).modify_alpha( 255 * anim_1.value ), 2 );

			std::string to_display;
			for ( int i = 0; i < items.size( ); i++ ) {
				if ( *items[ i ].value ) {
					if ( to_display != "" )
						to_display.append( ", " );

					to_display.append( items[ i ].name );
				}
			}

			if ( to_display == "" )
				to_display.append( "None" );

			auto text_size = sdk::drawing::get_text_size( to_display.c_str( ), 1 );
			if ( pos.x + 2 + text_size.x > pos.x + 200 ) {
				to_display.resize( 10 );
				to_display.append( "..." );
			}

			sdk::drawing::text( this->pos.x + space_element + 5, this->pos.y + 21,
								sdk::color::col_t( 220, 220, 220 ).blend( dependency::dep_controller->theme.accent, anim_3.value ).modify_alpha( 200 * anim_1.value ), sdk::drawing::font_4, to_display.c_str( ), false );

			sdk::drawing::text( this->pos.x + space_element + 5 + element_width + ( x / 2 ) - 30, this->pos.y + 21,
								sdk::color::col_t( 220, 220, 220 ).blend( dependency::dep_controller->theme.accent, anim_3.value ).modify_alpha( 200 * anim_1.value ), sdk::drawing::font_3, this->focused_element ? "a" : "d", false );

			/* external drawing */
			auto rect_fill = [ ]( int x, int y, int w, int h, sdk::color::col_t col, int r, bool overwrite ) -> void {
				if ( overwrite ) {
					dependency::external.rect_filled_a.insert( dependency::external.rect_filled_a.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
				} else {
					dependency::external.rect_filled.insert( dependency::external.rect_filled.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
				}
				};

			auto rect = [ ]( int x, int y, int w, int h, sdk::color::col_t col, int r, bool overwrite ) -> void {
				if ( overwrite ) {
					dependency::external.rect_a.insert( dependency::external.rect_a.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
				} else {
					dependency::external.rect.insert( dependency::external.rect.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
				}
				};

			auto image = [ ]( int x, int y, int w, int h, ImTextureID texture, sdk::color::col_t col ) -> void {
				dependency::external.image.insert( dependency::external.image.begin( ), dependency::draw_image( x, y, w, h, texture, col ) );
				};

			auto text = [ ]( int x, int y, int font_index, std::string text, sdk::color::col_t col, bool ds ) -> void {
				dependency::external.text.insert( dependency::external.text.begin( ), dependency::draw_text( x, y, text, col, font_index, ds ) );
				};


			if ( anim_3.value > 0.01f ) {
				for ( size_t i{ 0 }; i < this->items.size( ); i++ ) {
					if ( dependency::dep_input->key_pressed( VK_LBUTTON ) &&
						 dependency::dep_input->mouse_in_box( { this->pos.x + 7, this->pos.y + 45 + ( i * 23 ) }, { ( float )( element_width + ( x / 2 ) ), 23 } ) ) {

						*items[ i ].value = !*items[ i ].value;
					}

					auto anim_4 = animation_controller.get( this->items[ i ].name.c_str( ) + std::to_string( i ) + "#selected" + animation_controller.current_child );
					anim_4.adjust( anim_4.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) && ( dependency::dep_input->mouse_in_box( { this->pos.x + 7, this->pos.y + 45 + ( i * 23 ) }, { ( float )( element_width + ( x / 2 ) ), 23 } ) ) ? 1.f : -1.f ) );

					sdk::color::col_t color = *items[ i ].value ? dependency::dep_controller->theme.accent.modify_alpha( 170 * anim_3.value ) :
						sdk::color::col_t( 220, 220, 220 ).blend( dependency::dep_controller->theme.accent, anim_4.value ).modify_alpha( 200 * anim_3.value );

					text( this->pos.x + space_element + 5, this->pos.y + 19 + 30 + ( i * 21 ) * anim_3.value, sdk::drawing::font_4, this->items[ i ].name.c_str( ), color, false );
				}

				rect( this->pos.x + space_element, this->pos.y + 19 + 25, element_width + ( x / 2 ), this->items.size( ) * 23 * anim_3.value, sdk::color::col_t( 37, 37, 44 ).modify_alpha( 255 * anim_1.value ), 2, true );
				rect_fill( this->pos.x + 1 + space_element, this->pos.y + 20 + 25, element_width + ( x / 2 ) - 2, ( ( this->items.size( ) * 23 ) - 2 ) * anim_3.value,
						   sdk::color::col_t( 20, 20, 26 ).modify_alpha( 255 * anim_1.value ), 2, false );
			}
			dependency::dep_controller->control_iterate[ 2 ] = state_mdropdown;
		}

		__forceinline void add_item( std::string name, bool* value ) {
			this->items.push_back( multi_dropdown_system( name, value ) );
		}
	public:
		sdk::math::vec2_t pos{};
	private:
		std::vector< multi_dropdown_system > items{};
		std::string name{};

		/* stuff */
		bool focused_element{};
		bool hovered_element{};
	};

	class colorpicker_t {
	public:
		colorpicker_t( sdk::color::col_t* color, float* hue ) {
			this->select = color;
			this->hue = hue;

			HSV new_color = rgb_to_hsv( { ( double )( ( float )select->r / 255.f ),( double )
							   ( ( float )select->g / 255.f ),( double )( ( float )select->b / 255.f ) } );

			this->cursor_pos.x = ( float )new_color.s * 158.f;
			this->cursor_pos.y = 100.f - ( ( float )new_color.v * 100.f );

			this->alpha = ( float )select->a / 255.f;
		}
	public:
		__forceinline HSV rgb_to_hsv( const RGB& In ) {
			HSV m_Result;
			double Min, Max, Delta;

			Min = In.r < In.g ? In.r : In.g;
			Min = Min < In.b ? Min : In.b;

			Max = In.r > In.g ? In.r : In.g;
			Max = Max > In.b ? Max : In.b;

			m_Result.v = Max;
			Delta = Max - Min;

			if ( Delta < 0.0001 ) {
				m_Result.s = 0;
				m_Result.h = 0;

				return m_Result;
			}

			if ( Max > 0 ) {
				m_Result.s = ( Delta / Max );
			} else {
				m_Result.s = 0;
				m_Result.h = NAN;

				return m_Result;
			}

			if ( In.r >= Max ) {
				m_Result.h = ( In.g - In.b ) / Delta;
			} else {
				if ( In.g >= Max ) {
					m_Result.h = 2 + ( In.b - In.r ) / Delta;
				} else {
					m_Result.h = 4 + ( In.r - In.g ) / Delta;
				}
			}

			m_Result.h *= 60;

			if ( m_Result.h < 0 )
				m_Result.h += 360;

			return m_Result;
		}
		__forceinline RGB hsv_to_rgb( const HSV& In ) {
			RGB m_Result;

			double HH, P, Q, T, FF;
			long i;

			if ( In.s <= 0 ) {
				m_Result.r = In.v;
				m_Result.g = In.v;
				m_Result.b = In.v;

				return m_Result;
			}

			HH = ( In.h >= 360 ? 0 : In.h ) / 60;
			i = ( long )HH;

			FF = HH - i;

			P = In.v * ( 1 - In.s );
			Q = In.v * ( 1 - ( In.s * FF ) );
			T = In.v * ( 1 - ( In.s * ( 1 - FF ) ) );

			switch ( i ) {
				case 0:
					m_Result.r = In.v;
					m_Result.g = T;
					m_Result.b = P;

					break;

				case 1:
					m_Result.r = Q;
					m_Result.g = In.v;
					m_Result.b = P;

					break;

				case 2:
					m_Result.r = P;
					m_Result.g = In.v;
					m_Result.b = T;

					break;

				case 3:
					m_Result.r = P;
					m_Result.g = Q;
					m_Result.b = In.v;

					break;

				case 4:
					m_Result.r = T;
					m_Result.g = P;
					m_Result.b = In.v;

					break;

				case 5:
				default:
					m_Result.r = In.v;
					m_Result.g = P;
					m_Result.b = Q;

					break;

			}

			return m_Result;
		}

		__forceinline void circle( ImVec2 pos, float radius, ImColor color, float thickness, bool overwrite ) {
			if ( overwrite ) {
				dependency::external.circle_a.insert( dependency::external.circle_a.begin( ), dependency::draw_circle( pos, radius, color, thickness ) );
			} else {
				dependency::external.circle.insert( dependency::external.circle.begin( ), dependency::draw_circle( pos, radius, color, thickness ) );
			}
		}

		__forceinline void circle_fill( ImVec2 pos, float radius, ImColor color, bool overwrite ) {
			if ( overwrite ) {
				dependency::external.fcircle_a.insert( dependency::external.fcircle_a.begin( ), dependency::draw_circle_fill( pos, radius, color ) );
			} else {
				dependency::external.fcircle.insert( dependency::external.fcircle.begin( ), dependency::draw_circle_fill( pos, radius, color ) );
			}
		}

		__forceinline void rounded( int x, int y, int w, int h, int r, bool overwrite ) {
			if ( overwrite ) {
				dependency::external.t_rect_a.insert( dependency::external.t_rect_a.begin( ), dependency::draw_thick_rect( x, y, w, h, r ) );
			} else {
				dependency::external.t_rect.insert( dependency::external.t_rect.begin( ), dependency::draw_thick_rect( x, y, w, h, r ) );
			}
		}

		void grd( int x, int y, int w, int h, sdk::color::col_t c, sdk::color::col_t c2, bool a, bool overwrite ) {
			if ( overwrite ) {
				dependency::external.gradient_a.insert( dependency::external.gradient_a.begin( ), dependency::draw_gradient( x, y, w, h, c, c2, a ) );
			} else {
				dependency::external.gradient.insert( dependency::external.gradient.begin( ), dependency::draw_gradient( x, y, w, h, c, c2, a ) );
			}
		}

		__forceinline void add_grade( int x, int y, int rad, sdk::color::col_t c, sdk::color::col_t c2, float thickness, bool overwrite ) {
			circle_fill( ImVec2( x, y ), rad, c.convert( ), overwrite );
			circle( ImVec2( x, y ), rad, c2.convert( ), thickness, overwrite );
		}

		__forceinline void think( ) {
			if ( dependency::dep_controller->is_posible( ) && dependency::dep_input->mouse_in_box( { this->pos.x, this->pos.y }, { 13, 13 } ) 
				 && dependency::dep_input->key_pressed( VK_LBUTTON ) && !dependency::dep_controller->control_iterate[ 3 ] ) {
				state_color = !state_color;
				str_idx_color = dependency::dep_controller->get_control_index( );
			}

			if ( str_idx_color == dependency::dep_controller->get_control_index( ) ) {
				/* we can now use this shit */
				this->focused_element = state_color;
			}

			if ( str_idx_color == dependency::dep_controller->get_control_index( ) && !dependency::dep_input->mouse_in_box( { this->pos.x, this->pos.y }, { 190, 206 + 17 } )
				 && dependency::dep_input->key_pressed( VK_LBUTTON ) ) {
				state_color = false;
			}

			dependency::dep_controller->control_iterate[ 0 ] = state_color;
			dependency::dep_controller->dependency.col_opened = this->focused_element;
			dependency::dep_controller->control_state[ dependency::dependency_controller::color_selection ][ dependency::dep_controller->get_control_index( ) ] = this->focused_element;

			/* position */
			int w = 188;
			int h = 204;
			int x = this->pos.x;
			int y = this->pos.y + 17;

			if ( this->focused_element ) {
				if ( dependency::dep_input->mouse_in_box( sdk::math::vec2_t( x + 13, y + 13 ), sdk::math::vec2_t( 158, 100 ) ) && GetAsyncKeyState( VK_LBUTTON ) ) {
					this->modifying_hue = true;

					this->cursor_pos.x = dependency::dep_input->get_mouse_position( ).x - ( x + 13 );
					this->cursor_pos.y = dependency::dep_input->get_mouse_position( ).y - ( y + 23 );

					if ( cursor_pos.x < 0.f )
						cursor_pos.x = 0.f;

					if ( cursor_pos.x > 158.f )
						cursor_pos.x = 158.f;

					if ( cursor_pos.y < 0.f )
						cursor_pos.y = 0.f;

					if ( cursor_pos.y > 100.f )
						cursor_pos.y = 100.f;

					float saturation = cursor_pos.x / 158.f;
					float bright = 1.f - ( cursor_pos.y / 100.f );

					RGB new_color = hsv_to_rgb( { *this->hue * 360.f, saturation, bright } );
					*this->select = sdk::color::col_t( ( int )( new_color.r * 255.f ), ( int )( new_color.g * 255.f ), ( int )( new_color.b * 255.f ), this->select->a );

				}  else if ( dependency::dep_input->mouse_in_box( sdk::math::vec2_t( x + 13, y + 13 + 105 ), sdk::math::vec2_t( 158, 14 ) ) && GetAsyncKeyState( VK_LBUTTON ) ) {
					this->modifing_hsv = true;
				
					*this->hue = ( dependency::dep_input->get_mouse_position( ).x - ( x + 13 ) ) / 158.f;

					float saturation = cursor_pos.x / 158.f;
					float bright = 1.f - ( cursor_pos.y / 100.f );

					RGB new_color = hsv_to_rgb( { *this->hue * 360.f, saturation, bright } );
					*this->select = sdk::color::col_t( ( int )( new_color.r * 255.f ), ( int )( new_color.g * 255.f ), ( int )( new_color.b * 255.f ), this->select->a );
				} 
				else if ( dependency::dep_input->mouse_in_box( sdk::math::vec2_t( x + 13, y + 105 + 37 - 5 ), sdk::math::vec2_t( 158, 14 ) ) && GetAsyncKeyState( VK_LBUTTON ) ) {
					this->modifieng_alpha = true;
				
					this->alpha = ( dependency::dep_input->get_mouse_position( ).x - ( x + 13 ) ) / 158.0f;
				
					if ( alpha < 0.f )
						alpha = 0.f;
				
					if ( alpha > 1.f )
						alpha = 1.f;
				
					if ( alpha > 0.99f )
						alpha = 1.f;
				
					*this->select = sdk::color::col_t( this->select->r, this->select->g, this->select->b, ( this->alpha * 255.f ) );
				}
				
				if ( dependency::dep_input->mouse_in_box( sdk::math::vec2_t( x + 13, y + 13 ), sdk::math::vec2_t( 158, 100 ) ) && !GetAsyncKeyState( VK_LBUTTON ) ) {
					this->modifying_hue = false;
				} 
				else if ( dependency::dep_input->mouse_in_box( sdk::math::vec2_t( x + 13, y + 13 + 105 ), sdk::math::vec2_t( 158, 14 ) ) && !GetAsyncKeyState( VK_LBUTTON ) ) {
					this->modifing_hsv = false;
		
				} 
				else if ( dependency::dep_input->mouse_in_box( sdk::math::vec2_t( x + 13, y + 105 + 37 - 5 ), sdk::math::vec2_t( 158, 14 ) ) && !GetAsyncKeyState( VK_LBUTTON ) ) {
					this->modifieng_alpha = false;
				}

				int y_button = y + 105 + 37 - 5 + 14 + 10;
				/* copy */
				if ( dependency::dep_input->mouse_in_box( sdk::math::vec2_t( x + 13, y_button ), sdk::math::vec2_t( 67, 26 ) ) && dependency::dep_input->key_pressed( VK_LBUTTON ) ) {
					dependency::dep_controller.get( )->dependency.copied_color = *this->select;
				} 

				/* pasted */
				if ( dependency::dep_input->mouse_in_box( sdk::math::vec2_t( x + 13 + ( ( 67 * 2 ) / 2 ) + 24, y_button ), sdk::math::vec2_t( 67, 26 ) ) && GetAsyncKeyState( VK_LBUTTON ) ) {
					*this->select = dependency::dep_controller.get( )->dependency.copied_color;
				} 
			}
		}

		__forceinline void draw( ) {
			/* size:
				x: 13
				y: 13
			*/

			/* animations */
			auto anim_1 = animation_controller.get( std::to_string( dependency::dep_controller->get_control_index( ) ) + "#colorpicker" + animation_controller.current_child );
			anim_1.adjust( anim_1.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			auto anim_2 = animation_controller.get( std::to_string( dependency::dep_controller->get_control_index( ) ) + "#colorpicker_focused" + animation_controller.current_child );
			anim_2.adjust( anim_2.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) && this->focused_element ? 1.f : -1.f ) );

			auto modify_hue = animation_controller.get( std::to_string( dependency::dep_controller->get_control_index( ) ) + "#modify" + animation_controller.current_child );
			modify_hue.adjust( modify_hue.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( this->modifying_hue && dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			auto modify_hsv = animation_controller.get( std::to_string( dependency::dep_controller->get_control_index( ) ) + "#modify_hsv" + animation_controller.current_child );
			modify_hsv.adjust( modify_hsv.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( this->modifing_hsv && dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			auto modify_apha = animation_controller.get( std::to_string( dependency::dep_controller->get_control_index( ) ) + "#modify_alpha" + animation_controller.current_child );
			modify_apha.adjust( modify_apha.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( this->modifieng_alpha && dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			auto copy_col = animation_controller.get( std::to_string( dependency::dep_controller->get_control_index( ) ) + "#copy_col" + animation_controller.current_child );
			copy_col.adjust( copy_col.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( this->copied && dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			auto paste_col = animation_controller.get( std::to_string( dependency::dep_controller->get_control_index( ) ) + "#paste_col" + animation_controller.current_child );
			paste_col.adjust( paste_col.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( this->pasted && dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			sdk::drawing::rect_filled( this->pos.x, this->pos.y, 13, 13, this->select->modify_alpha( anim_1.value * 255 ), 2 );

			/* position */
			int w = 188;
			int h = 204;
			int x = this->pos.x;
			int y = this->pos.y + 17;

			/* external drawing */
			auto rect_fill = [ ]( int x, int y, int w, int h, sdk::color::col_t col, int r, bool overwrite ) -> void {
				if ( overwrite ) {
					dependency::external.rect_filled_a.insert( dependency::external.rect_filled_a.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
				} else {
					dependency::external.rect_filled.insert( dependency::external.rect_filled.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
				}
				};

			auto rect = [ ]( int x, int y, int w, int h, sdk::color::col_t col, int r, bool overwrite ) -> void {
				if ( overwrite ) {
					dependency::external.rect_a.insert( dependency::external.rect_a.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
				} else {
					dependency::external.rect.insert( dependency::external.rect.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
				}
				}; 

			auto text = [ ]( int x, int y, int font_index, std::string text, sdk::color::col_t col, bool ds ) -> void {
				dependency::external.text.insert( dependency::external.text.begin( ), dependency::draw_text( x, y, text, col, font_index, ds ) );
				};

			if ( anim_2.value > 0.0f ) {
				rect_fill( x - 1, y - 1, 188, 204, dependency::dep_controller.get( )->theme.backround.modify_alpha( anim_2.value * 255 ), 1, false );
				rect( x - 1, y - 1, 188, 204, dependency::dep_controller.get( )->theme.outline.modify_alpha( anim_2.value * 255 ), 1, true );

				/* this is hue picker */
				RGB hue_color_t = hsv_to_rgb( { ( double )*this->hue * 360.f, 1.f, 1.f } );

				/* hsv */
				grd( x + 13, y + 13, 158, 100 , sdk::color::col_t( ).modify_alpha( anim_2.value * 255 ),
					 sdk::color::col_t( ( int )( hue_color_t.r * 255.0f ), ( int )( hue_color_t.g * 255.0f ), ( int )( hue_color_t.b * 255.0f ) ).modify_alpha( anim_2.value * 255 ), false, false );

				grd( x + 13, y + 13, 158, 100, sdk::color::col_t( 0, 0, 0, 0 ),
					 sdk::color::col_t( 0, 0, 0, 255 ).modify_alpha( anim_2.value * 255 ), true , true);

				rounded( x + 13, y + 13, 158, 100, 3, true );

				if ( modify_hue.value > 0.f ) {
					draw_list->PushClipRect( { ( float )x + 17, ( float )y + 17 }, { ( float )x + 154.f, ( float )y + 96.f }, true );

					add_grade( x + 13 + ( 158.f * ( this->cursor_pos.x / 158.f ) ), y + 20 + ( 100.f * ( this->cursor_pos.y / 100.f ) ),
							   5 * modify_hue.value, this->select->modify_alpha( anim_2.value * 255 ), sdk::color::col_t( 0, 0, 0 ).modify_alpha( anim_2.value * 255 ), 2, true );

					draw_list->PopClipRect( );
				}

				/* hue bar */
				for ( int it = 0; it < 6; it++ ) {
					grd( x + 13 + ( 26.33333333333333 * it ), y + 13 + 105, ( it == 2 ? 28 : 26.33333333333333 ), 14, 
						 bar_colors[ it ].modify_alpha( anim_2.value * 255 ), bar_colors[ it + 1 ].modify_alpha( anim_2.value * 255 ), false, true );
				}

				rounded( x + 13, y + 13 + 105, 158, 14, 3, true );

				add_grade( x + 13 + ( 158 * *this->hue ), y + 16 + 109,
						   4 * modify_hsv.value, sdk::color::col_t( ( int )( hue_color_t.r * 255.0f ), ( int )( hue_color_t.g * 255.0f ), ( int )( hue_color_t.b * 255.0f ) ), sdk::color::col_t( 0, 0, 0 ), 2, true );

				/* alpha bar */
				grd( x + 13, y + 105 + 38 - 5, 158, 14, sdk::color::col_t( 0, 0, 0 ).modify_alpha( anim_2.value * 255 ),
					 this->select->modify_alpha( anim_2.value * this->select->a ), false, true );

				rounded( x + 13, y + 105 + 38 - 5, 158, 14, 3, true );

				add_grade( x + 13 + ( 158 * this->alpha ), y + 105 + 38 - 5 + 7,
						   4 * modify_apha.value, sdk::color::col_t( ), sdk::color::col_t( 5, 5, 5 ), 2, true );

				/* copy & paste buttons */
				int y_button = y + 105 + 37 - 5 + 14 + 10;

				rect_fill( x + 13, y_button, 67, 26, dependency::dep_controller->theme.color_1.modify_alpha( anim_2.value * 255 ), 1, true );
				rect( x + 13, y_button, 67, 26, dependency::dep_controller->theme.outline.modify_alpha( anim_2.value * 255 ), 1, true );

				rect_fill( x + 13 + ( ( 67 * 2 ) / 2 ) + 24, y_button, 67, 26, dependency::dep_controller->theme.color_1.modify_alpha( anim_2.value * 255 ), 1, true );
				rect( x + 13 + ( ( 67 * 2 ) / 2 ) + 24, y_button, 67, 26, dependency::dep_controller->theme.outline.modify_alpha( anim_2.value * 255 ), 1, true );

				text( x + 55 - 24, y_button + 3, sdk::drawing::font_4, "Copy", sdk::color::col_t( 220, 220, 220 ).modify_alpha( anim_2.value * 100 ).blend( dependency::dep_controller->theme.accent, copy_col.value ), false );
				text( x + 122, y_button + 3, sdk::drawing::font_4, "Paste", sdk::color::col_t( 220, 220, 220 ).modify_alpha( anim_2.value * 100 ).blend( dependency::dep_controller->theme.accent, copy_col.value ), false );
			}

		}
	public:
		sdk::math::vec2_t pos{}, cursor_pos{};
	private:
		bool focused_element{}, modifying_hue{}, modifing_hsv{}, modifieng_alpha{}, copied{}, pasted{};
		float* hue{};
		float alpha{};
		sdk::color::col_t* select;
	};

	class text_t {
	public:
		text_t( std::string text ) {
			this->text = text;
		}
	public:
		__forceinline void draw( ) {
			/* animations */
			auto anim_1 = animation_controller.get( this->text + std::to_string( dependency::dep_controller->get_control_index( ) ) + animation_controller.current_child );
			anim_1.adjust( anim_1.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			sdk::drawing::text( this->pos.x + space_element, this->pos.y - 3, sdk::color::col_t( 220, 220, 220 ).modify_alpha( 255 * anim_1.value ), sdk::drawing::font_4, this->text.c_str( ), false );
		}
	public:
		sdk::math::vec2_t pos{};
	private:
		std::string text{};
	};

	class keybind_t {
	public:
		keybind_t( int* value, int* bind_type ) {
			this->value = value;
			this->bind_type = bind_type;
		}
	public:
		__forceinline void think( ) {
			/* text size */
			auto text_size = sdk::drawing::get_text_size( bind_name[ dependency::dep_controller->get_control_index( ) ].c_str( ), sdk::drawing::font_4 ).x;

			/* stuff */
			static int main_size{ 8 };
			static int main_sizey{ 6 };

			/* handling */
			if ( !dependency::dep_controller->dependency.col_opened && dependency::dep_controller->is_posible( ) && GetAsyncKeyState( VK_LBUTTON )
				 && dependency::dep_input->mouse_in_box( { this->pos.x, this->pos.y }, { text_size + main_size, float( 13 + main_sizey ) } ) ) { /* not sure yet */
				if ( !state_bind )
					state_bind = true;


				str_idx_bind = dependency::dep_controller->get_control_index( );
			}

			if ( str_idx_bind == dependency::dep_controller->get_control_index( ) )
				focused = state_bind;

			if ( focused ) {
				for ( int i = 0; i < 255; i++ ) {
					if ( dependency::dep_input->key_pressed( i ) ) {
						if ( i == VK_ESCAPE ) { /* see this it might be broken*/
							*value = -1;
							str_idx_bind = -1;
							return;
						}

						*value = i;
						str_idx_bind = -1;
						return;
					}
				}
			}

			if ( focused ) {
				bind_name[ dependency::dep_controller->get_control_index( ) ] = "...";
			} else {
				if ( *value >= 0 ) {
					bind_name[ dependency::dep_controller->get_control_index( ) ] = stable_keys[ *value ];

					if ( bind_name[ dependency::dep_controller->get_control_index( ) ].c_str( ) ) {
						is_input_good = true;
					} else {
						if ( GetKeyNameText( *value << 16, buffer_bind, 127 ) ) {
							bind_name[ dependency::dep_controller->get_control_index( ) ] = buffer_bind;
							is_input_good = true;
						}
					}
				}

				if ( !is_input_good ) {
					bind_name[ dependency::dep_controller->get_control_index( ) ] = "None";
				}
			}
		}

		__forceinline void think_list( ) {
			/* text size */
			auto text_size = sdk::drawing::get_text_size( bind_name[ dependency::dep_controller->get_control_index( ) ].c_str( ), sdk::drawing::font_4 ).x;

			/* stuff */
			static int main_size{ 8 };
			static int main_sizey{ 6 };

			/* input */
			if ( state_list && stored_index_list == dependency::dep_controller->get_control_index( ) && dependency::dep_input->key_pressed( VK_LBUTTON )
				 && !dependency::dep_input->mouse_in_box( { this->pos.x, this->pos.y + 15 }, { 80, ( float )( bind_list.size( ) * 20 ) } ) ) {
				state_list = !state_list;
				stored_index_list = -1;
			}

			/* input */
			if ( !dependency::dep_controller->control_iterate[ 1 ] && !dependency::dep_controller->control_iterate[ 0 ] &&
				 !dependency::dep_controller->control_iterate[ 2 ] && dependency::dep_controller->is_posible( ) && time_list == -1 &&
				 dependency::dep_input->mouse_in_box( { this->pos.x, this->pos.y }, { 25, 12 } ) && dependency::dep_input->key_pressed( VK_RBUTTON ) ) {
				state_list = !state_list;
				stored_index_list = dependency::dep_controller->get_control_index( );
			}

			if ( time_list >= 12 )
				time_list = 12;

			else if ( time_list <= 0 )
				time_list = 0;

			if ( state_list )
				time_list++;
			else
				time_list--;

			dependency::dep_controller->control_opened[ dependency::dependency_controller::bind ] = time_list;
			if ( stored_index_list == dependency::dep_controller->get_control_index( ) )
				this->focused_list = state_list;

			dependency::dep_controller->control_state[ dependency::dependency_controller::bind ][ dependency::dep_controller->get_control_index( ) ] = this->focused_list;
		}

		__forceinline void draw( ) {
			/* animations */
			auto anim_1 = animation_controller.get( std::to_string( dependency::dep_controller->get_control_index( ) ) + "#bind" + animation_controller.current_child );
			anim_1.adjust( anim_1.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			/* update name */
			if ( bind_name[ dependency::dep_controller->get_control_index( ) ] == "" )
				bind_name[ dependency::dep_controller->get_control_index( ) ] = "None";

			/* text size */
			auto text_size = sdk::drawing::get_text_size( bind_name[ dependency::dep_controller->get_control_index( ) ].c_str( ), sdk::drawing::font_4 ).x;

			/* drawing */
			static int main_size{ 8 };
			static int main_sizey{ 6 };

			sdk::drawing::rect( this->pos.x - ( main_size / 2 ), this->pos.y - ( main_sizey / 2 ), text_size + main_size, 13 + main_sizey, sdk::color::col_t( 37, 37, 44 ).modify_alpha( 255 * anim_1.value ), 2 );
			sdk::drawing::rect_filled( this->pos.x + 1 - ( main_size / 2 ), this->pos.y - ( main_sizey / 2 ) + 1, text_size + main_size - 2, 13 + main_sizey - 2, sdk::color::col_t( 20, 20, 26 ).modify_alpha( 255 * anim_1.value ), 2 );

			sdk::drawing::text( this->pos.x, this->pos.y - 2, sdk::color::col_t( 220, 220, 220 ).modify_alpha( anim_1.value * 170 ), sdk::drawing::font_4, bind_name[ dependency::dep_controller->get_control_index( ) ].c_str( ), false );
		}

		__forceinline void draw_obj_list( ) {
			/* text size */
			auto text_size = sdk::drawing::get_text_size( bind_name[ dependency::dep_controller->get_control_index( ) ].c_str( ), sdk::drawing::font_4 ).x;

			/* stuff */
			static int main_size{ 8 };
			static int main_sizey{ 6 };

			/* animations */
			auto anim_1 = animation_controller.get( std::to_string( dependency::dep_controller->get_control_index( ) ) + "#bind_list" + animation_controller.current_child );
			anim_1.adjust( anim_1.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) && this->focused_list ? 1.f : -1.f ) );

			if ( anim_1.value > 0.f ) {
				/* external drawing */
				auto rect_fill = [ ]( int x, int y, int w, int h, sdk::color::col_t col, int r, bool overwrite ) -> void {
					if ( overwrite ) {
						dependency::external.rect_filled_a.insert( dependency::external.rect_filled_a.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
					} else {
						dependency::external.rect_filled.insert( dependency::external.rect_filled.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
					}
					};

				auto rect = [ ]( int x, int y, int w, int h, sdk::color::col_t col, int r, bool overwrite ) -> void {
					if ( overwrite ) {
						dependency::external.rect_a.insert( dependency::external.rect_a.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
					} else {
						dependency::external.rect.insert( dependency::external.rect.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
					}
					};

				auto text = [ ]( int x, int y, int font_index, std::string text, sdk::color::col_t col, bool ds ) -> void {
					dependency::external.text.insert( dependency::external.text.begin( ), dependency::draw_text( x, y, text, col, font_index, ds ) );
					};

				float text_size_x;

				/* for list */
				for ( size_t i{ 0 }; i < bind_list.size( ); i++ ) {
					text_size_x = sdk::drawing::get_text_size( bind_list[ i ].c_str( ), sdk::drawing::font_4 ).x;

					if ( dependency::dep_input->key_pressed( VK_LBUTTON ) &&
						 dependency::dep_input->mouse_in_box( { this->pos.x, this->pos.y + 15 + ( i * 20 ) }, { 80, 20 } ) ) {
						*bind_type = i;
						state_list = !state_list;
						stored_index_list = -1;
					}

					/* animations */
					auto anim_2 = animation_controller.get( std::to_string( i ) + "#binad" + animation_controller.current_child );
					anim_2.adjust( anim_2.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * 
								   ( dependency::dep_controller->get_menu_state( ) && ( dependency::dep_input->mouse_in_box( { this->pos.x, this->pos.y + 15 + ( i * 20 ) }, { 80, 20 } ) || ( *bind_type == i ) ) ? 1.f : -1.f ) );

					sdk::color::col_t c = sdk::color::col_t( 220, 220, 220 ).blend( dependency::dep_controller->theme.accent, anim_2.value );
		
					text( this->pos.x + 5, this->pos.y + 19 + ( 20 * i ), sdk::drawing::font_4, bind_list[ i ].c_str( ), c.modify_alpha( anim_1.value * 220 ), false );
				}

				rect_fill( this->pos.x, this->pos.y + 18, 80, ( bind_list.size( ) * 20 ) * anim_1.value, dependency::dep_controller->theme.backround.modify_alpha( anim_1.value * 255 ), 2, false );
				rect_fill( this->pos.x, this->pos.y + 18, 80, ( bind_list.size( ) * 20 ) * anim_1.value, dependency::dep_controller->theme.outline.modify_alpha( anim_1.value * 255 ), 2, true );
			}
			dependency::dep_controller->control_iterate[ 3 ] = state_list;
		}
	public:
		sdk::math::vec2_t pos{};
	private:
		int* value{};
		int* bind_type{};

		bool focused{}, focused_list{};
	};

	class button_t {
	public:
		button_t( std::string name, std::function< void( ) > function ) {
			this->name = name;
			this->function = function;
		}
	public:
		__forceinline void think( ) {
			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;

			/* update key */
			if ( !dependency::dep_input->key_pressed( VK_LBUTTON ) && dependency::dep_input->mouse_in_box( { this->pos.x + space_element, this->pos.y }, 
																										   { element_width + ( x / 2 ), 23 } ) ) {
				if ( state_button )
					this->function( );

				state_button = false;
			} 

			/* check colorpicker */
			if ( !dependency::dep_controller->dependency.col_opened && dependency::dep_controller->is_posible( ) &&
				 dependency::dep_input->key_pressed( VK_LBUTTON ) && dependency::dep_input->mouse_in_box( { this->pos.x + space_element, this->pos.y }, 
																										  { element_width + ( x / 2 ), 23 } ) ) {
				state_button = true;

				str_indx_button = dependency::dep_controller->get_control_index( );
			}

			if ( str_indx_button == dependency::dep_controller->get_control_index( ) )
				this->focused_element = state_button;
		}

		__forceinline void draw( ) {
			/* animations */
			auto anim_1 = animation_controller.get( this->name + std::to_string( dependency::dep_controller->get_control_index( ) ) + animation_controller.current_child );
			anim_1.adjust( anim_1.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;

			/* animations */
			auto anim_2 = animation_controller.get( this->name + std::to_string( dependency::dep_controller->get_control_index( ) ) + "#chover" + animation_controller.current_child );
			anim_2.adjust( anim_2.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) && dependency::dep_input->mouse_in_box(
				{ this->pos.x + space_element, this->pos.y }, { element_width + ( x / 2 ), 23 }
			) ? 1.f : -1.f ) );

			/* rendering */
			sdk::drawing::rect( this->pos.x + space_element, this->pos.y, element_width + ( x / 2 ), 23, sdk::color::col_t( 37, 37, 44 ).modify_alpha( 255 * anim_1.value ), 2 );
			sdk::drawing::rect_filled( this->pos.x + 1 + space_element, this->pos.y + 1, element_width + ( x / 2 ) - 2, 23 - 2,
									   sdk::color::col_t( 20, 20, 26 ).blend( sdk::color::col_t( 20, 20, 26 ).darker( -5 ), anim_2.value ).modify_alpha( 255 * anim_1.value ), 2 );

			int text_si = sdk::drawing::get_text_size( this->name.c_str( ), sdk::drawing::font_4 ).x;

			sdk::drawing::text( this->pos.x + space_element + ( element_width + ( x / 2 ) ) * 0.5 - text_si * 0.5, this->pos.y + 2,
								sdk::color::col_t( 220, 220, 220 ).modify_alpha( 255 * anim_1.value ), sdk::drawing::font_4, this->name.c_str( ), false );
		}
	public:
		sdk::math::vec2_t pos{};
	private:
		bool focused_element{};
		std::string name{};
		std::function< void( ) > function = { };
	};

	class listbox_t {
	public:
		listbox_t( std::string name, int* value, std::vector<std::string>& items, std::string& filter ) {
			this->name = name;
			this->selected = value;
			this->items = &items;
			this->filter = &filter;
		}
	public:
		__forceinline std::string to_lower( std::string string ) {
			std::transform( string.begin( ), string.end( ), string.begin( ), ( int( * )( int ) )::tolower );
			return string;
		}

		__forceinline void get_items( ) {
			if ( this->lower_items.empty( ) || this->lower_items.size( ) != this->items->size( ) ) {
				this->lower_items.clear( );
				for ( int i = 0; i < this->items->size( ); i++ ) {
					std::string temp_s = ( *this->items )[ i ];
					this->lower_items.push_back( temp_s );
				}
			}

			if ( !this->filter && temp.empty( ) || this->temp.size( ) != this->items->size( ) && ( !this->filter || this->filter->empty( ) ) ) {
				this->temp.clear( );
				for ( int i = 0; i < this->items->size( ); i++ ) {
					this->temp.emplace_back( ( *this->items )[ i ], i );
				}
			}

			/* assume str vars */
			if ( !this->filter )
				return;

			if ( this->prev_filter == *this->filter )
				return;

			this->prev_filter = *this->filter;
			const auto l_filter = to_lower( *this->filter );

			/* convert the first letter */
			if ( !this->filter->empty( ) ) {
				if ( std::islower( ( *this->filter )[ 0 ] ) ) {
					( *this->filter )[ 0 ] = std::toupper( ( *this->filter )[ 0 ] );
				}
			}

			this->temp.clear( );

			for ( int i = 0; i < this->items->size( ); i++ ) {
				if ( this->filter != nullptr ) {
					std::string temp_s = this->items->at( i ); /* use the original case-sensitive version of the item */
					const auto l_item = to_lower( temp_s ); /* convert the item to lowercase for comparison */
					if ( !( l_item.find( l_filter ) != std::string::npos ) )
						continue;
				}

				this->temp.emplace_back( this->items->at( i ), i );
			}
		}

		__forceinline void update_item( ) {
			this->get_items( );
		}

		__forceinline void scroll( int limit ) {
			/* resize */
			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;

			static size_t lost[ 1000 ];
			if ( lost[ dependency::dep_controller->get_control_index( ) ] != this->temp.size( ) ) {
				scroll_pos_list[ dependency::dep_controller->get_control_index( ) ] = 0;
				lost[ dependency::dep_controller->get_control_index( ) ] = this->temp.size( );
			}

			bool hovered = dependency::dep_input->mouse_in_box( { this->pos.x + space_element, this->pos.y }, { element_width + ( x / 2 ), ( float )150 } );

			if ( !scroll_state_list[ dependency::dep_controller->get_control_index( ) ] && this->temp.size( ) > 7 ) {
				float ratio = 7 / static_cast< float >( this->temp.size( ) );
				float some_shit = this->pos.y;

				float size_ratio = 7 / static_cast< float >( this->temp.size( ) );
				size_ratio *= ( float )( element_width + ( x / 2 ) );

				float height_delta = some_shit + size_ratio - ( ( float )( element_width + ( x / 2 ) ) );
				if ( height_delta > 0 )
					some_shit -= height_delta;

				float pos_ratio = some_shit / ( float )( element_width + ( x / 2 ) );
				float m_prikol = pos_ratio * this->temp.size( );

				if ( dependency::dep_input->get_mouse_wheel( ) != 0 && hovered ) {
					scroll_pos_list[ dependency::dep_controller->get_control_index( ) ] += dependency::dep_input->get_mouse_wheel( ) * ( -1 );
					dependency::dep_input->set_mouse_wheel( 0 );

					if ( scroll_pos_list[ dependency::dep_controller->get_control_index( ) ] < 0 )
						scroll_pos_list[ dependency::dep_controller->get_control_index( ) ] = 0;

					if ( scroll_pos_list[ dependency::dep_controller->get_control_index( ) ] > static_cast< int >( m_prikol ) )
						scroll_pos_list[ dependency::dep_controller->get_control_index( ) ] = static_cast< int >( m_prikol );
				}

				/* filled rectangle */
				sdk::drawing::rect_filled( pos.x + 14 + element_width + ( x / 2 ), pos.y + 3 + scroll_pos_list[ dependency::dep_controller->get_control_index( ) ],
										   3, 150 - m_prikol - 6, sdk::color::col_t( 78, 77, 84 ), 22 );
			}
		}

		__forceinline void think( ) {
			/* resize */
			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;

			/* animations */
			auto anim_1 = animation_controller.get( this->name + std::to_string( dependency::dep_controller->get_control_index( ) ) + animation_controller.current_child );
			anim_1.adjust( anim_1.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			if ( !this->temp.empty( ) ) {
				int drawn_items = 0;
				int items_to_draw = 7;

				for ( int i{ scroll_pos_list[ dependency::dep_controller->get_control_index( ) ] }; ( i < this->temp.size( ) && drawn_items < items_to_draw ); i++ ) {
					bool hovered = dependency::dep_input->mouse_in_box(
						{ this->pos.x + space_element + 7, this->pos.y + 6 + ( 20 * drawn_items ) },
						{ element_width + ( x / 2 ), 20 }
					);

					if ( hovered && dependency::dep_input->key_pressed( VK_LBUTTON ) && dependency::dep_controller->is_posible( ) 
						 && !dependency::dep_controller->control_iterate[ 0 ] && !dependency::dep_controller->dependency.col_opened )
						*this->selected = this->temp[ i ].second;

					/* animations */
					auto anim_2 = animation_controller.get( this->name + std::to_string( i ) + "sa#lut" + animation_controller.current_child );
					anim_2.adjust( anim_2.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) && ( hovered || *this->selected == this->temp[ i ].second ) ? 1.f : -1.f ) );


					sdk::drawing::text( this->pos.x + space_element + 7, this->pos.y + 6 + ( 20 * drawn_items ), sdk::color::col_t( 220, 220, 220 ).blend(
						dependency::dep_controller->theme.accent, anim_2.value
					).modify_alpha( 200 * anim_1.value ), sdk::drawing::font_4, this->temp[ i ].first.c_str( ) );

					drawn_items++;
				}

				/* handle scrolling */
				this->scroll( 0 );
			}	
		}
		
		__forceinline void think_filter_bar( ) {
			/* resize */
			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;

			if ( state_textbar_list && str_indx_textbar_list == dependency::dep_controller->get_control_index( ) && dependency::dep_input->key_pressed( VK_LBUTTON ) ) {
				state_textbar_list = !state_textbar_list;
				str_indx_textbar_list = -1;
			}

			if ( !dependency::dep_controller->dependency.col_opened && dependency::dep_controller->is_posible( ) && dependency::dep_input->key_pressed( VK_LBUTTON ) && 
				 dependency::dep_input->mouse_in_box( { this->pos.x + space_element, this->pos.y }, { ( float )( element_width + ( x / 2 ) ), 23 } ) ) {
				state_textbar_list = !state_textbar_list;
				str_indx_textbar_list = dependency::dep_controller->get_control_index( );
			}

			if ( str_indx_textbar_list == dependency::dep_controller->get_control_index( ) )
				this->focused_bar = state_textbar_list;

			dependency::dep_controller->control_state[ dependency::dependency_controller::handling_t::textbox ][ dependency::dep_controller->get_control_index( ) ] = this->focused_bar;

			if ( this->focused_bar ) {
				std::string str = *this->filter;

				for ( int i = 0; i < 255; i++ ) {
					if ( dependency::dep_input->key_pressed( i ) ) {
						if ( i == VK_ESCAPE || i == VK_RETURN || i == VK_INSERT ) {
							str_indx_textbar_list = -1;
							return;
						}

						if ( strlen( str.c_str( ) ) != 0 ) {
							if ( GetAsyncKeyState( VK_BACK ) ) {
								*this->filter = str.substr( 0, strlen( str.c_str( ) ) - 1 );
							}
						}

						if ( strlen( str.c_str( ) ) < 27 && i != NULL && upper_case[ i ] != nullptr ) {
							if ( GetAsyncKeyState( VK_SHIFT ) ) {
								*this->filter = str + upper_case[ i ];
							} else {
								*this->filter = str + lower_case[ i ];
							}

							return;
						}

						if ( strlen( str.c_str( ) ) < 27 && i == 32 ) {
							*this->filter = str + " ";
							return;
						}
					}
				}
			}
		}

		__forceinline void paint( ) {
			/* resize */
			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;

			/* animations */
			auto anim_1 = animation_controller.get( this->name + std::to_string( dependency::dep_controller->get_control_index( ) ) + animation_controller.current_child );
			anim_1.adjust( anim_1.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			/* some vars */
			int height = 150 + 23;

		
			/* paint main */
			sdk::drawing::rect( this->pos.x + space_element, this->pos.y, element_width + ( x / 2 ), height, sdk::color::col_t( 37, 37, 44 ).modify_alpha( 255 * anim_1.value ), 2 );
			sdk::drawing::rect_filled( this->pos.x + 1 + space_element, this->pos.y + 1, element_width + ( x / 2 ) - 2, height - 2,
									   sdk::color::col_t( 20, 20, 26 ).modify_alpha( 255 * anim_1.value ), 2 );

			/* text input */
			sdk::drawing::rect( this->pos.x + space_element, this->pos.y, element_width + ( x / 2 ), 23, sdk::color::col_t( 37, 37, 44 ).modify_alpha( 255 * anim_1.value ), 2 );
			sdk::drawing::rect_filled( this->pos.x + 1 + space_element, this->pos.y + 1, element_width + ( x / 2 ) - 2, 23 - 2,
									   sdk::color::col_t( 20, 20, 26 ).modify_alpha( 255 * anim_1.value ), 2 );

			std::string to_draw = *this->filter;

			if ( GetTickCount64( ) >= blink )
				blink = GetTickCount64( ) + 800;

			if ( this->focused_bar && GetTickCount64( ) > ( blink - 400 ) )
				to_draw += "|";
			
			std::string text{};
			if ( !this->focused_bar && to_draw.empty( ) ) {
				text = "waiting for user's input";
			} else {
				text = to_draw;
			}

			sdk::drawing::text( this->pos.x + space_element + 5, this->pos.y + 2, sdk::color::col_t( 220, 220, 220 ).modify_alpha( 160 * anim_1.value ), sdk::drawing::font_4, to_draw.c_str( ) );

			/* create space */
			this->pos.y += 23;

			/* we are going to init think in our paint function */
			this->think( );
		}
	public:
		sdk::math::vec2_t pos{};
	private:
		std::string name{}, prev_filter{};
		std::string* filter{};
		std::vector <std::string>* items{};
		std::vector<std::string> lower_items{};
		std::vector< std::pair<std::string, int>> temp;
		int* selected{};

		bool focused_bar{};
	};

	struct textbox_t {
	public:
		textbox_t( std::string name, std::string* label ) {
			this->name = name;
			this->label = label;
		}
	public:
		__forceinline void think( ) {
			/* resize */
			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;

			if ( state_textbar_list2 && str_indx_textbar_list2 == dependency::dep_controller->get_control_index( ) && dependency::dep_input->key_pressed( VK_LBUTTON ) ) {
				state_textbar_list2 = !state_textbar_list2;
				str_indx_textbar_list2 = -1;
			}

			if ( !dependency::dep_controller->dependency.col_opened && dependency::dep_controller->is_posible( ) && dependency::dep_input->key_pressed( VK_LBUTTON ) &&
				 dependency::dep_input->mouse_in_box( { this->pos.x + space_element, this->pos.y + 20 }, { ( float )( element_width + ( x / 2 ) ), 23 } ) ) {
				state_textbar_list2 = !state_textbar_list2;
				str_indx_textbar_list2 = dependency::dep_controller->get_control_index( );
			}

			if ( str_indx_textbar_list2 == dependency::dep_controller->get_control_index( ) )
				this->focused_bar = state_textbar_list2;

			dependency::dep_controller->control_state[ dependency::dependency_controller::handling_t::textbox_main ][ dependency::dep_controller->get_control_index( ) ] = this->focused_bar;

			if ( this->focused_bar ) {
				std::string str = *this->label;

				for ( int i = 0; i < 255; i++ ) {
					if ( dependency::dep_input->key_pressed( i ) ) {
						if ( i == VK_ESCAPE || i == VK_RETURN || i == VK_INSERT ) {
							str_indx_textbar_list2 = -1;
							return;
						}

						if ( strlen( str.c_str( ) ) != 0 ) {
							if ( GetAsyncKeyState( VK_BACK ) ) {
								*this->label = str.substr( 0, strlen( str.c_str( ) ) - 1 );
							}
						}

						if ( strlen( str.c_str( ) ) < 27 && i != NULL && upper_case[ i ] != nullptr ) {
							if ( GetAsyncKeyState( VK_SHIFT ) ) {
								*this->label = str + upper_case[ i ];
							} else {
								*this->label = str + lower_case[ i ];
							}

							return;
						}

						if ( strlen( str.c_str( ) ) < 27 && i == 32 ) {
							*this->label = str + " ";
							return;
						}
					}
				}
			}
		}

		__forceinline void paint( ) {
			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;

			/* animations */
			auto anim_1 = animation_controller.get( this->name + std::to_string( dependency::dep_controller->get_control_index( ) ) + animation_controller.current_child );
			anim_1.adjust( anim_1.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			/* animations */
			auto anim_2 = animation_controller.get( this->name + std::to_string( dependency::dep_controller->get_control_index( ) ) + "#choverdrpptext" + animation_controller.current_child );
			anim_2.adjust( anim_2.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) && dependency::dep_input->mouse_in_box(
				{ this->pos.x + space_element, this->pos.y + 20 }, { element_width + ( x / 2 ), 23 }
			) ? 1.f : -1.f ) );

			sdk::drawing::text( this->pos.x + space_element, this->pos.y - 3,
								sdk::color::col_t( 220, 220, 220 ).modify_alpha( 255 * anim_1.value ), sdk::drawing::font_4, this->name.c_str( ), false );

			sdk::drawing::rect( this->pos.x + space_element, this->pos.y + 19, element_width + ( x / 2 ), 23, sdk::color::col_t( 37, 37, 44 ).modify_alpha( 255 * anim_1.value ), 2 );
			sdk::drawing::rect_filled( this->pos.x + 1 + space_element, this->pos.y + 20, element_width + ( x / 2 ) - 2, 23 - 2,
									   sdk::color::col_t( 20, 20, 26 ).blend( sdk::color::col_t( 20, 20, 26 ).darker( -5 ), anim_2.value ).modify_alpha( 255 * anim_1.value ), 2 );

			std::string to_draw = *this->label;

			if ( GetTickCount64( ) >= blink )
				blink = GetTickCount64( ) + 800;

			if ( this->focused_bar && GetTickCount64( ) > ( blink - 400 ) )
				to_draw += "|";

			std::string text{};
			if ( !this->focused_bar && to_draw.empty( ) ) {
				text = "waiting for user's input";
			} else {
				text = to_draw;
			}

			sdk::drawing::text( this->pos.x + space_element + 5, this->pos.y + 21, sdk::color::col_t( 220, 220, 220 ).modify_alpha( 160 * anim_1.value ), sdk::drawing::font_4, to_draw.c_str( ) );

		}
	public:
		sdk::math::vec2_t pos{};
	private:
		std::string name{};
		std::string* label{};

		bool focused_bar{};
	};

	/* group */
	class group_t {
	public: /* constructor */
		group_t( std::string name, sdk::math::rect_t coordonates, int tab, int id ) { 
			this->name = name;

			/* pos */
			this->pos.x = coordonates.x;
			this->pos.y = coordonates.y;

			/* size */
			this->size.x = coordonates.w;
			this->size.y = coordonates.h;

			/* tab */
			this->tab = tab;
			this->id = id;
		}
	public: /* functions */
		__forceinline bool vis_flag( ) {
			return this->visible;
		}

		__forceinline sdk::math::vec2_t get_position( ) {
			return this->pos;
		}

		__forceinline void set_position( sdk::math::vec2_t new_pos ) {
			this->pos += new_pos; /* guh */
		}

		__forceinline void set_vis_flag( bool target ) {
			this->visible = target;
		}

		__forceinline void set_limit( ) {
			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;
			auto y = dependency::dep_controller->dependency.ry - dependency::dep_controller->dependency.ory;

			draw_list->PushClipRect( { pos.x, pos.y + 35 }, { pos.x + size.x + ( x / 2 ), pos.y + size.y + ( y / 2 ) - 10 }, true );
		}

		__forceinline bool hovered_flag( ) {
			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;
			auto y = dependency::dep_controller->dependency.ry - dependency::dep_controller->dependency.ory;


			return dependency::dep_input->mouse_in_box( this->pos, this->size + sdk::math::vec2_t( ( x / 2 ), ( y / 2 ) ) );
		}
	public:
		__forceinline void obj( checkbox_t* checkbox ) {
			if ( this->vis_flag( ) ) {
				checkbox->pos = { this->element.offset_x, this->element.offset_y[ this->id ] };

				/* we want to update the object if we hover the group */
				if ( this->hovered_flag( ) )
					checkbox->think( );
				
				/* set drawing limit */
				this->set_limit( );

				/* draw object */
				checkbox->draw( );

				/* this is the spacing between elements */
				/*
					* element_formula: element_size + 10px
				*/
				this->element.offset_y[ this->id ] += 23;
				slide_offset[ this->id ] += 23;

				/* set index */
				dependency::dep_controller->set_control_index( dependency::dep_controller->get_control_index( ) + 1 );

				/* original clip rect */
				draw_list->PopClipRect( );
			} delete checkbox;
		}

		__forceinline void obj( slider_t* slider ) {
			if ( this->vis_flag( ) ) {
				slider->pos = { this->element.offset_x, this->element.offset_y[ this->id ] };

				/* we want to update the object if we hover the group */
				if ( this->hovered_flag( ) )
					slider->think( );

				/* set drawing limit */
				this->set_limit( );

				/* draw object */
				slider->draw( );

				/* this is the spacing between elements */
				/*
					* element_formula: element_size + 10px
					* 
					* element_slider: 10px + 5px + 15px
				*/
				this->element.offset_y[ this->id ] += 40 - 2;
				slide_offset[ this->id ] += 40 - 2;

				/* set index */
				dependency::dep_controller->set_control_index( dependency::dep_controller->get_control_index( ) + 1 );

				/* original clip rect */
				draw_list->PopClipRect( );
			} delete slider;
		}

		__forceinline void obj( dropdown_t* dropdown ) {
			if ( this->vis_flag( ) ) {
				dropdown->pos = { this->element.offset_x, this->element.offset_y[ this->id ] };

				/* we want to update the object if we hover the group */
				if ( !dependency::dep_controller->control_state[ dependency::dependency_controller::combo ][ dependency::dep_controller->get_control_index( ) ] ) {
					if ( this->hovered_flag( ) ) {
						dropdown->think( );
					}
				} else if ( dependency::dep_controller->control_state[ dependency::dependency_controller::combo ][ dependency::dep_controller->get_control_index( ) ] ) {
					dropdown->think( );
				}

				/* set drawing limit */
				this->set_limit( );

				/* original clip rect */
				draw_list->PopClipRect( );

				/* do some sanity checks after setting viewport */
				if ( !dependency::dep_controller->control_state[ dependency::dependency_controller::combo ][ dependency::dep_controller->get_control_index( ) ] )
					this->set_limit( ); /* set the limit of drawing even when the open state is not true */

				/* draw object */
				dropdown->draw( );

				/* this is the spacing between elements */
				/*
					* element_formula: element_size + 10px
					*
					* element_slider: 10px + 5px + 15px
				*/
				this->element.offset_y[ this->id ] += 50;
				slide_offset[ this->id ] += 50;

				/* set index */
				dependency::dep_controller->set_control_index( dependency::dep_controller->get_control_index( ) + 1 );

				/* original clip rect */
				draw_list->PopClipRect( );
			} delete dropdown;
		}

		__forceinline void obj( multi_dropdown_t* multi_dropdown ) {
			if ( this->vis_flag( ) ) {
				multi_dropdown->pos = { this->element.offset_x, this->element.offset_y[ this->id ] };

				/* we want to update the object if we hover the group */
				if ( !dependency::dep_controller->control_state[ dependency::dependency_controller::multicombo ][ dependency::dep_controller->get_control_index( ) ] ) {
					if ( this->hovered_flag( ) ) {
						multi_dropdown->think( );
					}
				} else if ( dependency::dep_controller->control_state[ dependency::dependency_controller::multicombo ][ dependency::dep_controller->get_control_index( ) ] ) {
					multi_dropdown->think( );
				}

				/* set drawing limit */
				this->set_limit( );

				/* original clip rect */
				draw_list->PopClipRect( );

				/* do some sanity checks after setting viewport */
				if ( !dependency::dep_controller->control_state[ dependency::dependency_controller::multicombo ][ dependency::dep_controller->get_control_index( ) ] )
					this->set_limit( ); /* set the limit of drawing even when the open state is not true */

				/* draw object */
				multi_dropdown->draw( );

				/* this is the spacing between elements */
				/*
					* element_formula: element_size + 10px
					*
					* element_slider: 10px + 5px + 15px
				*/
				this->element.offset_y[ this->id ] += 50;
				slide_offset[ this->id ] += 50;

				/* set index */
				dependency::dep_controller->set_control_index( dependency::dep_controller->get_control_index( ) + 1 );

				/* original clip rect */
				draw_list->PopClipRect( );
			} delete multi_dropdown;
		}

		__forceinline void obj( colorpicker_t* colorpicker ) {
			if ( this->vis_flag( ) ) {
				/* set drawing limit */
				this->set_limit( );

				auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;
				auto y = dependency::dep_controller->dependency.ry - dependency::dep_controller->dependency.ory;

				colorpicker->pos = { this->element.offset_x + element_width + ( x / 2 ), this->element.offset_y[ this->id ] };

				/* we want to update the object if we hover the group */
				if ( !dependency::dep_controller->control_state[ dependency::dependency_controller::color_selection ][ dependency::dep_controller->get_control_index( ) ] ) {
					if ( this->hovered_flag( ) ) {
						colorpicker->think( );
					}
				} else {
					colorpicker->think( );
				}

				/* paint colorpicker */
				colorpicker->draw( );
			
				/* set index */
				dependency::dep_controller->set_control_index( dependency::dep_controller->get_control_index( ) + 1 );

				/* original clip rect */
				draw_list->PopClipRect( );
			} delete colorpicker;
		}

		__forceinline void obj( text_t* text ) {
			if ( this->vis_flag( ) ) {
				text->pos = { this->element.offset_x, this->element.offset_y[ this->id ] };

				/* set drawing limit */
				this->set_limit( );

				/* draw object */
				text->draw( );

				/* this is the spacing between elements */
				/*
					* element_formula: element_size + 10px
				*/
				this->element.offset_y[ this->id ] += 23;
				slide_offset[ this->id ] += 23;

				/* set index */
				dependency::dep_controller->set_control_index( dependency::dep_controller->get_control_index( ) + 1 );

				/* original clip rect */
				draw_list->PopClipRect( );
			} delete text;
		}

		__forceinline void obj( keybind_t* keybind ) {
			if ( this->vis_flag( ) ) {
				/* set drawing limit */
				this->set_limit( );

				auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;
				auto y = dependency::dep_controller->dependency.ry - dependency::dep_controller->dependency.ory;

				/* text size */
				auto text_size = sdk::drawing::get_text_size( bind_name[ dependency::dep_controller->get_control_index( ) ].c_str( ), sdk::drawing::font_4 ).x;

				keybind->pos = { this->element.offset_x + element_width + ( x / 2 ) - text_size + 17, this->element.offset_y[ this->id ] };

				/* update normal */
				if ( this->hovered_flag( ) ) {
					keybind->think( );
				}

				/* paint normal */
				keybind->draw( );

				/* we want to update the object if we hover the group */
				if ( !dependency::dep_controller->control_state[ dependency::dependency_controller::bind ][ dependency::dep_controller->get_control_index( ) ] ) {
					if ( this->hovered_flag( ) ) {
						keybind->think_list( );
					}
				} else {
					keybind->think_list( );
				}

				/* draw list */
				keybind->draw_obj_list( );

				/* set index */
				dependency::dep_controller->set_control_index( dependency::dep_controller->get_control_index( ) + 1 );

				/* original clip rect */
				draw_list->PopClipRect( );
			} delete keybind;
		}

		__forceinline void obj( button_t* button ) {
			if ( this->vis_flag( ) ) {
				/* set drawing limit */
				this->set_limit( );

				/* update pos */
				button->pos = { this->element.offset_x, this->element.offset_y[ this->id ] };

				/* we want to update the object if we hover the group */
				if ( this->hovered_flag( ) ) {
					button->think( );
				}

				/* draw object */
				button->draw( );

				/* this is the spacing between elements */
				/*
					* element_formula: element_size + 10px
				*/
				this->element.offset_y[ this->id ] += 33;
				slide_offset[ this->id ] += 33;

				/* set index */
				dependency::dep_controller->set_control_index( dependency::dep_controller->get_control_index( ) + 1 );

				/* original clip rect */
				draw_list->PopClipRect( );
			} delete button;
		}

		__forceinline void obj( listbox_t* listbox ) {
			if ( this->vis_flag( ) ) {
				/* set drawing limit */
				this->set_limit( );

				/* update pos */
				listbox->pos = { this->element.offset_x, this->element.offset_y[ this->id ] };

				/* we want to update the object if we hover the group */
				if ( !dependency::dep_controller->control_state[ dependency::dependency_controller::textbox ][ dependency::dep_controller->get_control_index( ) ] ) {
					if ( this->hovered_flag( ) ) {
						listbox->think_filter_bar( );
					}
				} else {
					listbox->think_filter_bar( );
				}

				listbox->update_item( );
				listbox->paint( );

				/* this is the spacing between elements */
				/*
					* element_formula: element_size + 10px
					* element_listbox: main_y = 150 {
					*	main_y + 10;
					* }
				*/
				this->element.offset_y[ this->id ] += 160 + 23;
				slide_offset[ this->id ] += 160 + 23;

				/* set index */
				dependency::dep_controller->set_control_index( dependency::dep_controller->get_control_index( ) + 1 );

				/* original clip rect */
				draw_list->PopClipRect( );
			} delete listbox;
		}

		__forceinline void obj( textbox_t* textbox ) {
			if ( this->vis_flag( ) ) {
				/* set drawing limit */
				this->set_limit( );

				/* update pos */
				textbox->pos = { this->element.offset_x, this->element.offset_y[ this->id ] };

				/* we want to update the object if we hover the group */
				if ( !dependency::dep_controller->control_state[ dependency::dependency_controller::textbox_main ][ dependency::dep_controller->get_control_index( ) ] ) {
					if ( this->hovered_flag( ) ) {
						textbox->think( );
					}
				} else {
					textbox->think( );
				}

				textbox->paint( );

				/* this is the spacing between elements */
				/*
					* element_formula: element_size + 10px
					* element_listbox: main_y = 150 {
					*	main_y + 10;
					* }
				*/
				this->element.offset_y[ this->id ] += 50;
				slide_offset[ this->id ] += 50;

				/* set index */
				dependency::dep_controller->set_control_index( dependency::dep_controller->get_control_index( ) + 1 );

				/* original clip rect */
				draw_list->PopClipRect( );
			} delete textbox;
		}
	public:
		__forceinline void initialize( ) {
			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;
			auto y = dependency::dep_controller->dependency.ry - dependency::dep_controller->dependency.ory;

			this->element.offset_x = this->pos.x + 15;
			this->element.offset_y[ this->id ] = this->pos.y + 35 + slide[ this->id ];
			
			/* slide offset */
			slide_offset[ this->id ] = 0.f;

			/* paint */
			this->paint( );
		}

		__forceinline void scroll( ) {
			if ( !this->vis_flag( ) )
				return;

			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;
			auto y = dependency::dep_controller->dependency.ry - dependency::dep_controller->dependency.ory;


			/* the content doesn't exceed the box size - 40, reset the scroll and exit */
			if ( !( slide_offset[ this->id ] > size.y + ( y / 2 ) - 15 ) ) {
				slide[ this->id ] = 0;
				return;
			}

			bool is_color_picker = dependency::dep_controller->dependency.col_opened;
			bool is_hovered = this->hovered_flag( );

			/* animations */
			auto anim_1 = animation_controller.get( this->name + "#scroll" + std::to_string( this->id ) + animation_controller.current_child );
			anim_1.adjust( anim_1.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( ( dependency::dep_controller->get_menu_state( ) && is_hovered ) ? 1.f : -1.f ) );

			auto scroll_process = [ this ]( int y ) {
				if ( slide[ this->id ] > 0 )
					slide[ this->id ] = 0;

				if ( slide[ this->id ] < ( size.y - 40 ) + ( y / 2 ) - slide_offset[ this->id ] )
					slide[ this->id ] = ( size.y - 40 ) + ( y / 2 ) - slide_offset[ this->id ];
				};

			if ( !is_color_picker && dependency::dep_controller->is_posible( ) && is_hovered &&
				 dependency::dep_input->get_mouse_wheel( ) != 0 ) {
				slide[ this->id ] += dependency::dep_input->get_mouse_wheel( ) * 8;
				dependency::dep_input->set_mouse_wheel( 0 );

				scroll_process( y );
			}

			int max = slide_offset[ this->id ] - size.y + ( y / 2 ) + 61.f;
			int i_scroll_process = ( ( float )slide[ this->id ] /
									 ( float )slide_offset[ this->id ] ) * ( size.y + ( y / 2 ) - 40 ) * ( -1 );
			float scroll_pos_max = max / ( float )slide_offset[ this->id ] * ( size.y + ( y / 2 ) - 40 );
			static int old[ 1000 ];

			if ( old[ this->id ] != max ) {
				scroll_process( y );
				old[ this->id ] = max;
			}

			draw_list->PushClipRect( { this->pos.x, this->pos.y }, { this->pos.x + this->size.x, this->pos.y + this->size.y }, true );

			sdk::drawing::rect_filled( this->pos.x + ( x / 4 ) + this->size.x - 4, this->pos.y + 1 + i_scroll_process,
											  3, this->size.y - scroll_pos_max + 14, sdk::color::col_t( 78, 77, 84 ).modify_alpha( 255 * anim_1.value ), 22 );

			draw_list->PopClipRect( );
		}

		__forceinline void paint( ) {
			/* animations */
			auto anim_1 = animation_controller.get( this->name + "#grop" + animation_controller.current_child );
			anim_1.adjust( anim_1.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			auto x = dependency::dep_controller->dependency.rx - dependency::dep_controller->dependency.orx;
			auto y = dependency::dep_controller->dependency.ry - dependency::dep_controller->dependency.ory;


			sdk::drawing::rect_filled( this->pos.x, this->pos.y, this->size.x, this->size.y,
									   dependency::dep_controller->theme.color_1.modify_alpha( 255 * anim_1.value ), 3 );

			sdk::drawing::text( this->pos.x + 15, this->pos.y + 10,
								dependency::dep_controller->theme.accent.modify_alpha( 255 * anim_1.value ), sdk::drawing::font_4, this->name.c_str( ) );
		}
	public: /* pub vars */
		int tab{}, id{};
		bool visible{}, hovered{};

		/* element handling */
		struct {
			float offset_x;
			float offset_y[ 1000 ];
		} element;
	private: /* priv vars */
		std::string name{};
		sdk::math::vec2_t size{}, pos{};
	};

	/* main window */
	class window_t {	
	public:
		window_t( std::string name, sdk::math::vec2_t* pos, sdk::math::vec2_t size, int* selected, int* sel_subtab ) {
			this->name = name;
			this->pos = pos;
			this->size = size;
			this->selected = selected;
			//this->selected_subtab = sel_subtab;

			/* paint window */
			this->handler( );
			this->handler_resize( );
			this->paint( );
		}

		/* we will need this public */
		int* selected{};
		int* selected_subtab{};
		std::vector<tab_system_t> tabs{};
		std::vector<subtab_system_t> s_tabs{};
	public:
		/* functions */
		/* handling */
		__forceinline void handler( ) {
			static bool drag{ false };
			static sdk::math::vec2_t offset{};

			/* define delta */
			sdk::math::vec2_t delta = dependency::dep_input->get_mouse_position( ) - offset;

			/* setup dragging */
			if ( drag && !GetAsyncKeyState( VK_LBUTTON ) ) {
				drag = false;
			}

			/* update delta */
			if ( drag && GetAsyncKeyState( VK_LBUTTON ) ) {
				*this->pos = delta;
			}

			/* update the menu position */
			if ( dependency::dep_input->mouse_in_box( sdk::math::vec2_t( this->pos->x, this->pos->y ), 
													  sdk::math::vec2_t( 210, 30 ) ) ) {
				drag = true;
				offset = dependency::dep_input->get_mouse_position( ) - *this->pos;
			}

			/* clamp */
			if ( this->pos->x < 0 ) { this->pos->x = 0; }
			if ( this->pos->y < 0 ) { this->pos->y = 0; }
		}

		__forceinline void handler_resize( ) {
			/* update the menu position */
			static bool resizing{ false };

			if ( sdk::c_input->is_key_down( VK_LBUTTON ) ) {
				if ( dependency::dep_input->mouse_in_box( sdk::math::vec2_t( ( this->pos->x + this->size.x ) - 10, ( this->pos->y + this->size.y ) - 10 ),
														  sdk::math::vec2_t( 20, 20 ) ) && !resizing ) {
					resizing = true;
				}
			} else {
				resizing = false;
			}

			if ( resizing ) {
				this->size.x = sdk::c_input->get_mouse_position( ).x - this->pos->x;
				this->size.y = sdk::c_input->get_mouse_position( ).y - this->pos->y;

				dependency::dep_controller->dependency.window_size = this->size;

				dependency::dep_controller->dependency.rx = dependency::dep_controller->dependency.window_size.x;
				dependency::dep_controller->dependency.ry = dependency::dep_controller->dependency.window_size.y;
			}

			/* clamp gheto */
			if ( this->size.x < dependency::dep_controller->dependency.orx ) {
				this->size.x = dependency::dep_controller->dependency.orx;
			}
			if ( this->size.x > 1500 ) {
				this->size.x = 1500; this->is_resizing = false;
			}
			if ( this->size.y < dependency::dep_controller->dependency.ory ) {
				this->size.y = dependency::dep_controller->dependency.ory;
			}
			if ( this->size.y > 1500 ) {
				this->size.y = 1500; this->is_resizing = false;
			}
			if ( dependency::dep_controller->dependency.window_size.x < dependency::dep_controller->dependency.orx ) {
				dependency::dep_controller->dependency.window_size.x = dependency::dep_controller->dependency.orx;
			}
			if ( dependency::dep_controller->dependency.window_size.y < dependency::dep_controller->dependency.ory ) {
				dependency::dep_controller->dependency.window_size.y = dependency::dep_controller->dependency.ory;
			}

			if ( dependency::dep_controller->dependency.rx < dependency::dep_controller->dependency.orx ) {
				dependency::dep_controller->dependency.rx = dependency::dep_controller->dependency.orx;
			}
			if ( dependency::dep_controller->dependency.ry < dependency::dep_controller->dependency.ory ) {
				dependency::dep_controller->dependency.ry = dependency::dep_controller->dependency.ory;
			}

			/* store */
			this->is_resizing = resizing;
		}

		__forceinline void add_tab( std::string name, int tab_id ) {
			tabs_out.push_back( { name, tab_id } );
		}

		__forceinline void add_subtab( std::string name, int selected_tab ) {
			subtabs_out.push_back( { name, selected_tab } );
		}

		/* paint */
		__forceinline void paint( ) {
			sdk::drawing::image( 0, 0, 1920, 1090, backround );

			/* animations */
			auto anim_1 = animation_controller.get( this->name + animation_controller.current_child );
			anim_1.adjust( anim_1.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) ? 1.f : -1.f ) );

			auto anim_2 = animation_controller.get( this->name + "#resize" + animation_controller.current_child );
			anim_2.adjust( anim_2.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) && this->is_resizing ? 1.f : -1.f ) );

			sdk::drawing::rect_filled( this->pos->x + this->size.x - 99, this->pos->y + this->size.y - 99,
									   100, 100, dependency::dep_controller.get( )->theme.accent.modify_alpha( 200 * anim_2.value ), 3 );

			sdk::drawing::rect_filled( this->pos->x, this->pos->y,
									   this->size.x, this->size.y, dependency::dep_controller.get( )->theme.backround.modify_alpha( 255 * anim_1.value ), 3 );

			sdk::drawing::rect_filled( this->pos->x, this->pos->y + this->size.y - 25,
									   this->size.x, 25, sdk::color::col_t( 17, 17, 23 ).modify_alpha( 255 * anim_1.value ), 3 );

			sdk::drawing::text( this->pos->x + 5, this->pos->y + this->size.y - 22, sdk::color::col_t( 220, 220, 220 ).modify_alpha( 255 * anim_1.value ), sdk::drawing::font_4,
								"amnesia for Counter-Strike: Global Offensive", false );

			sdk::drawing::text( this->pos->x + 5 + this->size.x - 120, this->pos->y + this->size.y - 22, sdk::color::col_t( 220, 220, 220 ).modify_alpha( 255 * anim_1.value ), sdk::drawing::font_4,
								"amnesia | lifetime", false );

			sdk::drawing::rect_filled( this->pos->x + 70, this->pos->y + 40,
									   this->size.x - 90, 2, sdk::color::col_t( 26, 26, 32 ).modify_alpha( 255 * anim_1.value ), 3 );

			/* external drawing */
			auto rect_fill = [ ]( int x, int y, int w, int h, sdk::color::col_t col, int r, bool overwrite ) -> void {
				if ( overwrite ) {
					dependency::external.rect_filled_a.insert( dependency::external.rect_filled_a.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
				} else {
					dependency::external.rect_filled.insert( dependency::external.rect_filled.begin( ), dependency::draw_rect( x, y, w, h, col, r ) );
				}
				};

			auto image = [ ]( int x, int y, int w, int h, ImTextureID texture, sdk::color::col_t col ) -> void {
				dependency::external.image.insert( dependency::external.image.begin( ), dependency::draw_image( x, y, w, h, texture, col ) );
				};

			auto text = [ ]( int x, int y, int font_index, std::string text, sdk::color::col_t col, bool ds ) -> void {
				dependency::external.text.insert( dependency::external.text.begin( ), dependency::draw_text( x, y, text, col, font_index, ds ) );
				};

			bool is_tab_element_hovered{ dependency::dep_input->mouse_in_box( { this->pos->x, this->pos->y }, { 49, this->size.y - 25 } ) };
			bool is_expand_hovered{ dependency::dep_input->mouse_in_box( { this->pos->x + 15, this->pos->y + 15 }, { 15, 15 } ) };
			static bool allow_expanding{ true };
			static bool element_was_hovered{ };

			/* system #1 */
			if ( is_tab_element_hovered ) {
				element_was_hovered = true;
			} else if ( !dependency::dep_input->mouse_in_box( { this->pos->x, this->pos->y }, { 49 + 79, this->size.y - 25 } ) ) {
				element_was_hovered = false;
			}

			/* system #2 */
			if ( is_expand_hovered && dependency::dep_input->key_pressed( VK_LBUTTON ) ) {
				allow_expanding = !allow_expanding;
			}

			auto anim_3 = animation_controller.get( this->name + "#hovered" + animation_controller.current_child );
			anim_3.adjust( anim_3.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( ( dependency::dep_controller->get_menu_state( ) && element_was_hovered && allow_expanding ) ? 1.f : -1.f ) );

			auto anim_4 = animation_controller.get( this->name + "#allow_expanding" + animation_controller.current_child );
			anim_4.adjust( anim_4.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) && allow_expanding ? 1.f : -1.f ) );


			rect_fill( this->pos->x, this->pos->y,
									   this->size.x, this->size.y, sdk::color::col_t( 0, 0, 0 ).modify_alpha( 50 * anim_3.value ), 3, false );

			rect_fill( this->pos->x, this->pos->y,
					   49 + ( 79 * anim_3.value ), this->size.y - 25, dependency::dep_controller.get( )->theme.color_1.modify_alpha( 255 * anim_1.value ), 1, true );

			/* expanded button */
			rect_fill( this->pos->x, this->pos->y,
					   49 + ( 79 * anim_3.value ), 49, dependency::dep_controller.get( )->theme.outline.modify_alpha( 255 * anim_1.value ), 1, true );

			/* expanded */
			text( this->pos->x + 15, this->pos->y + 15, sdk::drawing::font_3, "c", sdk::color::col_t( 71, 71, 77 ).blend( dependency::dep_controller.get( )->theme.accent, anim_4.value ).modify_alpha( 255 * anim_1.value ), false );
			text( this->pos->x + 15 + 8, this->pos->y + 15, sdk::drawing::font_3, "c", sdk::color::col_t( 71, 71, 77 ).blend( dependency::dep_controller.get( )->theme.accent, anim_4.value ).modify_alpha( 255 * anim_1.value ), false );

			if ( element_was_hovered && anim_3.value > 0.50f ) {
				text( this->pos->x + 50, this->pos->y + 15, sdk::drawing::font_4, "Expand", sdk::color::col_t( ).modify_alpha( 255 * anim_3.value ), false );
			}

			bool upd_tab{ false };

			/* update tabs */
			{
				this->tabs = tabs_out;

				/* we need these */
				auto form_id{ 0 };
				int tab_spacing = 30; /* set the spacing between tabs */

				int start_y = this->pos->y + 70;
				for ( int i = 0; i < this->tabs.size( ); ++i ) {
					sdk::math::vec2_t tab_size = {
					sdk::drawing::get_text_size(
						this->tabs[ i ].name.c_str( ), 0 ).x, sdk::drawing::get_text_size( this->tabs[ i ].name.c_str( ), 0 ).y
					};
					sdk::math::vec2_t tab_pos = sdk::math::vec2_t( this->pos->x + 15, start_y );

					/* input */
					if ( dependency::dep_input->mouse_in_box( tab_pos, sdk::math::vec2_t( element_was_hovered ?
																						  tab_size.x + 40 : 49, 35 ) ) ) {
						if ( dependency::dep_input->key_pressed( VK_LBUTTON ) ) {
							*this->selected = i;

							upd_tab = true; /* update */
						} else {
							upd_tab = false; /* update */
						}
					}

					/* drawing */
					auto anim_5 = animation_controller.get( this->tabs[ i ].name + animation_controller.current_child );
					anim_5.adjust( anim_5.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) && *this->selected == i ? 1.f : -1.f ) );


					/* first draw images */
					if ( this->tabs[ i ].tab_id == 0 ) {
						image( tab_pos.x, tab_pos.y, 20, 20, oneicon, sdk::color::col_t( 71, 71, 77 ).blend( dependency::dep_controller.get( )->theme.accent, anim_5.value ).modify_alpha( 255 * anim_1.value ) );
					}

					if ( this->tabs[ i ].tab_id == 1 ) {
						image( tab_pos.x, tab_pos.y, 20, 20, twoicon, sdk::color::col_t( 71, 71, 77 ).blend( dependency::dep_controller.get( )->theme.accent, anim_5.value ).modify_alpha( 255 * anim_1.value ) );
					}

					if ( this->tabs[ i ].tab_id == 2 ) {
						image( tab_pos.x, tab_pos.y, 20, 20, threeicon, sdk::color::col_t( 71, 71, 77 ).blend( dependency::dep_controller.get( )->theme.accent, anim_5.value ).modify_alpha( 255 * anim_1.value ) );
					}

					if ( this->tabs[ i ].tab_id == 3 ) {
						image( tab_pos.x, tab_pos.y, 20, 20, fouricon, sdk::color::col_t( 71, 71, 77 ).blend( dependency::dep_controller.get( )->theme.accent, anim_5.value ).modify_alpha( 255 * anim_1.value ) );
					}

					if ( this->tabs[ i ].tab_id == 4 ) {
						image( tab_pos.x, tab_pos.y, 20, 20, fiveicon, sdk::color::col_t( 71, 71, 77 ).blend( dependency::dep_controller.get( )->theme.accent, anim_5.value ).modify_alpha( 255 * anim_1.value ) );
					}

					if ( element_was_hovered && anim_3.value > 0.50f ) {
						text( tab_pos.x + 50 - 15, tab_pos.y + 1, sdk::drawing::font_4, tabs[ i ].name.c_str( ), sdk::color::col_t( 71, 71, 77 ).blend( sdk::color::col_t( 220, 220, 220 ), anim_5.value ).modify_alpha( 255 * anim_3.value ), false );
					}

					start_y += tab_size.y + tab_spacing;
				}

			}

			int old_tab = *this->selected;
			if ( this->selected_subtab == nullptr ) { /* update tab index */
				if ( *this->selected == 0 ) {
					this->selected_subtab = &dependency::dep_controller->dependency.subtab_index_rage;
				} else if ( *this->selected == 1 ) {
					this->selected_subtab = &dependency::dep_controller->dependency.subtab_index_legit;
				} else if ( *this->selected == 2 ) {
					this->selected_subtab = &dependency::dep_controller->dependency.subtab_index_vis;
				} else if ( *this->selected == 3 ) {
					this->selected_subtab = &dependency::dep_controller->dependency.subtab_index_misc;
				} else if ( *this->selected == 4 ) {
					this->selected_subtab = &dependency::dep_controller->dependency.subtab_index_profile;
				}
			}

			static bool updated_subtabs{ false };
			if ( !updated_subtabs ) {
				std::cout << "updated tabs first time != nullptr\n";

				dependency::dep_controller->dependency.subtab_index_rage = 0;
				dependency::dep_controller->dependency.subtab_index_legit = 0;
				dependency::dep_controller->dependency.subtab_index_vis = 0;
				dependency::dep_controller->dependency.subtab_index_misc = 0;
				dependency::dep_controller->dependency.subtab_index_profile = 0;
				updated_subtabs = true;
			}

			auto selected_is_in_range = [ ]( int value, int lower, int upper ) -> bool {
				return ( value >= lower && value <= upper );
				};

			/* update tabs */
			if ( *this->selected_subtab < 4 && *this->selected == 1 ) {
				*this->selected_subtab = 3;
			} else if ( !selected_is_in_range( *this->selected_subtab, 4, 8 ) && *this->selected == 2 ) {
				*this->selected_subtab = 5;
			} else if ( !selected_is_in_range( *this->selected_subtab, 7, 10 ) && *this->selected == 3 ) {
				*this->selected_subtab = 8;
			} else if ( *this->selected_subtab != 10 && *this->selected == 4 ) {
				*this->selected_subtab = 10;
			}

			/* update subtabs */
			{
				this->s_tabs = subtabs_out;

				/* we need these */
				auto form_id{ 0 };
				int tab_spacing = 30; /* set the spacing between tabs */

				int start_x = this->pos->x + 90;

				for ( int i = 0; i < this->s_tabs.size( ); ++i ) {
					sdk::math::vec2_t tab_size = {
					sdk::drawing::get_text_size(
						this->s_tabs[ i ].name.c_str( ), 0 ).x, sdk::drawing::get_text_size( this->s_tabs[ i ].name.c_str( ), 0 ).y
					};
					sdk::math::vec2_t tab_pos = sdk::math::vec2_t( start_x, this->pos->y + 15 );

					/* input */
					if ( dependency::dep_input->mouse_in_box( tab_pos, sdk::math::vec2_t( tab_size.x, 35 ) ) ) {
						if ( dependency::dep_input->key_pressed( VK_LBUTTON ) ) {
							*this->selected_subtab = i;
						}
					}

					/* drawing */
					auto anim_6 = animation_controller.get( this->s_tabs[ i ].name + animation_controller.current_child );
					anim_6.adjust( anim_6.value + 3.f * animation_controller.get_min_deltatime( 0.3f ) * ( dependency::dep_controller->get_menu_state( ) && *this->selected_subtab == i ? 1.f : -1.f ) );


					/* we have now the same index */
					if ( this->s_tabs[ i ].selected_tab == *this->selected ) {
						sdk::drawing::text( tab_pos.x, tab_pos.y - 3, 
											sdk::color::col_t( 71, 71, 77 ).blend( dependency::dep_controller.get( )->theme.accent, anim_6.value ).modify_alpha( 255 * anim_1.value ), 
											sdk::drawing::font_4, this->s_tabs[ i ].name.c_str( ), false );


						if ( *this->selected_subtab == i )
							sdk::drawing::rect_filled( tab_pos.x + ( tab_size.x * 0.5 ) - 3, tab_pos.y + 22, 4, 4, sdk::color::col_t( 71, 71, 77 ).blend( dependency::dep_controller.get( )->theme.accent, anim_6.value ).modify_alpha( 255 * anim_1.value ), 22 );

						start_x += tab_size.x + tab_spacing;
					}
				}
			}

			sdk::drawing::rect( this->pos->x, this->pos->y,
									   this->size.x, this->size.y, dependency::dep_controller.get( )->theme.outline.modify_alpha( 255 * anim_1.value ), 3 );
		}

		/* add group */
		__forceinline void sub_window( group_t* grp ) {
			grp->set_vis_flag( grp->tab == *this->selected_subtab );

			if ( grp->vis_flag( ) ) {
				grp->set_position( *this->pos );
				grp->initialize( );
			}
		}
	private:
		std::string name{};
		sdk::math::vec2_t* pos{};
		sdk::math::vec2_t size{};

		bool is_resizing{};
		bool is_hovering_tab[ 1000 ];
	};
}

const char* stable_keys[ 254 ] = {
		"None", "m1", "m2", "brk", "m3", "m4", "m5",
		"None", "bspc", "tab", "None", "None", "None", "enter", "None", "None", "shift",
		"ctrl", "alt", "pau", "caps", "None", "None", "None", "None", "None", "None",
		"esc", "None", "None", "None", "None", "space", "pgup", "pgdown", "end", "home", "left",
		"up", "right", "down", "None", "prnt", "None", "prtscr", "ins", "del", "None", "0", "1",
		"2", "3", "4", "5", "6", "7", "8", "9", "None", "None", "None", "None", "None", "None",
		"None", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u",
		"v", "w", "x", "y", "z", "lftwin", "rghtwin", "None", "None", "None", "num0", "num1",
		"num2", "num3", "num4", "num5", "num6", "num7", "num8", "num9", "*", "+", "_", "None", ".", "/", "f1", "f2", "f3",
		"f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15", "f16", "f17", "f18", "f19", "f20",
		"f21",
		"f22", "f23", "f24", "None", "None", "None", "None", "None", "None", "None", "None",
		"num lock", "scroll lock", "None", "None", "None", "None", "None", "None", "None",
		"None", "None", "None", "None", "None", "None", "None", "lshft", "rshft", "lctrl",
		"rctrl", "lmenu", "rmenu", "None", "None", "None", "None", "None", "None", "None",
		"None", "None", "None", "ntrk", "ptrk", "stop", "play", "None", "None",
		"None", "None", "None", "None", ";", "+", ",", "None", ".", "/?", "~", "None", "None",
		"None", "None", "None", "None", "None", "None", "None", "None", "None",
		"None", "None", "None", "None", "None", "None", "None", "None", "None",
		"None", "None", "None", "None", "None", "None", "{", "\\|", "}", "'\"", "None",
		"None", "None", "None", "None", "None", "None", "None", "None", "None",
		"None", "None", "None", "None", "None", "None", "None", "None", "None",
		"None", "None", "None", "None", "None", "None", "None", "None", "None",
		"None", "None"
};

const char* lower_case[ 254 ] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
	"y", "z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
	"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "/?", "~", nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "[", "\\", "]", "'", nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr 
};

const char* upper_case[ 254 ] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
"Y", "Z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "_", ".", "?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "{", "|", "}", "\"", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr 
};