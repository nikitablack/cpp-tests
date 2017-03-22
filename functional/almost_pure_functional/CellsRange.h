#pragma once

struct CellsRange
{
	CellsRange(int const cStart = 0, int const rStart = 0, int const cEnd = 0, int const rEnd = 0) : colStart{ cStart }, rowStart{ rStart }, colEnd{ cEnd }, rowEnd{ rEnd }
	{}

	int const colStart;
	int const rowStart;
	int const colEnd;
	int const rowEnd;
};