#include "Minesweeper.h"

Game::Game (Point xy, const string& title)
	:Graph_lib::Window{xy, 0,0, title}
{
	smileyface = new Smile(Point{0, 0}, cb_restart_click);
	attach(*smileyface);
	create_board(10,10,9);
	create_board(16, 30, 99);
	Fl::run();
}

Game::~Game()
{
	int rows=board.size();
	int cols=board[0].size();
	for (int r=0; r<rows; r++)
	{
		for (int c=0; c<cols; c++)
		{
			delete board[r][c];
		}
	}
}

void Game::place_mines(int num, int row, int col)
{
	srand (time(NULL));
	Tile* t = nullptr;
	int r = 0;
	int c = 0;
	for (int x =0; x < num; x++)
	{
		do
		{
			r = rand() % board.size();
			c = rand() % board[0].size();
			t = board[r][c];
		} while((r==row && c==col) || t->get_mine());
		place_mine(r, c);
		if (debug) t->change_state(Tile::State::unclicked);
	}
}

void Game::place_mine (int row, int col)
{
	board[row][col]-> put_mine(true);
	int maxRow = board.size();
	int maxCol = board[0].size();
	int bottom = min((row+2),maxRow);
	int right = min((col+2),maxCol);
	for (int r = max(row-1,0); r < bottom; r++)
	{
		for (int c = max(col-1,0); c < right; c++)
		{
			Tile* temp = board[r][c];
			if (!(temp -> get_mine()))
			{
				temp -> add_adj_mines();
			}
		}
	}
}

void Game::show_mines(int row, int col)
{
	int rowMax = board.size();
	int columnMax = board[0].size();
	for (int r=0; r < rowMax; r++)
	{
		for (int c = 0; c < columnMax; c++)
		{
			if (r!=row || c!=col)
			{
				board[r][c]->loss();
			}
		}
	}
}

void Game::clear_board()
{
	while (board.size() > 0)
	{
		int lastR = board.size()-1;
		while (board[lastR].size() > 0)
		{
			int lastC = board[lastR].size()-1;
			detach(*board[lastR][lastC]);
			delete board[lastR][lastC];
			board[lastR].pop_back();
		}
		board.pop_back();
	}
}

void Game::create_board(int rows, int cols, int mines)
{
	clear_board();
	
	resize(cols*Tile::tileSide, rows*Tile::tileSide + y_offset);
	resizable(NULL);
	size_range(x_max(),y_max(),x_max(),y_max());
	
	smileyface -> move(Point{(x_max()-26)/2, (y_offset-26)/2});
	
	for (int r=0; r<rows; r++)
	{
		board.push_back(vector<Tile*>{});
		for (int c = 0; c < cols; c++)
		{
			board[r].push_back(new Tile(Point{c*Tile::tileSide,r*Tile::tileSide+y_offset},cb_tile_click));
			attach(*board[r][c]);
		}
		
	}
	mine_total=mines;
}

void Game::lose_game(int row, int col)
{
	show_mines(row, col);
	smileyface->change_image(TileImage::imageDead);
	game_over = true;
}

void Game::start_game(int row, int col)
{
	place_mines(mine_total, row, col);
}

void Game::win_game()
{
	int rowMax = board.size();
	int columnMax = board[0].size();
	for (int r = 0; r < rowMax; r++)
	{
		for (int c = 0; c < columnMax; c++)
		{
			if (board[r][c] -> get_mine())
			{
				board[r][c] -> change_state(Tile::State::flag);
			}
		}
	}
	smileyface -> change_image(TileImage::imageCool);
	game_over = true;
}

void Game::restart_game()
{
	if (Fl::event() == FL_PUSH)
	{
		smileyface -> change_image(TileImage::imageSmileDown);
	}
	else
	{
		int rowMax=board.size();
		int columnMax=board[0].size();
		for (int r=0; r < rowMax; r++)
		{
			for (int c = 0; c < columnMax; c++)
			{
				board[r][c] -> set_adj_mines(0);
				board[r][c] -> put_mine(false);
				board[r][c] -> change_state(Tile::State::unclicked);
				game_over = false;
				game_started = false;
				uncovered=0;
			}
		}
		smileyface -> change_image(TileImage::imageSmile);
	}
}

void Game::cb_tile_click (Address pt, Address pw)
{
	if (Fl::event() == FL_RELEASE)
	{
		int row = (reference_to<MyBox>(pt).y()-y_offset)/Tile::tileSide;
		int col = reference_to<MyBox>(pt).x()/Tile::tileSide;
		reference_to<Game>(pw).click(row, col);
	}
	else
	{
		reference_to<Game>(pw).show_scared();
	}
}

void Game::cb_restart_click (Address, Address pw)
{
	reference_to<Game>(pw).restart_game();
}

