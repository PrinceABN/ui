#pragma once
#include "../../c_includes.hpp"

namespace dependency {
	struct draw_rect {
		draw_rect( int pos_x, int pos_y, int width, int height, sdk::color::col_t c, int rounding = 0 ) {
			this->pos_x = pos_x;
			this->pos_y = pos_y;
			this->width = width;
			this->height = height;
			this->c = c;
			this->rounding = rounding;
		}

		int pos_x;
		int pos_y;
		int width;
		int height;
		int rounding;
		sdk::color::col_t c;
	};

	struct draw_circle {
		draw_circle( ImVec2 pos, float radius, ImColor color, float thickness ) {
			this->pos = pos;
			this->radius = radius;
			this->color = color;
			this->thickness = thickness;
		}

		ImVec2 pos;  
		ImColor color;
		float radius; 
		float thickness;
	};

	struct draw_circle_fill {
		draw_circle_fill( ImVec2 pos, float radius, ImColor color ) {
			this->pos = pos;
			this->radius = radius;
			this->color = color;
		}

		ImVec2 pos;
		ImColor color;
		float radius;
	};

	struct draw_thick_rect {
		draw_thick_rect( int x, int y, int w, int h, int r ) {
			this->x = x;
			this->y = y;
			this->w = w;
			this->h = h;
			this->r = r;
		}

		int x, y, w, h, r;
	};

	struct draw_image {
		draw_image( int x, int y, int w, int h, ImTextureID texture, sdk::color::col_t c ) {
			this->x = x;
			this->y = y;
			this->w = w;
			this->h = h;
			this->texture = texture;
			this->c = c;
		}

		int x, y, w, h;
		sdk::color::col_t c;
		ImTextureID texture;
	};

	struct draw_text {
		draw_text( float pos_x, float pos_y, std::string text, sdk::color::col_t c, int font_index = 0, bool drop_shadow = false ) {
			this->pos_x = pos_x;
			this->pos_y = pos_y;
			this->text = text;
			this->c = c;
			this->font_index = font_index;
			this->drop_shadow = drop_shadow;
		}

		float pos_x;
		float pos_y;
		int font_index;
		bool drop_shadow;
		std::string text;
		sdk::color::col_t c;
	};

	struct draw_gradient {
		draw_gradient( int pos_x, int pos_y, int width, int height, sdk::color::col_t c, sdk::color::col_t c1, bool change ) {
			this->pos_x = pos_x;
			this->pos_y = pos_y;
			this->width = width;
			this->height = height;
			this->c = c;
			this->c1 = c1;
			this->change = change;
		}

		int pos_x;
		int pos_y;
		int width;
		int height;
		bool change;
		sdk::color::col_t c;
		sdk::color::col_t c1;
	};

	struct externals_t {
		/* normal draw */
		std::vector< draw_rect > rect_filled{};
		std::vector< draw_rect > rect{};
		std::vector< draw_gradient > gradient{};
		std::vector< draw_circle > circle{};
		std::vector< draw_circle_fill > fcircle{};
		std::vector< draw_thick_rect > t_rect{};

		/* after draw */
		std::vector< draw_rect > rect_filled_a{};
		std::vector< draw_rect > rect_a{};
		std::vector< draw_gradient > gradient_a{};
		std::vector< draw_circle > circle_a{};
		std::vector< draw_circle_fill > fcircle_a{};
		std::vector< draw_thick_rect > t_rect_a{};

		std::vector< draw_text > text{};
		std::vector< draw_image > image{};
	}; inline externals_t external;

