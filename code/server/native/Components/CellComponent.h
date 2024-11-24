#pragma once

#include "Game/GridCell.h"

struct CellComponent
{
    gsl::not_null<GridCell*> pCell;
};