void Game::click (int row, int col) 
{
	if (game_over) return;
		smileyface -> change_image(TileImage::imageSmile);
	int which = Fl::event_button();
	
	switch (which)
	{
		case FL_LEFT_MOUSE: left_click(row, col);
							break;
		case FL_RIGHT_MOUSE: right_click(row, col);
							 break;
	}
}

void Game::show_scared()
{
	if(!game_over) 
		smileyface -> change_image(TileImage::imageScared);
}

void Game::left_click(int row, int col)
{
	if(!game_started) 
	{
		start_game(row, col);
		game_started=true;
	}
	Tile* t = board[row][col];
	Tile::State curr = t -> get_state();
	if (curr == Tile::State::flag || curr == Tile::State::clicked)
	{
		return;
	}
	t -> change_state(Tile::State::clicked);
	if (t -> get_mine()) 
	{
		lose_game(row, col);
		return;
	}
	
	
	int maxRow = board.size();
	int maxCol = board[0].size();

	if (t -> get_adj_mines()==0)
	{
		int bottom = min((row+2),maxRow);
		int right = min((col+2),maxCol);
		for (int r = max(row-1,0); r<bottom; r++)
		{
			for (int c = max(col-1,0); c<right; c++)
			{
				left_click(r,c); 
			}
		}
	}
	uncovered++;
	
	if (uncovered == (maxRow*maxCol - mine_total)) win_game();
}

void Game::right_click(int row, int col)
{
	Tile* t = board[row][col];
	Tile::State current = t -> get_state();
	switch (current)
	{
		case Tile::State::clicked: break;
		case Tile::State::unclicked: t -> change_state(Tile::State::flag);							
									 break;
		case Tile::State::flag: t -> change_state(Tile::State::question);
								break;
		case Tile::State::question: t -> change_state(Tile::State::unclicked);
									break;
	}
}

void Tile::change_state(State s) 
{
	switch(s)
	{
		case State::unclicked: if (get_mine() && debug) 
									changeImage(TileImage::imageBomb);
							   else changeImage(TileImage::imageUnclicked);
							   break;
		case State::clicked: changeImage(*clicked_img);
							 break;
		case State::flag:  changeImage(TileImage::imageFlag);
						   break;
		case State::question: changeImage(TileImage::imageQuestionMark);
							  break;
	}
	current_state = s;
}

void Tile::attach(Graph_lib::Window& win)
{
	pw = new MyBox(loc.x, loc.y, width, height, label.c_str());
    pw -> callback(reinterpret_cast<Fl_Callback*>(do_it), &win); 
    own = &win;
	pw -> align(FL_ALIGN_IMAGE_BACKDROP);
	change_state(State::unclicked);
}

void Tile::put_mine(bool addmine)
{
	mine = addmine;
	if(mine)
		clicked_img = &TileImage::imageRedBomb;
	else 
		clicked_img = &TileImage::imageBlank;
}

void Tile::add_adj_mines()
{
	set_adj_mines(adj_mines + 1);
}

void Tile::set_adj_mines(int numMines)
{
	adj_mines = numMines;
	switch (adj_mines)
	{
		case 0: clicked_img = &TileImage::imageBlank;
				break;
		case 1: clicked_img = &TileImage::imagenumber1;
				break;
		case 2: clicked_img = &TileImage::imagenumber2;
				break;
		case 3: clicked_img = &TileImage::imagenumber3;
				break;
		case 4: clicked_img = &TileImage::imagenumber4;
				break;
		case 5: clicked_img = &TileImage::imagenumber5;
				break;
		case 6: clicked_img = &TileImage::imagenumber6;
				break;
		case 7: clicked_img = &TileImage::imagenumber7;
				break;
		case 8: clicked_img = &TileImage::imagenumber8;
				break;
	}
}

void Tile::loss()
{
	if(mine && current_state!= State::flag)
	{
		clicked_img = &TileImage::imageBomb;
		change_state(State::clicked);
	}
	else if (!mine && current_state== State::flag)
	{
		clicked_img = &TileImage::imageXBomb;
		change_state(State::clicked);
	}
}

void Tile::changeImage(Fl_PNG_Image& im)
{
	pw -> image(im);
	pw -> redraw(); 
}

void Smile::change_image(Fl_PNG_Image& im)
{
	pw -> image(im);
	pw -> redraw(); 
}

void Smile::attach(Graph_lib::Window& win)
{
	pw = new MyBox(loc.x, loc.y, width, height, label.c_str());
    pw -> callback(reinterpret_cast<Fl_Callback*>(do_it), &win); 
    own = &win;
	pw -> align(FL_ALIGN_IMAGE_BACKDROP);
	change_image(TileImage::imageSmile);
}
