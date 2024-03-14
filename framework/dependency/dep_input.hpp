#pragma once
#include "../../c_includes.hpp"
#include "dep_controller.hpp"

namespace dependency {
	class dependency_input {
	public:
		struct helpers_t {
			bool key_state[ 256 ]{};
			bool prev_key_state[ 256 ]{};
		};

		/* helpers */
		helpers_t helpers;

		/* initialize input */
		__forceinline void init_input( ) {
			for ( size_t i{ 0 }; i < 256; i++ ) {
				helpers.prev_key_state[ i ] = helpers.key_state[ i ];
				helpers.key_state[ i ] = GetAsyncKeyState( i );
			}
		}

		/* get mouse position */
		__forceinline sdk::math::vec2_t get_mouse_position( ) {
			return dep_controller->dependency.mouse_pos;
		}

		/* set mouse position */
		__forceinline void set_mouse_position( const sdk::math::vec2_t pos ) { 
			dep_controller->dependency.mouse_pos = sdk::math::vec2_t{ pos.x, pos.y };
		} 

		/* key pressed */
		__forceinline bool key_pressed( const uintptr_t key ) {
			return helpers.key_state[ key ] && !helpers.prev_key_state[ key ];
		}

		/* mouse pointer */
		__forceinline bool mouse_in_box( const sdk::math::vec2_t pos, const sdk::math::vec2_t size ) {
			std::pair<bool, bool> box_position{
				this->get_mouse_position( ).x > pos.x && this->get_mouse_position( ).y > pos.y,
				this->get_mouse_position( ).x < pos.x + size.x && this->get_mouse_position( ).y < pos.y + size.y
			};

			return box_position.first && box_position.second;
		}

		/* set mouse wheel */
		__forceinline void set_mouse_wheel( const float wheel ) {
			dep_controller->dependency.next_mouse_wheel = wheel;
		}

		/* get mouse wheel */
		__forceinline float get_mouse_wheel( ) {
			return dep_controller->dependency.next_mouse_wheel;
		}
	};

	/* go for it */
	inline const auto dep_input = std::make_unique< dependency_input >( );
}