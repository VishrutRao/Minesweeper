#ifndef MINESWEEPER_H
#define MINESWEEPER_H 

#include "std_lib_facilities_4.h"
#include "Simple_window.h"
#include "Window.h"
#include "Graph.h"
#include "GUI.h"
#include <iostream>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl.H>
#include <FL/Fl_Timer.H>

#ifndef debug
#define debug true
#endif

#ifndef TileImage
#define TileImage

namespace TileImage
{
	Fl_PNG_Image imagenumber1{string("1.png").c_str()};
	Fl_PNG_Image imagenumber2{string("2.png").c_str()};
	Fl_PNG_Image imagenumber3{string("3.png").c_str()};
	Fl_PNG_Image imagenumber4{string("4.png").c_str()};
	Fl_PNG_Image imagenumber5{string("5.png").c_str()};
	Fl_PNG_Image imagenumber6{string("6.png").c_str()};
	Fl_PNG_Image imagenumber7{string("7.png").c_str()};
	Fl_PNG_Image imagenumber8{string("8.png").c_str()};
	Fl_PNG_Image imageBlank{string("Blank.png").c_str()};
	Fl_PNG_Image imageFlag{string("Flag.png").c_str()};
	Fl_PNG_Image imageBomb{string("GrayBomb.png").c_str()};
	Fl_PNG_Image imageQuestionMark{string("QuestionMark.png").c_str()};
	Fl_PNG_Image imageRedBomb{string("RedBomb.png").c_str()};
	Fl_PNG_Image imageUnclicked{string("Unclicked.png").c_str()};
	Fl_PNG_Image imageXBomb{string("XBomb.png").c_str()};
	Fl_PNG_Image imageSmile{string("Smile.png").c_str()};
	Fl_PNG_Image imageSmileDown{string("SmileDown.png").c_str()};
	Fl_PNG_Image imageDead{string("Dead.png").c_str()};
	Fl_PNG_Image imageCool{string("Cool.png").c_str()};
	Fl_PNG_Image imageScared{string("Scared.png").c_str()};
}

#endif

struct MyBox:Fl_Box
{
	MyBox(int x, int y, int w, int h, const char * 	l = 0 )
		:Fl_Box(x,y,w,h,l)
	{
	}
	virtual int handle(int event);
};

struct Tile:Widget 
{
	enum class State {unclicked, clicked, flag, question};
	
	static const int tileSide = 16 ;
	
	public:
		Tile(Point xy, Callback cb)
			:Widget(xy, tileSide, tileSide, "", cb),
			 mine{false}, adj_mines{0}, clicked_img{&TileImage::imageBlank}
		{
		}
		
		State get_state() const {return current_state;}
		void change_state(State s);
		void attach(Graph_lib::Window& win);
		void put_mine(bool put);
		bool get_mine() const {return mine;}
		void add_adj_mines();
		void set_adj_mines(int numMines); 
		int get_adj_mines() const {return adj_mines;}
		void loss();
		
	private:
		bool mine;
		int adj_mines;  
		State current_state;
		Fl_PNG_Image* clicked_img;  
		
		void changeImage(Fl_PNG_Image& im);
};

struct Smile:Widget
{
	public:
		Smile(Point xy, Callback cb)
			:Widget(xy, 30, 30, "", cb)
		{
		}
		void change_image(Fl_PNG_Image& im);
		void attach(Graph_lib::Window& win);
		void move(Point xy) {Widget::move(xy.x-loc.x,xy.y-loc.y);}
	private:
		
};

struct Game: Graph_lib::Window
{
	public:
		static const int y_offset = 40;
	
		Game(Point xy, const string& title );
		~Game();
		void place_mines (int num, int row, int col); 
		void place_mine (int row, int col);  
		void show_mines(int row, int col);  
		void clear_board();
		void create_board(int rows, int cols, int mines);
		void gamemode();
		void lose_game(int row, int col);
		void start_game(int row, int col);  
		void win_game();
		void restart_game();
		
	private:
		vector<vector<Tile*>> board; 
		Smile* smileyface;
		bool game_started=false;
		bool game_over = false;
		int mine_total;
		int uncovered=0;
		
		static void cb_tile_click (Address, Address);
		static void cb_restart_click (Address, Address);
		
		void click (int row, int col);
		void show_scared();
		void left_click(int row, int col);
		void middle_click(int row, int col);
		void right_click(int row, int col);
};

#endif