#include "Minesweeper.h"

int MyBox::handle(int event)
{
	if (event==FL_PUSH || event==FL_RELEASE) 
	{
		do_callback();
		clear_changed();
		return 1;
	}
	return 0;
}

int main()
{
	using namespace Graph_lib;
	Game win(Point{100,200},"Minesweeper"); 
}