	class external_drawing {
	public:
		__forceinline void begin( ) {
			for ( size_t i{ 0 }; i < external.rect_filled.size( ); i++ ) {
				sdk::drawing::rect_filled( external.rect_filled[ i ].pos_x, external.rect_filled[ i ].pos_y,
										   external.rect_filled[ i ].width, external.rect_filled[ i ].height, external.rect_filled[ i ].c, external.rect_filled[ i ].rounding );
			}

			for ( size_t i{ 0 }; i < external.rect.size( ); i++ ) {
				sdk::drawing::rect( external.rect[ i ].pos_x, external.rect[ i ].pos_y,
										   external.rect[ i ].width, external.rect[ i ].height, external.rect[ i ].c, external.rect[ i ].rounding );
			}

			for ( size_t i{ 0 }; i < external.gradient.size( ); i++ ) {
				if ( external.gradient[ i ].change ) {
					sdk::drawing::gradient_horizontal( external.gradient[ i ].pos_x, external.gradient[ i ].pos_y,
													   external.gradient[ i ].width, external.gradient[ i ].height, external.gradient[ i ].c, external.gradient[ i ].c1 );
				} else {
					sdk::drawing::gradient_vertical( external.gradient[ i ].pos_x, external.gradient[ i ].pos_y,
													   external.gradient[ i ].width, external.gradient[ i ].height, external.gradient[ i ].c, external.gradient[ i ].c1 );
				}
			}

			for ( size_t i{ 0 }; i < external.circle.size( ); i++ ) {
				draw_list->AddCircle( external.circle[ i ].pos, external.circle[ i ].radius, external.circle[ i ].color, 0, external.circle[ i ].thickness );
			}

			for ( size_t i{ 0 }; i < external.fcircle.size( ); i++ ) {
				draw_list->AddCircleFilled( external.fcircle[ i ].pos, external.fcircle[ i ].radius, external.fcircle[ i ].color, 0 );
			}

			for ( size_t i{ 0 }; i < external.t_rect.size( ); i++ ) {
				sdk::drawing::rect_thik( external.t_rect[ i ].x, external.t_rect[ i ].y,
									external.t_rect[ i ].w, external.t_rect[ i ].h, sdk::color::col_t( 19, 19, 19 ), external.t_rect[ i ].r, 4 );
			}

			for ( size_t i{ 0 }; i < external.rect_filled_a.size( ); i++ ) {
				sdk::drawing::rect_filled( external.rect_filled_a[ i ].pos_x, external.rect_filled_a[ i ].pos_y,
										   external.rect_filled_a[ i ].width, external.rect_filled_a[ i ].height, external.rect_filled_a[ i ].c, external.rect_filled_a[ i ].rounding );
			}

			for ( size_t i{ 0 }; i < external.rect_a.size( ); i++ ) {
				sdk::drawing::rect( external.rect_a[ i ].pos_x, external.rect_a[ i ].pos_y,
										   external.rect_a[ i ].width, external.rect_a[ i ].height, external.rect_a[ i ].c, external.rect_a[ i ].rounding );
			}

			for ( size_t i{ 0 }; i < external.gradient_a.size( ); i++ ) {
				if ( external.gradient_a[ i ].change ) {
					sdk::drawing::gradient_horizontal( external.gradient_a[ i ].pos_x, external.gradient_a[ i ].pos_y,
													   external.gradient_a[ i ].width, external.gradient_a[ i ].height, external.gradient_a[ i ].c, external.gradient_a[ i ].c1 );
				} else {
					sdk::drawing::gradient_vertical( external.gradient_a[ i ].pos_x, external.gradient_a[ i ].pos_y,
													 external.gradient_a[ i ].width, external.gradient_a[ i ].height, external.gradient_a[ i ].c, external.gradient_a[ i ].c1 );
				}
			}

			for ( size_t i{ 0 }; i < external.circle_a.size( ); i++ ) {
				draw_list->AddCircle( external.circle_a[ i ].pos, external.circle_a[ i ].radius, external.circle_a[ i ].color, 0, external.circle_a[ i ].thickness );
			}

			for ( size_t i{ 0 }; i < external.fcircle_a.size( ); i++ ) {
				draw_list->AddCircleFilled( external.fcircle_a[ i ].pos, external.fcircle_a[ i ].radius, external.fcircle_a[ i ].color, 0 );
			}

			for ( size_t i{ 0 }; i < external.t_rect_a.size( ); i++ ) {
				sdk::drawing::rect_thik( external.t_rect_a[ i ].x, external.t_rect_a[ i ].y,
										 external.t_rect_a[ i ].w, external.t_rect_a[ i ].h, sdk::color::col_t( 20, 20, 26 ), external.t_rect_a[ i ].r, 4 );
			}

			for ( size_t i{ 0 }; i < external.text.size( ); i++ ) {
				sdk::drawing::text( external.text[ i ].pos_x, external.text[ i ].pos_y,
									external.text[ i ].c, external.text[ i ].font_index, external.text[ i ].text.c_str( ), external.text[ i ].drop_shadow );
			}

			for ( size_t i{ 0 }; i < external.image.size( ); i++ ) {
				sdk::drawing::image( external.image[ i ].x, external.image[ i ].y, external.image[ i ].w, external.image[ i ].h,
									 external.image[ i ].texture, external.image[ i ].c );
			}

			/* clear */
			external.rect_filled.clear( );
			external.rect.clear( );
			external.gradient.clear( );
			external.circle.clear( );
			external.fcircle.clear( );
			external.t_rect.clear( );
			external.rect_filled_a.clear( );
			external.rect_a.clear( );
			external.gradient_a.clear( );
			external.circle_a.clear( );
			external.fcircle_a.clear( );
			external.t_rect_a.clear( );
			external.text.clear( );
			external.image.clear( );
		}
	};
	inline const auto dep_externals = std::make_unique < external_drawing >( );
}